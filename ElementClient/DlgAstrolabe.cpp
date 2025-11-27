#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_Configs.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_UIConfigs.h"
#include "CSplit.h"
#include "EC_TimeSafeChecker.h"
#include "EC_Resource.h"
#include "A3DGFXExMan.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "elementdataman.h"
#include "EC_FixedMsg.h"
#include "EC_Player.h"
#include "DlgAstrolabe.h"
#include "DlgAstrolabeSwallow.h"
#include "EC_IvtrAstrolabeItem.h"
#include "EC_GPDataType.h"
#include "DlgInventory.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgAstrolabe, CDlgBase)
	AUI_ON_COMMAND("IDCANCEL", OnCommandClose)
	AUI_ON_COMMAND("Btn_Point*", OnCommandButtonPress)
	AUI_ON_COMMAND("Btn_Swallow", OnCommandSwallow)
	AUI_ON_COMMAND("Btn_IncPointValue", OnCommandIncPointValue)
	AUI_ON_COMMAND("Btn_RandAtt", OnCommandRandAtt)
	AUI_ON_COMMAND("Btn_ShuffleAttPos", OnCommandShuffleAtt)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAstrolabe, CDlgBase)
AUI_END_EVENT_MAP()

#define HELP_PREFIX "HelpName_"

CDlgAstrolabe::CDlgAstrolabe() : m_pCurrentItem(NULL), m_bNeedsUpdate(false), m_pCmd(NULL)
{
	m_pSubDlg = NULL;
}

CDlgAstrolabe::~CDlgAstrolabe()
{
	//if (m_pSubDlg) {
	//	delete m_pSubDlg;
	//	m_pSubDlg = NULL;
	//}
}

