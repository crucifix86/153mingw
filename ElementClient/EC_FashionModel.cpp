// Filename	: EC_FashionModel.cpp
// Creator	: Xu Wenbin
// Date		: 2014/5/30

#include "EC_FashionModel.h"

#include "EC_Global.h"

#include "roleinfo"

#include "EC_LoginPlayer.h"
#include "EC_ProfConfigs.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrFashion.h"

#include "elementdataman.h"
#include "itemdataman.h"

extern CECGame * g_pGame;

static const int MIN_CAMERA_DISTANCE = -100;	//	����ƶ�Զ����Χ
static const int MAX_CAMERA_DISTANCE = 82;
static const int MIN_CAMERA_HEIGHT = -100;		//	����ƶ����·�Χ
static const int MAX_CAMERA_HEIGHT = 100;
static const int MIN_CAMERA_LEFTRIGHT = 0;		//	����ƶ����ҷ�Χ
static const int MAX_CAMERA_LEFTRIGHT = 100;
static const int DEFAULT_CAMERA_ANGLE = 0;
static const int DEFAULT_CAMERA_HEIGHT = 0;
static const int DEFAULT_CAMERA_DISTANCE = 20;
static const int DEFAULT_CAMERA_LEFTRIGHT = 50;

//	class CECFashionModel
CECFashionModel::CECFashionModel()
: m_pPlayer(NULL)
, m_bCharLButtonDown(false)
, m_bCharRButtonDown(false)
, m_mouseDownPositionX(0)
, m_mouseDownPositionY(0)
, m_pComputeMoveFunction(NULL)
{
	memset(m_aEquips, 0, sizeof(m_aEquips));
	ResetCamera();
}

CECFashionModel::~CECFashionModel(){
	A3DRELEASE(m_pPlayer);
}

void CECFashionModel::Tick(){
	if (m_pPlayer){
		m_pPlayer->Stand();
		m_pPlayer->Tick(g_pGame->GetTickTime());
	}
}

void CECFashionModel::GenerateEquipsFromHostPlayer(int aEquips[SIZE_ALL_EQUIPIVTR])const{
	//	�� HostPlayer ������ǰʱװ
	//	ͬʱ������������ʱװ������������Ӧ��ʾʱװ�����߼�
	//
	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
	CECInventory *pPack = pHost->GetEquipment();
	for(int i = 0; i < SIZE_EQUIPIVTR; i++ ){
		aEquips[i] = 0;	//	Ĭ�����
		CECIvtrItem *pItem = pPack->GetItem(i);
		if (pItem && pItem->IsEquipment()){
			CECIvtrEquip *pEquip = dynamic_cast<CECIvtrEquip*>(pItem);
			if (pEquip->IsFashion()){
				if (i == EQUIPIVTR_FASHION_WEAPON){
					aEquips[i] = pItem->GetTemplateID();	//	ʱװ����
				}else{
					CECIvtrFashion *pFashion = dynamic_cast<CECIvtrFashion *>(pItem);
					aEquips[i] = GetColoredFashion(pFashion->GetEssence().color, pItem->GetTemplateID());	//	����ʱװ
				}
			}else if (pEquip->IsWeapon()){
				aEquips[i] = pItem->GetTemplateID();		//	����
			}
		}
	}
	if (!aEquips[EQUIPIVTR_FASHION_WEAPON] ||
		!CanShowFashionWeapon(aEquips[EQUIPIVTR_FASHION_WEAPON], aEquips[EQUIPIVTR_WEAPON])){
		aEquips[EQUIPIVTR_WEAPON] = 0;
	}
}

DWORD CECFashionModel::GetColoredFashion(unsigned short color, int id){
	return ((color << 16) | id);
}

int CECFashionModel::GetIDFromColoredFashion(DWORD coloredID){
	return (coloredID & 0x0000ffff);
}

