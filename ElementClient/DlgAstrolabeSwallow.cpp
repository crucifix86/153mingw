#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_Configs.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_UIConfigs.h"
#include "DlgAstrolabeSwallow.h"
#include "EC_FixedMsg.h"
#include "elementdataman.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgAstrolabeSwallow, CDlgBase)
	AUI_ON_COMMAND("Btn_ChooseNew", OnCommandChooseNew)
	AUI_ON_COMMAND("Btn_ChooseCurrent", OnCommandChooseOld)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAstrolabeSwallow, CDlgBase)
AUI_END_EVENT_MAP()


CDlgAstrolabeSwallow::CDlgAstrolabeSwallow()
{
}

CDlgAstrolabeSwallow::~CDlgAstrolabeSwallow()
{

}

void CDlgAstrolabeSwallow::OnCommandChooseNew(const char* szCommand) 
{
	if (m_pSwallow->SumAptit() < m_pCurrent->SumAptit()) {
		PAUIDIALOG MsgBox;
		GetGameUIMan()->MessageBox("Game_Swallow_DiscardHigher", GetStringFromTable(11531), MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &MsgBox);
		MsgBox->SetDataPtr(m_pSwallow, "KeepItem");
	}
	else {
		// good to send cmd now
		SendSwallowCmd(m_pSwallow);
		Show(false);
	}
}

void CDlgAstrolabeSwallow::OnCommandChooseOld(const char* szCommand)
{
	if (m_pSwallow->SumAptit() > m_pCurrent->SumAptit()) {
		PAUIDIALOG MsgBox;
		GetGameUIMan()->MessageBox("Game_Swallow_DiscardHigher", GetStringFromTable(11531), MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &MsgBox);
		MsgBox->SetDataPtr(m_pCurrent, "KeepItem");
	}
	else {
		// good to send cmd now
		SendSwallowCmd(m_pCurrent);
		Show(false);
	}
}

void CDlgAstrolabeSwallow::SendSwallowCmd(CECIvtrItem* pKeepItem) {

	if (pKeepItem == m_pCurrent) {
		GetGameSession()->c2s_CmdAstrolabeSwallow(NO_INHERIT, m_iSlot, m_pCurrent->GetTemplateID());
	}
	else {
		GetGameSession()->c2s_CmdAstrolabeSwallow(INHERIT, m_iSlot, m_pCurrent->GetTemplateID());
	}
}