bool CDlgAstrolabe::OnInitDialog()
{
	char* strGfxPointValues[] = { "Gfx_IncPointValue01", "Gfx_IncPointValue03", "Gfx_IncPointValue05", "Gfx_IncPointValue07", "Gfx_IncPointValue09" };
	for (int i = 0; i < 5; i++) {
		m_pGfxPointValues[i] = (PAUIIMAGEPICTURE)GetDlgItem(strGfxPointValues[i]);
	}

	// White progress circle on each button (star)
	char* strImgFullPoint[] = { "Img_ValueFullPoint00", "Img_ValueFullPoint01", "Img_ValueFullPoint02", "Img_ValueFullPoint03", "Img_ValueFullPoint04", "Img_ValueFullPoint05", "Img_ValueFullPoint06", "Img_ValueFullPoint07", "Img_ValueFullPoint08", "Img_ValueFullPoint09" };
	for (int i = 0; i < ASTROLABE_ADDON_MAX; i++) {
		m_pStarFullProgress[i] = (PAUIIMAGEPICTURE)GetDlgItem(strImgFullPoint[i]);
	}
	
	char* strImgPoint[] = { "Img_Point00", "Img_Point01", "Img_Point02", "Img_Point03", "Img_Point04", "Img_Point05", "Img_Point06", "Img_Point07", "Img_Point08", "Img_Point09" };
	for (int i = 0; i < ASTROLABE_ADDON_MAX; i++) {
		m_pImgPoint[i] = (PAUIIMAGEPICTURE)GetDlgItem(strImgPoint[i]);
	}

	char* strGfxPointActive[] = { "Gfx_PointActive00", "Gfx_PointActive01", "Gfx_PointActive02", "Gfx_PointActive03", "Gfx_PointActive04", "Gfx_PointActive05", "Gfx_PointActive06", "Gfx_PointActive07", "Gfx_PointActive08", "Gfx_PointActive09" };
	for (int i = 0; i < ASTROLABE_ADDON_MAX; i++) {
		m_pGfxPointActive[i] = (PAUIIMAGEPICTURE)GetDlgItem(strGfxPointActive[i]);
	}
	
	// Attribute List on the right side
	char* strTxtAttrValue[] = { "Txt_AttValue01", "Txt_AttValue02", "Txt_AttValue03", "Txt_AttValue04", "Txt_AttValue05", "Txt_AttValue06", "Txt_AttValue07", "Txt_AttValue08", "Txt_AttValue09", "Txt_AttValue10" };
	char* strTxtAttrName[] = { "Txt_AttName01", "Txt_AttName02", "Txt_AttName03", "Txt_AttName04", "Txt_AttName05", "Txt_AttName06", "Txt_AttName07", "Txt_AttName08", "Txt_AttName09", "Txt_AttName10" };
	for (int i = 0; i < ASTROLABE_ADDON_MAX; i++) {
		m_pTextAttrValue[i] = (PAUILABEL)GetDlgItem(strTxtAttrValue[i]);
		m_pTextAttrName[i] = (PAUILABEL)GetDlgItem(strTxtAttrName[i]);
	}

	// Star Aptitude values
	char* strTxtPointValue[] = { "Txt_PointValue00", "Txt_PointValue01", "Txt_PointValue02", "Txt_PointValue03", "Txt_PointValue04", "Txt_PointValue05", "Txt_PointValue06", "Txt_PointValue07", "Txt_PointValue08", "Txt_PointValue09" };
	for (int i = 0; i < ASTROLABE_ADDON_MAX; i++) {
		m_pTxtPointValue[i] = (PAUILABEL)GetDlgItem(strTxtPointValue[i]);
	}

	// Button faces (stars)
	char* strBtnName[] = { "Btn_Point00", "Btn_Point01", "Btn_Point02", "Btn_Point03", "Btn_Point04", "Btn_Point05", "Btn_Point06", "Btn_Point07", "Btn_Point08", "Btn_Point09" };
	for (int i = 0; i < ASTROLABE_ADDON_MAX; i++) {
		m_pButtonPoint[i] = (PAUISTILLIMAGEBUTTON)GetDlgItem(strBtnName[i]);
	}

	char* strFaceNames[] = { "Img_HighFace00", "Img_HighFace01", "Img_HighFace02", "Img_HighFace03", "Img_HighFace04", "Img_HighFace05" };
	for (int i = 0; i < 6; i++) {
		m_pFace[i] = (PAUIIMAGEPICTURE)GetDlgItem(strFaceNames[i]);
	}
	char* strLineNames[] = { "Img_HighLine00", "Img_HighLine01", "Img_HighLine02", "Img_HighLine03", "Img_HighLine04", "Img_HighLine05", "Img_HighLine06", "Img_HighLine07", "Img_HighLine08",
							 "Img_HighLine09" , "Img_HighLine10" , "Img_HighLine11" , "Img_HighLine12" , "Img_HighLine13" , "Img_HighLine14"};
	for (int i = 0; i < 15; i++) {
		m_pLines[i] = (PAUIIMAGEPICTURE)GetDlgItem(strLineNames[i]);
	}

	m_pLevelProgress = (PAUIPROGRESS)GetDlgItem("Prog_Level");
	m_pEnergyProgress = (PAUIPROGRESS)GetDlgItem("Prog_VIPLevel");
	m_pLevelText = (PAUILABEL)GetDlgItem("Txt_Level");
	m_pEnergyText = (PAUILABEL)GetDlgItem("Txt_VIPLevel");
	m_pTotalAptitude = (PAUILABEL)GetDlgItem("Txt_PointsValue");
	m_pTitle = (PAUILABEL)GetDlgItem("Lbl_Title");
	m_pAllGfx = (PAUIIMAGEPICTURE)GetDlgItem("Gfx_AllFull");

	m_pRndAttGfx = (PAUIIMAGEPICTURE)GetDlgItem("Gfx_RandAtt");
	
	m_pSwallowGfx = (PAUIIMAGEPICTURE)GetDlgItem("Gfx_Swallow");
	m_pShuffleGfx = (PAUIIMAGEPICTURE)GetDlgItem("Gfx_ShuffleAttPos");

	// Blue Bar over attribute
	m_pAttrHighlight = (PAUIIMAGEPICTURE)GetDlgItem("Img_HighLight");

	AXMLItem item;
	item.Init(_AL("<SUBDIALOG Name=\"SubDialog\" x=\"20\" y=\"449\" Width=\"483\" Height=\"94\" />"));

	m_pSubDlg = new AUISubDialog();
	m_pSubDlg->SetParent(this);

	m_pSubDlg->InitXML(m_pA3DEngine, m_pA3DDevice, &item);
	AddObjectToList((PAUIOBJECT)m_pSubDlg);

	return true;
}

void CDlgAstrolabe::OnShowDialog()
{
	CDlgBase::OnShowDialog();

	m_pSubDlg->SetDialog(NULL);
	m_pSubDlg->Show(false);
	// Do no show gfx initially
	m_pSwallowGfx->Show(false);
	m_pRndAttGfx->Show(false);
	m_pShuffleGfx->Show(false);
	for (int i = 0; i < ASTROLABE_SLOT_COUNT; i++) {
			m_pGfxPointValues[i]->Show(false);
	}
}


