#pragma once
#include <afx.h>
namespace PATCH
{
	class Signature{
		void *m_key;
		static Signature instance;
		Signature();
	public:
		~Signature();
		bool Verify(LPCTSTR filename);
		static Signature* GetInstance() { return &instance; }
	};

}