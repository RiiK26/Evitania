#include "Menu.hpp"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "MinHook.h"
#include <iostream>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

extern void Log(const char* msg);

namespace Menu
{
  ConfigData Config;

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

  LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    if (uMsg == WM_KEYDOWN && wParam == VK_INSERT) {
      Config.bMenuOpen = !Config.bMenuOpen;
      return 1;
    }

    if (Config.bMenuOpen) {
      if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

      if (
        uMsg == WM_MOUSEMOVE || uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONDOWN
        || uMsg == WM_RBUTTONUP || uMsg == WM_MOUSEWHEEL
      ) {
        return true;  // block input to game while menu open
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
        // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Disabled due to crash/freeze in Proton

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
      ImGui::Begin("Evitania Online - [INSERT] for show/hide menu");

      if (ImGui::CollapsingHeader("Character")) {
        ImGui::Checkbox("God Mode", &Config.bGodMode);
        ImGui::Checkbox("Aura Kill", &Config.bAuraKill);
        ImGui::Checkbox("EXP Multiplier", &Config.bExpMultiplier);
        ImGui::Checkbox("Monster Instant Respawn", &Config.bMonsterInstantRespawn);
        ImGui::Checkbox("100% Enhance Item", &Config.bEnhanceItem100);
        ImGui::Checkbox("Item Magnet", &Config.bMagnet);
      }

      if (ImGui::CollapsingHeader("Economy / Currencies")) {
        ImGui::Checkbox("Infinite Gold/Diamonds", &Config.bInfiniteGold);
        ImGui::Checkbox("Infinite Items", &Config.bInfiniteItems);
        ImGui::Checkbox("Free Crafting / Smelting", &Config.bFreeCrafting);
        ImGui::Checkbox("Discounted Vendor (Free)", &Config.bDiscountedVendor);
        ImGui::Checkbox("Increased Vendor Stock", &Config.bIncreasedVendorStock);
      }

      ImGui::End();
    }

    ImGui::Render();

    pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return oPresent(pSwapChain, SyncInterval, Flags);
  }

  void Initialize()
  {
    Log("Menu::Initialize started.");

    // Dummy DX11 swap chain creation to get the vtable address of Present
    D3D_FEATURE_LEVEL    featureLevel = D3D_FEATURE_LEVEL_11_0;
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount       = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage       = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow      = GetForegroundWindow();
    sd.SampleDesc.Count  = 1;
    sd.Windowed          = TRUE;
    sd.SwapEffect        = DXGI_SWAP_EFFECT_DISCARD;

    if (!sd.OutputWindow) {
      Log("GetForegroundWindow() returned NULL.");
    }

    IDXGISwapChain*      pDummySwapChain = nullptr;
    ID3D11Device*        pDummyDevice    = nullptr;
    ID3D11DeviceContext* pDummyContext   = nullptr;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
      NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &sd, &pDummySwapChain,
      &pDummyDevice, NULL, &pDummyContext
    );

    if (SUCCEEDED(hr) && pDummySwapChain) {
      Log("D3D11CreateDeviceAndSwapChain success.");
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
    else {
      Log("D3D11CreateDeviceAndSwapChain failed.");
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