unsigned short CECFashionModel::GetColorFromColoredFashion(DWORD coloredID){
	return (unsigned short)((coloredID >> 16) & 0x0000ffff);
}

bool CECFashionModel::IsFashionWeapon(const FASHION_ESSENCE *pEssence){
	bool result(false);
	while (pEssence){
		DATA_TYPE dataType = DT_INVALID;
		const void *p = g_pGame->GetElementDataMan()->get_data_ptr(pEssence->id_sub_type, ID_SPACE_ESSENCE, dataType);
		if (!p || dataType != DT_FASHION_SUB_TYPE){
			break;
		}
		const FASHION_SUB_TYPE *pSubTypeEssence = static_cast<const FASHION_SUB_TYPE *>(p);
		if (EQUIP_MASK64_FASHION_WEAPON == pSubTypeEssence->equip_fashion_mask){
			result = true;
		}
		break;
	}
	return result;
}

bool CECFashionModel::GetFashionWeaponType(int tid, unsigned int &weaponType){
	bool result(false);
	while (tid > 0){
		DATA_TYPE dataType = DT_INVALID;
		const void * p = g_pGame->GetElementDataMan()->get_data_ptr(tid, ID_SPACE_ESSENCE, dataType);
		if (!p || dataType != DT_FASHION_ESSENCE){
			break;
		}
		const FASHION_ESSENCE *pFashionEssence = static_cast<const FASHION_ESSENCE *>(p);
		if (!IsFashionWeapon(pFashionEssence)){
			break;
		}
		weaponType = pFashionEssence->action_type;
		result = true;
		break;
	}
	return result;
}

bool CECFashionModel::GetWeaponType(int tid, unsigned int &weaponType){
	bool result(false);
	while (tid > 0){
		DATA_TYPE dataType = DT_INVALID;
		const void *p = g_pGame->GetElementDataMan()->get_data_ptr(tid, ID_SPACE_ESSENCE, dataType);
		if (!p || dataType != DT_WEAPON_ESSENCE){
			break;
		}
		const WEAPON_ESSENCE *pEssence = static_cast<const WEAPON_ESSENCE *>(p);
		p = g_pGame->GetElementDataMan()->get_data_ptr(pEssence->id_sub_type, ID_SPACE_ESSENCE, dataType);
		if (!p || dataType != DT_WEAPON_SUB_TYPE){
			break;
		}
		const WEAPON_SUB_TYPE *pSubType = static_cast<const WEAPON_SUB_TYPE *>(p);
		weaponType = pSubType->action_type;
		result = true;
		break;
	}
	return result;
}

bool CECFashionModel::CanShowFashionWeapon(int fashionWeaponID, int weaponID){
	bool result(false);
	while (true){
		unsigned int fashionWeaponType = DEFAULT_ACTION_TYPE;
		if (fashionWeaponID <= 0 ||
			!GetFashionWeaponType(fashionWeaponID, fashionWeaponType)){
			break;
		}
		unsigned int weaponType = DEFAULT_ACTION_TYPE;
		if (weaponID > 0 && !GetWeaponType(weaponID, weaponType)){
			break;
		}
		result = CECPlayer::IsFashionWeaponTypeFit(weaponType, fashionWeaponType);
		break;
	}
	return result;
}

bool CECFashionModel::IsLikeHostPlayer(int profession, int gender)const{
	const GNET::RoleInfo &hostRoleInfo = g_pGame->GetGameRun()->GetSelectedRoleInfo();
	return hostRoleInfo.occupation == profession && hostRoleInfo.gender == gender;
}

bool CECFashionModel::IsLikeHostPlayer()const{
	return IsLikeHostPlayer(GetProfession(), GetGender());
}

