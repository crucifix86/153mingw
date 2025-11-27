// Filename	: DlgChangeEqp.cpp
// Creator	: Adriano Lopes
// Date		: 2021/06/29

#include "AFI.h"
#include "AUIImagePicture.h"
#include "AUIDef.h"
#include "AUICTranslate.h"
#include "DlgChangeEqp.h"
#include "EC_HostPlayer.h"
#include "EC_GameUIMan.h"
#include "EC_IvtrEquip.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgChangeEqp, CDlgBase)

AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgChangeEqp, CDlgBase)

AUI_ON_EVENT("*", WM_MOUSEWHEEL, OnEventMouseWheel)
AUI_ON_EVENT(NULL, WM_MOUSEWHEEL, OnEventMouseWheel)
AUI_ON_EVENT("Item_*", WM_LBUTTONDOWN, OnEventLButtonDown)
AUI_ON_EVENT("Img_No*", WM_LBUTTONDOWN, OnEventLButtonDown)
AUI_ON_EVENT("Item_*", WM_RBUTTONDOWN, OnEventRButtonDown)

AUI_END_EVENT_MAP()

CDlgChangeEqp::CDlgChangeEqp()
{
}

CDlgChangeEqp::~CDlgChangeEqp()
{
}

bool CDlgChangeEqp::OnInitDialog()
{
	m_pScroll_Suite = (PAUISCROLL)GetDlgItem("Scl_Right");

	for (int i = 0; i < SuiteSizeHeight; ++i)
	{
		AString str;
		str.Format("Img_No0%d", i + 1);
		m_pImage_Suite[i] = (PAUIIMAGEPICTURE)GetDlgItem(str);
	}

	return true;
}

void CDlgChangeEqp::OnCommand_CANCEL(const char * szCommand)
{
	Show(false);
}

void CDlgChangeEqp::OnEventMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	// Scroll fashion short cut display
//
	if (m_pScroll_Suite->IsShow())
	{
		int zDelta = (short)HIWORD(wParam);
		zDelta /= 120;
		m_pScroll_Suite->SetBarLevel(m_pScroll_Suite->GetBarLevel() - zDelta);
	}
}

void CDlgChangeEqp::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	// Drag move or discard given fashion short cut
	//
	if (strstr(pObj->GetName(), "Item_") &&
		IsEquipShortCutChangeable() &&
		pObj->GetDataPtr() ||
		strstr(pObj->GetName(), "Img_No"))
	{
		A3DVIEWPORTPARAM* p = m_pA3DEngine->GetActiveViewport()->GetParam();
		CECGameUIMan* pGameUIMan = GetGameUIMan();
		pGameUIMan->m_ptLButtonDown.x = GET_X_LPARAM(lParam) - p->X;
		pGameUIMan->m_ptLButtonDown.y = GET_Y_LPARAM(lParam) - p->Y;
		pGameUIMan->InvokeDragDrop(this, pObj, pGameUIMan->m_ptLButtonDown);
	}
}

void CDlgChangeEqp::OnEventRButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	// Discard fashion short cut with right button
	//
	if (!strstr(pObj->GetName(), "Item_") ||
		!IsEquipShortCutChangeable() ||
		!pObj->GetDataPtr())
		return;

	Discard(pObj);
}

bool CDlgChangeEqp::Tick()
{
	if (IsShow())
	{
		CECHostPlayer* pHost = GetHostPlayer();
		CECGameUIMan* pGameUIMan = GetGameUIMan();

		// Update suite and according items
		//


		// Update scroll size
		int num = 16;
		if (num <= SuiteSizeHeight)
		{
			m_pScroll_Suite->SetScrollRange(0, 0);
			m_pScroll_Suite->Show(false);
		}
		else
		{
			m_pScroll_Suite->SetScrollRange(0, num - SuiteSizeHeight);
			m_pScroll_Suite->SetScrollStep(1);
			m_pScroll_Suite->SetBarLength(-1);
			m_pScroll_Suite->Show(true);
		}

		// Update suite appearance
		int pos = m_pScroll_Suite->GetBarLevel();
		int height = min((int)SuiteSizeHeight, 16);
		for (int i = 0; i < SuiteSizeHeight; ++i)
		{
			// Update suite cover
			if (i < height)
			{
				int iconSet = CECGameUIMan::ICONS_CHANGEEQP;

				A2DSprite* cover = pGameUIMan->m_pA2DSpriteIcons[iconSet];

				AString str = GetIconFile(pos + i);
				AString iconFile;
				af_GetFileTitle(str, iconFile);
				int index = pGameUIMan->m_IconMap[iconSet][iconFile];
				m_pImage_Suite[i]->SetCover(cover, index);
				m_pImage_Suite[i]->SetDataPtr((void*)(pos + i + 1)); // STORE suite offset in order to be used by short cut
			}

			// Update suite item cover
			int equipOffset = (pos + i) * SuiteSizeWidth;
			int itemOffset = i * SuiteSizeWidth;
			for (int j = 0; j < SuiteSizeWidth; ++j, ++equipOffset, ++itemOffset)
			{
				// Get item name, like Item_002
				AString itemName;
				itemName.Format("Item_%d%d%d", itemOffset / 100, (itemOffset % 100) / 10, itemOffset % 10);
				PAUIIMAGEPICTURE pItem = (PAUIIMAGEPICTURE)GetDlgItem(itemName);

				if (i < height)
				{
					// Get new fashion associated with the item
					int id = pHost->GetEquipSCSets(equipOffset);
					int storage2Index(-1);
					CECIvtrEquip* pEquip = pHost->GetEquipByID(id, storage2Index);

					//Change item cover
					SetEquipShortCut(pItem, pEquip);
				}
				else if (pItem)	pItem->Show(false);
			}
		}
	}


	return CDlgBase::Tick();
}