void CDlgAstrolabe::OnTick()
{
	CDlgBase::OnTick();

	CECIvtrAstrolabe* item = (CECIvtrAstrolabe*)GetHostPlayer()->GetEquipment()->GetItem(EQUIPIVTR_ASTROLABE);

	// Changed equipment, or we did something to edit the current one
	if (m_pCurrentItem != item || m_bNeedsUpdate )
	{
		using namespace C2S;

		UpdateAstrolabeState(item);
		m_bNeedsUpdate = false;
		CDlgAstrolabeRandomAddon* dlg = GetGameUIMan()->m_pDlgAstrolabeRandomAddon;
		CDlgAstrolabeIncPointValue* dlg2 = GetGameUIMan()->m_pDlgAstrolabeIncPoint;

		if (m_pCmd) {
			switch (m_pCmd->opttype) {
			case ASTROLABE_OPT_SLOT_ROLL:
				m_pShuffleGfx->Show(false);
				m_pShuffleGfx->SetAlpha(255);
				m_pShuffleGfx->Enable(true);
				m_pShuffleGfx->Show(true);
				break;

			case ASTROLABE_OPT_APTIT_INC:
				if (dlg2->IsShow()) {
					dlg2->UpdateNow();
					// flash point that changed
					for (int i = 0; i < ASTROLABE_SLOT_COUNT; i++) {
						unsigned short apt = m_pCurrentItem->GetEssence().aptit[i];
						if (apt != prevApt[i] ) {

							m_pGfxPointValues[i]->Show(false);
							m_pGfxPointValues[i]->SetAlpha(255);
							m_pGfxPointValues[i]->Enable(true);
							m_pGfxPointValues[i]->Show(true);
						}
						prevApt[i] = apt;
					}
				}
				break;

			case ASTROLABE_OPT_ADDON_ROLL:
				if (dlg->IsShow()) {
					dlg->UpdateNow(m_pCmd);
					m_pRndAttGfx->Show(false);
					m_pRndAttGfx->SetAlpha(255);
					m_pRndAttGfx->Enable(true);
					m_pRndAttGfx->Show(true);

				}
				break;

			case ASTROLABE_OPT_SWALLOW:
				m_pSwallowGfx->Show(false);
				m_pSwallowGfx->SetAlpha(255);
				m_pSwallowGfx->Enable(true);
				m_pSwallowGfx->Show(true);
				break;
			}
		}

	}
}

