// Stub implementations for CELForest and CELTree (SpeedTree-based forest system)
// SpeedTree SDK library (SpeedTreeRT.lib) is proprietary and not available for MinGW build
// This provides stub implementations of all CELForest and CELTree methods

#include "CCommon/EL_Forest.h"
#include "CCommon/EL_Tree.h"

// CELTree implementation stubs
CELTree::CELTree()
    : m_pA3DDevice(nullptr)
    , m_pLogFile(nullptr)
    , m_pForest(nullptr)
    , m_bBadTree(true)
    , m_dwTypeID(0)
    , m_dwIDNext(0)
    , m_pSpeedTree(nullptr)
    , m_pTextureInfo(nullptr)
    , m_pGeometryCache(nullptr)
    , m_pTextureComposite(nullptr)
    , m_pTextureBranch(nullptr)
    , m_nBranchVertCount(0)
    , m_pBranchIndexCount(nullptr)
    , m_pBranchVerts(nullptr)
    , m_pBranchIndices(nullptr)
    , m_pBranchStream(nullptr)
    , m_nFrondVertCount(0)
    , m_pFrondIndexCount(nullptr)
    , m_pFrondVerts(nullptr)
    , m_pFrondIndices(nullptr)
    , m_pFrondStream(nullptr)
    , m_nCurLeavesCount(0)
    , m_nCurBillboardCount(0)
    , m_nNumCDObject(0)
    , m_ppConvexHulls(nullptr)
    , m_nNumCDBrushes(0)
    , m_ppCDBrushes(nullptr)
{
    memset(m_szTreeName, 0, sizeof(m_szTreeName));
    memset(m_szSPTFile, 0, sizeof(m_szSPTFile));
    memset(m_szCompositeMap, 0, sizeof(m_szCompositeMap));
    memset(m_szBranchMap, 0, sizeof(m_szBranchMap));
#ifdef TREE_BUMP_ENABLE
    memset(m_szBranchNormalMap, 0, sizeof(m_szBranchNormalMap));
    m_pTextureBranchNormal = nullptr;
#endif
    memset(m_afBoundingBox, 0, sizeof(m_afBoundingBox));
}

CELTree::~CELTree() {
    Release();
}

bool CELTree::Init(A3DDevice* pA3DDevice, CELForest* pForest, DWORD dwTypeID, const char* szTreeFile, const char* szCompositeMap, ALog* pLogFile) {
    m_pA3DDevice = pA3DDevice;
    m_pForest = pForest;
    m_dwTypeID = dwTypeID;
    m_pLogFile = pLogFile;
    m_bBadTree = true; // SpeedTree not available
    return true;
}

bool CELTree::Release() {
    return true;
}

bool CELTree::ReComputeAllTrees() { return true; }
bool CELTree::LoadTrees(AFile* pFileToLoad) { return true; }
bool CELTree::SaveTrees(AFile* pFileToSave) { return true; }
bool CELTree::UpdateForRender(A3DViewport* pViewport) { return true; }
bool CELTree::RenderBranches(A3DViewport* pViewport) { return true; }
bool CELTree::RenderFronds(A3DViewport* pViewport) { return true; }
bool CELTree::DrawLeavesAndBillboardsToBuffer(TREE_VERTEX1* pVerts, int nMaxBlade, int& nBladeCount, int& nTree) {
    nBladeCount = 0;
    nTree = 0;
    return true;
}
bool CELTree::GetFrondsData() { return true; }
bool CELTree::GetLeafData() { return true; }
bool CELTree::GetBillboardData() { return true; }

bool CELTree::AddTree(const A3DVECTOR3& vecPos, DWORD& dwID) {
    dwID = 0;
    return false; // SpeedTree not available
}

bool CELTree::DeleteTree(DWORD dwID) {
    return false;
}

bool CELTree::GetTreeAABB(DWORD dwID, A3DAABB& aabb) {
    return false;
}

bool CELTree::AdjustTreePosition(DWORD dwID, const A3DVECTOR3& vecPos) {
    return false;
}

bool CELTree::ReleaseAllTrees() { return true; }
int CELTree::GetTreeIndexByID(DWORD dwID) { return -1; }
bool CELTree::CreateVertexBuffers() { return true; }
bool CELTree::ReleaseVertexBuffers() { return true; }
bool CELTree::CreateBranchVertexBuffer() { return true; }
bool CELTree::ReleaseBranchVertexBuffer() { return true; }
bool CELTree::CreateFrondVertexBuffer() { return true; }
bool CELTree::ReleaseFrondVertexBuffer() { return true; }
bool CELTree::InitCDObjects() { return true; }
bool CELTree::ReleaseCDObjects() { return true; }

