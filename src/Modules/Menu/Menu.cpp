#include "Menu.hpp"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "MinHook.h"
#include <cstdio>
#include <fstream>
#include <string>
#include <sstream>
#include "../../Cores/skCrypter.h"
#include "../../Features/Combat/SpeedHack.hpp"
#include "Logger.hpp"
#include <shellapi.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Menu
{
  ConfigData         Config;
  static std::string g_IniPath;

  std::string GetConfigPath()
  {
    char    path[MAX_PATH];
    HMODULE hMod = GetModuleHandleA(skCrypt("Evitania.dll"));
    if (hMod && GetModuleFileNameA(hMod, path, MAX_PATH)) {
      std::string fullPath(path);
      size_t      lastSlash = fullPath.find_last_of("\\/");
      if (lastSlash != std::string::npos) {
        return fullPath.substr(0, lastSlash) + skCrypt("\\config.txt");
      }
    }
    return skCrypt("config.txt");
  }

  static float SafeParseFloat(const std::string& value, float defaultValue)
  {
    try {
      return std::stof(value);
    } catch (...) {
      return defaultValue;
    }
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
          else if (key == "god_mode_nullify")
            bGodMode_Nullify = (value == "1");
          else if (key == "god_mode_damage_toggle")
            bGodMode_Damage = (value == "1");
          else if (key == "god_mode_speed_toggle")
            bGodMode_Speed = (value == "1");
          else if (key == "god_mode_damage")
            fGodModeDamage = SafeParseFloat(value, fGodModeDamage);
          else if (key == "god_mode_speed_multiplier")
            fGodModeSpeedMultiplier = SafeParseFloat(value, fGodModeSpeedMultiplier);
          else if (key == "aura_kill")
            bAuraKill = (value == "1");
          else if (key == "exp_multiplier")
            bExpMultiplier = (value == "1");
          else if (key == "exp_multiplier_value")
            fExpMultiplierValue = SafeParseFloat(value, fExpMultiplierValue);
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
            fSpeedMultiplier = SafeParseFloat(value, fSpeedMultiplier);
          else if (key == "hourglass_bypass")
            bHourglassBypass = (value == "1");
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
    out << "god_mode_nullify=" << (bGodMode_Nullify ? "1" : "0") << "\n";
    out << "god_mode_damage_toggle=" << (bGodMode_Damage ? "1" : "0") << "\n";
    out << "god_mode_speed_toggle=" << (bGodMode_Speed ? "1" : "0") << "\n";
    out << "god_mode_damage=" << fGodModeDamage << "\n";
    out << "god_mode_speed_multiplier=" << fGodModeSpeedMultiplier << "\n";
    out << "aura_kill=" << (bAuraKill ? "1" : "0") << "\n";
    out << "exp_multiplier=" << (bExpMultiplier ? "1" : "0") << "\n";
    out << "exp_multiplier_value=" << (long long) fExpMultiplierValue << "\n";
    out << "infinite_items=" << (bInfiniteItems ? "1" : "0") << "\n";
    out << "enhance_item_100=" << (bEnhanceItem100 ? "1" : "0") << "\n";
    out << "infinite_currency=" << (bInfiniteCurrency ? "1" : "0") << "\n";
    out << "free_store=" << (bFreeStore ? "1" : "0") << "\n";
    out << "speed_hack=" << (bSpeedHack ? "1" : "0") << "\n";
    out << "speed_multiplier=" << fSpeedMultiplier << "\n";
    out << "hourglass_bypass=" << (bHourglassBypass ? "1" : "0") << "\n";
  }

  typedef HRESULT(__stdcall* Present_t)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
  Present_t oPresent = nullptr;

  typedef HRESULT(__stdcall* ResizeBuffers_t)(
    IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags
  );
  ResizeBuffers_t oResizeBuffers = nullptr;

  typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
  WNDPROC oWndProc                             = nullptr;

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

  HRESULT __stdcall hkResizeBuffers(
    IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags
  )
  {
    CleanupRenderTarget();
    HRESULT hr = oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
    CreateRenderTarget(pSwapChain);
    return hr;
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

        char    path[MAX_PATH];
        HMODULE hMod = GetModuleHandleA(skCrypt("Evitania.dll"));
        if (hMod && GetModuleFileNameA(hMod, path, MAX_PATH)) {
          std::string fullPath(path);
          size_t      lastSlash = fullPath.find_last_of("\\/");
          if (lastSlash != std::string::npos) {
            g_IniPath      = fullPath.substr(0, lastSlash) + skCrypt("\\imgui.ini");
            io.IniFilename = g_IniPath.c_str();
          }
        }

        ImGui_ImplWin32_Init(window);
        ImGui_ImplDX11_Init(pDevice, pContext);

        ImGui::StyleColorsDark();

        // --- Apply Styling ---
        ImGuiStyle& style       = ImGui::GetStyle();
        style.WindowRounding    = 8.0f;
        style.FrameRounding     = 6.0f;
        style.PopupRounding     = 6.0f;
        style.ScrollbarRounding = 6.0f;
        style.GrabRounding      = 6.0f;
        style.TabRounding       = 6.0f;

        style.WindowPadding     = ImVec2(12, 12);
        style.FramePadding      = ImVec2(8, 4);
        style.ItemSpacing       = ImVec2(8, 8);
        style.ItemInnerSpacing  = ImVec2(6, 6);

        // Custom Dark/Vibrant Palette
        ImVec4* colors                    = style.Colors;
        colors[ImGuiCol_WindowBg]         = ImVec4(0.08f, 0.08f, 0.09f, 0.96f);
        colors[ImGuiCol_Header]           = ImVec4(0.18f, 0.18f, 0.20f, 1.00f);
        colors[ImGuiCol_HeaderHovered]    = ImVec4(0.24f, 0.24f, 0.26f, 1.00f);
        colors[ImGuiCol_HeaderActive]     = ImVec4(0.30f, 0.30f, 0.32f, 1.00f);
        colors[ImGuiCol_Button]           = ImVec4(0.20f, 0.25f, 0.30f, 1.00f);
        colors[ImGuiCol_ButtonHovered]    = ImVec4(0.26f, 0.35f, 0.44f, 1.00f);
        colors[ImGuiCol_ButtonActive]     = ImVec4(0.36f, 0.45f, 0.54f, 1.00f);
        colors[ImGuiCol_FrameBg]          = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
        colors[ImGuiCol_FrameBgHovered]   = ImVec4(0.18f, 0.18f, 0.20f, 1.00f);
        colors[ImGuiCol_FrameBgActive]    = ImVec4(0.24f, 0.24f, 0.26f, 1.00f);
        colors[ImGuiCol_CheckMark]        = ImVec4(0.30f, 0.65f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrab]       = ImVec4(0.30f, 0.65f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.38f, 0.73f, 1.00f, 1.00f);
        colors[ImGuiCol_TitleBg]          = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
        colors[ImGuiCol_TitleBgActive]    = ImVec4(0.15f, 0.15f, 0.18f, 1.00f);

        init                              = true;
      }
      else
        return oPresent(pSwapChain, SyncInterval, Flags);
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (Config.bMenuOpen) {
      ImGui::SetNextWindowSize(ImVec2(650, 650), ImGuiCond_FirstUseEver);
      ImGui::Begin(
        "Evitania Online v" PROJECT_VERSION, nullptr,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
      );

      if (ImGui::BeginTabBar("CheatTabs")) {
        if (ImGui::BeginTabItem("Features")) {
          ImGui::BeginChild("FeaturesChild", ImVec2(0, -30), false, 0);
          if (ImGui::CollapsingHeader("Combat", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Checkbox("God Mode", &Config.bGodMode);
            ImGui::SameLine();
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered()) {
              ImGui::SetTooltip("Enable to use the granular God Mode settings below.");
            }

            if (ImGui::TreeNode("God Mode Settings")) {
              // Disable individual toggles if master God Mode is off, but still show them
              ImGui::BeginDisabled(!Config.bGodMode);

              ImGui::Checkbox("Nullify Damage (Infinite HP)", &Config.bGodMode_Nullify);

              ImGui::Checkbox("High Damage", &Config.bGodMode_Damage);
              if (Config.bGodMode_Damage) {
                ImGui::InputFloat("Damage Value", &Config.fGodModeDamage);
                if (Config.fGodModeDamage < 0.0f)
                  Config.fGodModeDamage = 0.0f;
              }

              ImGui::Checkbox("Movement Speed", &Config.bGodMode_Speed);
              if (Config.bGodMode_Speed) {
                ImGui::SliderFloat("Speed Multiplier##GodMode", &Config.fGodModeSpeedMultiplier, 1.0f, 10.0f);
              }

              ImGui::EndDisabled();
              ImGui::TreePop();
            }
            ImGui::Checkbox("Aura Kill", &Config.bAuraKill);
            ImGui::Checkbox("Exp Multiplier", &Config.bExpMultiplier);
            if (Config.bExpMultiplier) {
              ImGui::InputFloat("Exp Multiplier Amount", &Config.fExpMultiplierValue);
              if (Config.fExpMultiplierValue < 1.0f)
                Config.fExpMultiplierValue = 1.0f;
            }
            if (ImGui::Checkbox("Speed Hack (Global time scale)", &Config.bSpeedHack)) {
              Features::SpeedHack::ApplySpeedHack();
            }
            if (Config.bSpeedHack) {
              if (ImGui::SliderFloat("Speed Multiplier##SpeedHack", &Config.fSpeedMultiplier, 1.0f, 10.0f)) {
                Features::SpeedHack::ApplySpeedHack();
              }
            }
          }

          if (ImGui::CollapsingHeader("Economy")) {
            ImGui::Checkbox("Infinite items on inventory", &Config.bInfiniteItems);
            ImGui::Checkbox("100% Success enhance Item", &Config.bEnhanceItem100);
            ImGui::Checkbox("Infinite currency (Diamonds, Golds, Sands, etc)", &Config.bInfiniteCurrency);
            ImGui::Checkbox("Free Store (IAP Bypass)", &Config.bFreeStore);
            ImGui::Checkbox("Use Hourglass (Timeskip) anywhere", &Config.bHourglassBypass);
          }

          ImGui::Spacing();
          ImGui::Separator();
          if (ImGui::Button("Save Config", ImVec2(-1, 0))) {
            Config.SaveConfig();
          }
          ImGui::Spacing();

          ImGui::EndChild();
          ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Logs")) {
          ImGui::BeginChild("LogsChild", ImVec2(0, -30), false, 0);
          Menu::Logger::Draw();
          ImGui::EndChild();
          ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(skCrypt("Donate"))) {
          ImGui::BeginChild("DonateChild", ImVec2(0, -30), false, ImGuiWindowFlags_HorizontalScrollbar);
          ImGui::TextWrapped("%s", skCrypt("Support the development! Your contributions help keep the project alive."));
          ImGui::Spacing();

          float halfWidth = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;
          if (ImGui::Button(skCrypt("GitHub Sponsor"), ImVec2(halfWidth, 0))) {
            ShellExecuteA(
              NULL, skCrypt("open"), skCrypt("https://github.com/sponsors/RiiK26"), NULL, NULL, SW_SHOWNORMAL
            );
          }
          ImGui::SameLine();
          if (ImGui::Button(skCrypt("Donate via PayPal"), ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            ShellExecuteA(
              NULL, skCrypt("open"), skCrypt("https://www.paypal.com/paypalme/MuhamadSyakir"), NULL, NULL, SW_SHOWNORMAL
            );
          }

          ImGui::Spacing();
          ImGui::Separator();
          ImGui::Spacing();
          ImGui::TextUnformatted(skCrypt("Crypto Addresses"));
          ImGui::Spacing();

          if (
            ImGui::BeginTable(
              skCrypt("CryptoTable"), 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX
            )
          ) {
            ImGui::TableSetupColumn(skCrypt("Currency"), ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn(skCrypt("Address (Click to Copy)"), ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            auto drawCryptoRow = [](const char* name, const char* address) {
              ImGui::TableNextRow();
              ImGui::TableSetColumnIndex(0);
              ImGui::TextUnformatted(name);
              ImGui::TableSetColumnIndex(1);

              ImGui::PushID(name);
              if (ImGui::Button(skCrypt("Copy"))) {
                ImGui::SetClipboardText(address);
              }
              ImGui::PopID();
              ImGui::SameLine();
              ImGui::TextUnformatted(address);
            };

            drawCryptoRow(skCrypt("Bitcoin"), skCrypt("bc1qpp50c2wuz5n2rq9jy3fxdmte7smcwu5rnegu6q"));
            drawCryptoRow(skCrypt("Ethereum"), skCrypt("0xbF16e9cC4F75Dcd5c1DaD4443b9d8348eC196592"));
            drawCryptoRow(skCrypt("Tether (USDT)"), skCrypt("0xbF16e9cC4F75Dcd5c1DaD4443b9d8348eC196592"));
            drawCryptoRow(skCrypt("BNB"), skCrypt("0xbF16e9cC4F75Dcd5c1DaD4443b9d8348eC196592"));
            drawCryptoRow(skCrypt("XRP"), skCrypt("rnDnG9QBce7sbmY86HXqBUpcCq6LN3xfXg"));
            drawCryptoRow(skCrypt("USDC"), skCrypt("0xbF16e9cC4F75Dcd5c1DaD4443b9d8348eC196592"));
            drawCryptoRow(skCrypt("Solana"), skCrypt("FLFVbCaYQWoPrm9rH1WLuoYmiVnC5CFWoCPzkNk2vzy2"));
            drawCryptoRow(skCrypt("Tron"), skCrypt("TYNLxxQWERit64uNo8dSQX3CxdLmomtAq7"));
            drawCryptoRow(skCrypt("Dogecoin"), skCrypt("DFwJXDQsqPEdpnWhXCMmBWMru9iqYqWhDn"));
            drawCryptoRow(
              skCrypt("Cardano"), skCrypt(
                                    "addr1qyhapduj2uvu8x4ct75hujtsx63uq375cxda8m25xgn7wep06zmey4cecwdtshaf0eyhqd4rcpraf"
                                    "svm60k4gv38uajq2z2l6a"
                                  )
            );
            drawCryptoRow(skCrypt("Litecoin"), skCrypt("LMNbzEJ3M4qtAxsYyxMBkT4rzjSBJvUc2M"));
            drawCryptoRow(skCrypt("Avalanche"), skCrypt("0xbF16e9cC4F75Dcd5c1DaD4443b9d8348eC196592"));
            drawCryptoRow(skCrypt("Polkadot"), skCrypt("15FtRzNuwpbNAjvMzRCegqDwN7cgw2a44ogfobdS7UmNACFr"));
            drawCryptoRow(skCrypt("Polygon"), skCrypt("0xbF16e9cC4F75Dcd5c1DaD4443b9d8348eC196592"));
            drawCryptoRow(skCrypt("Cosmos"), skCrypt("cosmos1ashcczkgj884t9gert2se4m4zw5ea7t2fa4qh9"));
            drawCryptoRow(skCrypt("X0 Cash"), skCrypt("FLFVbCaYQWoPrm9rH1WLuoYmiVnC5CFWoCPzkNk2vzy2"));

            ImGui::EndTable();
          }
          ImGui::EndChild();
          ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
      }

      ImGui::Separator();
      ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "%s", skCrypt("[INSERT] show/hide menu"));

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

    HRESULT hr                           = D3D11CreateDeviceAndSwapChain(
      NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &sd, &pDummySwapChain,
      &pDummyDevice, NULL, &pDummyContext
    );

    if (SUCCEEDED(hr) && pDummySwapChain) {

      void** pVTable         = *reinterpret_cast<void***>(pDummySwapChain);
      void*  pPresent        = pVTable[8];

      MH_STATUS createStatus = MH_CreateHook(pPresent, (void*) hkPresent, (void**) &oPresent);
      if (createStatus != MH_OK) {
        char buf[64];
        snprintf(buf, sizeof(buf), skCrypt("MH_CreateHook for Present failed: %d"), (int) createStatus);
        MessageBoxA(NULL, buf, skCrypt("Evitania Error"), MB_OK);
      }

      MH_STATUS enableStatus = MH_EnableHook(pPresent);
      if (enableStatus != MH_OK) {
        char buf[64];
        snprintf(buf, sizeof(buf), skCrypt("MH_EnableHook for Present failed: %d"), (int) enableStatus);
        MessageBoxA(NULL, buf, skCrypt("Evitania Error"), MB_OK);
      }

      void*     pResizeBuffers = pVTable[13];
      MH_STATUS createStatusRB = MH_CreateHook(pResizeBuffers, (void*) hkResizeBuffers, (void**) &oResizeBuffers);
      if (createStatusRB == MH_OK) {
        MH_EnableHook(pResizeBuffers);
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