bool CECFashionModel::CreatePlayer(int profession, int gender){
	//	�����Ϸ��Լ���
	if (profession < 0 || profession >= NUM_PROFESSION ||
		gender < 0 || gender > NUM_GENDER){
		ASSERT(false);
		return false;
	}
	if (!CECProfConfig::Instance().IsExist(profession, gender)){	//	��Щְҵ�Ա�û�У�����������Ů����
		return false;
	}
	bool likeHostPlayer = IsLikeHostPlayer(profession, gender);
	if (m_pPlayer){
		ResetCamera();
		if (m_pPlayer->GetGender() == gender &&
			m_pPlayer->GetProfession() == profession){
			ClearFashion();
			if (likeHostPlayer){
				GenerateEquipsFromHostPlayer(m_aEquips);
				m_pPlayer->ShowEquipments(m_aEquips, true);
				m_pPlayer->UpdateHairModel(true, m_aEquips[EQUIPIVTR_FASHION_HEAD]);
			}
			return true;
		}
		A3DRELEASE(m_pPlayer);
		memset(m_aEquips, 0, sizeof(m_aEquips));
	}

	//	���ɴ�����ɫ�����ɫ��Ϣ
	GNET::RoleInfo roleInfo;
	roleInfo.race = CECProfConfig::Instance().GetRaceByProfession(profession);
	roleInfo.occupation = profession;
	roleInfo.gender = gender;
	
	const GNET::RoleInfo &hostRoleInfo = g_pGame->GetGameRun()->GetSelectedRoleInfo();
	CECPlayer::PLAYER_CUSTOMIZEDATA custom_data;
	if (likeHostPlayer){
		//	��ѡ�е�ְҵ���Ա�͵�ǰHostPlayerһ��ʱ��

		//	�������Ի�������
		roleInfo.custom_data = hostRoleInfo.custom_data;

		//	����ʱװ������ΪĬ��ʱװ
		GenerateEquipsFromHostPlayer(m_aEquips);
	}
	if (roleInfo.custom_data.size() == 0){
		if (CECPlayer::LoadDefaultCustomizeData(profession, gender, custom_data)){
			GNET::Octets tempOctets((const void *)&custom_data, sizeof(custom_data));
			roleInfo.custom_data.swap(tempOctets);
		}
	}
	
	m_pPlayer = new CECLoginPlayer(g_pGame->GetGameRun()->GetWorld()->GetPlayerMan());	
	if (!m_pPlayer->Load(roleInfo)){
		A3DRELEASE(m_pPlayer);
		return false;
	}else{		
		m_pPlayer->SetFashionMode(true);
		m_pPlayer->SetPos(A3DVECTOR3(0));
		m_pPlayer->ChangeModelMoveDirAndUp(A3DVECTOR3(0, 0, -1.0f), A3DVECTOR3(0, 1.0f, 0));
		m_pPlayer->GetPlayerModel()->SetAutoUpdateFlag(false);
		m_pPlayer->ShowEquipments(m_aEquips, true);
		m_pPlayer->UpdateHairModel(true, m_aEquips[EQUIPIVTR_FASHION_HEAD]);
		return true;
	}
}

int	CECFashionModel::GetProfession()const{
	const CECLoginPlayer *pPlayer = m_pPlayer;
	return pPlayer ? const_cast<CECLoginPlayer *>(pPlayer)->GetProfession() : -1;
}

int CECFashionModel::GetGender()const{
	const CECLoginPlayer *pPlayer = m_pPlayer;
	return pPlayer ? const_cast<CECLoginPlayer *>(pPlayer)->GetGender() : -1;
}

