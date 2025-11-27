#ifndef __CPPGEN_GNET_SKILL3229
#define __CPPGEN_GNET_SKILL3229
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill3229:public Skill
    {
      public:
        enum
        { SKILL_ID = 3229 };
          Skill3229 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill3229Stub:public SkillStub
    {
      public:
	  #ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 500;

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
                return 1200;

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
                skill->SetPlus(7458.0);
                skill->SetRatio(( skill->GetLevel() == 2 ) ? 6.0 : 1.0);
                skill->SetDamage((int)((float)skill->GetAttack() * 0.5));
                skill->SetGolddamage((int)((float)skill->GetAttack() * 0.5));
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
	  
	  

      Skill3229Stub ():SkillStub (3229)
        {
            cls = 6;
            name = L"创世·勾魂灭空矢";
            nativename = "创世·勾魂灭空矢";
            icon = "创世勾魂灭空矢.dds";
            max_level = 2;
            type = 1;
            apcost = 0;
            arrowcost = 1;
            apgain = 0;
            attr = 7;
            rank = 8;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
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
                restrict_weapons.push_back (13);
            }
            effect = "创世_勾魂灭天矢_新引而不发.sgc";
            range.type = 0;
            doenchant = false;
            dobless = true;
            commoncooldown = 0;
            commoncooldowntime = 0;
            itemcost = 49982;
            itemcostcount = 1;
            pre_skills.push_back (std::pair < ID, int >(0, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
	
        }
        virtual ~ Skill3229Stub ()
        {
        }
        float GetMpcost(Skill *skill) const
        {
            return 225.0;
        }
        int GetExecutetime(Skill *skill) const
        {
            return 1200;
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
            static int array[10] = {49930, 49931};
            return array[skill->GetLevel () - 1];
        }
        float GetRadius(Skill *skill) const
        {
            return 12.0;
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
        bool BlessMe(Skill *skill) const
        {
            skill->GetVictim()->SetProbability( 10.0);
            skill->GetVictim()->SetTime( 100.0);
            skill->GetVictim()->SetRatio( 0.1);
            skill->GetVictim()->SetAmount( 10.0);
            skill->GetVictim()->SetValue( 3233.0);
            skill->GetVictim()->SetAurabless2(1);
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
        int GetAttackspeed(Skill *skill) const
        {
            return 9;
        }
        float GetHitrate(Skill *skill) const
        {
            return 1.0;
        }
        void ComboSkEndAction(Skill *skill) const
        {
            skill->GetPlayer()->GetComboarg()->SetValue(0, skill->GetPlayer()->GetRand());
        }
#endif
#endif
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format, skill->GetLevel());

        }
#endif
    };
}
#endif