void CDlgAstrolabe::UpdateAstrolabeState(CECIvtrAstrolabe* item) {
	ACString temp;

	if (!item)
		return;

	m_pCurrentItem = item;

	// Do not disable all the UI unless its affecting it
	// Starshifting will not modify the star points
	if (!m_bNeedsUpdate || (m_bNeedsUpdate && m_pCmd->opttype != C2S::ASTROLABE_OPT_APTIT_INC))
	{
		// set all to disabled
		ResetAstrolabe();
	}

	// If this is called and needs update is false
	// it means we are initializing the dialog
	if (!m_bNeedsUpdate) {
		for (int i = 0; i < ASTROLABE_SLOT_COUNT; i++) {
			unsigned short apt = m_pCurrentItem->GetEssence().aptit[i];
			prevApt[i] = apt;
		}
	}

	// Now fill it in
	m_pTitle->SetText(m_pCurrentItem->GetName());

	if (m_pCurrentItem->GetEssence().level == ASTROLABE_LEVEL_MAX)
		m_pLevelProgress->SetProgress(100);
	else
		m_pLevelProgress->SetProgress((m_pCurrentItem->GetEssence().exp * 100 / GetExpForLevel(m_pCurrentItem->GetEssence().level)));

	if ( GetHostPlayer()->GetAstralLevel() == ASTROLABE_VIP_GRADE_MAX)
		m_pEnergyProgress->SetProgress(100);
	else
		m_pEnergyProgress->SetProgress((GetHostPlayer()->GetAstralEnergy() * 100 / GetAstralEnergyForLevel(GetHostPlayer()->GetAstralLevel())));

	m_pLevelProgress->SetHint(MakeLevelHint());
	m_pEnergyProgress->SetHint(MakeEnergyHint());

	temp.Format(_AL("%d"), (m_pCurrentItem->GetEssence().level + 1));
	m_pLevelText->SetText(temp);
	
	temp.Format(_AL("%d"), (GetHostPlayer()->GetAstralLevel() + 1));
	m_pEnergyText->SetText(temp);

	temp.Format(_AL("%.2f"), (m_pCurrentItem->SumAptit() / 100.f));
	m_pTotalAptitude->SetText(temp);


	int slots = m_pCurrentItem->GetEssence().slot;
	// star slot to aptit map
	int birthStarIndex[] = { 0, 0, 0, 1, 0, 2, 0, 3, 0, 4 };
	int birthStarCount = 0;

	for (int i = 0; i < ASTROLABE_VIRTUAL_SLOT_COUNT; i++) {

		int pointProgress = 0;
		int points = 0;
		int aptitude = m_pCurrentItem->GetAptit(i);

		if (i % 2 == 0) {
			points = (int)( (aptitude) / 100.f);
			pointProgress = (int)((aptitude) % 100 );
			MakePointHint(i, false, aptitude);
		}
		else {
			points = (int)(aptitude / 100.f);
			pointProgress = (int)(aptitude % 100);
			MakePointHint(i, true, aptitude);
		}

		if (slots & (1 << i)) {
	
			if (i % 2 != 0)
				birthStarCount++;

			SetPointActive(i, pointProgress, points);
		}
		else
			SetPointInactive(i, pointProgress, points);
	}

	// Face Gfx
	if ( m_pCurrentItem->SumAptit() == ASTROLABE_APTIT_SUM_MAX )
		m_pFace[0]->Show(true);

	if (slots & (1 << 9) && slots & (1 << 0) && slots & (1 << 1))
		m_pFace[1]->Show(true);
	
	if (slots & (1 << 1) && slots & (1 << 2) && slots & (1 << 3)) 
		m_pFace[2]->Show(true);

	if (slots & (1 << 3) && slots & (1 << 4) && slots & (1 << 5)) 
		m_pFace[3]->Show(true);

	if (slots & (1 << 5) && slots & (1 << 6) && slots & (1 << 7)) 
		m_pFace[4]->Show(true);

	if (slots & (1 << 7) && slots & (1 << 8) && slots & (1 << 9)) 
		m_pFace[5]->Show(true);

	// lines
	int birthLines = 0;
	for (int i = 0; i < ASTROLABE_VIRTUAL_SLOT_COUNT; i++) {
		int nextInd = i == 9 ? 0 : i + 1;

		// Outside lines
		if (slots & (1 << i) && slots & (1 << nextInd))
			m_pLines[i]->Show(true);

		// Inside lines
		if (i % 2 != 0) {
			nextInd = i == 9 ? 1 : i + 2;

			// last point connecting to first point 
			if (i == 9)
				birthLines = 0;
			else
				birthLines++;

			if (slots & (1 << i) && slots & (1 << nextInd)) {
				m_pLines[10 + birthLines]->Show(true);
			}
		}
	}

	// The star chart addons are here
	int num = m_pCurrentItem->GetPropertyNum();
	int attrTextIndex = 0;
	for (int i = 0; i < ASTROLABE_VIRTUAL_SLOT_COUNT; i++) {

		if (slots & (1 << i)) {

			const CECIvtrEquip::PROPERTY& prop = m_pCurrentItem->GetProperty(attrTextIndex);
			bool bBirthStar = attrTextIndex < birthStarCount ? true : false;

			MakeAttrTextHint(attrTextIndex, prop.iType);
			EnableAttrTextSlot(attrTextIndex, bBirthStar, prop.iType, prop.aParams[0]);
			attrTextIndex++;
		}
	}
}

void CDlgAstrolabe::MakeAttrTextHint(int index, int iType) {
	ACString text;
	ACString tmp;
	DATA_TYPE DataType;
	EQUIPMENT_ADDON* addon = (EQUIPMENT_ADDON*)GetGame()->GetElementDataMan()->get_data_ptr(iType, ID_SPACE_ADDON, DataType);

	float maxRatioBirth = m_pCurrentItem->GetMaxRatio(true);
	float maxRatioFate = m_pCurrentItem->GetMaxRatio(false);
	float addonValue = *(float*)(&addon->param1);

	text.Format(_AL("%s +%d\r"), GetGameUIMan()->GetStringFromTable(11521), (int)(maxRatioBirth * addonValue) );
	tmp.Format(_AL("%s +%d\r"), GetGameUIMan()->GetStringFromTable(11522), (int)(maxRatioFate * addonValue ) );
	text += tmp;

	m_pTextAttrName[index]->SetHint(text);
	m_pTextAttrValue[index]->SetHint(text);
}

