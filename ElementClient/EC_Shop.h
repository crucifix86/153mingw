// Filename	: EC_Shop.h
// Creator	: Xu Wenbin
// Date		: 2013/12/11

#pragma once

#include "EC_Observer.h"
#include <map>
#include <vector.h>
#include <hashmap.h>

struct _GSHOP_ITEM;
typedef struct _GSHOP_ITEM GSHOP_ITEM;

class AWString;

//	��װ GSHOP_ITEM �ķ�����Ϣ
class CECShopItemCategory{
	int		m_mainType;
	int		m_subType;
public:
	CECShopItemCategory(int mainType=-1, int subType=-1);
	void InitFromQShopConfig(int idCategory);
	void SetCategory(int mainType, int subType);
	int  GetMainType()const;
	int  GetSubType()const;
	bool MatchMainType(int mainType)const;
	bool MatchItem(const GSHOP_ITEM &item)const;
};

//	��װ GSHOP_ITEM �Ĺҿ� NPC ��ѯ
class CECShopItemOwnerNPC{
	unsigned int m_ownerNPCID;
public:
	CECShopItemOwnerNPC(unsigned int ownerNPCID=0);
	void SetOwnerNPCID(unsigned int ownerNPCID);
	unsigned int GetOwnerNPCID()const;
	bool IsEmpty()const;
	bool operator == (const CECShopItemOwnerNPC &rhs)const;
	bool MatchID(unsigned int id)const;
	bool MatchItem(const GSHOP_ITEM &item)const;

	static bool HasOwnerNPC(const GSHOP_ITEM &item);
};

//	��װ CECShopBase ��ı仯
class CECShopBase;
class CECShopBaseChange : public CECObservableChange
{
public:
	enum ChangeMask{
		ITEM_CHANGED = 0x01,
	};
private:
	unsigned int m_changeMask;
public:
	CECShopBaseChange(unsigned int changeMask);

	unsigned int GetChangeMask()const;
	bool ItemChanged()const;
};

//	��װ gshop �� backshop �ȵ����ݷ���
extern const int CECSHOP_INVALID_PRICE;
class CECShopBase : public CECObservable<CECShopBase>
{
	CECShopItemOwnerNPC	m_ownerNPC;
public:
	enum ShoppingLimitTypes
	{
		CASH_VIP_SHOPPING_LIMIT_NONE = 0,
		CASH_VIP_SHOPPING_LIMIT_DAY,
		CASH_VIP_SHOPPING_LIMIT_WEEK,
		CASH_VIP_SHOPPING_LIMIT_MONTH,
		CASH_VIP_SHOPPING_LIMIT_YEAR,
		CASH_VIP_SHOPPING_LIMIT_COUNT,
	};
	std::map<int, std::map<int, int>> m_limitedItems; // <LimitType, <Item, Amount of Items Purchased in the period of time>>
	virtual ~CECShopBase() = 0;
	
	virtual bool GetFromServer(int beginIndex, int endIndex)=0;
	void OnItemChange();

	virtual unsigned int GetLocalTimeStamp()const=0;
	virtual unsigned int GetServerTimeStamp()const=0;
	bool ValidateTimeStamp()const;

	void SetOwnerNPCID(unsigned int ownerNPCID);
	unsigned int GetOwnerNPCID()const;
	bool HasOwnerNPC()const;
	const CECShopItemOwnerNPC & GetOwnerNPC()const;
	bool MatchOwnerNPC(const GSHOP_ITEM &rhs)const;

	virtual int GetCount()const=0;
	virtual const GSHOP_ITEM * GetItem(int index)const=0;
	virtual GSHOP_ITEM * GetItem(int index)=0;

	virtual int GetCash()const=0;
	virtual bool Buy(int itemIndex, int buyIndex)const=0;

	virtual bool IsFashionShopEnabled()const=0;
	virtual const CECShopItemCategory & GetFashionShopCategory()const=0;
	virtual bool IsFashionShopFlashSaleEnabled()const=0;
	virtual const AWString &GetFashionShopFlashSaleTitle()const=0;
	
	bool IsValid(int itemIndex, int buyIndex)const;
	bool ReadyToBuy(int itemIndex, int buyIndex)const;
	bool CalcBuyIndex(int itemIndex, int *pBuyIndex, int cash=-1)const;
	bool CalcBuyType(int itemIndex, int *buyTypes)const;
	bool HasSameBuyType(int itemIndexA, int itemIndexB)const;
	int  GetPrice(int itemIndex, int buyIndex)const;
	unsigned int  GetStatus(int itemIndex, int buyIndex)const;