bool CDlgAstrolabeSwallow::OnInitDialog()
{
	// Button faces (stars)
	char* strOldBtnName[] = { "Btn_OldPoint00", "Btn_OldPoint01", "Btn_OldPoint02", "Btn_OldPoint03", "Btn_OldPoint04", "Btn_OldPoint05", "Btn_OldPoint06", "Btn_OldPoint07", "Btn_OldPoint08", "Btn_OldPoint09" };
	char* strNewBtnName[] = { "Btn_NewPoint00", "Btn_NewPoint01", "Btn_NewPoint02", "Btn_NewPoint03", "Btn_NewPoint04", "Btn_NewPoint05", "Btn_NewPoint06", "Btn_NewPoint07", "Btn_NewPoint08", "Btn_NewPoint09" };
	// Button Text (stars)
	char* strOldTxtPointValue[] = { "Txt_OldPointValue00", "Txt_OldPointValue01", "Txt_OldPointValue02", "Txt_OldPointValue03", "Txt_OldPointValue04", "Txt_OldPointValue05", "Txt_OldPointValue06", "Txt_OldPointValue07", "Txt_OldPointValue08", "Txt_OldPointValue09" };
	char* strNewTxtPointValue[] = { "Txt_NewPointValue00", "Txt_NewPointValue01", "Txt_NewPointValue02", "Txt_NewPointValue03", "Txt_NewPointValue04", "Txt_NewPointValue05", "Txt_NewPointValue06", "Txt_NewPointValue07", "Txt_NewPointValue08", "Txt_NewPointValue09" };


	// Attribute List on the right side
	char* strOldTxtAttrValue[] = { "Txt_OldAttValue01", "Txt_OldAttValue02", "Txt_OldAttValue03", "Txt_OldAttValue04", "Txt_OldAttValue05", "Txt_OldAttValue06", "Txt_OldAttValue07", "Txt_OldAttValue08", "Txt_OldAttValue09", "Txt_OldAttValue10" };
	char* strOldTxtAttrName[] = { "Txt_OldAttName01", "Txt_OldAttName02", "Txt_OldAttName03", "Txt_OldAttName04", "Txt_OldAttName05", "Txt_OldAttName06", "Txt_OldAttName07", "Txt_OldAttName08", "Txt_OldAttName09", "Txt_OldAttName10" };
	char* strNewTxtAttrValue[] = { "Txt_NewAttValue01", "Txt_NewAttValue02", "Txt_NewAttValue03", "Txt_NewAttValue04", "Txt_NewAttValue05", "Txt_NewAttValue06", "Txt_NewAttValue07", "Txt_NewAttValue08", "Txt_NewAttValue09", "Txt_NewAttValue10" };
	char* strNewTxtAttrName[] = { "Txt_NewAttName01", "Txt_NewAttName02", "Txt_NewAttName03", "Txt_NewAttName04", "Txt_NewAttName05", "Txt_NewAttName06", "Txt_NewAttName07", "Txt_NewAttName08", "Txt_NewAttName09", "Txt_NewAttName10" };
	char* strDec[] = {"Img_AttDec01","Img_AttDec02","Img_AttDec03","Img_AttDec04","Img_AttDec05","Img_AttDec06","Img_AttDec07","Img_AttDec08","Img_AttDec09","Img_AttDec10" };
	char* strInc[] = { "Img_AttInc01","Img_AttInc02","Img_AttInc03","Img_AttInc04","Img_AttInc05","Img_AttInc06","Img_AttInc07","Img_AttInc08","Img_AttInc09","Img_AttInc10" };
	


	for (int i = 0; i < ASTROLABE_ADDON_MAX; i++) {
		m_pOldButtonPoint[i] = (PAUISTILLIMAGEBUTTON)GetDlgItem(strOldBtnName[i]);
		m_pNewButtonPoint[i] = (PAUISTILLIMAGEBUTTON)GetDlgItem(strNewBtnName[i]);
		m_pOldButtonText[i] = (PAUILABEL)GetDlgItem(strOldTxtPointValue[i]);
		m_pNewButtonText[i] = (PAUILABEL)GetDlgItem(strNewTxtPointValue[i]);

		m_pOldTextAttrValue[i] = (PAUILABEL)GetDlgItem(strOldTxtAttrValue[i]);
		m_pOldTextAttrName[i] = (PAUILABEL)GetDlgItem(strOldTxtAttrName[i]);
		m_pNewTextAttrValue[i] = (PAUILABEL)GetDlgItem(strNewTxtAttrValue[i]);
		m_pNewTextAttrName[i] = (PAUILABEL)GetDlgItem(strNewTxtAttrName[i]);

		m_pAttrDec[i] = (PAUILABEL)GetDlgItem(strDec[i]);
		m_pAttInc[i] = (PAUILABEL)GetDlgItem(strInc[i]);
	}

	m_pOldTotalAptitude = (PAUILABEL)GetDlgItem("Txt_OldPointsValue");
	m_pNewTotalAptitude = (PAUILABEL)GetDlgItem("Txt_NewPointsValue");

	m_pChooseNew = (PAUILABEL)GetDlgItem("Lbl_ChooseNewAttention");
	m_pChooseNewBtn = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_ChooseNew");


	return true;
}

void CDlgAstrolabeSwallow::OnTick()
{
	CDlgBase::OnTick();
}

void CDlgAstrolabeSwallow::OnShowDialog()
{
	CDlgBase::OnShowDialog();

}

void CDlgAstrolabeSwallow::SetContext(CECIvtrAstrolabe* pCurrent, CECIvtrAstrolabe* pSwallow, int iSlot) 
{
	m_pCurrent = pCurrent;
	m_pSwallow = pSwallow;
	m_iSlot = iSlot;

	ResetDialog();
	FillDialog();
}

