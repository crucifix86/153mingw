#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/md5.h>
#include <openssl/err.h>
#include "EC_Signature.h"

namespace PATCH
{


static char PUBKEY[] = "-----BEGIN PUBLIC KEY-----\n\
MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCBlTQ/HuP1lInM+MrJcOO5R/U1\n\
KhNYtk3G+T1xD+/RFlpMdf71if+CN2kfRTdac70I4wiFL0opYkcomABnQwvCrPqn\n\
g7iDcApeBgphuszy+ytKhXWrCPHgz8QDLQ2iBcDWuGI7wZ7iAphab/0+2xF0Jb9y\n\
sPteUtN1G9NQtKDX8QIDAQAB\n\
-----END PUBLIC KEY-----\n";

Signature Signature::instance;

	Signature::Signature()
	{
	    BIO *mem = BIO_new_mem_buf(PUBKEY, strlen(PUBKEY));
		m_key = PEM_read_bio_PUBKEY(mem, NULL, NULL, NULL);
		BIO_free(mem);
	}

	Signature::~Signature()
	{
		EVP_PKEY_free((EVP_PKEY*)m_key);
		ERR_remove_state(0);
		EVP_cleanup();
		CRYPTO_cleanup_all_ex_data();
	}

	bool Signature::Verify(LPCTSTR filename)
	{
		int ret = 0;
		int len = 0;
		char buf[256];
		char dgst[256];
		FILE* fp;
		EVP_MD_CTX     md_ctx;

		if(!m_key)
			return false;
    
		if(!filename || !filename[0] || !(fp=_tfopen(filename, TEXT("r"))))
			return false;
		EVP_VerifyInit(&md_ctx, EVP_md5());
		
		while(fgets(buf, 256, fp))
		{   
			if(strncmp(buf, "-----BEGIN ELEMENT SIGNATURE-----", 33)==0)
			{   
				BIO *bio, *b64;
				int n, offset = 0;
				b64 = BIO_new(BIO_f_base64());
				bio = BIO_new_fp(fp, BIO_NOCLOSE);
				bio = BIO_push(b64, bio);
				while((n = BIO_read(bio, dgst+offset, 256-offset )) > 0)
					offset += n;
				len = offset;
				BIO_free_all(bio);
				break;
			}
			EVP_VerifyUpdate (&md_ctx, buf, strlen(buf));
		}
		fclose(fp);
		if(len>0)
			ret =  EVP_VerifyFinal (&md_ctx, (unsigned char *)dgst, len, (EVP_PKEY*)m_key);

		EVP_MD_CTX_cleanup(&md_ctx);
		return ret==1;
	}
}
