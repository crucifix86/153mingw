
#ifndef __GNET_SETLOCKEDLOGOUTUSER_HPP
#define __GNET_SETLOCKEDLOGOUTUSER_HPP

#include "rpcdefs.h"
#include "callid.hxx"
#include "state.hxx"

namespace GNET
{

class SetLockedLogoutUser : public GNET::Protocol
{
	#include "setlockedlogoutuser"

	void Process(Manager *manager, Manager::Session::ID sid)
	{
		// TODO
	}
};

};

#endif
