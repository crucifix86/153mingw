
#include "p2spStableHeadres.h"
#include "p2spVersion.h"
#include <sstream>

namespace p2sp4 {
	version version::zeroVersion;

	//----------------------------------------------------------------------------------
	version::version(const std::string& ver, const std::string& split) 
		: _hi(0), _major(0), _minor(0), _least(0) 
	{	
		assign(ver, split);
	}

	//----------------------------------------------------------------------------------
	void version::assign(const std::string& ver, const std::string& split)
	{
		fagex::StringVector vers = fagex::StringUtil::split(ver, split);
		assert(vers.size() <= 4);

		if(vers.size() >= 1) {
			_least = atoi(vers[vers.size() - 1].c_str());
		}
		if(vers.size() >= 2) {
			_minor = atoi(vers[vers.size() - 2].c_str());
		}
		if(vers.size() >= 3) {
			_major = atoi(vers[vers.size() - 3].c_str());
		}
		if(vers.size() == 4) {
			_hi = atoi(vers[vers.size() - 4].c_str());
		}
	}

	//----------------------------------------------------------------------------------
	std::string version::to_string() const
	{
		std::stringstream sss;
		sss << _hi << "." << _major << "." << _minor << "." << _least;
		return sss.str();
	}
}