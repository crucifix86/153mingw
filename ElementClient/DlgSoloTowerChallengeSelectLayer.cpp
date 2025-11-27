#include "DlgSoloTowerChallengeSelectLayer.h"

#include "AUIStillImageButton.h"
#include "AUIDialog.h"

#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "AUIDef.h"

#include "DlgSoloTowerChallengeAttention.h"

AUI_BEGIN_COMMAND_MAP(CDlgSoloTowerChallengeSelectLayer, CDlgBase)
AUI_ON_COMMAND("Btn_Close", OnCommand_CANCEL)
AUI_ON_COMMAND("Btn_Confirm", OnCommand_EnterSelectedLayer)
AUI_ON_COMMAND("Btn_Exit", OnCommand_ExitEvent)
AUI_ON_COMMAND("Btn_Layer*", OnCommand_SelectLayer)
AUI_END_COMMAND_MAP()

CDlgSoloTowerChallengeSelectLayer::CDlgSoloTowerChallengeSelectLayer()
{

}

CDlgSoloTowerChallengeSelectLayer::~CDlgSoloTowerChallengeSelectLayer()
{

}

bool CDlgSoloTowerChallengeSelectLayer::OnInitDialog()
{
	// Vars
	m_nSelectedLayer = 0;
	// UI elements
	m_pLblLayer = (PAUILABEL)GetDlgItem("Txt_Stage");
	m_pLblHint = (PAUILABEL)GetDlgItem("Txt_Hint");
	for (int i = 0; i < SOLO_CHALLENGE_MAX_LAYERS; i++) {
		char szName[20];
		sprintf(szName, "Btn_Layer%03d", i + 1);
		m_pBtnLayer[i] = (PAUISTILLIMAGEBUTTON)GetDlgItem(szName);
		m_pBtnLayer[i]->Enable(false);
	}
	m_pBtnConfirm = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_SelectLayer");
	m_pBtnExit = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_ViewAward");
	for (int i = 0; i < 6; i++) {
		char szName[20];
		sprintf(szName, "Gfx_%d", i + 1);
		m_pImgLayersSectionCompleteGfx[i] = (PAUIIMAGEPICTURE)GetDlgItem(szName);
		m_pImgLayersSectionCompleteGfx[i]->Show(false);
	}
	m_pImgAllCompleteGfx = (PAUIIMAGEPICTURE)GetDlgItem("Gfx_All");
	m_pImgAllCompleteGfx->Show(false);
	return true;
}

void CDlgSoloTowerChallengeSelectLayer::OnShowDialog()
{
	CECHostPlayer* pHost = GetHostPlayer();
	// Populate Window
	const CECHostPlayer::SOLO_CHALLENGE_INFO& soloTowerInfo = pHost->GetSoloChallengeInfo();
	
	// Enable interaction with this window
	EnableInteraction(true);
	// Show selected info
	SelectLayer(soloTowerInfo.maxLayerClimbed);

	// Set GFX depending on layers completion
	int amountOfLayersGroupsCompleted = (soloTowerInfo.maxLayerClimbed+1) / 18;
	for (int i = 0; i < amountOfLayersGroupsCompleted; i++) m_pImgLayersSectionCompleteGfx[i]->Show(true);
	if ((soloTowerInfo.maxLayerClimbed + 1) == SOLO_CHALLENGE_MAX_LAYERS) m_pImgAllCompleteGfx->Show(true);
}

/// <summary>
/// This function take cares of UI changes when selecting a layer, and hold variables related to selected layer
/// It is called everytime a player selects a new layer or on OnShowDialog()
/// </summary>
/// <param name="index"></param>
void CDlgSoloTowerChallengeSelectLayer::SelectLayer(int index) {

	// Unselect previous selected button
	m_pBtnLayer[m_nSelectedLayer]->SetPushed(false);
	// Select layer
	m_nSelectedLayer = index;
	// Select current button
	m_pBtnLayer[m_nSelectedLayer]->SetPushed(true);

	// Refresh layer related information in UI
	m_pLblHint->SetText(m_pBtnLayer[m_nSelectedLayer]->GetHint());
	ACString strTemp;
	m_pLblLayer->SetText(strTemp.Format(GetStringFromTable(11550), m_nSelectedLayer + 1));
}

void CDlgSoloTowerChallengeSelectLayer::OnTick() {

	DWORD dwTick = GetTickCount();

	// We check if we sent a Hello message to NPC but he didnt respond. 
	// If it takes more than 3 seconds to respond, we will Enable interaction again so player can try to teleport again.
	// This only happens when player attempted to teleport to selected layer
	if (m_bServiceHelloMsgSent && !m_bServiceHelloMsgReceived) {
		m_dwTimePassedSinceMsgSent += dwTick;
		if (m_dwTimePassedSinceMsgSent > 3000) {
			EnableInteraction(true);
		}
	}
}

