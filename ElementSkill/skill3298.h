#ifndef __CPPGEN_GNET_SKILL3298
#define __CPPGEN_GNET_SKILL3298
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill3298:public Skill
    {
      public:
        enum
        { SKILL_ID = 3298 };
          Skill3298 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill3298Stub:public SkillStub
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
	  
	  

      Skill3298Stub ():SkillStub (3298)
        {
            cls = 255;
            name = L"√‘µ˚√Œﬁí";
            nativename = "√‘µ˚√Œﬁí";
            icon = "√‘µ˚√Œﬁí.dds";
            max_level = 1;
            type = 2;
            apcost = 0;
            arrowcost = 0;
            apgain = 0;
            attr = 0;
            rank = 0;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 1;
            showorder = 7003;
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
                restrict_weapons.push_back (1);
            }
            {
                restrict_weapons.push_back (182);
            }
            {
                restrict_weapons.push_back (5);
            }
            {
                restrict_weapons.push_back (292);
            }
            {
                restrict_weapons.push_back (9);
            }
            {
                restrict_weapons.push_back (13);
            }
            {
                restrict_weapons.push_back (291);
            }
            {
                restrict_weapons.push_back (23749);
            }
            {
                restrict_weapons.push_back (25333);
            }
            {
                restrict_weapons.push_back (44878);
            }
            {
                restrict_weapons.push_back (44879);
            }
            effect = "–Ó∆¯.sgc";
            range.type = 5;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
#endif
	
        }
        virtual ~ Skill3298Stub ()
        {
        }
        float GetMpcost(Skill *skill) const
        {
            return 0.0;
        }
        int GetExecutetime(Skill *skill) const
        {
            return 0;
        }
        int GetCoolingtime(Skill *skill) const
        {
            return 30000;
        }
        int GetRequiredLevel(Skill *skill) const
        {
            ///TODO fix
            // return dword_940C61C[skill->GetLevel()];
            static int array[10] = { 0 };
            return array[skill->GetLevel() - 1];
        }
        int GetRequiredSp(Skill *skill) const
        {
            ///TODO fix
            // return dword_940C65C[skill->GetLevel()];
            static int array[10] = { 0 };
            return array[skill->GetLevel() - 1];
        }
        int GetRequiredMoney(Skill *skill) const
        {
            ///TODO fix
            // return dword_940C69C[skill->GetLevel()];
            static int array[10] = { 0 };
            return array[skill->GetLevel() - 1];
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
            return 0.0;
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
            skill->GetVictim()->SetTime((float)(1000 * skill->GetT1()));
            skill->GetVictim()->SetRatio( 1.0);
            skill->GetVictim()->SetAmount(( skill->GetT0() > 199 ) ? (float)(600.0 * (float)(0.1 * (float)skill->GetT0() / (float)(skill->GetT0() + 60000))) + 120.0 : (float)(1800.0 * (float)(0.1 * (float)skill->GetT0() / (float)(skill->GetT0() + 200))) + 30.0);
            skill->GetVictim()->SetIncvigour(1);
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
        float GetHitrate(Skill *skill) const
        {
            return 1.0;
        }
        float GetTalent0(PlayerWrapper * player) const
        {
            return (float)(int) player->GetDynskillcounter();
        }
        float GetTalent1(PlayerWrapper * player) const
        {
            return 3600.0;
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
