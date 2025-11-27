#ifndef __CPPGEN_GNET_SKILL2903
#define __CPPGEN_GNET_SKILL2903
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2903:public Skill
    {
      public:
        enum
        { SKILL_ID = 2903 };
          Skill2903 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2903Stub:public SkillStub
    {
      public:
	  #ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 0;

            }
            bool Quit (Skill * skill) const
            {
                return 0;

            }
            bool Loop (Skill * skill) const
            {
                return 0;

            }
            bool Bypass (Skill * skill) const
            {
                return 0;

            }
            void Calculate (Skill * skill) const
            {
                skill->GetPlayer()->SetDecmp( 116.0);
                skill->GetPlayer()->SetPerform( 1);

            }
            bool Interrupt (Skill * skill) const
            {
                return 0;

            }
            bool Cancel (Skill * skill) const
            {
                return 0;

            }
            bool Skip (Skill * skill) const
            {
                return 0;

            }
        };
#endif
	  
	  

      Skill2903Stub ():SkillStub (2903)
        {
            cls = 10;
            name = L"玄·月饮";
            nativename = "玄·月饮";
            icon = "仙月饮.dds";
            max_level = 1;
            type = 2;
            apcost = 50;
            arrowcost = 0;
            apgain = 0;
            attr = 1;
            rank = 20;
            eventflag = 0;
            is_senior = 1;
            posdouble = 0;
            clslimit = 0;
            time_type = 1;
            showorder = 1009;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 3;
            {
                restrict_weapons.push_back (44878);
            }
            {
                restrict_weapons.push_back (0);
            }
            effect = "夜影_月饮_击中.sgc";
            range.type = 5;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(2754, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
#endif
	
        }
        virtual ~ Skill2903Stub ()
        {
        }
        float GetMpcost(Skill *skill) const
        {
            return 116.0;
        }
        int GetExecutetime(Skill *skill) const
        {
            return 1068;
        }
        int GetCoolingtime(Skill *skill) const
        {
            return 60000;
        }
        int GetRequiredLevel(Skill * skill) const
        {
            static int array[10] = {100};
            return array[skill->GetLevel () - 1];
        }

        int GetRequiredSp(Skill * skill) const
        {
            static int array[10] = {1000000};
            return array[skill->GetLevel () - 1];
        }

        int GetRequiredMoney(Skill * skill) const
        {
            static int array[10] = {1000000};
            return array[skill->GetLevel () - 1];
        }

        int GetRequiredItem(Skill * skill) const
        {
            static int array[10] = {47432};
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
            skill->GetVictim()->SetTime( 14000.0);
            skill->GetVictim()->SetFreemove(1);
            skill->GetVictim()->SetProbability( 100.0);
            skill->GetVictim()->SetRatio( 0.0);
            skill->GetVictim()->SetAmount( 500.0);
            skill->GetVictim()->SetValue( 2559.0);
            skill->GetVictim()->SetSkillcooldown(1);
            skill->GetVictim()->SetProbability( 100.0);
            skill->GetVictim()->SetRatio( 0.0);
            skill->GetVictim()->SetAmount( 500.0);
            skill->GetVictim()->SetValue( 2752.0);
            skill->GetVictim()->SetSkillcooldown(1);
            skill->GetVictim()->SetProbability( 100.0);
            skill->GetVictim()->SetTime( 100.0);
            skill->GetVictim()->SetRatio( 0.1);
            skill->GetVictim()->SetAmount( 1.0);
            skill->GetVictim()->SetValue( 2860.0);
            skill->GetVictim()->SetAurabless2(1);
            return 1;
        }
        bool TakeEffect(Skill *skill) const
        {
            return 1;
        }
        float GetEffectdistance(Skill *skill) const
        {
            return 10.0;
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
