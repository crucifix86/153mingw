// Stub implementation for GT Overlay functionality
// GT (GameTracker?) overlay is not available with MinGW build
// This provides the implementation to satisfy the linker when GT SDK is not present

#include <windows.h>
#include "GT/gt_overlay.h"

namespace overlay {

// Static singleton instance
GTOverlay& GTOverlay::Instance() {
    static GTOverlay s_instance;
    return s_instance;
}

GTOverlay::GTOverlay()
    : hook_module_(NULL)
    , overlay_module_(NULL)
    , get_hook_game_(NULL)
    , get_overlay_(NULL)
    , render_type_(kGameRenderDX9)
    , protocal_error_(false)
    , m_bModuleLoaded(false)
    , m_bLogin(false)
    , m_bInTeam(false)
    , m_bInFaction(false)
    , m_bShown(false)
{
    memset(gt_installed_path_, 0, sizeof(gt_installed_path_));
}

GTOverlay::~GTOverlay() {}

// Main thread methods
bool GTOverlay::EnterMain(GameRenderType render_type) {
    // GT not available - return false indicating not loaded
    return false;
}

void GTOverlay::LeaveMain() {}

// Other thread methods
void GTOverlay::EnterCreateGameWindow(HWND game_window) {}
void GTOverlay::LeaveDestroyGameWindow() {}

void GTOverlay::Login(int zone_id, __int64 role_id, const char* passport, const char* token) {
    // GT not available - pretend we're logged in
    m_bLogin = true;
}

void GTOverlay::Logout() {
    m_bLogin = false;
}

void GTOverlay::Toggle(bool on) {
    m_bShown = on;
}

void GTOverlay::EnterTeam(__int64 team_id) {
    m_bInTeam = true;
}

void GTOverlay::LeaveTeam() {
    m_bInTeam = false;
}

void GTOverlay::EnterFaction() {
    m_bInFaction = true;
}

void GTOverlay::LeaveFaction() {
    m_bInFaction = false;
}

void GTOverlay::SetGTWindowPosition(int x, int y) {}

void GTOverlay::ClickGTIcon() {}

// OverlayListener callback methods
void GTOverlay::OnLogin() {
    m_bLogin = true;
}

void GTOverlay::OnLogout() {
    m_bLogin = false;
}

void GTOverlay::OnEnterTeam() {
    m_bInTeam = true;
}

void GTOverlay::OnLeaveTeam() {
    m_bInTeam = false;
}

void GTOverlay::OnEnterFaction() {
    m_bInFaction = true;
}

void GTOverlay::OnLeaveFaction() {
    m_bInFaction = false;
}

} // namespace overlay
