#ifndef __CPPGEN_GNET_SKILL3254
#define __CPPGEN_GNET_SKILL3254
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill3254:public Skill
    {
      public:
        enum
        { SKILL_ID = 3254 };
          Skill3254 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill3254Stub:public SkillStub
    {
      public:
	  #ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 1800;

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
                skill->GetPlayer()->SetDecmp( 136.0);
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
                return 1133;

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
                skill->GetPlayer()->SetDecmp( 544.0);
                skill->SetPlus(9538.0);
                skill->SetRatio(2.3);
                skill->SetWaterdamage(skill->GetMagicattack());
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
	  
	  

      Skill3254Stub ():SkillStub (3254)
        {
            cls = 2;
            name = L"玄·水瀑泓泱";
            nativename = "玄·水瀑泓泱";
            icon = "水瀑术仙技能.dds";
            max_level = 1;
            type = 1;
            apcost = 100;
            arrowcost = 0;
            apgain = 0;
            attr = 4;
            rank = 21;
            eventflag = 0;
            is_senior = 1;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1720;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 1;
            {
                restrict_weapons.push_back (25333);
            }
            effect = "巫师_水瀑术.sgc";
            range.type = 3;
            doenchant = true;
            dobless = true;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(1246, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
	
        }
        virtual ~ Skill3254Stub ()
        {
        }
        float GetMpcost(Skill *skill) const
        {
            return 680.0;
        }
        int GetExecutetime(Skill *skill) const
        {
            return 1133;
        }
        int GetCoolingtime(Skill *skill) const
        {
            return 30000;
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
            static int array[10] = {49950};
            return array[skill->GetLevel () - 1];
        }
        float GetRadius(Skill *skill) const
        {
            return 8.0;
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
            return 28.5;
        }
#ifdef _SKILL_SERVER
#ifdef _SKILL_SERVER
        int GetEnmity(Skill *skill) const
        {
            return 0;
        }
        bool StateAttack(Skill *skill) const
        {
            skill->GetVictim()->SetProbability( 80.0);
            skill->GetVictim()->SetTime( 9000.0);
            skill->GetVictim()->SetAmount( (float)skill->GetT0());
            skill->GetVictim()->SetBleeding(1);
            return 1;
        }
        bool BlessMe(Skill *skill) const
        {
            skill->GetVictim()->SetProbability( 50.0);
            skill->GetVictim()->SetTime( 1000.0);
            skill->GetVictim()->SetShowicon( 0);
            skill->GetVictim()->SetAmount( 100.0);
            skill->GetVictim()->SetValue( 0.0);
            skill->GetVictim()->SetApgencont2(1);
            return 1;
        }
        bool TakeEffect(Skill *skill) const
        {
            return 1;
        }
        float GetEffectdistance(Skill *skill) const
        {
            return 45.0;
        }
        int GetAttackspeed(Skill *skill) const
        {
            return 6;
        }
        float GetHitrate(Skill *skill) const
        {
            return 1.0;
        }
        float GetTalent0(PlayerWrapper * player) const
        {
            return (float) player->GetSoulpower();
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
