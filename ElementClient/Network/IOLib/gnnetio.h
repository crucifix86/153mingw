#ifndef __NETIO_H
#define __NETIO_H

#include <string>
#include <map>

#include "gnoctets.h"
#include "gnpollio.h"
#include "gnsecure.h"
#include "gnthread.h"
#include "gnconf.h"

namespace GNET
{

#define ERRSIZE 128
extern char errormsg[ERRSIZE];

class SockAddr
{
	Octets addr;
public:	
	SockAddr() {}
	template<typename T> SockAddr(const T &sa) : addr(&sa, sizeof(sa)) { }
	SockAddr(const SockAddr &rhs) : addr(rhs.addr) { }
	int GetLen() const { return addr.size(); }
	template<typename T> operator T* () { addr.resize(sizeof(T)); return (T *)addr.begin(); }
	template<typename T> operator const T* () const { return (const T *)addr.begin(); }
	operator const sockaddr * () const { return (const sockaddr *)addr.begin(); }
	operator const sockaddr_in * () const { return (const sockaddr_in *)addr.begin(); }
	operator sockaddr_in * () { addr.resize(sizeof(sockaddr_in)); return (sockaddr_in *)addr.begin(); }
};

class NetSession
{
	friend class NetIO;
	friend class StreamIO;
	friend class DgramServerIO;
	friend class DgramClientIO;
	enum { DEFAULTIOBUF = 8192 };
	bool    closing;
	Octets	ibuffer;
	Octets	obuffer;
	Octets  isecbuf;
	Security *isec;
	Security *osec;
protected:
	Thread::Mutex locker;
	virtual ~NetSession ()
	{
		isec->Destroy();
		osec->Destroy();
	}
	NetSession(const NetSession &rhs) : closing(false), ibuffer(rhs.ibuffer.capacity()), 
		obuffer(rhs.obuffer.capacity()), isec(rhs.isec->Clone()), osec(rhs.osec->Clone()), locker(true) { }

	bool Output(Octets &data)
	{
		if (data.size() + obuffer.size() > obuffer.capacity()) return false;
		osec->Update(data);
		obuffer.insert(obuffer.end(), data.begin(), data.end());
		return true;
	}

	Octets& Input()
	{
		isec->Update(ibuffer);
		isecbuf.insert(isecbuf.end(), ibuffer.begin(), ibuffer.end());
		ibuffer.clear();
		return isecbuf;
	}

	void SetISecurity(Security::Type type, const Octets &key)
	{
		Thread::Mutex::Scoped l(locker);
		isec->Destroy();
		isec = Security::Create(type);
		isec->SetParameter(key);
	}
	
	void SetOSecurity(Security::Type type, const Octets &key)
	{
		Thread::Mutex::Scoped l(locker);
		osec->Destroy();
		osec = Security::Create(type);
		osec->SetParameter(key);
	}
public:
	NetSession() : closing(false), ibuffer(DEFAULTIOBUF), obuffer(DEFAULTIOBUF), 
		isec(Security::Create(NULLSECURITY)), osec(Security::Create(NULLSECURITY)), locker(true) { }

	void LoadConfig()
	{
		// NOTE: Server has encryption disabled (isec/osec commented in config)
		// So we use NULLSECURITY to match. If server enables encryption,
		// uncomment the code below.
		#if 0
		Conf *conf = Conf::GetInstance();
		Conf::section_type section = Identification();
		size_t size;
		size = 9;
		Conf::value_type ikey = "b9ef51f7d77d3a0540991e6648ec9f87";
		SetISecurity(size, Octets(&ikey[0], ikey.size()));
		Conf::value_type okey = "0f5e0d00b83898f5b18d7a0fbaef6288";
		SetOSecurity(size, Octets(&okey[0], okey.size()));
		#endif
		// Use no encryption - server sends plaintext
	}

	virtual void Close() { closing = true; }
	bool Closing()const{ return closing; }

	virtual std::string Identification () const = 0;
	virtual void OnRecv() = 0;
	virtual bool NoMoreData() const = 0;
	virtual void OnSend() = 0;
	virtual void OnOpen() { }
	virtual void OnClose() { }
	virtual void OnAbort() { }
	virtual	NetSession* Clone() const = 0;
	virtual void Destroy() { delete this; }
	virtual void OnCheckAddress(SockAddr &) const { }
};

class NetIO : public PollIO
{
protected:
	NetSession *session;
	Octets	*ibuf;
	Octets	*obuf;
	~NetIO() { closesocket(fd); }
	NetIO(int fd, NetSession *s) : PollIO(fd), session(s), ibuf(&s->ibuffer), obuf(&s->obuffer) { }
};

class StreamIO : public NetIO
{
	void PollIn()
	{		
		int recv_bytes;
		if ((recv_bytes = recv(fd, (char*)ibuf->end(), ibuf->capacity() - ibuf->size(), 0)) > 0)
		{
			ibuf->resize(ibuf->size() + recv_bytes);
			return;
		}
		int err = WSAGetLastError();
		if(recv_bytes!=SOCKET_ERROR || err!=WSAEWOULDBLOCK)
		{
			obuf->clear();
			session->closing = true;
			_snprintf(errormsg, ERRSIZE, "Pollin: errno=%d \n", err);
		}
	}