	static bool IsSame(const GSHOP_ITEM &lhs, const GSHOP_ITEM &rhs);
	static int  GetOriginalPrice(int finalPrice, unsigned int discountStatus);
	bool IsLimitedPurchasesItem(int limitType, int itemId);
	int GetItemPurchasesAmount(int limitType, int itemId);
	void SetLimitedItem(int limitType, int itemId, int amountPurchased);
	void Clear();
};

class CECShopArrayItems : public CECShopBase
{
protected:
	typedef abase::vector<GSHOP_ITEM> GShopItems;
	typedef GShopItems * GShopItemsPtr;
	GShopItemsPtr	m_pItems;

	using CECShopBase::IsSame;
	static bool IsSame(const GShopItemsPtr lhs, const GShopItemsPtr rhs);

public:
	CECShopArrayItems(GShopItemsPtr pItems);
	virtual ~CECShopArrayItems() = 0;
	
	virtual int GetCount()const;
	virtual const GSHOP_ITEM * GetItem(int index)const;
	virtual GSHOP_ITEM * GetItem(int index);
};

namespace S2C{
	struct cmd_mall_item_price;
}
class CECQShop : public CECShopArrayItems
{
	CECShopItemCategory	m_fashionShopCategory;

	CECQShop();

	CECQShop(const CECQShop &);
	CECQShop & operator=(const CECQShop &);

public:
	virtual bool GetFromServer(int beginIndex, int endIndex);
	virtual unsigned int GetLocalTimeStamp()const;
	virtual unsigned int GetServerTimeStamp()const;
	bool UpdateFromServer(const S2C::cmd_mall_item_price *pCmd);
	virtual int GetCash()const;
	virtual bool Buy(int itemIndex, int buyIndex)const;
	virtual bool IsFashionShopEnabled()const;
	virtual bool IsFashionShopFlashSaleEnabled()const;
	virtual const AWString &GetFashionShopFlashSaleTitle()const;
	virtual const CECShopItemCategory & GetFashionShopCategory()const;
	static CECQShop & Instance();

private:
	bool ApplyChangesFromServer(GShopItemsPtr pItems, const S2C::cmd_mall_item_price *pCmd);
};

namespace S2C{
	struct cmd_dividend_mall_item_price;
}
class CECBackShop : public CECShopArrayItems
{
	CECShopItemCategory	m_fashionShopCategory;

	CECBackShop();
	
	CECBackShop(const CECBackShop &);
	CECBackShop & operator=(const CECBackShop &);
	
public:
	virtual bool GetFromServer(int beginIndex, int endIndex);
	virtual unsigned int GetLocalTimeStamp()const;
	virtual unsigned int GetServerTimeStamp()const;
	bool UpdateFromServer(const S2C::cmd_dividend_mall_item_price *);
	virtual int GetCash()const;
	virtual bool Buy(int itemIndex, int buyIndex)const;
	virtual bool IsFashionShopEnabled()const;	
	virtual bool IsFashionShopFlashSaleEnabled()const;
	virtual const AWString &GetFashionShopFlashSaleTitle()const;
	virtual const CECShopItemCategory & GetFashionShopCategory()const;
	static CECBackShop & Instance();
	
private:
	bool ApplyChangesFromServer(GShopItemsPtr pItems, const S2C::cmd_dividend_mall_item_price *pCmd);
};
namespace S2C {
	struct cmd_cash_vip_mall_item_price;
}
class CECVipShop : public CECShopArrayItems
{
	CECShopItemCategory	m_fashionShopCategory;
	CECVipShop();

	CECVipShop(const CECVipShop&);
	CECVipShop& operator=(const CECVipShop&);

public:
	virtual bool GetFromServer(int beginIndex, int endIndex);
	virtual unsigned int GetLocalTimeStamp()const;
	virtual unsigned int GetServerTimeStamp()const;
	bool UpdateFromServer(const S2C::cmd_cash_vip_mall_item_price*);
	virtual int GetCash()const;
	virtual bool Buy(int itemIndex, int buyIndex)const;
	virtual bool IsFashionShopEnabled()const;
	virtual bool IsFashionShopFlashSaleEnabled()const;
	virtual const AWString& GetFashionShopFlashSaleTitle()const;
	virtual const CECShopItemCategory& GetFashionShopCategory()const;
	static CECVipShop& Instance();

private:
	bool ApplyChangesFromServer(GShopItemsPtr pItems, const S2C::cmd_cash_vip_mall_item_price* pCmd);
};