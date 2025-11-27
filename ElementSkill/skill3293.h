#ifndef __CPPGEN_GNET_SKILL3293
#define __CPPGEN_GNET_SKILL3293
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill3293:public Skill
    {
      public:
        enum
        { SKILL_ID = 3293 };
          Skill3293 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill3293Stub:public SkillStub
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
                skill->GetPlayer()->SetDecmp( 400.0);
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
#ifdef _SKILL_SERVER
        class State2:public SkillStub::State
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
                ;

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
	  
	  

      Skill3293Stub ():SkillStub (3293)
        {
            cls = 11;
            name = L"ÖÁ¡¤ÉñÁ­·¥¹ð";
            nativename = "ÖÁ¡¤ÉñÁ­·¥¹ð";
            icon = "ÉñÁ­·¥¹ð.dds";
            max_level = 1;
            type = 2;
            apcost = 50;
            arrowcost = 0;
            apgain = 0;
            attr = 1;
            rank = 8;
            eventflag = 0;
            is_senior = 1;
            posdouble = 0;
            clslimit = 0;
            time_type = 1;
            showorder = 0;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 3;
            {
                restrict_weapons.push_back (0);
            }
            {
                restrict_weapons.push_back (44879);
            }
            effect = "ÔÂÏÉ_ÉñÁ­·¥¹ð_»÷ÖÐ.sgc";
            range.type = 5;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(2629, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
#endif
	
        }
        virtual ~ Skill3293Stub ()
        {
        }
        float GetMpcost(Skill *skill) const
        {
            return 400.0;
        }
        int GetExecutetime(Skill *skill) const
        {
            return 0;
        }
        int GetCoolingtime(Skill *skill) const
        {
            return 90000;
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
            static int array[10] = {49978};
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
            skill->GetVictim()->SetTime( 9000.0);
            skill->GetVictim()->SetAmount( 2.0);
            skill->GetVictim()->SetValue( 15.0);
            skill->GetVictim()->SetIncenchantrange(1);
            skill->GetVictim()->SetProbability( 100.0);
            skill->GetVictim()->SetTime((skill->GetPlayer()->GetBalls() % 5 == 3) ? 9000.0 : (skill->GetPlayer()->GetBalls() % 5 == 2) ? 6000.0 : (skill->GetPlayer()->GetBalls() % 5 == 1) ? 3000.0 : 100.0);
            skill->GetVictim()->SetRatio( 0.5);
            skill->GetVictim()->SetFastpray(1);
            skill->GetVictim()->SetProbability( 100.0);
            skill->GetVictim()->SetTime( 9000.0);
            skill->GetVictim()->SetShowicon( 0);
            skill->GetVictim()->SetValue(( skill->GetPlayer()->GetBalls() / 5 == 1 ) ? 10.0 : ( skill->GetPlayer()->GetBalls() / 5 == 2 ) ? 20.0 : ( skill->GetPlayer()->GetBalls() / 5 == 3 ) ? 30.0 : 1.0);
            skill->GetVictim()->SetAddattackdegree(1);
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
