// Filename	: WikiEquipmentProp.cpp
// Creator	: Feng Ning
// Date		: 2010/04/27

#include "WikiEquipmentProp.h"

#define new A_DEBUG_NEW



template<>
int EXTRACTOR_TYPE(FASHION)::GetGender() const {return m_pE->gender;};
template<>
unsigned int EXTRACTOR_TYPE(FASHION)::GetClass() const {return 0;};
template<>
int EXTRACTOR_TYPE(FASHION)::GetLevelupAddon() const {return 0;};
template<>
unsigned int EXTRACTOR_TYPE(FASHION)::GetSplit() const {return 0;};
template<>
int EXTRACTOR_TYPE(FASHION)::GetSplitNumber() const {return 0;};
template<>
unsigned int EXTRACTOR_TYPE(FASHION)::GetDamaged() const {return 0;};
template<>
int EXTRACTOR_TYPE(FASHION)::GetDamagedNumber() const {return 0;};
template<>
unsigned int EXTRACTOR_TYPE(FASHION)::GetRandPropID(unsigned int i) const { return 0;};
template<>
unsigned int EXTRACTOR_TYPE(FASHION)::GetRandPropSize() const {return 0;};
template<>
ACString EXTRACTOR_TYPE(FASHION)::GetRandPropName(unsigned int i) const {return _AL("");};

template<>
unsigned int EXTRACTOR_TYPE(WEAPON)::GetRandPropID(unsigned int i) const
{
	size_t uniqueSize = (sizeof(m_pE->uniques)/sizeof(m_pE->uniques[0]));
	
	unsigned int rand_id = 0;
	
	if(i < uniqueSize)
	{
		rand_id = m_pE->uniques[i].id_unique;
		
		if(rand_id != 0)
		{
			for(unsigned int j=0;j<i;j++)
			{
				if(rand_id == m_pE->uniques[j].id_unique)
				{
					// omit duplicated id
					rand_id = 0;
					break;
				}
			}
		}
	}
	else if(i < GetRandPropSize())
	{
		i -= uniqueSize;
		rand_id = m_pE->addons[i].id_addon;

		if(rand_id != 0)
		{
			for(unsigned int j=0;j<i;j++)
			{
				if(rand_id == m_pE->addons[j].id_addon)
				{
					// omit duplicated id
					rand_id = 0;
					break;
				}
			}
		}
	}

	return rand_id;
}


template<>
ACString EXTRACTOR_TYPE(WEAPON)::GetRandPropName(unsigned int i) const
{
	size_t uniqueSize = (sizeof(m_pE->uniques)/sizeof(m_pE->uniques[0]));
	
	CECGameUIMan* pUIMan = dynamic_cast<CECGameUIMan*>(g_pGame->GetGameRun()->GetUIManager()->GetCurrentUIManPtr());
	
	if(i < uniqueSize)
	{
		return pUIMan->GetStringFromTable(8660);
	}
	else if(i < GetRandPropSize())
	{
		return pUIMan->GetStringFromTable(8661);
	}
	
	return _AL("");
};

WikiEquipmentProp* WikiEquipmentProp::CreateProperty(unsigned int id)
{
	DATA_TYPE dataType = DT_INVALID;
	const void* pEP= g_pGame->GetElementDataMan()->get_data_ptr(id, ID_SPACE_ESSENCE, dataType);
	
	if(DT_WEAPON_ESSENCE == dataType)
	{
		return new EXTRACTOR_TYPE(WEAPON)(pEP);
	}
	else if(DT_ARMOR_ESSENCE == dataType)
	{
		return new EXTRACTOR_TYPE(ARMOR)(pEP);
	}
	else if(DT_DECORATION_ESSENCE == dataType)
	{
		return new EXTRACTOR_TYPE(DECORATION)(pEP);
	}
	else if(DT_FASHION_ESSENCE == dataType)
	{
		return new EXTRACTOR_TYPE(FASHION)(pEP);
	}
	
	return NULL;
}