void CDlgAstrolabeSwallow::ResetDialog() 
{
	for (int i = 0; i < ASTROLABE_ADDON_MAX; i++) {

		// Buttons Disabled
		m_pOldButtonPoint[i]->Enable(false);
		m_pNewButtonPoint[i]->Enable(false);
		m_pOldButtonText[i]->Enable(false);
		m_pOldButtonText[i]->SetColor(0x909090);
		m_pNewButtonText[i]->Enable(false);
		m_pNewButtonText[i]->SetColor(0x909090);

		// AttrList empty
		m_pOldTextAttrName[i]->Enable(false);
		m_pOldTextAttrValue[i]->Enable(false);
		m_pOldTextAttrName[i]->Show(false);
		m_pOldTextAttrValue[i]->Show(false);
		m_pNewTextAttrName[i]->Enable(false);
		m_pNewTextAttrValue[i]->Enable(false);
		m_pNewTextAttrName[i]->Show(false);
		m_pNewTextAttrValue[i]->Show(false);

		m_pAttrDec[i]->Show(false);
		m_pAttInc[i]->Show(false);
	}

	m_pChooseNew->Show(false);
	m_pChooseNewBtn->Enable(true);
}

void CDlgAstrolabeSwallow::FillDialog()
{
	ACString temp;

	temp.Format(_AL("%.2f"), (m_pCurrent->SumAptit() / 100.f));
	m_pOldTotalAptitude->SetText(temp);

	temp.Format(_AL("%.2f"), (m_pSwallow->SumAptit() / 100.f));
	m_pNewTotalAptitude->SetText(temp);

	// Both sides use the current chart attributes, only aptitude of star changes
	int oldSlots = m_pCurrent->GetEssence().slot;

	// star slot to aptit map
	int birthStarIndex[] = { 0, 0, 0, 1, 0, 2, 0, 3, 0, 4 };
	int birthStarCount = 0;

	for (int i = 0; i < ASTROLABE_VIRTUAL_SLOT_COUNT; i++) 
	{
		int aptitudeOld = m_pCurrent->GetAptit(i);
		int pointsOld = (int)((aptitudeOld) / 100.f);

		int aptitudeNew = m_pSwallow->GetAptit(i);
		int pointsNew = (int)((aptitudeNew) / 100.f);

		if (oldSlots & (1 << i)) {

			if (i % 2 != 0)
				birthStarCount++;

			SetPoint(i, pointsOld, pointsNew, true);
		}
		else
			SetPoint(i, pointsOld, pointsNew, false);
	}

	// The star chart addons are here
	int num = m_pCurrent->GetPropertyNum();
	int attrTextIndex = 0;
	int fateIndex = 0;
	for (int i = 0; i < ASTROLABE_VIRTUAL_SLOT_COUNT; i++) {

		if (oldSlots & (1 << i)) {
			int propIndex = attrTextIndex;

			bool bBirthStar ;

			// fate star index
			if (i % 2 == 0) {
				propIndex = birthStarCount + fateIndex;
				fateIndex++;
				bBirthStar = false;
			}
			else {
				attrTextIndex++;
				bBirthStar = true;
			}
		
			const CECIvtrEquip::PROPERTY& prop = m_pCurrent->GetProperty(propIndex);
			EnableAttrTextSlot(propIndex, i, bBirthStar, prop.iType, prop.aParams[0]);
		}
	}
}

void CDlgAstrolabeSwallow::SetPoint(int index, int pointsOld, int pointsNew, bool bActive) 
{
	AWString attrText;
	if (bActive) {
		m_pOldButtonPoint[index]->Enable(true);
		m_pNewButtonPoint[index]->Enable(true);

		// White text, reset sets it grey
		m_pOldButtonText[index]->SetColor(0xFFFFFFFF);
		m_pNewButtonText[index]->SetColor(0xFFFFFFFF);

	}
	else {
		m_pOldButtonPoint[index]->Enable(false);
		m_pNewButtonPoint[index]->Enable(false);
	}
	m_pOldButtonText[index]->SetText(attrText.Format(_AL("%d"), pointsOld));
	m_pNewButtonText[index]->SetText(attrText.Format(_AL("%d"), pointsNew));
}

