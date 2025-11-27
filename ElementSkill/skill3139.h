#ifndef __CPPGEN_GNET_SKILL3139
#define __CPPGEN_GNET_SKILL3139
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill3139:public Skill
    {
      public:
        enum
        { SKILL_ID = 3139 };
          Skill3139 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill3139Stub:public SkillStub
    {
      public:
	  #ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 3000;

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
                return 1000;

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
                skill->SetPlus(8776.0);
                skill->SetRatio(2.5);
                skill->SetDamage(skill->GetMagicattack());
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
	  
	  

      Skill3139Stub ():SkillStub (3139)
        {
            cls = 127;
            name = L"ÖÕÓüÁúÑÀ";
            nativename = "ÖÕÓüÁúÑÀ";
            icon = "";
            max_level = 10;
            type = 1;
            apcost = 0;
            arrowcost = 0;
            apgain = 0;
            attr = 7;
            rank = 0;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1425;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 0;
            effect = "Ò¹Ó°_ÓüÁúÑÀ_»÷ÖÐ.sgc";
            range.type = 3;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
	
        }
        virtual ~ Skill3139Stub ()
        {
        }
        float GetMpcost(Skill *skill) const
        {
            return 0.0;
        }
        int GetExecutetime(Skill *skill) const
        {
            return 1000;
        }
        int GetCoolingtime(Skill *skill) const
        {
            return 0;
        }
        int GetRequiredLevel(Skill *skill) const
        {
            ///TODO fix
            // return dword_94045DC[skill->GetLevel()];
            static int array[10] = { 0 };
            return array[skill->GetLevel() - 1];
        }
        int GetRequiredSp(Skill *skill) const
        {
            ///TODO fix
            // return dword_940461C[skill->GetLevel()];
            static int array[10] = { 0 };
            return array[skill->GetLevel() - 1];
        }
        float GetRadius(Skill *skill) const
        {
            return 5.0;
        }
        float GetAttackdistance(Skill *skill) const
        {
            return 0.0;
        }
        float GetAngle(Skill *skill) const
        {
            return (float)(1.0 - ((float)skill->GetLevel() * 2.5 + 60.0) * 0.0111111);
        }
        float GetPraydistance(Skill *skill) const
        {
            return 8.0;
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
            skill->GetVictim()->SetTime( 5000.0);
            //skill->GetVictim()->SetValue( 2.0);
            skill->GetVictim()->SetPalsy(1);
            return 1;
        }
        bool TakeEffect(Skill *skill) const
        {
            return 1;
        }
        float GetEffectdistance(Skill *skill) const
        {
            return 18.0;
        }
        int GetAttackspeed(Skill *skill) const
        {
            return 8;
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
