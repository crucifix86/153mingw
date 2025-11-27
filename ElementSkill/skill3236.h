#ifndef __CPPGEN_GNET_SKILL3236
#define __CPPGEN_GNET_SKILL3236
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill3236:public Skill
    {
      public:
        enum
        { SKILL_ID = 3236 };
          Skill3236 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill3236Stub:public SkillStub
    {
      public:
	  #ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 2400;

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
                skill->GetPlayer()->SetDecmp( 64.0);
                skill->GetPlayer()->SetPray(1);

            }
            bool Interrupt (Skill * skill) const
            {
                return skill->GetRand() <= 4;

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
                skill->GetPlayer()->SetDecmp( 256.0);
                skill->SetPlus(9943.0);
                skill->SetRatio(3.0);
                skill->SetGolddamage(skill->GetMagicattack());
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
	  
	  

      Skill3236Stub ():SkillStub (3236)
        {
            cls = 7;
            name = L"Ðþ¡¤ÉñÀ×·£";
            nativename = "Ðþ¡¤ÉñÀ×·£";
            icon = "ÐþÉñÀ×·£.dds";
            max_level = 1;
            type = 1;
            apcost = 0;
            arrowcost = 0;
            apgain = 45;
            attr = 2;
            rank = 20;
            eventflag = 0;
            is_senior = 1;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1426;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 3;
            {
                restrict_weapons.push_back (292);
            }
            {
                restrict_weapons.push_back (0);
            }
            effect = "1ÉñÀ×.sgc";
            range.type = 0;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(580, 1));
            pre_skills.push_back (std::pair < ID, int >(584, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
	
        }
        virtual ~ Skill3236Stub ()
        {
        }
        float GetMpcost(Skill *skill) const
        {
            return 320.0;
        }
        int GetExecutetime(Skill *skill) const
        {
            return 1200;
        }
        int GetCoolingtime(Skill *skill) const
        {
            return 8000;
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
            static int array[10] = {49934};
            return array[skill->GetLevel () - 1];
        }
        float GetRadius(Skill *skill) const
        {
            return (float)((float)skill->GetLevel() * 0.4 + 3.0);
        }
        float GetAttackdistance(Skill *skill) const
        {
            return (float)((float)skill->GetLevel() * 0.3 + 4.0);
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
            skill->GetVictim()->SetTime( 12000.0);
            skill->GetVictim()->SetAmount((( skill->GetT1() - skill->GetPlayer()->GetDefenddegree() > 0 ) ? ((float)(skill->GetT1() - skill->GetPlayer()->GetDefenddegree()) * 0.01 + 1.0) : 1.0) * (float)skill->GetT0());
            skill->GetVictim()->SetThunder(1);
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
            return 4;
        }
        float GetHitrate(Skill *skill) const
        {
            return 1.0;
        }
        float GetTalent0(PlayerWrapper * player) const
        {
            return (float)(6 * player->GetWeaponmagicdamage() + 6800);
        }
        float GetTalent1(PlayerWrapper * player) const
        {
            return (float) player->GetAttackdegree();
        }
#endif
#endif
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format, 28, 320);

        }
#endif
    };
}
#endif
