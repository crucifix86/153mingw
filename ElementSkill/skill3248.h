#ifndef __CPPGEN_GNET_SKILL3248
#define __CPPGEN_GNET_SKILL3248
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill3248:public Skill
    {
      public:
        enum
        { SKILL_ID = 3248 };
          Skill3248 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill3248Stub:public SkillStub
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
                skill->GetPlayer()->SetDecmp( 225.0);
                skill->GetPlayer()->SetPerform( 1);

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
	  
	  

      Skill3248Stub ():SkillStub (3248)
        {
            cls = 5;
            name = L"创世·消失";
            nativename = "创世·消失";
            icon = "创世消失.dds";
            max_level = 2;
            type = 2;
            apcost = 0;
            arrowcost = 0;
            apgain = 0;
            attr = 1;
            rank = 8;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 1;
            showorder = 1125;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 1;
            {
                restrict_weapons.push_back (23749);
            }
            effect = "刺客_创世_击中.sgc";
            range.type = 5;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            itemcost = 49982;
            itemcostcount = 1;
            pre_skills.push_back (std::pair < ID, int >(0, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
#endif
	
        }
        virtual ~ Skill3248Stub ()
        {
        }
        float GetMpcost(Skill *skill) const
        {
            return 225.0;
        }
        int GetExecutetime(Skill *skill) const
        {
            return 0;
        }
        int GetCoolingtime(Skill *skill) const
        {
            return 300000;
        }
        int GetRequiredLevel(Skill * skill) const
        {
            static int array[10] = {100, 100};
            return array[skill->GetLevel () - 1];
        }

        int GetRequiredSp(Skill * skill) const
        {
            static int array[10] = {1000000, 2000000};
            return array[skill->GetLevel () - 1];
        }

        int GetRequiredMoney(Skill * skill) const
        {
            static int array[10] = {1000000, 2000000};
            return array[skill->GetLevel () - 1];
        }

        int GetRequiredItem(Skill * skill) const
        {
            static int array[10] = {49946, 49947};
            return array[skill->GetLevel () - 1];
        }
        float GetRadius(Skill *skill) const
        {
            return 16.0;
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
            skill->GetVictim()->SetProbability( 2.0);
            skill->GetVictim()->SetTime( (float)(8000 * skill->GetLevel()));
            skill->GetVictim()->SetRatio( 1.0);
            skill->GetVictim()->SetAmount( 1.0);
            skill->GetVictim()->SetValue( 3249.0);
            skill->GetVictim()->SetAuracurse4asn(1);
            skill->GetVictim()->SetProbability( 100.0);
            skill->GetVictim()->SetTime( 3000.0);
            skill->GetVictim()->SetRatio( 0.0);
            skill->GetVictim()->SetAmount( 10.0);
            skill->GetVictim()->SetValue( 12.0);
            skill->GetVictim()->SetInvisible2(1);
            skill->GetVictim()->SetProbability( 100.0);
            skill->GetVictim()->SetTime( 100.0);
            skill->GetVictim()->SetRatio( 0.1);
            skill->GetVictim()->SetAmount( (float)skill->GetLevel());
            skill->GetVictim()->SetValue( 3250.0);
            skill->GetVictim()->SetAurabless2(1);
            // New effect 1.5.5 Primal Warfare
            skill->GetVictim()->SetProbability(100.0);
            skill->GetVictim()->SetTime((float)(8000 * skill->GetLevel()));
            skill->GetVictim()->SetRatio(1.5);
            skill->GetVictim()->SetIncdamageasn(1);
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
        int GetAttackspeed(Skill *skill) const
        {
            return 9;
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
            return _snwprintf (buffer, length, format, skill->GetLevel(), (8000 * skill->GetLevel()) / 1000);

        }
#endif
    };
}
#endif
