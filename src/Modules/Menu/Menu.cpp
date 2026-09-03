#include "Menu.hpp"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "MinHook.h"
#include <cstdio>
#include <fstream>
#include <string>
#include <sstream>
#include "../../Features/Combat/SpeedHack.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Menu
{
  ConfigData Config;

  std::string GetConfigPath()
  {
    char    path[MAX_PATH];
    HMODULE hMod = GetModuleHandleA("Evitania.dll");
    if (hMod && GetModuleFileNameA(hMod, path, MAX_PATH)) {
      std::string fullPath(path);
      size_t      lastSlash = fullPath.find_last_of("\\/");
      if (lastSlash != std::string::npos) {
        return fullPath.substr(0, lastSlash) + "\\config.txt";
      }
    }
    return "config.txt";
  }

  void ConfigData::LoadConfig()
  {
    std::ifstream f(GetConfigPath());
    if (!f.is_open())
      return;

    std::string line;
    while (std::getline(f, line)) {
      std::istringstream is_line(line);
      std::string        key;
      if (std::getline(is_line, key, '=')) {
        std::string value;
        if (std::getline(is_line, value)) {
          if (key == "god_mode")
            bGodMode = (value == "1");
          else if (key == "god_mode_damage") {
            try {
              fGodModeDamage = std::stof(value);
            } catch (...) {
            }
          }
          else if (key == "god_mode_speed_multiplier")
            fGodModeSpeedMultiplier = std::stof(value);
          else if (key == "fast_mob_spawn")
            bFastMobSpawn = (value == "1");
          else if (key == "fast_gathering")
            bFastGathering = (value == "1");
          else if (key == "aura_kill")
            bAuraKill = (value == "1");
          else if (key == "exp_multiplier")
            bExpMultiplier = (value == "1");
          else if (key == "exp_multiplier_value")
            fExpMultiplierValue = std::stof(value);
          else if (key == "infinite_items")
            bInfiniteItems = (value == "1");
          else if (key == "enhance_item_100")
            bEnhanceItem100 = (value == "1");
          else if (key == "infinite_currency")
            bInfiniteCurrency = (value == "1");
          else if (key == "free_store")
            bFreeStore = (value == "1");
          else if (key == "speed_hack")
            bSpeedHack = (value == "1");
          else if (key == "speed_multiplier")
            fSpeedMultiplier = std::stof(value);
        }
      }
    }
  }

  void ConfigData::SaveConfig()
  {
    std::ofstream out(GetConfigPath());
    if (!out.is_open())
      return;

    out << "menu_open=" << (bMenuOpen ? "1" : "0") << "\n";
    out << "god_mode=" << (bGodMode ? "1" : "0") << "\n";
    out << "god_mode_damage=" << fGodModeDamage << "\n";
    out << "god_mode_speed_multiplier=" << fGodModeSpeedMultiplier << "\n";
    out << "fast_mob_spawn=" << (bFastMobSpawn ? "1" : "0") << "\n";
    out << "fast_gathering=" << (bFastGathering ? "1" : "0") << "\n";
    out << "aura_kill=" << (bAuraKill ? "1" : "0") << "\n";
    out << "exp_multiplier=" << (bExpMultiplier ? "1" : "0") << "\n";
    out << "exp_multiplier_value=" << (long long) fExpMultiplierValue << "\n";
    out << "infinite_items=" << (bInfiniteItems ? "1" : "0") << "\n";
    out << "enhance_item_100=" << (bEnhanceItem100 ? "1" : "0") << "\n";
    out << "infinite_currency=" << (bInfiniteCurrency ? "1" : "0") << "\n";
    out << "free_store=" << (bFreeStore ? "1" : "0") << "\n";
    out << "speed_hack=" << (bSpeedHack ? "1" : "0") << "\n";
    out << "speed_multiplier=" << fSpeedMultiplier << "\n";
  }

  typedef HRESULT(__stdcall* Present_t)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
  Present_t oPresent = nullptr;

  typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
  WNDPROC oWndProc = nullptr;

  HWND                    window               = nullptr;
  ID3D11Device*           pDevice              = nullptr;
  ID3D11DeviceContext*    pContext             = nullptr;
  ID3D11RenderTargetView* mainRenderTargetView = nullptr;
  bool                    init                 = false;

  void CleanupRenderTarget()
  {
    if (mainRenderTargetView) {
      mainRenderTargetView->Release();
      mainRenderTargetView = nullptr;
    }
  }

  void CreateRenderTarget(IDXGISwapChain* pSwapChain)
  {
    ID3D11Texture2D* pBackBuffer;
    pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &pBackBuffer);
    pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
    pBackBuffer->Release();
  }

  static bool IsMouseMessage(UINT uMsg)
  {
    return uMsg == WM_MOUSEMOVE || uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONDOWN
        || uMsg == WM_RBUTTONUP || uMsg == WM_MOUSEWHEEL || uMsg == WM_XBUTTONDOWN || uMsg == WM_XBUTTONUP;
  }

  static bool IsKeyboardMessage(UINT uMsg)
  {
    return uMsg == WM_KEYDOWN || uMsg == WM_KEYUP || uMsg == WM_CHAR || uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP;
  }

  LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    if (uMsg == WM_KEYDOWN && wParam == VK_INSERT) {
      Config.bMenuOpen = !Config.bMenuOpen;
      return 1;
    }

    if (Config.bMenuOpen) {
      ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

      ImGuiIO& io = ImGui::GetIO();

      // Block mouse input to the game ONLY if ImGui wants to capture it (e.g. hovering over the menu)
      if (io.WantCaptureMouse && IsMouseMessage(uMsg)) {
        return 1;
      }

      // Block keyboard input to the game ONLY if ImGui is focused on a text input
      if (io.WantCaptureKeyboard && IsKeyboardMessage(uMsg)) {
        return 1;
      }
    }

    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
  }

  HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
  {
    if (!init) {
      if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**) &pDevice))) {
        pDevice->GetImmediateContext(&pContext);
        DXGI_SWAP_CHAIN_DESC sd;
        pSwapChain->GetDesc(&sd);
        window = sd.OutputWindow;
        CreateRenderTarget(pSwapChain);

        oWndProc = (WNDPROC) SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR) WndProc);

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui_ImplWin32_Init(window);
        ImGui_ImplDX11_Init(pDevice, pContext);

        ImGui::StyleColorsDark();

        init = true;
      }
      else
        return oPresent(pSwapChain, SyncInterval, Flags);
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (Config.bMenuOpen) {
      ImGui::Begin("Evitania Online v" PROJECT_VERSION);

      if (ImGui::CollapsingHeader("Combat")) {
        ImGui::Checkbox("God Mode", &Config.bGodMode);
        if (Config.bGodMode) {
          ImGui::InputFloat("Damage (Attack)", &Config.fGodModeDamage);
          if (Config.fGodModeDamage < 0.0f)
            Config.fGodModeDamage = 0.0f;
          ImGui::SliderFloat("Speed Multiplier##GodMode", &Config.fGodModeSpeedMultiplier, 1.0f, 10.0f);
        }
        ImGui::Checkbox("Fast Mob Spawn", &Config.bFastMobSpawn);
        ImGui::Checkbox("Aura Kill", &Config.bAuraKill);
        ImGui::Checkbox("Exp Multiplier", &Config.bExpMultiplier);
        if (Config.bExpMultiplier) {
          ImGui::InputFloat("Exp Multiplier Amount", &Config.fExpMultiplierValue);
          if (Config.fExpMultiplierValue < 1.0f)
            Config.fExpMultiplierValue = 1.0f;
        }
        if (ImGui::Checkbox("Speed Hack (Global TimeScale)", &Config.bSpeedHack)) {
          Features::SpeedHack::ApplySpeedHack();
        }
        if (Config.bSpeedHack) {
          if (ImGui::SliderFloat("Speed Multiplier##SpeedHack", &Config.fSpeedMultiplier, 1.0f, 10.0f)) {
            Features::SpeedHack::ApplySpeedHack();
          }
        }
      }

      if (ImGui::CollapsingHeader("Economy")) {
        ImGui::Checkbox("Fast Gathering (Mining/Woodcutting)", &Config.bFastGathering);
        ImGui::Checkbox("Infinite Items", &Config.bInfiniteItems);
        ImGui::Checkbox("100% Enhance Item", &Config.bEnhanceItem100);
        ImGui::Checkbox("Infinite Currency (Diamonds, etc.)", &Config.bInfiniteCurrency);
        ImGui::Checkbox("Free Store (IAP Bypass)", &Config.bFreeStore);
      }

      ImGui::Separator();
      if (ImGui::Button("Save Config", ImVec2(-1, 0))) {
        Config.SaveConfig();
      }

      ImGui::Separator();
      ImGui::TextDisabled("[INSERT] to show/hide menu");

      ImGui::End();
    }

    ImGui::Render();

    pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return oPresent(pSwapChain, SyncInterval, Flags);
  }

  void Initialize()
  {
    Config.LoadConfig();

    // Dummy DX11 swap chain creation to get the vtable address of Present
    D3D_FEATURE_LEVEL    featureLevel = D3D_FEATURE_LEVEL_11_0;
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount       = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage       = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    // Find the game's actual window instead of relying on Foreground (which might be the terminal)
    HWND gameWindow = nullptr;
    EnumWindows(
      [](HWND hwnd, LPARAM lParam) -> BOOL {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid == GetCurrentProcessId()) {
          if (GetWindow(hwnd, GW_OWNER) == (HWND) 0 && IsWindowVisible(hwnd)) {
            *(HWND*) lParam = hwnd;
            return FALSE;
          }
        }
        return TRUE;
      },
      (LPARAM) &gameWindow
    );

    sd.OutputWindow     = gameWindow ? gameWindow : GetForegroundWindow();
    sd.SampleDesc.Count = 1;
    sd.Windowed         = TRUE;
    sd.SwapEffect       = DXGI_SWAP_EFFECT_DISCARD;

    if (!sd.OutputWindow) { }

    IDXGISwapChain*      pDummySwapChain = nullptr;
    ID3D11Device*        pDummyDevice    = nullptr;
    ID3D11DeviceContext* pDummyContext   = nullptr;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
      NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &sd, &pDummySwapChain,
      &pDummyDevice, NULL, &pDummyContext
    );

    if (SUCCEEDED(hr) && pDummySwapChain) {

      void** pVTable  = *reinterpret_cast<void***>(pDummySwapChain);
      void*  pPresent = pVTable[8];

      MH_STATUS createStatus = MH_CreateHook(pPresent, (void*) hkPresent, (void**) &oPresent);
      if (createStatus != MH_OK) {
        char buf[64];
        snprintf(buf, sizeof(buf), "MH_CreateHook for Present failed: %d", (int) createStatus);
        MessageBoxA(NULL, buf, "Evitania Error", MB_OK);
      }

      MH_STATUS enableStatus = MH_EnableHook(pPresent);
      if (enableStatus != MH_OK) {
        char buf[64];
        snprintf(buf, sizeof(buf), "MH_EnableHook for Present failed: %d", (int) enableStatus);
        MessageBoxA(NULL, buf, "Evitania Error", MB_OK);
      }

      pDummySwapChain->Release();
      pDummyDevice->Release();
      pDummyContext->Release();
    }
  }

  void Uninitialize()
  {
    if (init) {
      SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR) oWndProc);
      ImGui_ImplDX11_Shutdown();
      ImGui_ImplWin32_Shutdown();
      ImGui::DestroyContext();
      CleanupRenderTarget();
    }
  }
}  // namespace Menu
