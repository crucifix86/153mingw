#pragma once

#include "DlgBase.h"
#include "AUIStillImageButton.h"
#include "AUICheckBox.h"
#include "AUILabel.h"
#include "AUITextArea.h"
#include "AUIImagePicture.h"

enum
{
	SOLO_CHALLENGE_MAX_LAYERS = 108,
};

/// <summary>
/// This Dialog class is in charge of layer (stage) selection.
/// In order to handle teleport, it creates a connection with a "unlimited range" NPC service.
/// This works by sending a "Hello" message to an NPC which is out of range, and server will send a "greetings" response message.
/// Once the connection is made with the NPC, we request the NPC to teleport us to a given layer.
/// </summary>
class CDlgSoloTowerChallengeSelectLayer : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP();
public:
	CDlgSoloTowerChallengeSelectLayer();
	virtual ~CDlgSoloTowerChallengeSelectLayer();

	void OnCommand_SelectLayer(const char* szCommand);
	void OnCommand_EnterSelectedLayer(const char* szCommand);
	void OnCommand_ExitEvent(const char* szCommand);

	void SelectLayer(int index); // Update info related to selected layer

	void SetServiceNPCNID(int npcNID);
	void SendHelloToServiceNPC();
	void SetReceivedNPCGreeting();

protected:

	PAUILABEL m_pLblLayer; // Label showing layer selected name
	PAUILABEL m_pLblHint; // Layer showing layer selected description (or long name actually)
	PAUISTILLIMAGEBUTTON m_pBtnLayer[SOLO_CHALLENGE_MAX_LAYERS]; // Buttons for every layer (stage)
	PAUISTILLIMAGEBUTTON m_pBtnConfirm; // Enter layer (stage) button
	PAUISTILLIMAGEBUTTON m_pBtnExit; // Exit event
	PAUIIMAGEPICTURE m_pImgLayersSectionCompleteGfx[6]; // GFX when completing a group of 18 layers
	PAUIIMAGEPICTURE m_pImgAllCompleteGfx; // GFX effect when all levels are complete

	int m_nSelectedLayer; // Selected layer
	int m_nServiceNPCNID; // It contains the NPCNID (unique NPC ID on runtime). We populate this information once received after entering this map.
	bool m_bServiceHelloMsgReceived; // Flag indicating server sent the client a "greetings" response from NPC. We are OK to make a teleport to selected layer.
	bool m_bServiceHelloMsgSent; // Flag indicating we sent a hello message to server NPC.

	DWORD m_dwTimePassedSinceMsgSent; // It keep track of time passed since we sent the hello message. Used as a safe measure if server do not respond on time.
	

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

private:
	void EnableInteraction(bool bEnable);
};
