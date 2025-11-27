#ifndef __CPPGEN_GNET_SKILL3292
#define __CPPGEN_GNET_SKILL3292
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill3292:public Skill
    {
      public:
        enum
        { SKILL_ID = 3292 };
          Skill3292 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill3292Stub:public SkillStub
    {
      public:
	  #ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 101;

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
                skill->GetPlayer()->SetDecmp( 71.0);
                skill->GetPlayer()->SetPray(1);

            }
            bool Interrupt (Skill * skill) const
            {
                return 0;

            }
            bool Cancel (Skill * skill) const
            {
                return 1;

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
                return 6001;

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
                skill->GetPlayer()->SetDecmp( 285.0);
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
        class State3:public SkillStub::State
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
	  
	  

      Skill3292Stub ():SkillStub (3292)
        {
            cls = 11;
            name = L"荒·月神之体";
            nativename = "荒·月神之体";
            icon = "魔月神之体.dds";
            max_level = 1;
            type = 2;
            apcost = 0;
            arrowcost = 0;
            apgain = 30;
            attr = 4;
            rank = 31;
            eventflag = 0;
            is_senior = 1;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1023;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 3;
            {
                restrict_weapons.push_back (44879);
            }
            {
                restrict_weapons.push_back (0);
            }
            effect = "";
            range.type = 5;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(2810, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
	
        }
        virtual ~ Skill3292Stub ()
        {
        }
        float GetMpcost(Skill *skill) const
        {
            return 356.0;
        }
        int GetExecutetime(Skill *skill) const
        {
            return 6001;
        }
        int GetCoolingtime(Skill *skill) const
        {
            return 180000;
        }
        int GetRequiredLevel(Skill * skill) const
        {
            static int array[10] = {100};
            return array[skill->GetLevel () - 1];
        }

        int GetRequiredSp(Skill * skill) const
        {
            static int array[10] = {2000000};
            return array[skill->GetLevel () - 1];
        }

        int GetRequiredMoney(Skill * skill) const
        {
            static int array[10] = {2000000};
            return array[skill->GetLevel () - 1];
        }

        int GetRequiredItem(Skill * skill) const
        {
            static int array[10] = {49977};
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
            return 10.0;
        }
#ifdef _SKILL_SERVER
#ifdef _SKILL_SERVER
        int GetEnmity(Skill *skill) const
        {
            return 0;
        }
        bool StateAttack(Skill *skill) const
        {
            skill->GetVictim()->SetTime( 6000.0);
            skill->GetVictim()->SetRatio( 0.75);
            skill->GetVictim()->SetValue( 1.0);
            skill->GetVictim()->SetMoongod(1);
            skill->GetVictim()->SetProbability( 8.0);
            skill->GetVictim()->SetTime( 6000.0);
            skill->GetVictim()->SetRatio( 1.0);
            skill->GetVictim()->SetAmount( 2.0);
            skill->GetVictim()->SetValue( 3295.0);
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
