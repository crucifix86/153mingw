#ifndef __CPPGEN_GNET_SKILL2871
#define __CPPGEN_GNET_SKILL2871
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2871:public Skill
    {
      public:
        enum
        { SKILL_ID = 2871 };
          Skill2871 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2871Stub:public SkillStub
    {
      public:
	  	  
	  

      Skill2871Stub ():SkillStub (2871)
        {
            cls = 255;
            name = L"传承之光技能效果";
            nativename = "传承之光技能效果";
            icon = "无式.dds";
            max_level = 10;
            type = 2;
            apcost = 0;
            arrowcost = 0;
            apgain = 0;
            attr = 1;
            rank = 0;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 0;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 1;
            effect = "";
            range.type = 0;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
	
        }
        virtual ~ Skill2871Stub ()
        {
        }
        float GetMpcost(Skill *skill) const
        {
            return 0.0;
        }
        int GetExecutetime(Skill *skill) const
        {
            return 50;
        }
        int GetCoolingtime(Skill *skill) const
        {
            return 0;
        }
        int GetRequiredLevel(Skill * skill) const
	    {
		    static int array[10] = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45};
		    return array[skill->GetLevel () - 1];
	    }
    
	    int GetRequiredSp(Skill * skill) const
	    {
		    static int array[10] = {0, 300, 1200, 2800, 5200, 8400, 12800, 18600, 26300, 36500};
		    return array[skill->GetLevel () - 1];
	    }
        float GetRadius(Skill *skill) const
        {
            return 15.0;
        }
        float GetAttackdistance(Skill *skill) const
        {
            return 0.0;
        }
        float GetAngle(Skill *skill) const
        {
            return 1.0;
        }
        float GetPraydistance(Skill *skill) const
        {
            return skill->GetPlayer()->GetRange();
        }
#ifdef _SKILL_SERVER
        int GetEnmity(Skill *skill) const
        {
            return 0;
        }
        bool StateAttack(Skill *skill) const
        {
            skill->GetVictim()->SetProbability(skill->GetPlayer()->GetMaxhp() == skill->GetT0() && skill->GetPlayer()->GetMaxmp() == skill->GetT1() ? 0.0 : 100.0);
            skill->GetVictim()->SetTime(12000.0);
            skill->GetVictim()->SetRatio((float)skill->GetLevel() * 0.5);
            skill->GetVictim()->SetValue((float)skill->GetLevel() * 0.5);
            skill->GetVictim()->SetExtraexpfactor(1);
            return 1;
        }
        bool TakeEffect(Skill *skill) const
        {
            return 1;
        }
        float GetEffectdistance(Skill *skill) const
        {
            return 0.0;
        }
        float GetHitrate(Skill *skill) const
        {
            return 1.0;
        }
        float GetTalent0(PlayerWrapper * player) const
        {
            return (float) player->GetMaxhp();
        }
        float GetTalent1(PlayerWrapper * player) const
        {
            return (float) player->GetMaxmp();
        }
#endif
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format);

        }
#endif
    };
}
#endif
