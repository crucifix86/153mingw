#ifndef __CPPGEN_GNET_SKILL2991
#define __CPPGEN_GNET_SKILL2991
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2991:public Skill
    {
      public:
        enum
        { SKILL_ID = 2991 };
          Skill2991 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2991Stub:public SkillStub
    {
      public:
	  #ifdef _SKILL_SERVER
        class State1:public SkillStub::State
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
                skill->GetPlayer()->SetPray(1);

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
	  
	  

      Skill2991Stub ():SkillStub (2991)
        {
            cls = 127;
            name = L"±¯·ßÆÐÌá³£×¤";
            nativename = "±¯·ßÆÐÌá³£×¤";
            icon = "";
            max_level = 10;
            type = 2;
            apcost = 0;
            arrowcost = 0;
            apgain = 0;
            attr = 2;
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
            effect = "±¯·ßÆÐÌá³£×¤.sgc";
            range.type = 5;
            doenchant = true;
            dobless = true;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
	
        }
        virtual ~ Skill2991Stub ()
        {
        }
        float GetMpcost(Skill *skill) const
        {
            return 0.0;
        }
        int GetExecutetime(Skill *skill) const
        {
            return 50;
        }
        int GetCoolingtime(Skill *skill) const
        {
            return 0;
        }
        int GetRequiredLevel(Skill *skill) const
        {
            ///TODO fix
            // return dword_94009DC[skill->GetLevel()];
            static int array[10] = { 0 };
            return array[skill->GetLevel() - 1];
        }
        int GetRequiredSp(Skill *skill) const
        {
            ///TODO fix
            // return dword_9400A1C[skill->GetLevel()];
            static int array[10] = { 0 };
            return array[skill->GetLevel() - 1];
        }
        float GetRadius(Skill *skill) const
        {
            return 25.0;
        }
        float GetAttackdistance(Skill *skill) const
        {
            return 11.0;
        }
        float GetAngle(Skill *skill) const
        {
            return -0.99999797;
        }
        float GetPraydistance(Skill *skill) const
        {
            return 60.0;
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
            skill->GetVictim()->SetTime( 3600000.0);
            skill->GetVictim()->SetAmount( 2.0);
            skill->GetVictim()->SetImmunemagical2(1);
            skill->GetVictim()->SetProbability( 100.0);
            skill->GetVictim()->SetTime( 3600000.0);
            skill->GetVictim()->SetValue( 0.0);
            skill->GetVictim()->SetImmunephysical2(1);
            skill->GetVictim()->SetProbability( 100.0);
            skill->GetVictim()->SetTime( 3600000.0);
            skill->GetVictim()->SetRatio( 0.0099999998);
            skill->GetVictim()->SetAmount( 2.0);
            skill->GetVictim()->SetValue( 10.0);
            skill->GetVictim()->SetIncmagic3(1);
            skill->GetVictim()->SetProbability( 100.0);
            skill->GetVictim()->SetCleardebuff(1);
            return 1;
        }
        bool BlessMe(Skill *skill) const
        {
            skill->GetVictim()->SetTime( 3600000.0);
            skill->GetVictim()->SetRatio( 0.0);
            skill->GetVictim()->SetAmount( 44.0);
            skill->GetVictim()->SetValue( 2.0);
            skill->GetVictim()->SetInsertvstate(1);
            return 1;
        }
        bool TakeEffect(Skill *skill) const
        {
            return 1;
        }
        float GetEffectdistance(Skill *skill) const
        {
            return 100.0;
        }
        int GetAttackspeed(Skill *skill) const
        {
            return 0;
        }
        float GetHitrate(Skill *skill) const
        {
            return 1.0;
        }
        float GetTalent0(PlayerWrapper * player) const
        {
            return (float) player->GetMagicattack();
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