//	�ж�ָ��ID��Ʒ�Ƿ��ǿ��Դ�ʱװ
bool CECFashionModel::CanFit(int tid)const{
	bool bCan(false);
	while (m_pPlayer && tid > 0){
		DATA_TYPE dataType = DT_INVALID;
		const void * p = g_pGame->GetElementDataMan()->get_data_ptr(tid, ID_SPACE_ESSENCE, dataType);
		if (!p || dataType != DT_FASHION_ESSENCE){
			break;
		}
		const FASHION_ESSENCE *pEssence = static_cast<const FASHION_ESSENCE *>(p);
		if (pEssence->gender != m_pPlayer->GetGender()){	//	�Ա�
			break;
		}
		if (IsFashionWeapon(pEssence)){	//	ʱװ�������ְҵ����
			if (!(pEssence->character_combo_id & (1 << m_pPlayer->GetProfession()))){
				break;
			}
		}
		if (pEssence->require_level && pEssence->require_level > g_pGame->GetGameRun()->GetHostPlayer()->GetMaxLevelSofar()){
			break;	//	ʹ�� HostPlayer �ĵȼ����жϣ��������ܴ���
		}
		bCan = true;
		break;
	}
	return bCan;
}

//	�Ƿ����ʹ�õ�ɫ���ɫ
bool CECFashionModel::CanChangeColor()const{
	return m_aEquips[EQUIPIVTR_FASHION_BODY] != 0 ||
		m_aEquips[EQUIPIVTR_FASHION_LEG] != 0 ||
		m_aEquips[EQUIPIVTR_FASHION_FOOT] != 0 ||
		m_aEquips[EQUIPIVTR_FASHION_WRIST] != 0;
}

//	��ȡĳʱװ�ĵ�ǰ��ɫ�����ڵ�ɫ�屣��ͻָ�
bool CECFashionModel::GetFashionColor(int equipSlot, unsigned short &color)const{
	if (!m_pPlayer){
		return false;
	}
	if (EQUIPIVTR_FASHION_BODY != equipSlot &&
		EQUIPIVTR_FASHION_LEG != equipSlot &&
		EQUIPIVTR_FASHION_FOOT != equipSlot &&
		EQUIPIVTR_FASHION_WRIST != equipSlot){
		return false;
	}
	if (m_aEquips[equipSlot]){
		color = GetColorFromColoredFashion(m_aEquips[equipSlot]);
		return true;
	}
	return false;
}

bool CECFashionModel::GetFashionBestColor(int equipSlot, unsigned short &color)const{
	if (!m_pPlayer){
		return false;
	}
	if (EQUIPIVTR_FASHION_BODY != equipSlot &&
		EQUIPIVTR_FASHION_LEG != equipSlot &&
		EQUIPIVTR_FASHION_FOOT != equipSlot &&
		EQUIPIVTR_FASHION_WRIST != equipSlot){
		return false;
	}
	if (m_aEquips[equipSlot]){
		int tid = (m_aEquips[equipSlot] & 0x0000ffff);
		return FindBestColorFor(tid, color);
	}
	return false;
}

//	������ʱװ
bool CECFashionModel::HasFashionOn()const{
	return m_pPlayer &&
		(m_aEquips[EQUIPIVTR_FASHION_HEAD] != 0 ||
		m_aEquips[EQUIPIVTR_FASHION_BODY] != 0 ||
		m_aEquips[EQUIPIVTR_FASHION_LEG] != 0 ||
		m_aEquips[EQUIPIVTR_FASHION_FOOT] != 0 ||
		m_aEquips[EQUIPIVTR_FASHION_WRIST] != 0 ||
		m_aEquips[EQUIPIVTR_FASHION_WEAPON] != 0);
}

CECLoginPlayer * CECFashionModel::GetPlayer(){
	return m_pPlayer;
}

//	����ָ��IDʱװ�������ʾɫ������߹�������
bool CECFashionModel::FindBestColorFor(int tid, unsigned short &bestColor){
	bool bFound(false);
	
	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	DATA_TYPE dataType = DT_INVALID;
	unsigned int configID = pDataMan->get_first_data_id(ID_SPACE_CONFIG, dataType);
	while (configID){
		if (dataType == DT_FASHION_BEST_COLOR_CONFIG){
			const FASHION_BEST_COLOR_CONFIG *pConfig = (const FASHION_BEST_COLOR_CONFIG *)pDataMan->get_data_ptr(configID,ID_SPACE_CONFIG, dataType);
			for (int i(0); i < ARRAY_SIZE(pConfig->list); ++ i){
				if ((int)pConfig->list[i].id == tid){
					bestColor = (unsigned short)FASHION_A3DCOLOR_TO_WORDCOLOR(pConfig->list[i].color);
					bFound = true;
					break;
				}
			}
			if(bFound){
				break;
			}
		}
		configID = pDataMan->get_next_data_id(ID_SPACE_CONFIG, dataType);
	}
	
	return bFound;
}

