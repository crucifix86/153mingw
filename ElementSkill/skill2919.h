#ifndef __CPPGEN_GNET_SKILL2919
#define __CPPGEN_GNET_SKILL2919
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2919:public Skill
    {
      public:
        enum
        { SKILL_ID = 2919 };
          Skill2919 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2919Stub:public SkillStub
    {
      public:
	  	  
	  

      Skill2919Stub ():SkillStub (2919)
        {
            cls = 255;
            name = L"海灵之息";
            nativename = "海灵之息";
            icon = "海灵之息.dds";
            max_level = 2;
            type = 5;
            apcost = 0;
            arrowcost = 0;
            apgain = 0;
            attr = 0;
            rank = 8;
            eventflag = 8;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 2006;
            allow_land = 1;
            allow_air = 0;
            allow_water = 0;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 0;
            effect = "";
            range.type = 0;
            doenchant = false;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
	
        }
        virtual ~ Skill2919Stub ()
        {
        }
        float GetMpcost(Skill *skill) const
        {
            return 0.0;
        }
        int GetExecutetime(Skill *skill) const
        {
            return 0;
        }
        int GetCoolingtime(Skill *skill) const
        {
            return 0;
        }
        int GetRequiredLevel(Skill * skill) const
        {
            static int array[10] = {100, 100};
            return array[skill->GetLevel () - 1];
        }

        int GetRequiredSp(Skill * skill) const
        {
            static int array[10] = {100000, 100000};
            return array[skill->GetLevel () - 1];
        }

        int GetRequiredMoney(Skill * skill) const
        {
            static int array[10] = {100000, 100000};
            return array[skill->GetLevel () - 1];
        }

        int GetRequiredItem(Skill * skill) const
        {
            static int array[10] = {47450, 47450};
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredRealmLevel(Skill *skill) const
        {
            static int array[10] = {30, 34};
            return array[skill->GetLevel() - 1];
        }
        float GetRadius(Skill *skill) const
        {
            return 0.0;
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
            return 0.0;
        }
#ifdef _SKILL_SERVER
#ifdef _SKILL_SERVER
        int GetEnmity(Skill *skill) const
        {
            return 0;
        }
        bool TakeEffect(Skill *skill) const
        {
            skill->GetPlayer()->SetIncswimspeed((skill->GetPlayer()->GetIntarg() == 163 || skill->GetPlayer()->GetIntarg() == 168) ? (float)skill->GetLevel() : 0.0);
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
#endif
#endif
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format, skill->GetLevel(), skill->GetLevel());

        }
#endif
    };
}
#endif