void CDlgAstrolabe::MakePointHint(int index, bool bBirthStar, int aptitude) {
	ACString text;
	ACString tmp;

	float value = aptitude / 100.f;

	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	text += pDescTab->GetWideString(bBirthStar ? ITEMDESC_BIRTHSTAR : ITEMDESC_FATESTAR);
	text += _AL("\r");
	text += pDescTab->GetWideString(ITEMDESC_CUR_APT);

	tmp.Format(_AL(" %.2f\r"), value);
	text += tmp;

	text += pDescTab->GetWideString(ITEMDESC_EFFECTIVE_APT);


	if (m_pCurrentItem->SumAptit() == ASTROLABE_APTIT_SUM_MAX)
		tmp.Format(_AL(" %.2f\r"), value);
	else
		tmp.Format(_AL(" %d\r"), (int)value);
	text += tmp;

	if (bBirthStar) {
		text += pDescTab->GetWideString(ITEMDESC_APT_CAP);
		tmp.Format(_AL(" %.2f\r"), (ASTROLABE_APTIT_MAX / 100.f));
		text += tmp;
		text += GetGameUIMan()->GetStringFromTable(11523);
	}
	else {
		text += GetGameUIMan()->GetStringFromTable(11524);
		text += _AL("\r");
		text += GetGameUIMan()->GetStringFromTable(11523);
	}

	m_pButtonPoint[index]->SetHint(text);
}

ACString CDlgAstrolabe::MakeLevelHint() {
	ACString text;
	ACString tmp;
	CECStringTab* pDescTab = g_pGame->GetItemDesc();

	if (m_pCurrentItem->GetEssence().level < ASTROLABE_LEVEL_MAX) {
		text.Format(_AL("%s %d/%d"), pDescTab->GetWideString(ITEMDESC_STARCHART_EXP), m_pCurrentItem->GetEssence().exp, GetExpForLevel(m_pCurrentItem->GetEssence().level));
		text += _AL("\r");
	}
	text += pDescTab->GetWideString(ITEMDESC_LEVEL_CAP);
	tmp.Format(_AL(" %d\r"), (ASTROLABE_LEVEL_MAX + 1));
	text += tmp;
	text += GetGameUIMan()->GetStringFromTable(11518);

	return text;
}

ACString CDlgAstrolabe::MakeEnergyHint() {
	ACString text;
	ACString tmp;
	CECStringTab* pDescTab = g_pGame->GetItemDesc();

	if (GetHostPlayer()->GetAstralLevel() < ASTROLABE_VIP_GRADE_MAX) {
		text.Format(_AL("%s %d/%d"), pDescTab->GetWideString(ITEMDESC_ASTRAL_EXP), GetHostPlayer()->GetAstralEnergy(), GetAstralEnergyForLevel(GetHostPlayer()->GetAstralLevel()));
		text += _AL("\r");
	}
	text += pDescTab->GetWideString(ITEMDESC_LEVEL_CAP);
	tmp.Format(_AL(" %d\r"), (ASTROLABE_VIP_GRADE_MAX + 1));
	text += tmp;

	text += GetGameUIMan()->GetStringFromTable(11519);


	return text;
}

void CDlgAstrolabe::EnableAttrTextSlot(int index, bool bBirthStar, int propertyType, int propertyValue) {

	AWString finalText;
	AWString attrText;
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	CECGame::ItemExtPropTable& PropTab = g_pGame->GetItemExtPropTable();
	CECGame::ItemExtPropTable::pair_type Pair = PropTab.get(propertyType);
	BYTE byPropType = Pair.second ? *Pair.first : 0xff;

	m_pTextAttrName[index]->Show(true);
	m_pTextAttrValue[index]->Show(true);

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
	m_pTextAttrName[index]->SetText(finalText.Format( GetGameUIMan()->GetStringFromTable(bBirthStar?11516:11517), attrText));

	attrText.Format(_AL("%d"), propertyValue);
	m_pTextAttrValue[index]->SetText(attrText);
}

void CDlgAstrolabe::SetPointActive(int index, int progress, int points) {
	AWString attrText;

	m_pGfxPointActive[index]->Show(true);
	m_pButtonPoint[index]->Enable(true);
	m_pButtonPoint[index]->Show(true);
	m_pTxtPointValue[index]->Enable(true);
	m_pTxtPointValue[index]->SetColor(0xFFFFFFFF);

	if (m_pCurrentItem->SumAptit() == ASTROLABE_APTIT_SUM_MAX) {
		// Active/Full/Progress
		m_pStarFullProgress[index]->Show(true);
		m_pStarFullProgress[index]->Enable(true);
		m_pStarFullProgress[index]->GetClockIcon()->SetProgressPos(progress);
		m_pStarFullProgress[index]->SetColor(0xFFFFFFFF);
		m_pStarFullProgress[index]->GetClockIcon()->SetColor(0xFF0000FF);
	}
	else {
		// Active/Not Full/Progress
		m_pImgPoint[index]->Show(true);
		m_pImgPoint[index]->Enable(true);
		m_pImgPoint[index]->GetClockIcon()->SetProgressPos(progress);
		m_pImgPoint[index]->GetClockIcon()->SetColor(0xFFFFFFCF);

	}

	m_pTxtPointValue[index]->SetText(attrText.Format(_AL("%d"), points));
}

