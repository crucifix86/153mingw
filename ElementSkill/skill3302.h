#ifndef __CPPGEN_GNET_SKILL3302
#define __CPPGEN_GNET_SKILL3302
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill3302:public Skill
    {
      public:
        enum
        { SKILL_ID = 3302 };
          Skill3302 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill3302Stub:public SkillStub
    {
      public:
	  	  
	  

      Skill3302Stub ():SkillStub (3302)
        {
            cls = 255;
            name = L"Ê´ÐÄÕ¶";
            nativename = "Ê´ÐÄÕ¶";
            icon = "";
            max_level = 1;
            type = 6;
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
            allow_forms = 0;
            effect = "¹ÖÎï»÷ÔÎÊõ.sgc";
            range.type = 0;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 1));
	
        }
        virtual ~ Skill3302Stub ()
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
        int GetRequiredLevel(Skill *skill) const
        {
            ///TODO fix
            // return dword_940C85C[skill->GetLevel()];
            static int array[10] = { 0 };
            return array[skill->GetLevel() - 1];
        }
        int GetRequiredSp(Skill *skill) const
        {
            ///TODO fix
            // return dword_940C89C[skill->GetLevel()];
            static int array[10] = { 0 };
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
        bool StateAttack(Skill *skill) const
        {
            skill->GetVictim()->SetProbability( 4.0);
            skill->GetVictim()->SetTime( 8000.0);
            skill->GetVictim()->SetRatio( 1.0);
            skill->GetVictim()->SetAmount( 2.0);
            skill->GetVictim()->SetValue( 1.0);
            skill->GetVictim()->SetDecphysicalmagicaldefense2(1);
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
#endif
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
