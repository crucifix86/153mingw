

#include "p2spInternalStruct.h"

namespace p2sp4 {
	GameCheckValue gGameCheckValue[] = 
	{
		{4,		".zup"		},
		{18,	".ytpch"	},
		{9,		".mzpch"	},
		{2,		".qup"		},
		{11,	".sup"		},
		{5,		".cbp"		},
		{6,		".rup"		},
		{1,		".eupd"		},
		{3,		".cup"		},
		{8,		"SGCQ"		},
		{7,		".xup"		},
		{12,	"SGSJ"		},
		{10,	"xlzj"		},
		{21,	"xlzj_jz"	},
		{26,	".sdpch"	}, //√Œª√…ÒµÒ\…ÒµÒœ¿¬¬
	};

	//01010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101
	const char* getGameCheckValue(GID gid)
	{
		size_t game_num = sizeof(gGameCheckValue) / sizeof(GameCheckValue);
		for(size_t i = 0; i < game_num; ++i) {
			if(gGameCheckValue[i].gid == gid) return gGameCheckValue[i].value;
		}
		return NULL;
	}
}