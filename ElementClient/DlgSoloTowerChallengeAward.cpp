#include "AFI.h"
#include "DlgSoloTowerChallengeAward.h"

#include "AUIStillImageButton.h"
#include "AUIDialog.h"

#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "AUIDef.h"

AUI_BEGIN_EVENT_MAP(CDlgSoloTowerChallengeAward, CDlgBase)

AUI_ON_EVENT("Img_DrawItem*", WM_LBUTTONDOWN, OnEventLDown_DrawItem)

AUI_END_EVENT_MAP()



CDlgSoloTowerChallengeAward::CDlgSoloTowerChallengeAward()
{
	m_DealerHelper = new CardDealer(m_pImgDrawItem, SOLO_CHALLENGE_PRIZES_COUNT);
}

CDlgSoloTowerChallengeAward::~CDlgSoloTowerChallengeAward()
{
	delete m_DealerHelper;
}

bool CDlgSoloTowerChallengeAward::OnInitDialog()
{

	m_pTxtLayer = (PAUILABEL)GetDlgItem("Txt_Layer");
	m_pTxtClimbedLayerTime = (PAUILABEL)GetDlgItem("Txt_ClimbeLayerTime");
	m_pTxtTotalTime = (PAUILABEL)GetDlgItem("Txt_TotalTime");
	m_pTxtLeftDrawTimes = (PAUILABEL)GetDlgItem("Txt_LeftDrawTimes");
	m_pImgJustDrawnItem = (PAUIIMAGEPICTURE)GetDlgItem("Img_JustDrawnItem");
	m_ImgFront1 = (PAUIIMAGEPICTURE)GetDlgItem("Img_Front1");
	m_ImgGfxAward = (PAUIIMAGEPICTURE)GetDlgItem("Gfx_Award");

	for (int i = 0; i < SOLO_CHALLENGE_PRIZES_COUNT; i++) {
		char szName[20];
		// Draw items (cards)
		sprintf(szName, "Img_DrawItem%d", i + 1);
		m_pImgDrawItem[i] = (PAUIIMAGEPICTURE)GetDlgItem(szName);

		// Select card gfx
		sprintf(szName, "Gfx_Card%d", i + 1);
		m_pImgDrawnItemGfx[i] = (PAUIIMAGEPICTURE)GetDlgItem(szName);
	}

	for (int i = 0; i < SOLO_CHALLENGE_REWARD_ICONS_COUNT; i++) {
		char szName[20];
		// Drawn items (slots for icons)
		sprintf(szName, "Img_DrawnItem%d", i + 1);
		m_pImgDrawnItem[i] = (PAUIIMAGEPICTURE)GetDlgItem(szName);
	}

	m_nLastLayerClaimed = -1;

	m_DealerHelper->Init();

	return true;
}

void CDlgSoloTowerChallengeAward::OnShowDialog()
{
	
	m_DealerHelper->Reset();
	
	CECHostPlayer* pHost = GetHostPlayer();
	
	// Populate Window
	const CECHostPlayer::SOLO_CHALLENGE_INFO& soloTowerInfo = pHost->GetSoloChallengeInfo();
	if (soloTowerInfo.awardInfo.totalDrawItemTimes > 0) {
		// Set UI fixed values
		ACString tmpString;
		m_pTxtLayer->SetText(tmpString.Format(L"%d", soloTowerInfo.awardInfo.layer));
		m_pTxtClimbedLayerTime->SetText(GetTimeStringFromSeconds(soloTowerInfo.awardInfo.time));
		m_pTxtTotalTime->SetText(GetTimeStringFromSeconds(soloTowerInfo.totalClimbingTime));
		m_pTxtLeftDrawTimes->SetText(tmpString.Format(L"%d", soloTowerInfo.awardInfo.totalDrawItemTimes - soloTowerInfo.awardInfo.drawnItemTimes));

		// Check if this is a new set of awards, so reset UI elements and perform deal animation.
		if (m_nLastLayerClaimed != soloTowerInfo.awardInfo.layer) {
			m_DealerHelper->PerformDeal();
		}

		// Reset last rewards icons
		for (int i = 0; i < SOLO_CHALLENGE_REWARD_ICONS_COUNT; i++) {
			ClearItemIcon(i);
		}

		// Set drawn prizes in last rewards list if any.
		if (soloTowerInfo.awardInfo.drawnItemTimes > 0) {
			for (int i = 0; i < soloTowerInfo.awardInfo.drawnItemTimes; i++) {
				SetItemIcon(i, soloTowerInfo.awardInfo.awards[i].item_id, soloTowerInfo.awardInfo.awards[i].item_count);
			}
		}
	}
	else {
		// If there  is no rewards to claim, hide this window as soon as it opens.
		Show(false);
	}

	// Reset gfx images
	m_ImgGfxAward->Show(false);
	m_ImgFront1->Show(false);
	m_pImgJustDrawnItem->Show(false);
	for (int i = 0; i < 8; i++) {
		m_pImgDrawnItemGfx[i]->Show(false);
	}

	// initialize tick time
	m_dwLastTickTime = 0;
}