bool CDlgChangeEqp::IsEquipShortCutChangeable()
{
	return true;
}

bool CDlgChangeEqp::IsEquipShortCutExchangeable(PAUIOBJECT pItem1, PAUIOBJECT pItem2)
{
	// Check if given items can store same kind of fashion short cut
//
	int offset1 = GetEquipShortCutOffset(pItem1);
	int offset2 = GetEquipShortCutOffset(pItem2);
	return offset1 >= 0
		&& offset2 >= 0
		&& (offset1 % SuiteSizeWidth) == (offset2 % SuiteSizeWidth);
}

void CDlgChangeEqp::Store(PAUIOBJECT pItem, CECIvtrEquip* pEquip)
{
	// Store fashion short cut set
	//
	if (!IsEquipShortCutChangeable())
		return;

	CECHostPlayer* pHost = GetHostPlayer();

	// Validate match if not empty
	if (pEquip)
	{
		//if (!pHost->CanUseEquipment(pEquip))
		//{
		//	GetGameUIMan()->AddChatMessage(GetStringFromTable(7831), GP_CHAT_MISC);
		//	return;
		//}

		if (!IsEquipMatch(pItem, pEquip))
		{
			GetGameUIMan()->AddChatMessage(GetStringFromTable(20002), GP_CHAT_MISC);
			return;
		}
	}

	// Store Equip short cut
	SetEquipShortCut(pItem, pEquip);
}

void CDlgChangeEqp::Replace(PAUIOBJECT pSrcItem, PAUIOBJECT pDstItem)
{
	// Remove source short cut from one place to another
	//

	if (!IsEquipShortCutChangeable() ||
		pSrcItem == pDstItem)
		return;

	if (!IsEquipShortCutExchangeable(pSrcItem, pDstItem))
	{
		GetGameUIMan()->AddChatMessage(GetStringFromTable(20002), GP_CHAT_MISC);
		return;
	}

	CECIvtrEquip* equip1 = GetEquip(pSrcItem);
	CECIvtrEquip* equip2 = GetEquip(pDstItem);
	Store(pSrcItem, equip2);
	Store(pDstItem, equip1);
}

void CDlgChangeEqp::Discard(PAUIOBJECT pItem)
{
	// Discard fashion short cut
	//
	if (!IsEquipShortCutChangeable())
		return;

	SetEquipShortCut(pItem, NULL);
}

int CDlgChangeEqp::GetEquipShortCutOffset(PAUIOBJECT pItem)
{
	// Get corresponding fashion short cut offset with current item
//
	int offset(-1);

	while (true)
	{
		if (!IsShow())
			break;

		if (!pItem ||
			!strstr(pItem->GetName(), "Item_"))
			break;

		int temp = atoi(pItem->GetName() + strlen("Item_"));
		if (temp < 0 ||
			temp >= SuiteSizeShown)
			break;

		// Take scroll bar as offset
		offset = m_pScroll_Suite->IsShow()
			? m_pScroll_Suite->GetBarLevel() * SuiteSizeWidth
			: 0;

		offset += temp;
		break;
	}

	return offset;
}

