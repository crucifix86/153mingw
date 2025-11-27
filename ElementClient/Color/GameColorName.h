#include <Windows.h>

class GameColorName
{
	inline unsigned int SET_RGBA(int red, int grin, int blue)
	{
		return (_byteswap_ulong(RGB(red, grin, blue)) >> 8) | 0xff000000;
	}

	inline int game_tick()
	{
		return GetTickCount();
	}
public:
	int UIColor_svetofor();
	int UIColor_infarkt();
	int UIColor_police();
	int UIColor_black();
	int UIColor_Chamelion();
	int UIColor_raduga();
	int UIColor_sweet();
	int UIColor_disko();
	int UIColor_fireice();
	int UIColor_vio();
	int UIColor_nebo();
	int UIColor_izumrud();
	int UIColor_puls_red();
	int UIColor_red();
	int UIColor_beryza();
	int UIColor_orange();
	int UIColor_violet();
	int UIColor_roz();
	int UIColor_sin();
	int UIColor_green();
	int UIColor_yellow();
	int UIColor_kislot();
	int UIColor_cold();
	int UIColor_puls_green();
	int UIColor_puls_blue();
	int UIColor_puls_black();
	int UIColor_emocore();
	int UIColor_puls_ellow();
	int UIColor_puls_multi();

	int GetColorName(int name_id);
public:
	static GameColorName* GetInstance()
	{
		if (!instance)
			instance = new GameColorName();
		return instance;
	}
	static GameColorName* instance;
};

