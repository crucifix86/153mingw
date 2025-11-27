#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>
#include "EC_MD5Hash.h"

namespace PATCH
{
	MD5Hash::MD5Hash() 
	{ 
		m_pctx = malloc(sizeof(MD5_CTX));
		if(m_pctx)
			MD5_Init((MD5_CTX *)m_pctx);
	}

	MD5Hash::~MD5Hash() 
	{
		if(m_pctx)
			free(m_pctx);
	}

	void MD5Hash::Update(const void *data, unsigned int length) 
	{
		if(m_pctx)
			MD5_Update((MD5_CTX *)m_pctx,  data, length);
	}

	const unsigned char* MD5Hash::Final()
	{
		if(m_pctx)
			MD5_Final(m_digest, (MD5_CTX *)m_pctx);
		return m_digest;
	}

	char* MD5Hash::GetString(char* buffer, unsigned int& length) const
	{
		if(!buffer || length < 33)
			return NULL;
		length = 32;
		for(int i=0;i<16;i++){
			sprintf(buffer+i*2, "%02x", m_digest[i]);
		}
		buffer[32] = 0;
		return buffer;
	}

	bool MD5Hash::Equal(const char* strdigest) const
	{
		char buf[33];
		for(int i=0;i<16;i++){
			sprintf(buf+i*2, "%02x", m_digest[i]);
		}
		if(strnicmp(strdigest, buf, 32)==0)
			return true;
		return false;
	}

	bool MD5Hash::Equal(MD5Hash& hash) const
	{
		return (memcmp((const void*)m_digest, (const void*)hash.GetDigest(), 16)==0);
	}
}