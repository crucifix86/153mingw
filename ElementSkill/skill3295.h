#ifndef __CPPGEN_GNET_SKILL3295
#define __CPPGEN_GNET_SKILL3295
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill3295:public Skill
    {
      public:
        enum
        { SKILL_ID = 3295 };
          Skill3295 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill3295Stub:public SkillStub
    {
      public:
	  	  
	  

      Skill3295Stub ():SkillStub (3295)
        {
            cls = 11;
            name = L"荒月神之体技能效果";
            nativename = "荒月神之体技能效果";
            icon = "朔光咏.dds";
            max_level = 1;
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
            range.type = 5;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
	
        }
        virtual ~ Skill3295Stub ()
        {
        }
        float GetMpcost(Skill *skill) const
        {
            return 0.0;
        }
        int GetExecutetime(Skill *skill) const
        {
            return 802;
        }
        int GetCoolingtime(Skill *skill) const
        {
            return 3000;
        }
        int GetRequiredLevel(Skill *skill) const
        {
            ///TODO fix
            // return dword_9460A1C[skill->GetLevel()];
            static int array[10] = { 0 };
            return array[skill->GetLevel() - 1];
        }
        int GetRequiredSp(Skill *skill) const
        {
            ///TODO fix
            // return dword_9460A5C[skill->GetLevel()];
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
            return 28.0;
        }
#ifdef _SKILL_SERVER
#ifdef _SKILL_SERVER
        int GetEnmity(Skill *skill) const
        {
            return 0;
        }
        bool StateAttack(Skill *skill) const
        {
            skill->GetVictim()->SetProbability( 100.0);
            skill->GetVictim()->SetRatio( 1.0);
            skill->GetVictim()->SetScaleinchp(1);
            skill->GetVictim()->SetProbability( 100.0);
            skill->GetVictim()->SetTime( 5000.0);
            skill->GetVictim()->SetShowicon( 0);
            skill->GetVictim()->SetInvincible(1);
            skill->GetVictim()->SetProbability( 100.0);
            skill->GetVictim()->SetTime( 100.0);
            skill->GetVictim()->SetRatio( 10.0);
            skill->GetVictim()->SetAmount( 1.0);
            skill->GetVictim()->SetValue( 3296.0);
            skill->GetVictim()->SetAuracurse(1);
            return 1;
        }
        bool TakeEffect(Skill *skill) const
        {
            return 1;
        }
        float GetEffectdistance(Skill *skill) const
        {
            return 35.0;
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