/// <summary>
/// Command handler: Selects a layer
/// </summary>
/// <param name="szCommand"></param>
void CDlgSoloTowerChallengeSelectLayer::OnCommand_SelectLayer(const char* szCommand)
{
	int pressedBtnIndex = atoi(szCommand + strlen("Btn_Layer")) - 1;
	if (m_pBtnLayer[pressedBtnIndex]->IsEnabled()) {
		SelectLayer(pressedBtnIndex);
	}
}

/// <summary>
/// Command handler: Enter to instance button
/// </summary>
/// <param name="szCommand"></param>
void CDlgSoloTowerChallengeSelectLayer::OnCommand_EnterSelectedLayer(const char* szCommand)
{
	CECHostPlayer* pHost = GetHostPlayer();
	// Populate Window
	const CECHostPlayer::SOLO_CHALLENGE_INFO& soloTowerInfo = pHost->GetSoloChallengeInfo();
	// Check if there are awards to claim, if so, warn the user these will get lost if it moves to next layer.
	if (soloTowerInfo.awardInfo.drawnItemTimes < soloTowerInfo.awardInfo.totalDrawItemTimes) {
		GetGameUIMan()->m_pDlgSoloTowerChallengeAttention->Show(true);
	}
	else {
		SendHelloToServiceNPC();
	}
}

/// <summary>
/// Command handler: Click on button to exit instance. It teleports to map entrance.
/// </summary>
/// <param name="szCommand"></param>
void CDlgSoloTowerChallengeSelectLayer::OnCommand_ExitEvent(const char* szCommand)
{
	GetGameSession()->c2s_CmdSoloChallengeOperateRequest(5, NULL);
}

/// <summary>
/// This function is called from EC_GameRun.cpp when receive : MSG_SCENE_SERVICE_NPC_LIST
/// This packet is a list of NPCs with "unlimited range" for interaction. 
/// The type of service that it is used to teleport to layers from UI.
/// </summary>
/// <param name="npcNID"></param>
void CDlgSoloTowerChallengeSelectLayer::SetServiceNPCNID(int npcNID)
{
	m_nServiceNPCNID = npcNID;
}

/// <summary>
/// We send a Hello message to server, to m_nServiceNPCNID, which will be handled quietly.
/// It will allow the player to interact with the NPC from remote, without even noticing it.
/// 
/// This function is called when player hit the Enter Stage button, (or whatever is actually called).
/// </summary>
void CDlgSoloTowerChallengeSelectLayer::SendHelloToServiceNPC() {
	
	if (m_nServiceNPCNID != 0) {

		// Prevent interaction
		EnableInteraction(false);

		// Prepare vars to receive response
		m_bServiceHelloMsgReceived = false;
		m_dwTimePassedSinceMsgSent = 0;
		// Send the actual reqest
		GetGameSession()->c2s_CmdNPCSevHello(m_nServiceNPCNID);
		m_bServiceHelloMsgSent = true;
	}
}

/// <summary>
/// It is called from EC_HostMsg.cpp when receiving OnMsgHstNPCGreeting() message.
/// Now we are ready to request to move to stage. (teleport).
/// </summary>
void CDlgSoloTowerChallengeSelectLayer::SetReceivedNPCGreeting() {
	m_bServiceHelloMsgReceived = true;

	if (m_nServiceNPCNID != 0)
		GetGameSession()->c2s_CmdNPCSevSoloChallengeSelectStage(m_nSelectedLayer + 1);

	// Hide current dialog
	Show(false);
}

/// <summary>
/// This function enables and disables user interaction with UI (when in the process of teleporting)
/// Since it requires to contact an NPC, receive a response, things tha can fail or take time
/// </summary>
/// <param name="bEnable"></param>
void CDlgSoloTowerChallengeSelectLayer::EnableInteraction(bool bEnable) {
	
	CECHostPlayer* pHost = GetHostPlayer();
	// Populate Window
	const CECHostPlayer::SOLO_CHALLENGE_INFO& soloTowerInfo = pHost->GetSoloChallengeInfo();

	for (int i = 0; i < SOLO_CHALLENGE_MAX_LAYERS; i++) {
		bool layerEnabled = false;
		if (i <= soloTowerInfo.maxLayerClimbed) {
			//if (i <= 50) { // Test purposes
			layerEnabled = (bEnable) ? true : false;
		}
		m_pBtnLayer[i]->Enable(layerEnabled);
	}

}