//	����ʱװ ID ����䰲װ��λ�ã������ж��Ƿ���ʱװ������ʱװ
int	CECFashionModel::GetFashionEquipSlot(int tid){
	int equipSlot = -1;
	while (true){		
		elementdataman *pDataMan = g_pGame->GetElementDataMan();
		DATA_TYPE dataType = DT_INVALID;
		const void * p = pDataMan->get_data_ptr(tid, ID_SPACE_ESSENCE, dataType);
		if (dataType != DT_FASHION_ESSENCE){
			break;
		}
		const FASHION_ESSENCE *pEssence = static_cast<const FASHION_ESSENCE *>(p);
		p = pDataMan->get_data_ptr(pEssence->id_sub_type, ID_SPACE_ESSENCE, dataType);
		if (!p || dataType != DT_FASHION_SUB_TYPE){
			ASSERT(false);
			break;
		}	
		const FASHION_SUB_TYPE *pSubTypeEssence = static_cast<const FASHION_SUB_TYPE *>(p);
		switch (pSubTypeEssence->equip_fashion_mask){
		case EQUIP_MASK64_FASHION_HEAD:
			equipSlot = EQUIPIVTR_FASHION_HEAD;
			break;
		case EQUIP_MASK64_FASHION_BODY:
			equipSlot = EQUIPIVTR_FASHION_BODY;
			break;
		case EQUIP_MASK64_FASHION_LEG:
			equipSlot = EQUIPIVTR_FASHION_LEG;
			break;
		case EQUIP_MASK64_FASHION_FOOT:
			equipSlot = EQUIPIVTR_FASHION_FOOT;
			break;
		case EQUIP_MASK64_FASHION_WRIST:
			equipSlot = EQUIPIVTR_FASHION_WRIST;
			break;
		case EQUIP_MASK64_FASHION_WEAPON:
			equipSlot = EQUIPIVTR_FASHION_WEAPON;
			break;
		}
		break;
	}
	return equipSlot;
}

//	��ָ��IDʱװ�Դ���ģ������
bool CECFashionModel::Fit(int tid){
	if (!CanFit(tid)){
		return false;
	}
	int equipSlot = GetFashionEquipSlot(tid);
	if (equipSlot < 0){
		return false;
	}
	int equipID = -1;
	DATA_TYPE dataType = DT_INVALID;
	const void * p = g_pGame->GetElementDataMan()->get_data_ptr(tid, ID_SPACE_ESSENCE, dataType);
	const FASHION_ESSENCE *pEssence = static_cast<const FASHION_ESSENCE *>(p);
	if (EQUIPIVTR_FASHION_WEAPON == equipSlot){
		equipID = pEssence->id;
		if (equipID == m_aEquips[equipSlot]){
			return true;
		}
	}else{
		unsigned short color = 0;
		if (!FindBestColorFor(tid, color)){
			//	�޷����ҵ����ɫ����ʱ������ģ�����������ɫ
			float h = abase::Rand(pEssence->h_min, pEssence->h_max);
			float s = abase::Rand(pEssence->s_min, pEssence->s_max);
			float v = abase::Rand(pEssence->v_min, pEssence->v_max);
			int a3dColor = hsv2rgb(h,s,v);	
			color = FASHION_A3DCOLOR_TO_WORDCOLOR(a3dColor);
		}
		equipID = GetColoredFashion(color, pEssence->id);
		if (equipID == m_aEquips[equipSlot]){
			return true;
		}else if ((int)pEssence->id == (m_aEquips[equipSlot] & 0x0000ffff)){
			ChangeFashionColor(equipSlot, color);
			return true;
		}
	}
	m_aEquips[equipSlot] = equipID;
	if (equipSlot == EQUIPIVTR_FASHION_WEAPON && IsLikeHostPlayer()){
		//	�뵱ǰ���ְҵ�Ա���ͬʱ��ʹ��ʱװ����ǰ���ȼ��ص�ǰ�����������ʵ����ʾʱװ�����߼�
		CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
		CECInventory *pPack = pHost->GetEquipment();
		m_aEquips[EQUIPIVTR_WEAPON] = 0;
		if (CECIvtrItem *pItem = pPack->GetItem(EQUIPIVTR_WEAPON)){
			if (CanShowFashionWeapon(equipID, pItem->GetTemplateID())){
				m_aEquips[EQUIPIVTR_WEAPON] = pItem->GetTemplateID();
			}
		}
	}
	m_pPlayer->ShowEquipments(m_aEquips, true);
	if (EQUIPIVTR_FASHION_HEAD == equipSlot){
		m_pPlayer->UpdateHairModel(true, equipID);
	}
	return true;
}