	void PollOut()
	{
		int send_bytes;
		if ((send_bytes = send(fd,(const char*) obuf->begin(), obuf->size(), 0)) > 0)
		{
			obuf->erase(obuf->begin(), (char*)obuf->begin() + send_bytes);
			return;
		}
		int err = WSAGetLastError();
		if(send_bytes!=SOCKET_ERROR || err!=WSAEWOULDBLOCK)
		{
			obuf->clear();
			_snprintf(errormsg, 128, "Pollout: errno=%d \n", err);
			session->closing = true;
		}
	}

	int UpdateEvent()
	{
		int event = 0;
		
		Thread::Mutex::Scoped l(session->locker);
		if (ibuf->size())
			session->OnRecv();
		/* BW 2010/12/6 �����鱨��˵���ͻ��������ݻ���δ����ʱ���ߣ��ͻ����޷������յ�������Ϣ
		if (!session->closing)
			session->OnSend();
		if (obuf->size()||!(session->NoMoreData()))
			event = IO_POLLOUT;
		else if (session->closing)
			return -1;
		*/
		if (!session->closing)
		{
			session->OnSend();
			if (obuf->size()||!(session->NoMoreData()))
				event = IO_POLLOUT;
		}
		else 
			return -1;
		if (ibuf->size() < ibuf->capacity())
			event |= IO_POLLIN;
		return event;
	}
public:
	~StreamIO()
	{
		session->OnClose();
		session->Destroy();
	}

	StreamIO(SOCKET fd, NetSession *s) : NetIO(fd, s)
	{
		session->OnOpen();
	}
};

class DgramClientIO : public NetIO
{
	SockAddr peer;
	void PollIn()
	{
		int recv_bytes;
		if ((recv_bytes = recv(fd, (char*)ibuf->end(), ibuf->capacity() - ibuf->size(),0)) > 0)
		{
			ibuf->resize(ibuf->size() + recv_bytes);
			return;
		}

	}

	int UpdateEvent()
	{
		Thread::Mutex::Scoped l(session->locker);
		if (ibuf->size())
			session->OnRecv();
		session->OnSend();
		if (obuf->size())
		{
			sendto(fd, (const char*)obuf->begin(), obuf->size(), 0, peer, peer.GetLen());
			obuf->clear();
		}
		return session->closing ? -1 : IO_POLLIN;
	}
public:
	~DgramClientIO()
	{
		session->OnClose();
		session->Destroy();
	}

	DgramClientIO(SOCKET fd, NetSession *s, const SockAddr &sa) : NetIO(fd, s), peer(sa) 
	{
		session->OnOpen();
	}
};

class DgramServerIO : public NetIO
{
	struct compare_SockAddr
	{
		bool operator() (const SockAddr &sa1, const SockAddr &sa2) const
		{
			const struct sockaddr_in *s1 = sa1;
			const struct sockaddr_in *s2 = sa2;

			return	s1->sin_addr.s_addr < s2->sin_addr.s_addr  ||
				s1->sin_addr.s_addr == s2->sin_addr.s_addr && 
				s1->sin_port < s2->sin_port;
		}
	};
	typedef std::map<SockAddr, NetSession *, compare_SockAddr> Map;
	Map map;

	void PollIn()
	{
		struct sockaddr_in addr;
		int len;
		int recv_bytes;
		while ((recv_bytes = recvfrom(fd, (char*)ibuf->begin(), ibuf->capacity(), 0, (struct sockaddr*)&addr, &len)) > 0)
		{
			Map::iterator it = map.find(addr);
			if (it == map.end())
			{
				NetSession *s = session->Clone();
				it = map.insert(std::make_pair(addr, s)).first;
				s->OnOpen();
			}
			Octets &i = (*it).second->ibuffer;
			if (i.size() + recv_bytes < i.capacity())
				i.insert(i.end(), ibuf->begin(), recv_bytes);
		}
	}

	int UpdateEvent()
	{
		for (Map::iterator it = map.begin(); it != map.end(); ++it)
		{
			const SockAddr &sa = (*it).first;
			NetSession *session = (*it).second;
			Octets *ibuf  = &session->ibuffer;
			Octets *obuf  = &session->obuffer;
			Thread::Mutex::Scoped l(session->locker);
			if (ibuf->size())
				session->OnRecv();
			session->OnSend();
			if (obuf->size())
			{
				sendto(fd, (const char*)obuf->begin(), obuf->size(), 0, sa, sa.GetLen());
				obuf->clear();
			}
			if (session->closing)
			{
				map.erase(it);
				session->OnClose();
				session->Destroy();
			}
		}
		session->OnSend();
		return session->closing ? -1 : IO_POLLIN;
	}
public:
	~DgramServerIO()
	{
		for (Map::iterator it = map.begin(); it != map.end(); ++it)
		{
			NetSession *session = (*it).second;
			session->OnClose();
			session->Destroy();
		}
		session->Destroy();
	}

	DgramServerIO(int fd, NetSession *s) : NetIO(fd, s)
	{
	}
};

};

#endif