void CDlgChangeEqp::SetEquipShortCut(PAUIOBJECT pItem, CECIvtrEquip*pEquip)
{
	// Save fashion short cut to given item and Host player
	//
	int equipOffset = GetEquipShortCutOffset(pItem);
	if (equipOffset < 0)
		return;

	CECHostPlayer* pHost = GetHostPlayer();
	pHost->SetEquipSCSets(equipOffset, pEquip);
	int id = pHost->GetEquipSCSets(equipOffset);

	// Set item cover, color, data, hint, etc.

	if (!pEquip)
	{
		// Clear all
		((PAUIIMAGEPICTURE)pItem)->SetCover(NULL, -1);
		pItem->SetColor(A3DCOLORRGB(255, 255, 255));
		pItem->SetText(_AL(""));
		pItem->SetDataPtr(NULL);
		pItem->SetHint(_AL(""));
		return;
	}

	CECGameUIMan* pGameUIMan = GetGameUIMan();
	AString iconFile;
	af_GetFileTitle(pEquip->GetIconFile(), iconFile);
	iconFile.MakeLower();
	((PAUIIMAGEPICTURE)pItem)->SetCover(
		pGameUIMan->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		pGameUIMan->m_IconMap[CECGameUIMan::ICONS_INVENTORY][iconFile]
	);

	pItem->SetColor(A3DCOLORRGB(255, 255, 255));

	pItem->SetDataPtr((void*)id);  // STORE color and template id for later use

	const wchar_t* pszHint = pEquip->GetDesc(
		CECIvtrItem::DESC_NORMAL,
		pHost->GetEquipment()
	);
	if (pszHint)
	{
		AUICTranslate trans;
		AWString strHint = trans.Translate(pszHint);
		pItem->SetHint(strHint);
	}
	else
	{
		pItem->SetHint(_AL(""));
	}
}

CECIvtrEquip* CDlgChangeEqp::GetEquip(PAUIOBJECT pItem)
{
	// Get fashion associated with given item
//
	DWORD id = (DWORD)pItem->GetDataPtr();
	int index(-1);
	return GetHostPlayer()->GetEquipByID(id, index);
}

bool CDlgChangeEqp::IsEquipMatch(PAUIOBJECT pItem, CECIvtrEquip*pEquip)
{
	static __int64 s_EquipMask[SuiteSizeWidth] =
	{
			EQUIP_MASK64_HEAD, // elmo
			EQUIP_MASK64_BODY, // peitoral
			EQUIP_MASK64_LEG, // calcas
			EQUIP_MASK64_FOOT, // botas
			EQUIP_MASK64_WRIST, // manopla 
			EQUIP_MASK64_WEAPON, // arma
			EQUIP_MASK64_NECK, // colar
			EQUIP_MASK64_WAIST, // ornamento
			EQUIP_MASK64_SHOULDER, // manta
			EQUIP_MASK64_FINGER1, // anel 1
			EQUIP_MASK64_FINGER2, // anel 2
			EQUIP_MASK64_BIBLE, // Livro
			EQUIP_MASK64_POCKET, // Bolsa tatica ?
			EQUIP_MASK64_PROJECTILE // Projetil
	};

		if(pEquip){
			for (int i(0); i < SuiteSizeWidth; ++i)
			{
				if (pEquip->GetEquipMask() & s_EquipMask[i])
				{
					return true;
				}
			}
		}

		return false;


	// Check if the type of the fashion matches given item
	//
	//if (pItem && pEquip)
	//{
	//	int EquipOffset = GetEquipShortCutOffset(pItem);
	//	if (EquipOffset >= 0)
	//	{
	//		int itemType = EquipOffset % SuiteSizeWidth;

	//		int EquipType = -1;
	//		__int64 EquipMask = pEquip->GetEquipMask();

	//		static __int64 s_EquipMask[SuiteSizeWidth] =
	//		{
	//				EQUIP_MASK64_HEAD, // elmo
	//				EQUIP_MASK64_BODY, // peitoral
	//				EQUIP_MASK64_LEG, // calcas
	//				EQUIP_MASK64_FOOT, // botas
	//				EQUIP_MASK64_WRIST, // manopla 
	//				EQUIP_MASK64_WEAPON, // arma
	//				EQUIP_MASK64_NECK, // colar
	//				EQUIP_MASK64_WAIST, // ornamento
	//				EQUIP_MASK64_SHOULDER, // manta
	//				EQUIP_MASK64_FINGER1, // anel 1
	//				EQUIP_MASK64_FINGER2, // anel 2
	//				EQUIP_MASK64_BIBLE, // Livro
	//				EQUIP_MASK64_POCKET, // Bolsa tatica ?
	//				EQUIP_MASK64_PROJECTILE // Projetil
	//		};
	//		for (int i(0); i < SuiteSizeWidth; ++i)
	//		{
	//			if (EquipMask & s_EquipMask[i])
	//			{
	//				EquipType = i;
	//				break;
	//			}
	//		}
	//		if ((itemType == 9 || itemType == 10) && EquipMask & EQUIP_MASK64_FINGER1)
	//			return true;
	//		return itemType == EquipType;
	//	}
	//}
	//return false;
}

AString CDlgChangeEqp::GetIconFile(int iSuite)
{
	// Return suite icon file name that can be used in CECGameUIMan to give
	// AUIImagePicture appearance in Win_ChangeSkirt��Quickbar fashion short cut
	//
	// iSuite starts from 0
	ASSERT(iSuite >= 0 && iSuite < 16);
	AString iconFile;
	iconFile.Format("ac%d.dds", iSuite + 1);
	return iconFile;
}