void CDlgAstrolabe::SetPointInactive(int index, int progress, int points) {
	AWString attrText;

	m_pButtonPoint[index]->Enable(false);
	m_pButtonPoint[index]->Show(true);

	if (m_pCurrentItem->SumAptit() == ASTROLABE_APTIT_SUM_MAX) {
		// Not Active/Full/Progress
		m_pStarFullProgress[index]->Show(true);
		m_pStarFullProgress[index]->Enable(true);
		m_pStarFullProgress[index]->GetClockIcon()->SetProgressPos(progress);
		m_pStarFullProgress[index]->SetColor(0xFFC0C0C0);
		m_pStarFullProgress[index]->GetClockIcon()->SetColor(0xFFCFCFCF);

	}
	else {
		// Not Active/Not Full/Progress
		m_pImgPoint[index]->Show(true);
		m_pImgPoint[index]->Enable(false);
		m_pImgPoint[index]->GetClockIcon()->SetProgressPos(progress);
		m_pImgPoint[index]->GetClockIcon()->SetColor(0xFFEF9F9F);
	}

	m_pTxtPointValue[index]->SetText(attrText.Format(_AL("%d"), points));
}

void CDlgAstrolabe::ResetAstrolabe() {

	m_pAllGfx->Enable(false);
	m_pAllGfx->Show(false);
	
	m_pAttrHighlight->Enable(false);
	m_pAttrHighlight->Show(false);

	for (int i = 0; i < ASTROLABE_ADDON_MAX; i++) {
		
		m_pImgPoint[i]->Show(false);
		m_pImgPoint[i]->GetClockIcon()->SetProgressPos(60);

		m_pStarFullProgress[i]->Enable(false);
		m_pStarFullProgress[i]->Show(false);

		m_pGfxPointActive[i]->Show(false);

		m_pTxtPointValue[i]->Enable(false);
		m_pTxtPointValue[i]->SetColor(0x909090);

		m_pTextAttrName[i]->Enable(false);
		m_pTextAttrValue[i]->Enable(false);
		m_pButtonPoint[i]->Enable(false);

		m_pTextAttrName[i]->Show(false);
		m_pTextAttrValue[i]->Show(false);
		m_pButtonPoint[i]->Show(false);
		m_pButtonPoint[i]->SetHint(_AL("Test popup text"));
	}
	for (int i = 0; i < 5; i++) {
		m_pGfxPointValues[i]->Enable(false);
		m_pGfxPointValues[i]->Show(false);
	}
	for (int i = 0; i < 6; i++) {
		m_pFace[i]->Enable(false);
		m_pFace[i]->Show(false);
	}
	for (int i = 0; i < 15; i++) {
		m_pLines[i]->Enable(false);
		m_pLines[i]->Show(false);
	}
}

// Level is level + 1
int CDlgAstrolabe::GetExpForLevel(int level)
{
	return m_pCurrentItem->GetExpForLevel(level);

}

// Energy is just level...
int CDlgAstrolabe::GetAstralEnergyForLevel(int level)
{
	DATA_TYPE DataType;
	ASTROLABE_ADDON_RANDOM_CONFIG* config = (ASTROLABE_ADDON_RANDOM_CONFIG*)GetGame()->GetElementDataMan()->get_data_ptr(2015, ID_SPACE_CONFIG, DataType);

	int sum = 0;
	if ( config && level >= 0 && level < ASTROLABE_VIP_GRADE_MAX ) 
		sum += config->levelup_exp[level];		
	return sum;
}