//	�޸�ָ��λ��ʱװ����ɫ
bool CECFashionModel::ChangeFashionColor(int equipSlot, unsigned short newColor){
	if (!m_pPlayer){
		ASSERT(false);
		return false;
	}
	if (EQUIPIVTR_FASHION_BODY != equipSlot &&
		EQUIPIVTR_FASHION_LEG != equipSlot &&
		EQUIPIVTR_FASHION_FOOT != equipSlot &&
		EQUIPIVTR_FASHION_WRIST != equipSlot){
		return false;
	}
	if (!m_pPlayer->ChangeFashionColor(equipSlot, newColor)){
		return false;
	}else{
		m_aEquips[equipSlot] = GetColoredFashion(newColor, GetIDFromColoredFashion(m_aEquips[equipSlot]));
		return true;
	}
}

//	������ϵ�ʱװ���װ��
void CECFashionModel::ClearFashion(){
	m_aEquips[EQUIPIVTR_WEAPON] = 0;	//	����ʼ�����
	if (!HasFashionOn()){
		return;
	}
	m_aEquips[EQUIPIVTR_FASHION_BODY] = 0;
	m_aEquips[EQUIPIVTR_FASHION_LEG] = 0;
	m_aEquips[EQUIPIVTR_FASHION_FOOT] = 0;
	m_aEquips[EQUIPIVTR_FASHION_WRIST] = 0;
	m_aEquips[EQUIPIVTR_FASHION_WEAPON] = 0;
	if (m_aEquips[EQUIPIVTR_FASHION_HEAD]){
		m_aEquips[EQUIPIVTR_FASHION_HEAD] = 0;
		m_pPlayer->UpdateHairModel(true, 0);
	}
	m_pPlayer->ShowEquipments(m_aEquips, true);
}

void CECFashionModel::ClearFashion(int equipSlot) {
	if (EQUIPIVTR_FASHION_BODY != equipSlot &&
		EQUIPIVTR_FASHION_LEG != equipSlot &&
		EQUIPIVTR_FASHION_FOOT != equipSlot &&
		EQUIPIVTR_FASHION_WRIST != equipSlot &&
		EQUIPIVTR_FASHION_WEAPON != equipSlot &&
		EQUIPIVTR_FASHION_HEAD != equipSlot){
		return;
	}
	m_aEquips[equipSlot] = 0;
	if (EQUIPIVTR_FASHION_HEAD == equipSlot) {
		m_pPlayer->UpdateHairModel(true, 0);
	}
	m_pPlayer->ShowEquipments(m_aEquips, true);
}