void CDlgSoloTowerChallengeAward::OnTick() {

	// Send tick to animation helper
	m_DealerHelper->OnTick();
	
	DWORD dwTick = GetTickCount();
	DWORD timePassed = CECTimeSafeChecker::ElapsedTime(dwTick, m_dwLastTickTime);

	// Check if we are doing a prize animation, so we show the prize after that.
	if (m_nMillisShowPrize > 0) {
		m_nMillisShowPrize -= timePassed;
		if (m_nMillisShowPrize <= 0) {
			ShowPrize();
		}
	}

	CECHostPlayer* pHost = GetHostPlayer();
	// Populate Window
	const CECHostPlayer::SOLO_CHALLENGE_INFO& soloTowerInfo = pHost->GetSoloChallengeInfo();

	// Keep left draw times updated
	if (soloTowerInfo.awardInfo.totalDrawItemTimes > 0) {
		ACString tmpString;
		m_pTxtLeftDrawTimes->SetText(tmpString.Format(L"%d", soloTowerInfo.awardInfo.totalDrawItemTimes - soloTowerInfo.awardInfo.drawnItemTimes));
	}

	m_dwLastTickTime = dwTick;

}

/// <summary>
/// Event when click on a "card" (prize slot)
/// </summary>
/// <param name="wParam"></param>
/// <param name="lParam"></param>
/// <param name="pObj"></param>
void CDlgSoloTowerChallengeAward::OnEventLDown_DrawItem(WPARAM wParam, LPARAM lParam, AUIObject* pObj)
{
	CECHostPlayer* pHost = GetHostPlayer();
	// Populate Window
	const CECHostPlayer::SOLO_CHALLENGE_INFO& soloTowerInfo = pHost->GetSoloChallengeInfo();

	// Prevent to claim a new prize if there is no more prizes to claim
	if (soloTowerInfo.awardInfo.totalDrawItemTimes - soloTowerInfo.awardInfo.drawnItemTimes <= 0)
		return;


	// Make sure we are not daling animation and let the player pick a prize
	if (!m_DealerHelper->IsDealing()) {
		// Hide the card
		pObj->Show(false);

		// Show gfx 
		int pressedSlot = atoi(pObj->GetName() + strlen("Img_DrawItem")) - 1;
		m_pImgDrawnItemGfx[pressedSlot]->Show(false);
		m_pImgDrawnItemGfx[pressedSlot]->Show(true);

		m_nLastSelectedSlot = pressedSlot;
		m_bSelectedSlots[pressedSlot] = true;

		// Do the actual item draw
		DoDrawItem();

	}
}

/// <summary>
/// This function sends a request to server to get the prize.
/// </summary>
void  CDlgSoloTowerChallengeAward::DoDrawItem() 
{
	using namespace C2S;

	//m_bDrawingPrize = true;
	m_nMillisShowPrize = 2000;

	CECHostPlayer* pHost = GetHostPlayer();
	// Populate Window
	const CECHostPlayer::SOLO_CHALLENGE_INFO& soloTowerInfo = pHost->GetSoloChallengeInfo();

	cmdopt_solo_challenge_opt_select_award awardInfo;
	awardInfo.max_stage_level = m_nLastLayerClaimed; // This info is not really used by server. It was in the past, allowing client to exploit server.

	GetGameSession()->c2s_CmdSoloChallengeOperateRequest(1, &awardInfo);
}