void CDlgAstrolabe::OnCommandButtonPress(const char* szCommand)
{
	if (m_pCurrentItem) {

		int index = GetNameIndex(szCommand, "Btn_Point");

		int attrIndex = 0;

		if (index % 2 != 0) {
			// clicked a birth star
			for (int i = 0; i < ASTROLABE_VIRTUAL_SLOT_COUNT; i++) {

				if (m_pCurrentItem->GetEssence().slot & (1 << i)) {

					if (i % 2 != 0) {
						if (i == index) {
							// found it
							break;
						}
						else
							attrIndex++;
					}
				}
			}
		}
		else {
			// clicked a fate star
			int birthStarCount = 0;
			for (int i = 0; i < ASTROLABE_VIRTUAL_SLOT_COUNT; i++) {

				if (m_pCurrentItem->GetEssence().slot & (1 << i)) {

					if (i % 2 != 0)
						birthStarCount++;
				}
			}
			for (int i = 0; i < ASTROLABE_VIRTUAL_SLOT_COUNT; i++) {

				if (m_pCurrentItem->GetEssence().slot & (1 << i)) {

					if (i % 2 == 0) {
						if (i == index) {
							break;
						}
						else
							attrIndex++;
					}
				}
			}
			attrIndex += birthStarCount;
		}

		POINT p = m_pTextAttrName[attrIndex]->GetPos(true);
		POINT pL = m_pAttrHighlight->GetPos(true);
		A3DRECT txtRect = m_pTextAttrName[attrIndex]->GetRect();
		A3DRECT lightRect = m_pAttrHighlight->GetRect();

		m_pAttrHighlight->SetPos(pL.x, p.y - ((lightRect.Height() - txtRect.Height()) / 2.f));
		m_pAttrHighlight->Show(true);

	}
}
void CDlgAstrolabe::OnCommandClose(const char* szCommand)
{
	CDlgAstrolabeRandomAddon* dlg = GetGameUIMan()->m_pDlgAstrolabeRandomAddon;
	CDlgAstrolabeIncPointValue* dlg2 = GetGameUIMan()->m_pDlgAstrolabeIncPoint;

	if ( IsSwallowing() )
		SetSwallowing(false);

	else if (dlg->IsShow() ) {

		dlg->OnCommand("IDCANCEL");
		m_pSubDlg->SetDialog(NULL);
	}
	else if (dlg2->IsShow()) {

		dlg2->OnCommand("IDCANCEL");
		m_pSubDlg->SetDialog(NULL);
	}
	else
		Show(false);
}

void CDlgAstrolabe::OnCommand_Help(const char* szCommand)
{
	if (IsSwallowing())
		SetSwallowing(false);

	PAUIDIALOG pDlg = NULL;
	if (strstr(szCommand, HELP_PREFIX))
	{
		pDlg = m_pAUIManager->GetDialog(szCommand + strlen(HELP_PREFIX));
	}
	else
	{
		AString helpDlg;
		pDlg = m_pAUIManager->GetDialog(helpDlg.Format("%sHelp", GetName()));
	}

	if (pDlg)
	{
		pDlg->SetPosEx(GetPos().x, GetPos().y);
		// show next
		pDlg->Show(!pDlg->IsShow());
	}
}

void CDlgAstrolabe::NeedsUpdate(bool bUpdate, S2C::cmd_astrolabe_operate_result* pCmd)
{
	m_pCmd = pCmd;
	m_bNeedsUpdate = bUpdate; 
}

void CDlgAstrolabe::OnCommandSwallow(const char* szCommand)
{
	// Astral Infusion
	if (!IsSwallowing()) {
		GetGame()->ChangeCursor(RES_CUR_SWALLOW);
		SetSwallowing(true);
		GetGameUIMan()->m_pDlgInventory->Show(true);
	}
	else {
		GetGame()->ChangeCursor(RES_CUR_NORMAL);
		SetSwallowing(false);
	}
}

void CDlgAstrolabe::ShowSwallowDialog(AstrolabeMsg* pItem) {
	// Show compare dialog now
	GetGameUIMan()->m_pDlgAstrolabeSwallow->SetContext(m_pCurrentItem, pItem->pItem, pItem->inventorySlot);
	GetGameUIMan()->m_pDlgAstrolabeSwallow->Show(true, true);
}


bool CDlgAstrolabe::SwallowItem(int inventorySlot, CECIvtrItem* pItem)
{
	m_bSwallowChart = false;

	m_iSwallowSlot = inventorySlot;
	m_iSwallowId = pItem->GetTemplateID();

	CECIvtrAstrolabe* pChart = dynamic_cast<CECIvtrAstrolabe*>(pItem);

	if (pChart) {
		AstrolabeMsg* msg = new AstrolabeMsg;
		msg->inventorySlot = inventorySlot;
		msg->pItem = pChart;

		int curSize = m_pCurrentItem->GetPropertyNum();
		int swallowSize = pChart->GetPropertyNum();
		// if swallowing a chart with more addons than this one, check with user
		if (swallowSize > curSize) {

			PAUIDIALOG MsgBox;

			GetGameUIMan()->MessageBox("Game_Swallow_Use", GetStringFromTable(11535), MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &MsgBox);
			MsgBox->SetDataPtr(msg, "SwallowItem");
		}
		else {
			ShowSwallowDialog(msg);
		}
		return true;
	}
	else {
		CECIvtrAstrolabeExpPill* pPill = dynamic_cast<CECIvtrAstrolabeExpPill*>(pItem);
		if (pPill) {
			SendSwallowCmd(CDlgAstrolabeSwallow::NO_INHERIT);
			return true;
		}
	}

	return false;
}

