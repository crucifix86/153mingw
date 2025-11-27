#ifndef __CPPGEN_GNET_SKILL3212
#define __CPPGEN_GNET_SKILL3212
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill3212:public Skill
    {
      public:
        enum
        { SKILL_ID = 3212 };
          Skill3212 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill3212Stub:public SkillStub
    {
      public:
	  #ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 600;

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
                return 50;

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
                skill->GetPlayer()->SetPerform( 2);

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
                return 2950;

            }
            bool Quit (Skill * skill) const
            {
                return 0;

            }
            bool Loop (Skill * skill) const
            {
                return (skill->GetPlayer()->GetMp() > 215 && skill->GetSection() <= 4 );
            }
            bool Bypass (Skill * skill) const
            {
                return 0;

            }
            void Calculate (Skill * skill) const
            {
                skill->SetSection(skill->GetSection() + 1);
                skill->GetPlayer()->SetMp( (float)skill->GetPlayer()->GetMp() * 0.9);
                skill->GetPlayer()->SetHp( (float)skill->GetPlayer()->GetHp() * 0.9);
                skill->SetPlus((float)skill->GetPlayer()->GetMaxhp() * 0.5 + 9545.0);
                skill->SetRatio(3.0);
                skill->SetDamage(skill->GetAttack());
                skill->GetPlayer()->SetCombat(1);
                skill->GetPlayer()->SetPerform( 0);

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
        class State4:public SkillStub::State
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
	  
	  

      Skill3212Stub ():SkillStub (3212)
        {
            cls = 4;
            name = L"´´ÊÀ¡¤ºé»ÄÊÞ³±";
            nativename = "´´ÊÀ¡¤ºé»ÄÊÞ³±";
            icon = "´´ÊÀºé»ÄÊÞ³±.dds";
            max_level = 2;
            type = 1;
            apcost = 0;
            arrowcost = 0;
            apgain = 0;
            attr = 1;
            rank = 8;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 2;
            showorder = 1125;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 2;
            {
                restrict_weapons.push_back (0);
            }
            {
                restrict_weapons.push_back (9);
            }
            effect = "ÑýÊÞ_ºé»ÄÊÞ³±_»÷ÖÐ.sgc";
            range.type = 2;
            doenchant = true;
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
            statestub.push_back (new State4 ());
#endif
	
        }
        virtual ~ Skill3212Stub ()
        {
        }
        float GetMpcost(Skill *skill) const
        {
            return 0.0;
        }
        int GetExecutetime(Skill *skill) const
        {
            return -1;
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
            static int array[10] = {49916, 49917};
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
        bool StateAttack(Skill *skill) const
        {
            skill->GetVictim()->SetProbability( (float)skill->GetLevel() * 50.0);
            skill->GetVictim()->SetTime( 2500.0);
            //skill->GetVictim()->SetValue( 2.0);
            skill->GetVictim()->SetPalsy(1);
            return 1;
        }
        bool BlessMe(Skill *skill) const
        {
            skill->GetVictim()->SetTime(( skill->GetSection() == 1 ) ? 16000.0 : 0.0);
            skill->GetVictim()->SetFreemove(1);
            return 1;
        }
        bool TakeEffect(Skill *skill) const
        {
            return 1;
        }
        float GetEffectdistance(Skill *skill) const
        {
            return 5.0;
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
            return _snwprintf (buffer, length, format, skill->GetLevel(), (int)(skill->GetLevel() * 50));

        }
#endif
    };
}
#endif
