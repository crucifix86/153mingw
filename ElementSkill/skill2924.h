#ifndef __CPPGEN_GNET_SKILL2924
#define __CPPGEN_GNET_SKILL2924
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2924:public Skill
    {
      public:
        enum
        { SKILL_ID = 2924 };
          Skill2924 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2924Stub:public SkillStub
    {
      public:
	  	  
	  

      Skill2924Stub ():SkillStub (2924)
        {
            cls = 10;
            name = L"荒夜袭技能效果";
            nativename = "荒夜袭技能效果";
            icon = "";
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
            allow_forms = 0;
            effect = "";
            range.type = 0;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 1));
	
        }
        virtual ~ Skill2924Stub ()
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
            // return dword_945929C[GNET::Skill::GetLevel(skill)];
            static int array[10] = { 0 };
            return array[skill->GetLevel() - 1];
        }
        int GetRequiredSp(Skill *skill) const
        {
            ///TODO fix
            // return dword_94592DC[GNET::Skill::GetLevel(skill)];
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
            skill->GetVictim()->SetProbability( 100.0);
            skill->GetVictim()->SetTime( 7001.0);
            skill->GetVictim()->SetRatio( 100.0);
            skill->GetVictim()->SetAmount( 1.0);
            skill->GetVictim()->SetValue( 2547.0);
            skill->GetVictim()->SetIncspecskillcrit(1);
            skill->GetVictim()->SetProbability( 100.0);
            skill->GetVictim()->SetTime( 7001.0);
            skill->GetVictim()->SetRatio( 0.64999998);
            skill->GetVictim()->SetAmount( 1.0);
            skill->GetVictim()->SetValue( 2729.0);
            skill->GetVictim()->SetIncspecskilldamage(1);
            skill->GetVictim()->SetProbability( 100.0);
            skill->GetVictim()->SetTime( 7001.0);
            skill->GetVictim()->SetRatio( 0.64999998);
            skill->GetVictim()->SetAmount( 1.0);
            skill->GetVictim()->SetValue( 2547.0);
            skill->GetVictim()->SetIncspecskilldamage(1);
            skill->GetVictim()->SetProbability( 100.0);
            skill->GetVictim()->SetTime( 7001.0);
            skill->GetVictim()->SetRatio( 100.0);
            skill->GetVictim()->SetAmount( 1.0);
            skill->GetVictim()->SetValue( 2729.0);
            skill->GetVictim()->SetIncspecskillcrit(1);
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
