#ifndef __CPPGEN_GNET_SKILL3250
#define __CPPGEN_GNET_SKILL3250
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill3250:public Skill
    {
      public:
        enum
        { SKILL_ID = 3250 };
          Skill3250 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill3250Stub:public SkillStub
    {
      public:
	  	  
	  

      Skill3250Stub ():SkillStub (3250)
        {
            cls = 5;
            name = L"创世消失技能效果1";
            nativename = "创世消失技能效果1";
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
            range.type = 5;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
	
        }
        virtual ~ Skill3250Stub ()
        {
        }
        float GetMpcost(Skill *skill) const
        {
            return 0.0;
        }
        int GetExecutetime(Skill *skill) const
        {
            return 1001;
        }
        int GetCoolingtime(Skill *skill) const
        {
            return 1000;
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
            return skill->GetPlayer()->GetRange();
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
            skill->GetVictim()->SetRatio( 0.0);
            skill->GetVictim()->SetAmount( 500.0);
            skill->GetVictim()->SetValue( 1119.0);
            skill->GetVictim()->SetSkillcooldown(1);
            skill->GetVictim()->SetProbability( 100.0);
            skill->GetVictim()->SetRatio( 0.0);
            skill->GetVictim()->SetAmount( 500.0);
            skill->GetVictim()->SetValue( 1300.0);
            skill->GetVictim()->SetSkillcooldown(1);
            skill->GetVictim()->SetProbability( 100.0);
            skill->GetVictim()->SetRatio( 0.0);
            skill->GetVictim()->SetAmount( 500.0);
            skill->GetVictim()->SetValue( 1301.0);
            skill->GetVictim()->SetSkillcooldown(1);
            // Santified Knife Throw
            skill->GetVictim()->SetProbability(100.0);
            skill->GetVictim()->SetRatio(0.0);
            skill->GetVictim()->SetAmount(500.0);
            skill->GetVictim()->SetValue(3799.0);
            skill->GetVictim()->SetSkillcooldown(1);
            skill->GetVictim()->SetProbability(100.0);
            skill->GetVictim()->SetRatio(0.0);
            skill->GetVictim()->SetAmount(500.0);
            skill->GetVictim()->SetValue(3800.0);
            skill->GetVictim()->SetSkillcooldown(1);
            skill->GetVictim()->SetProbability(100.0);
            skill->GetVictim()->SetRatio(0.0);
            skill->GetVictim()->SetAmount(500.0);
            skill->GetVictim()->SetValue(3801.0);
            skill->GetVictim()->SetSkillcooldown(1);
            skill->GetVictim()->SetProbability(100.0);
            skill->GetVictim()->SetRatio(0.0);
            skill->GetVictim()->SetAmount(500.0);
            skill->GetVictim()->SetValue(3802.0);
            skill->GetVictim()->SetSkillcooldown(1);
            skill->GetVictim()->SetProbability(100.0);
            skill->GetVictim()->SetRatio(0.0);
            skill->GetVictim()->SetAmount(500.0);
            skill->GetVictim()->SetValue(3803.0);
            skill->GetVictim()->SetSkillcooldown(1);
            skill->GetVictim()->SetProbability(100.0);
            skill->GetVictim()->SetRatio(0.0);
            skill->GetVictim()->SetAmount(500.0);
            skill->GetVictim()->SetValue(3804.0);
            skill->GetVictim()->SetSkillcooldown(1);
            //
            skill->GetVictim()->SetProbability( 100.0);
            skill->GetVictim()->SetTime( 100.0);
            skill->GetVictim()->SetRatio( 0.1);
            skill->GetVictim()->SetAmount( (float)skill->GetLevel());
            skill->GetVictim()->SetValue( 3251.0);
            skill->GetVictim()->SetAurabless(1);
            return 1;
        }
        bool TakeEffect(Skill *skill) const
        {
            return 1;
        }
        float GetEffectdistance(Skill *skill) const
        {
            return skill->GetPlayer()->GetRange() + 5.0;
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