bool CELTree::RayTraceBranchMesh(const A3DVECTOR3& vecLocalStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float* pvFraction, A3DVECTOR3& vecNormal) {
    return false;
}

bool CELTree::RayTraceConvexHull(const A3DVECTOR3& vecStart, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float* pvFraction, A3DVECTOR3& vecNormal) {
    return false;
}

bool CELTree::TraceWithBrush(BrushTraceInfo* pInfo) {
    return false;
}

bool CELTree::SphereCollideWithMe(const A3DVECTOR3& vecStart, float fRadius, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float* pvFraction, A3DVECTOR3& vecNormal) {
    return false;
}

bool CELTree::DrawLeavesToBufferForShadowMap(TREE_VERTEX1* pVerts, int nMaxBlade, int& nBladeCount, int& nTree) {
    nBladeCount = 0;
    nTree = 0;
    return true;
}


// CELForest implementation stubs
CELForest::CELForest()
    : m_pA3DDevice(nullptr)
    , m_pDirLight(nullptr)
    , m_pLogFile(nullptr)
    , m_vTime(0.0f)
    , m_nTreeCount(0)
    , m_nMaxBlades(0)
    , m_nBladesCount(0)
    , m_pStream(nullptr)
    , m_vLODLevel(1.0f)
#ifdef TREE_BUMP_ENABLE
    , m_pBranchPixelShader(nullptr)
    , m_pBranchVertexShader(nullptr)
#endif
{
    memset(m_szConfigFile, 0, sizeof(m_szConfigFile));
    m_vecLightDir = A3DVECTOR3(0.0f, -1.0f, 0.0f);
}

CELForest::~CELForest() {
    Release();
}

bool CELForest::Init(A3DDevice* pA3DDevice, A3DLight* pDirLight, ALog* pLogFile) {
    m_pA3DDevice = pA3DDevice;
    m_pDirLight = pDirLight;
    m_pLogFile = pLogFile;
    return true;
}

bool CELForest::Release() {
    // Release all tree types
    for (int i = 0; i < m_TreeTypes.GetSize(); i++) {
        if (m_TreeTypes[i]) {
            m_TreeTypes[i]->Release();
            delete m_TreeTypes[i];
        }
    }
    m_TreeTypes.RemoveAll();
    return true;
}

bool CELForest::Update(DWORD dwDeltaTime) {
    return true;
}

bool CELForest::Render(A3DViewport* pViewport) {
    return true;
}

bool CELForest::Load(const char* szForestFile) {
    return true;
}

bool CELForest::Save(const char* szForestFile) {
    return true;
}

bool CELForest::SetLODLevel(float level) {
    m_vLODLevel = level;
    return true;
}

float CELForest::GetWindStrength(const A3DVECTOR3& vecPos) {
    return 0.0f;
}

CELTree* CELForest::AddTreeType(DWORD dwTypeID, const char* szTreeFile, const char* szCompositeMap) {
    return nullptr; // SpeedTree not available
}

bool CELForest::DeleteTreeType(CELTree* pTreeType) {
    return false;
}

CELTree* CELForest::GetTreeTypeByID(DWORD dwTypeID) {
    for (int i = 0; i < m_TreeTypes.GetSize(); i++) {
        if (m_TreeTypes[i] && m_TreeTypes[i]->GetTypeID() == dwTypeID) {
            return m_TreeTypes[i];
        }
    }
    return nullptr;
}

bool CELForest::SphereCollideWithMe(const A3DVECTOR3& vecStart, float fRadius, const A3DVECTOR3& vecDelta, A3DVECTOR3& vecHitPos, float* pvFraction, A3DVECTOR3& vecNormal) {
    return false;
}

bool CELForest::TraceWithBrush(BrushTraceInfo* pInfo) {
    return false;
}

bool CELForest::AddTree(int nIndex, const A3DVECTOR3& vPos) {
    return false;
}

bool CELForest::DeleteAllTreeTypes() {
    return Release();
}

int CELForest::GetTreeTypeNumber() {
    return m_TreeTypes.GetSize();
}

bool CELForest::LoadTypesFromConfigFile(const char* szConfigFile) {
    if (szConfigFile) {
        strncpy(m_szConfigFile, szConfigFile, sizeof(m_szConfigFile) - 1);
    }
    return true;
}

bool CELForest::SortTreeTypeByCompositeMap() {
    return true;
}

bool CELForest::CreateStream(int nMaxBlades) {
    m_nMaxBlades = nMaxBlades;
    return true;
}

bool CELForest::ReleaseStream() {
    m_pStream = nullptr;
    return true;
}
