#ifndef NEWSKIN_SINGLETON_H_
#define NEWSKIN_SINGLETON_H_

template <typename T> class Singleton
{
protected:
	static T* ms_Singleton;
	
public:
	Singleton(void)
	{
		ASSERT(!ms_Singleton);
		ms_Singleton = static_cast< T* >(this);
	}
	
	~Singleton(void)
	{  
		ASSERT(ms_Singleton);  
		ms_Singleton = 0; 
	}
	
	static T& GetSingleton(void)
	{	
		return (*GetSingletonPtr()); 
	}
	
	static T* GetSingletonPtr(void)
	{ 
		if (ms_Singleton == NULL)
		{
			ms_Singleton = new T;
		}
		return ms_Singleton; 
	}
	static void Release()
	{
		if (ms_Singleton)
		{
			delete ms_Singleton;
			ms_Singleton = NULL;
		}
	}
};
#endif