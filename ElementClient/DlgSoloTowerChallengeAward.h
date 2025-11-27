#pragma once

#include "DlgBase.h"
#include "AUIStillImageButton.h"
#include "AUILabel.h"
#include "AUIImagePicture.h"

enum
{
	SOLO_CHALLENGE_PRIZES_COUNT = 8,
	SOLO_CHALLENGE_REWARD_ICONS_COUNT = 10,
};

/*
* This class is a Helper class that is in change of animating "cards" (prize slots)
* in the Awards Dialog.
* 
*/
class CardDealer {

	PAUIIMAGEPICTURE* _Cards; // Cards to deal
	int _AmountCards;

	struct {
		A3DVECTOR3 originPos, targetPos, dirVector;
		int magnitude;
	} m_DrawItemAnimationDataVec[SOLO_CHALLENGE_PRIZES_COUNT];

	int m_nItemMoving; // This is the index of the slot moving during animation
	DWORD m_dwLastTickTime;
	int m_nMillisPassed; // Milliseconds recorded used for animation

	bool _Dealing;
	bool _Dealed;

	int _DealingAnimationTimePerCard = 200;
	int _CardsBackToCenterAnimationTime = 1000;

public:
	CardDealer(PAUIIMAGEPICTURE* items, int itemsSize)
		: _Cards(items), _AmountCards(itemsSize)
	{

	}

	~CardDealer() {}

	void Init()
	{
		_Dealing = false;
		_Dealed = false;
		m_nMillisPassed = 0;
		m_nItemMoving = 0;

		for (int i = 0; i < _AmountCards; i++) {

			// When opening the window and there is prizes left to claim, 
			// it executes an animation like dealing cards
			// We prepare that information in here
			POINT targetPos = _Cards[i]->GetPos(true);
			m_DrawItemAnimationDataVec[i].originPos = { 477,235,0 }; // Origin position
			m_DrawItemAnimationDataVec[i].targetPos = { (float)targetPos.x, (float)targetPos.y,0 }; // Target position
			m_DrawItemAnimationDataVec[i].dirVector = m_DrawItemAnimationDataVec[i].targetPos - m_DrawItemAnimationDataVec[i].originPos;
			m_DrawItemAnimationDataVec[i].magnitude = m_DrawItemAnimationDataVec[i].dirVector.Normalize();

			_Cards[i]->SetPos(m_DrawItemAnimationDataVec[i].originPos.x, m_DrawItemAnimationDataVec[i].originPos.y); // Middle of the window

			_Cards[i]->Show(true);

		}
	}

	void PerformDeal() {
		m_dwLastTickTime = GetTickCount();
		m_nItemMoving = 0;
		m_nMillisPassed = 0;
		_Dealing = true;
		for (int i = 0; i < _AmountCards; i++) {
			_Cards[i]->Show(true);
		}
	}

	void Reset()
	{
		_Dealing = false;
		_Dealed = false;
		for (int i = 0; i < _AmountCards; i++) {
			_Cards[i]->SetPos(m_DrawItemAnimationDataVec[i].originPos.x, m_DrawItemAnimationDataVec[i].originPos.y); // Middle of the window
			_Cards[i]->Show(true);
		}
	}

	void OnTick() {

		if (!_Dealing) return;

		// Deal cards
		DWORD dwTick = GetTickCount();
		DWORD timePassed = (dwTick >= m_dwLastTickTime) ? (dwTick - m_dwLastTickTime) : 0;

		// If there is less than 20ms between ticks, wait for next tick
		if (timePassed < 20) return;

		m_nMillisPassed += timePassed;

		float animationTime = (_Dealed) ? _CardsBackToCenterAnimationTime : _DealingAnimationTimePerCard;
		float animationFactor = m_nMillisPassed / animationTime;
		a_Clamp(animationFactor, 0.f, 1.f);

		// Get cards back to center before dealing again
		if (_Dealed) {

			animationFactor = 1 - animationFactor; // Invert card animation direction by inverting factor

			A3DVECTOR3 newPos;
			for (int i = 0; i < _AmountCards; i++) {

				newPos = m_DrawItemAnimationDataVec[i].originPos
					+ m_DrawItemAnimationDataVec[i].dirVector
					* (animationFactor * m_DrawItemAnimationDataVec[i].magnitude);

				_Cards[i]->SetPos(newPos.x, newPos.y);
			}

			// Check if we need to move to the next element
			if (m_nMillisPassed >= animationTime) {
				_Dealed = false;
				m_nMillisPassed = 0;
			}
		}
		else {
			// Calculate next position (newPos = curPos + offset_for_1_ms * ms_passed)
			A3DVECTOR3 newPos = m_DrawItemAnimationDataVec[m_nItemMoving].originPos
				+ m_DrawItemAnimationDataVec[m_nItemMoving].dirVector
				* (animationFactor * m_DrawItemAnimationDataVec[m_nItemMoving].magnitude);

			// Set actual UI image position
			_Cards[m_nItemMoving]->SetPos(newPos.x, newPos.y);

			// Check if we need to move to the next element
			if (m_nMillisPassed >= animationTime) {
				m_nItemMoving++;
				m_nMillisPassed = 0;
			}

			// Check if we finished animation
			if (m_nItemMoving >= 8) {
				_Dealed = true;
				_Dealing = false;
				m_nItemMoving = 0;
			}
		}

		// Save current tick for next loop
		m_dwLastTickTime = dwTick;

		return;
	}

	bool IsDealing() { return _Dealing; }
};

/*
* This Dialog class is in charge of Awards in Solo Tower Challenge
*/
class CDlgSoloTowerChallengeAward : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
public:
	CDlgSoloTowerChallengeAward();
	virtual ~CDlgSoloTowerChallengeAward();

	void OnEventLDown_DrawItem(WPARAM wParam, LPARAM lParam, AUIObject* pObj);
	void DoDrawItem();
	void AwardReceivedFromServer(int itemId, int itemCount, int totalAwardsClaimed);
	void ShowPrize();
	void ClearItemIcon(int index);
	void SetItemIcon(int index, int itemId, int itemCount);
	ACString GetTimeStringFromSeconds(int seconds);

protected:

	PAUILABEL m_pTxtLayer; // Text displaying current layer (stage)
	PAUILABEL m_pTxtClimbedLayerTime; // Text displaying time player completed current stage
	PAUILABEL m_pTxtTotalTime; // Text displaying total time for completing up to this stage
	PAUILABEL m_pTxtLeftDrawTimes; // Text displaying amount of prizes left to claim
	PAUISTILLIMAGEBUTTON Btn_Close;
	PAUIIMAGEPICTURE m_pImgDrawItem[8]; // Question marks
	PAUIIMAGEPICTURE m_pImgDrawnItemGfx[8]; // Gfx when a slot gets picked
	PAUIIMAGEPICTURE m_pImgDrawnItem[10]; // Item icons of items drawn
	PAUIIMAGEPICTURE m_pImgJustDrawnItem; // Item that just got drawn from the last request
	PAUIIMAGEPICTURE m_ImgFront1; // background image related to item Just Drawn
	PAUIIMAGEPICTURE m_ImgGfxAward; // GFX effect when showing the award in the middle

	CardDealer* m_DealerHelper; // Cards animation helper class

	bool m_nLastLayerClaimed; // Last completed layer (stage)
	int m_dwLastTickTime; // keeps last tick time
	int m_nMillisShowPrize; // ms to show prize animation

	bool m_bSelectedSlots[8]; // This array contains which slots were already claimed
	int m_nLastSelectedSlot; // Last selected slot

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();
};