void CECFashionModel::ClampCamera(){
	a_Clamp(m_nCameraDistanceChange, MIN_CAMERA_DISTANCE, MAX_CAMERA_DISTANCE);
	a_Clamp(m_nCameraHeightChange, MIN_CAMERA_HEIGHT, MAX_CAMERA_HEIGHT);
	a_Clamp(m_nCameraLeftRightChange, MIN_CAMERA_LEFTRIGHT, MAX_CAMERA_LEFTRIGHT);
}

void CECFashionModel::ResetCamera(){
	m_nAngle = DEFAULT_CAMERA_ANGLE;
	m_nCameraHeightChange = DEFAULT_CAMERA_HEIGHT;
	m_nCameraDistanceChange = DEFAULT_CAMERA_DISTANCE;
	m_nCameraLeftRightChange = DEFAULT_CAMERA_LEFTRIGHT;
	
	m_nAngleAtButtonDown = m_nAngle;
	m_nCameraHeightChangeAtButtonDown = m_nCameraHeightChange;
	m_nCameraLeftRightChangeAtButtonDown = m_nCameraLeftRightChange;
}

bool CECFashionModel::IsCameraDefault()const{
	return DEFAULT_CAMERA_ANGLE == m_nAngle
		&& DEFAULT_CAMERA_HEIGHT == m_nCameraHeightChange
		&& DEFAULT_CAMERA_DISTANCE == m_nCameraDistanceChange
		&& DEFAULT_CAMERA_LEFTRIGHT == m_nCameraLeftRightChange;
}

int	CECFashionModel::GetAngle()const{
	return m_nAngle;
}

int CECFashionModel::ComposeCameraParameter()const{
	int nHeight = (static_cast<char>(m_nCameraHeightChange) & 0xff);
	return ((m_nCameraLeftRightChange & 0xff) << 16) | ((m_nCameraDistanceChange & 0xff) << 8) | (nHeight & 0xff);
}

bool CECFashionModel::OnEventLButtonDown(int x, int y){
	if (!GetPlayer()){
		return false;
	}
	if (m_bCharRButtonDown || m_bCharLButtonDown){
		return false;
	}
	m_bCharLButtonDown = true;
	m_mouseDownPositionX = x;
	m_mouseDownPositionY = y;
	
	//	���浱ǰ���λ�ã��� MouseMove ��ʹ�ã��Ա����ۻ����
	m_nCameraHeightChangeAtButtonDown = m_nCameraHeightChange;
	m_nCameraLeftRightChangeAtButtonDown = m_nCameraLeftRightChange;
	return true;
}

bool CECFashionModel::OnEventLButtonUp(int x, int y){
	if (m_bCharLButtonDown){
		m_bCharLButtonDown = false;
		return true;
	}
	return false;
}

bool CECFashionModel::OnEventRButtonDown(int x, int y){
	if (!GetPlayer()){
		return false;
	}
	if (m_bCharLButtonDown || m_bCharRButtonDown){
		return false;
	}
	m_bCharRButtonDown = true;
	m_mouseDownPositionX = x;
	m_mouseDownPositionY = y;
	
	//	���浱ǰ��תλ�ã��� MouseMove ��ʹ�ã��Ա����ۻ����
	m_nAngleAtButtonDown = m_nAngle;
	return true;
}

bool CECFashionModel::OnEventRButtonUp(int x, int y){
	if (m_bCharRButtonDown){
		m_bCharRButtonDown = false;
		return true;
	}
	return false;
}

//	��֪���� [begin, end] ��Ӧ������ [scaleBegin, scaleEnd]�������� pos ��Ӧ scale
static float MapScaleRatio(int begin, int end, float scaleBegin, float scaleEnd, int pos){
	a_Clamp(pos, begin, end);
	float r = (pos-begin)/(float)(end-begin);
	return scaleBegin + r*(scaleEnd - scaleBegin);
}

