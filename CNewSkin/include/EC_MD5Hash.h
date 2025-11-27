#pragma once

namespace PATCH
{
	class MD5Hash{
		void *m_pctx;
		unsigned char m_digest[16];
	public:
		MD5Hash();
		~MD5Hash();
		
		void Update(const void *data, unsigned int length);
		const unsigned char* Final();

		bool Equal(const char* strdigest) const;
		bool Equal(MD5Hash& hash) const;
		
		const unsigned char* GetDigest() const { return m_digest; }
		char* GetString(char* buffer, unsigned int& length) const;
		int Length() const { return 16; }
	};

}