void CDlgAstrolabe::SendSwallowCmd(int type) {
	GetGameSession()->c2s_CmdAstrolabeSwallow(type, m_iSwallowSlot, m_iSwallowId);
}


void CDlgAstrolabe::SetSwallowing(bool swallow)
{
	m_bSwallowing = swallow;
	PAUISTILLIMAGEBUTTON pBtn = dynamic_cast<PAUISTILLIMAGEBUTTON>(GetDlgItem("Btn_Swallow"));
	if (pBtn) 
		pBtn->SetPushed(swallow);
}


void CDlgAstrolabe::OnCommandRandAtt(const char* szCommand)
{
	// Horoscope
	if (IsSwallowing())
		SetSwallowing(false);

	CDlgAstrolabeRandomAddon* dlg = GetGameUIMan()->m_pDlgAstrolabeRandomAddon;

	if (dlg) {
		CECIvtrAstrolabeRandomAddonPill* pItem = dlg->FindUsableItem();

		if (pItem == NULL) {
			PAUIDIALOG MsgBox;
			GetGameUIMan()->MessageBox("Game_Horoscope_PillMissing", GetStringFromTable(11527), MB_OK, A3DCOLORRGBA(255, 255, 255, 160), &MsgBox);
			MsgBox->SetLife(3);
		}
		else {

			if (dlg) {

				dlg->SetContext(m_pCurrentItem, pItem);
				m_pSubDlg->SetDialog((PAUIDIALOG)dlg);
				m_pSubDlg->Show(true);
			}
		}
	}
}

void CDlgAstrolabe::OnCommandShuffleAtt(const char* szCommand)
{
	// Stargazing
	if (IsSwallowing())
		SetSwallowing(false);

	// Starseeker Powder - 47384 or 47500
	int id = ASTROLABE_SLOT_ROLL_ITEM_1;
	CECHostPlayer* pPlayer = GetHostPlayer();
	CECInventory* pInv = pPlayer->GetPack();
	int iSlot = pInv->FindItem(id);
	if (iSlot == -1)
	{
		id = ASTROLABE_SLOT_ROLL_ITEM_2;
		iSlot = pInv->FindItem(id);
	}

	if (iSlot < 0) {
		PAUIDIALOG MsgBox;
		GetGameUIMan()->MessageBox("Game_Stargaze_PowderMissing", GetStringFromTable(11512), MB_OK, A3DCOLORRGBA(255, 255, 255, 160), &MsgBox);
		MsgBox->SetLife(3);
	}
	else {
		PAUIDIALOG MsgBox;
		ACString tmp;
		DATA_TYPE DataType;
		
		TASKNORMALMATTER_ESSENCE* config = (TASKNORMALMATTER_ESSENCE*)GetGame()->GetElementDataMan()->get_data_ptr(id, ID_SPACE_ESSENCE, DataType);
		if (config) {
			tmp.Format(GetStringFromTable(11513), 1, config->name, m_pCurrentItem->GetName());
			GetGameUIMan()->MessageBox("Game_Stargaze_Use", tmp, MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &MsgBox);
			MsgBox->SetData(id, "ID");
		}
	}
}

void CDlgAstrolabe::OnConfirmStargaze(int id)
{
	CECHostPlayer* pPlayer = GetHostPlayer();
	CECInventory* pInv = pPlayer->GetPack();
	int iSlot = pInv->FindItem(id);
	GetGameSession()->c2s_CmdAstrolabeShuffleAtt(iSlot, id);
}


void CDlgAstrolabe::OnCommandIncPointValue(const char* szCommand)
{
	// Starshift
	if (IsSwallowing())
		SetSwallowing(false);

	CDlgAstrolabeIncPointValue* dlg = GetGameUIMan()->m_pDlgAstrolabeIncPoint;

	if (dlg) {
		CECIvtrAstrolabePointIncPill* pItem = dlg->FindUsableItem(m_pCurrentItem->SumAptit());

		if (pItem == NULL) {
			PAUIDIALOG MsgBox;
			GetGameUIMan()->MessageBox("Game_Stargaze_PowderMissing", GetStringFromTable(11510), MB_OK, A3DCOLORRGBA(255, 255, 255, 160), &MsgBox);
			MsgBox->SetLife(3);
		}
		else {

			if (dlg) {

				dlg->SetContext(m_pCurrentItem, pItem);
				m_pSubDlg->SetDialog((PAUIDIALOG)dlg);
				m_pSubDlg->Show(true);
			}
		}
	}
}