void CDlgAstrolabeSwallow::EnableAttrTextSlot(int index, int slotIndex, bool bBirthStar, int propertyType, int propertyValue) {

	AWString finalText;
	AWString attrText;
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	CECGame::ItemExtPropTable& PropTab = g_pGame->GetItemExtPropTable();
	CECGame::ItemExtPropTable::pair_type Pair = PropTab.get(propertyType);
	BYTE byPropType = Pair.second ? *Pair.first : 0xff;

	m_pOldTextAttrName[index]->Show(true);
	m_pOldTextAttrValue[index]->Show(true);
	m_pNewTextAttrName[index]->Show(true);
	m_pNewTextAttrValue[index]->Show(true);

	switch (byPropType)
	{
	case 161: //Physical Attack(Astrolabe)
		attrText += pDescTab->GetWideString(ITEMDESC_PHYDAMAGE);
		break;

	case 162: //Magic Attack(Astrolabe)
		attrText += pDescTab->GetWideString(ITEMDESC_MAGICDAMAGE);
		break;

	case 165: //Life(Astrolabe)
		attrText += pDescTab->GetWideString(ITEMDESC_ADDHP);
		break;

	case 167: //Golden Guard(Astrolabe)
		attrText += pDescTab->GetWideString(ITEMDESC_GOLDDEFENCE);
		break;

	case 168: //Wood Defense(Astrolabe)
		attrText += pDescTab->GetWideString(ITEMDESC_WOODDEFENCE);
		break;

	case 169: //Water Defense(Astrolabe)
		attrText += pDescTab->GetWideString(ITEMDESC_WATERDEFENCE);
		break;

	case 170: //Fire Prevention(Astrolabe)
		attrText += pDescTab->GetWideString(ITEMDESC_FIREDEFENCE);
		break;

	case 171: //Earth Defense(Astrolabe)
		attrText += pDescTab->GetWideString(ITEMDESC_EARTHDEFENCE);
		break;

	case 175: //Ignore physical defense level(astrolabe)
		attrText += pDescTab->GetWideString(ITEMDESC_PPEN);
		break;

	case 176: //Ignore Magic Defense Level(Astrolabe)
		attrText += pDescTab->GetWideString(ITEMDESC_MPEN);
		break;

	case 166: //Aura(Astrolabe)
		attrText += pDescTab->GetWideString(ITEMDESC_VIGOUR);
		break;

	case 164: //Five Elements Defense(Astrolabe)
		attrText += pDescTab->GetWideString(ITEMDESC_ALLMAGICDEF);
		break;

	case 173: //Dodge Degree(Astrolabe)
		attrText += pDescTab->GetWideString(ITEMDESC_EVASION);
		break;

	case 163: //Physical Defense(Astrolabe)
		attrText += pDescTab->GetWideString(ITEMDESC_PDEF);
		break;

	case 172: //Hit(Astrolabe)
		attrText += pDescTab->GetWideString(ITEMDESC_ACCURACY2);
		break;

	case 174: //True Qi(Astrolabe)
		attrText += pDescTab->GetWideString(ITEMDESC_ADDMP);
		break;

	}

	m_pOldTextAttrName[index]->SetText(finalText.Format(GetGameUIMan()->GetStringFromTable(bBirthStar ? 11516 : 11517), attrText));
	m_pNewTextAttrName[index]->SetText(finalText.Format(GetGameUIMan()->GetStringFromTable(bBirthStar ? 11516 : 11517), attrText));

	attrText.Format(_AL("%d"), propertyValue);
	m_pOldTextAttrValue[index]->SetText(attrText);

	// Now get new value using swallow aptitude of the star
	// Determine new level of chart then get the addon ratio to compute new value
	int newExpAmount = m_pCurrent->GetEssence().exp + m_pSwallow->SumExp();
	int newLevel = m_pCurrent->TestInsertExp(newExpAmount);
	int newRatio = m_pSwallow->GetAddonRatioForAptitude(m_pSwallow->GetAptit(slotIndex), newLevel);

	// Get the addon base value
	DATA_TYPE DataType;
	EQUIPMENT_ADDON* addon = (EQUIPMENT_ADDON*)g_pGame->GetElementDataMan()->get_data_ptr(propertyType, ID_SPACE_ADDON, DataType);

	float addonValue = *(float*)(&addon->param1);
	int newValue = (int)(newRatio * addonValue);

	attrText.Format(_AL("%d"), newValue);
	m_pNewTextAttrValue[index]->SetText(attrText);

	if (newValue > propertyValue)
		m_pAttInc[index]->Show(true);
	else if (newValue < propertyValue)
		m_pAttrDec[index]->Show(true); 

	attrText.Format(_AL("NewLvl: %d\tNewBase:%d\tAddon: %.2f\tAddonID:%d\tSlot: %d\tCurApt: %d\tSwallowApt: %d\n"), (newLevel+1), newRatio, addonValue, propertyType, slotIndex, m_pCurrent->GetAptit(slotIndex), m_pSwallow->GetAptit(slotIndex));
	OutputDebugString(attrText);

}