/// <summary>
/// This function is called from OnTick() once prize animation is done. (actually controlled by timer, not gfx itself).
/// </summary>
void CDlgSoloTowerChallengeAward::ShowPrize() 
{
	CECHostPlayer* pHost = GetHostPlayer();
	const CECHostPlayer::SOLO_CHALLENGE_INFO& soloTowerInfo = pHost->GetSoloChallengeInfo();

	m_pImgJustDrawnItem->Show(true);
	m_ImgFront1->Show(true);

	m_ImgGfxAward->Show(false);
	m_ImgGfxAward->Show(true);

	// If there are more prizes to claim, deal again.
	if (soloTowerInfo.awardInfo.drawnItemTimes < soloTowerInfo.awardInfo.totalDrawItemTimes) {
		m_DealerHelper->PerformDeal();
	}

}

/// <summary>
/// This function is called when client receives a response from server with successfull award response
/// </summary>
/// <param name="itemId"></param>
/// <param name="itemCount"></param>
/// <param name="totalAwardsClaimed"></param>
void CDlgSoloTowerChallengeAward::AwardReceivedFromServer(int itemId, int itemCount, int totalAwardsClaimed)
{
	// Show the prize item icon and description in the middle of the screen
	SetItemIcon(-1, itemId, itemCount);
	// Show the item in the reward slots
	SetItemIcon((totalAwardsClaimed -1), itemId, itemCount);
}

/// <summary>
/// Clear items icons from given slot
/// </summary>
/// <param name="index"></param>
void CDlgSoloTowerChallengeAward::ClearItemIcon(int index)
{

	PAUIIMAGEPICTURE icon;

	// If index -1, then clear the item in the middle of the screen, otherwise clear it in the prizes slot bar
	if (index == -1) {
		icon = m_pImgJustDrawnItem;
	}
	else if (index >= 0 && index < SOLO_CHALLENGE_REWARD_ICONS_COUNT) {
		icon = m_pImgDrawnItem[index];
	}
	else {
		return;
	}

	// Clear all information for this icon
	icon->ClearCover();
	icon->SetText(_AL(""));
	icon->SetDataPtr(NULL);
	icon->SetColor(A3DCOLORRGB(255, 255, 255));
}

/// <summary>
/// Displays an item icon (and description) in a given slot
/// </summary>
/// <param name="index"></param>
/// <param name="itemId"></param>
/// <param name="itemCount"></param>
void CDlgSoloTowerChallengeAward::SetItemIcon(int index, int itemId, int itemCount) {

	PAUIIMAGEPICTURE icon;
	// If index -1, then show the item in the middle of the screen, otherwise show it in the prizes slot bar
	if (index == -1) {
		icon = m_pImgJustDrawnItem;
	}
	else if (index >= 0 && index < SOLO_CHALLENGE_REWARD_ICONS_COUNT) {
		icon = m_pImgDrawnItem[index];
	}
	else {
		return;
	}

	// Create temporary item out of elemenman
	CECIvtrItem* pItem = CECIvtrItem::CreateItem(itemId, itemCount, 1);
	if (!pItem) return;
	// 
	pItem->GetDetailDataFromLocal();

	// Prepare item name and icon
	AString strFile;
	ACHAR szText[40];
	af_GetFileTitle(pItem->GetIconFile(), strFile);
	strFile.MakeLower();
	icon->SetCover(
		GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);

	// Set description
	icon->SetHint(GetGameUIMan()->GetItemDescHint(pItem));

	// Set amount
	if (pItem->GetCount() > 1) {
		a_sprintf(szText, _AL("%d"), pItem->GetCount());
		icon->SetText(szText);
	}
	else {
		icon->SetText(_AL(""));
	}

	delete pItem;

}

/// <summary>
/// Convert seconds into mm:ss format to be used in UI
/// </summary>
/// <param name="seconds"></param>
/// <returns></returns>
ACString CDlgSoloTowerChallengeAward::GetTimeStringFromSeconds(int seconds)
{
	int minutes = seconds / 60;
	int remainingSeconds = seconds - minutes * 60;

	ACString tmpString;
	tmpString.Format(L"%02d:%02d", minutes, remainingSeconds);
	return tmpString;
}