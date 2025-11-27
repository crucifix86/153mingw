// Filename	: WikiEquipmentProp.h
// Creator	: Feng Ning
// Date		: 2010/04/26

#include "AAssist.h"
#include "EC_Game.h"
#include "EC_Global.h"
#include "EC_GameRun.h"
#include "elementdataman.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_IvtrItem.h"
#include "AFI.h"

#define EXTRACTOR_TYPE(TYPE) EssenceExtractor<TYPE##_ESSENCE, TYPE##_SUB_TYPE>

// use this interface to get all necessary properties
class WikiEquipmentProp
{
public:
	virtual ACString GetName() const = 0;
	virtual ACString GetTypeName() const = 0;
	virtual ACString GetDesc() const = 0;
	virtual AString GetIconFile() const = 0;

	virtual unsigned int GetID() const =0;
	virtual int GetGender() const =0;
	virtual unsigned int GetClass() const =0;
	virtual int GetLevel() const =0;
	virtual int GetLevelRequire() const =0;
	virtual int GetLevelupAddon() const =0;
	
	virtual unsigned int GetSplit() const =0;
	virtual int GetSplitNumber() const =0;
	
	virtual unsigned int GetDamaged() const =0;
	virtual int GetDamagedNumber() const =0;
	
	virtual unsigned int GetRandPropID(unsigned int i) const = 0;
	virtual unsigned int GetRandPropSize() const = 0;
	
	virtual ACString GetRandPropName(unsigned int i) const = 0;
	
	virtual ~WikiEquipmentProp() {};
	
	static WikiEquipmentProp* CreateProperty(unsigned int id);
	
protected:
	WikiEquipmentProp(){};
	WikiEquipmentProp(const WikiEquipmentProp&);
	WikiEquipmentProp& operator=(const WikiEquipmentProp&);
};

// =======================================================================
// Data Extractor
// =======================================================================
template <typename Essence, typename SubType>
class EssenceExtractor : public WikiEquipmentProp
{
	friend WikiEquipmentProp;

private:
	const Essence* m_pE;
	mutable CECIvtrItem* m_pItem;

	EssenceExtractor(const void* pE) :m_pE((const Essence*)pE), m_pItem(NULL)
	{}

public:
	~EssenceExtractor()
	{
		if (m_pItem)
		{
			delete m_pItem;
		}
	}

	virtual ACString GetName() const { return m_pE->name; };

	virtual ACString GetTypeName() const
	{
		DATA_TYPE dataType;
		const SubType* pType =
			(const SubType*)g_pGame->GetElementDataMan()->get_data_ptr(m_pE->id_sub_type, ID_SPACE_ESSENCE, dataType);
		return pType->name;
	}

	virtual ACString GetDesc() const
	{
		// lazy initializaton
		if (!m_pItem)
		{
			m_pItem = CECIvtrItem::CreateItem(GetID(), 0, 1);
			m_pItem->GetDetailDataFromLocal();
		}

		return m_pItem->GetDesc();
	}

	virtual AString GetIconFile() const
	{
		// lazy initializaton
		if (!m_pItem)
		{
			m_pItem = CECIvtrItem::CreateItem(GetID(), 0, 1);
			m_pItem->GetDetailDataFromLocal();
		}

		AString szFile;
		af_GetFileTitle(m_pItem->GetIconFile(), szFile);
		szFile.MakeLower();
		return szFile;
	}

	virtual unsigned int GetID() const { return m_pE->id; };
	virtual int GetGender() const { return -1; };
	virtual unsigned int GetClass() const { return m_pE->character_combo_id; };
	virtual int GetLevel() const { return m_pE->level; };
	virtual int GetLevelRequire() const { return m_pE->require_level; };
	virtual int GetLevelupAddon() const { return m_pE->levelup_addon; };

	virtual unsigned int GetSplit() const { return m_pE->element_id; };
	virtual int GetSplitNumber() const { return m_pE->element_num; };

	virtual unsigned int GetDamaged() const { return m_pE->id_drop_after_damaged; };
	virtual int GetDamagedNumber() const { return m_pE->num_drop_after_damaged; };

	virtual unsigned int GetRandPropID(unsigned int i) const
	{
		unsigned int rand_id = 0;

		if (i < GetRandPropSize())
		{
			rand_id = m_pE->addons[i].id_addon;
		}

		if (rand_id != 0)
		{
			for (unsigned int j = 0; j < i; j++)
			{
				if (rand_id == m_pE->addons[j].id_addon)
				{
					// omit duplicated id
					rand_id = 0;
					break;
				}
			}
		}

		return rand_id;
	};

	virtual unsigned int GetRandPropSize() const { return sizeof(m_pE->addons) / sizeof(m_pE->addons[0]); };
	virtual ACString GetRandPropName(unsigned int i) const {
		return i >= GetRandPropSize() ?
			_AL("") : g_pGame->GetGameRun()->GetUIManager()->GetCurrentUIManPtr()->GetStringFromTable(8661);
	};

};
template<>
inline unsigned int EXTRACTOR_TYPE(WEAPON)::GetRandPropSize() const
{
	// both unique and addon property
	return (sizeof(m_pE->uniques) / sizeof(m_pE->uniques[0]))
		+ (sizeof(m_pE->addons) / sizeof(m_pE->addons[0]));
}
