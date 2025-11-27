#include "DlgSoloTowerChallengeAttention.h"
#include "DlgInfo.h"

#include "AUIStillImageButton.h"

#include "EC_GameUIMan.h"

#include "DlgSoloTowerChallengeSelectLayer.h"
#include "DlgSoloTowerChallengeAward.h"

AUI_BEGIN_COMMAND_MAP(CDlgSoloTowerChallengeAttention, CDlgBase)
AUI_ON_COMMAND("Btn_Delete", OnCommand_SkipAwards)
AUI_ON_COMMAND("Btn_Award", OnCommand_GetAwards)
AUI_END_COMMAND_MAP()



CDlgSoloTowerChallengeAttention::CDlgSoloTowerChallengeAttention()
{

}

CDlgSoloTowerChallengeAttention::~CDlgSoloTowerChallengeAttention()
{

}

bool CDlgSoloTowerChallengeAttention::OnInitDialog()
{
	m_pBtnSkipAwards = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Delete");
	m_pBtnGetAwards = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Award");
	return true;
}

void CDlgSoloTowerChallengeAttention::OnShowDialog()
{
}

void CDlgSoloTowerChallengeAttention::OnTick() {


}

void CDlgSoloTowerChallengeAttention::OnCommand_SkipAwards(const char* szCommand)
{
	GetGameUIMan()->RemoveInformation(CDlgInfo::INFO_SOLOCHALLENGEREWARD, _AL(""));
	GetGameUIMan()->m_pDlgSoloTowerChallengeSelectLayer->SendHelloToServiceNPC();
	Show(false);
}
void CDlgSoloTowerChallengeAttention::OnCommand_GetAwards(const char* szCommand)
{
	GetGameUIMan()->m_pDlgSoloTowerChallengeAward->Show(true);
	Show(false);
}
