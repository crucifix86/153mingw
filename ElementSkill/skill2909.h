#ifndef __CPPGEN_GNET_SKILL2909
#define __CPPGEN_GNET_SKILL2909
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2909:public Skill
    {
      public:
        enum
        { SKILL_ID = 2909 };
          Skill2909 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2909Stub:public SkillStub
    {
      public:
	  #ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 950;

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
                skill->GetPlayer()->SetDecmp( 44.0);
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
                return 735;

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
                skill->GetPlayer()->SetDecmp( 177.0);
                skill->SetPlus(5628.0);
                skill->SetRatio(1.67);
                skill->SetGolddamage((int)((float)skill->GetMagicattack() * 0.6));
                skill->GetPlayer()->SetPerform(1);

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
	  
	  

      Skill2909Stub ():SkillStub (2909)
        {
            cls = 11;
            name = L"Ðþ¡¤ËªÀ×Êõ";
            nativename = "Ðþ¡¤ËªÀ×Êõ";
            icon = "ÏÉËªÀ×Êõ.dds";
            max_level = 1;
            type = 1;
            apcost = 0;
            arrowcost = 0;
            apgain = 10;
            attr = 2;
            rank = 20;
            eventflag = 0;
            is_senior = 1;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1012;
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
            effect = "ÔÂÏÉ_ËªÀ×Êõ_»÷ÖÐ.sgc";
            range.type = 0;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(2805, 1));
            is_movingcast = 1;
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
	
        }
        virtual ~ Skill2909Stub ()
        {
        }
        float GetMpcost(Skill *skill) const
        {
            return 221.0;
        }
        int GetExecutetime(Skill *skill) const
        {
            return 735;
        }
        int GetCoolingtime(Skill *skill) const
        {
            return ( skill->GetPlayer()->GetForm() == FORM_CLASS ) ? 1000 : 3000;
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
            static int array[10] = {47439};
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
            
            return (float)(skill->GetPlayer()->GetPrayrangeplus() + 27.0);
        }
#ifdef _SKILL_SERVER
        int GetEnmity(Skill *skill) const
        {
            return 0;
        }
        bool StateAttack(Skill *skill) const
        {
            skill->GetVictim()->SetProbability( 100.0);
            skill->GetVictim()->SetTime( 18000.0);
            skill->GetVictim()->SetRatio( 2.0);
            skill->GetVictim()->SetAmount(((float)skill->GetT0() * 0.6) + (skill->GetT1() == FORM::FORM_CLASS ? 0.1 : 0.0) * (float)skill->GetT0());
            skill->GetVictim()->SetValue(((float)skill->GetT0() * 0.6) + (skill->GetT1() == FORM::FORM_CLASS ? 0.1 : 0.0) * (float)skill->GetT0());
            skill->GetVictim()->SetAddfrosteffect(1);
            return 1;
        }
        bool TakeEffect(Skill *skill) const
        {
            return 1;
        }
        float GetEffectdistance(Skill *skill) const
        {
            
            return (float)(skill->GetPlayer()->GetPrayrangeplus() + 36.5);
        }
        float GetHitrate(Skill *skill) const
        {
            return 1.0;
        }
        float GetTalent0(PlayerWrapper * player) const
        {
            return (float) player->GetMagicattack();
        }
        float GetTalent1(PlayerWrapper * player) const
        {
            return (float) player->GetForm();
        }
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