bool CECFashionModel::OnEventMouseMove(int x, int y){
	if (!GetPlayer()){
		return false;
	}
	if (!m_bCharLButtonDown && !m_bCharRButtonDown){
		return false;
	}
	A3DPOINT2 newPos;
	newPos.x = x;
	newPos.y = y;
	int dx = newPos.x - m_mouseDownPositionX;
	int dy = newPos.y - m_mouseDownPositionY;
	if (m_bCharLButtonDown){
		if (m_pComputeMoveFunction == NULL) {
			//	�����ƶ�������ֵ
			float xRatio = MapScaleRatio(MIN_CAMERA_DISTANCE, MAX_CAMERA_DISTANCE, 0.5f, 0.1f, m_nCameraDistanceChange);
			float xMove = dx * xRatio;
			if (xMove > 0){
				xMove = (int)(xMove + 0.5f);	//	��������Ϊ����
			}else if (xMove < 0){
				xMove = (int)(xMove - 0.5f);
			}
			m_nCameraLeftRightChange = m_nCameraLeftRightChangeAtButtonDown - (int)xMove;

			//	�����ƶ�������ֵ
			float yRatio = MapScaleRatio(MIN_CAMERA_DISTANCE, MAX_CAMERA_DISTANCE, 1.0f, 0.2f, m_nCameraDistanceChange);
			float yMove = dy * yRatio;
			if (yMove > 0){
				yMove = (int)(yMove + 0.5f);	//	��������Ϊ����
			}else if (yMove < 0){
				yMove = (int)(yMove - 0.5f);
			}
			m_nCameraHeightChange = m_nCameraHeightChangeAtButtonDown + (int)yMove;
		} else {
			int xMove, yMove;
			m_pComputeMoveFunction->ComputeMoveParam(dx, dy, xMove, yMove);
			m_nCameraLeftRightChange = m_nCameraLeftRightChangeAtButtonDown + xMove;
			m_nCameraHeightChange = m_nCameraHeightChangeAtButtonDown + yMove;
		}
	}else{
		m_nAngle = (m_nAngleAtButtonDown + 360 + dx) % 360;	//	�Ҽ���ת
		if (m_nAngle < 0) {
			m_nAngle += 360;
		}
	}
	ClampCamera();
	return true;
}

bool CECFashionModel::OnEventMouseWheel(int zDelta){
	if (!GetPlayer()){
		return false;
	}
	if (m_bCharLButtonDown || m_bCharRButtonDown){
		return false;
	}
	m_nCameraDistanceChange += zDelta/120*6;
	ClampCamera();
	return true;
}

void CECFashionModel::SetMoveParamFunction(CECComputeMoveParamFunction * pFunction) {
	m_pComputeMoveFunction = pFunction;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

CECGeneralComputeMoveParamFunction::CECGeneralComputeMoveParamFunction(int viewportWidth, int viewportHeight, bool isMale, CECFashionModel* pModel)
: m_iViewportWidth(viewportWidth), m_iViewportHeight(viewportHeight), m_isMale(isMale), m_pModel(pModel) {

}

void CECGeneralComputeMoveParamFunction::ComputeMoveParam(int screenDx, int screenDy, int &cameraLeftRightChange, int &cameraHeightChange) {
	float fDefaultZ = m_isMale ? 9.2f : 8.4f;
	float fMinZ = 1.0f;
	int iDistanceChange = (char)((m_pModel->ComposeCameraParameter() >> 8) & 0xff);
	float fCurZ = (fDefaultZ - fMinZ) / fMinZ * (100.0f - iDistanceChange) / 100.0f + fMinZ;
	
	float fWidthAtMinZ = 0.27f;
	float fHeightAtMinZ = fWidthAtMinZ * m_iViewportHeight / m_iViewportWidth;

	cameraLeftRightChange = -fWidthAtMinZ / m_iViewportWidth * 50.0f * screenDx * fCurZ + 0.5f;
	cameraHeightChange = fHeightAtMinZ / m_iViewportHeight * 100.0f * screenDy * fCurZ + 0.5f;
}