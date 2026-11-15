/*
 * MIT License
 *
 * Copyright © 2026 - Present RiiK
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Menu.hpp"
#include "MenuInternal.hpp"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_dx12.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include "MinHook.h"
#include <cstdio>
#include <mutex>
#include <string>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Menu
{
  static std::string          g_IniPath;
  static std::mutex           g_InsertMutex;
  static std::recursive_mutex g_RenderMutex;
  static std::recursive_mutex g_ImGuiMutex;

  typedef HRESULT(__stdcall* Present_t)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
  Present_t oPresent    = nullptr;
  Present_t oPresentAlt = nullptr;

  typedef HRESULT(__stdcall* Present1_t)(
    IDXGISwapChain1* pSwapChain, UINT SyncInterval, UINT PresentFlags, const DXGI_PRESENT_PARAMETERS* pPresentParameters
  );
  Present1_t oPresent1    = nullptr;
  Present1_t oPresent1Alt = nullptr;

  typedef HRESULT(__stdcall* ResizeBuffers_t)(
    IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags
  );
  ResizeBuffers_t oResizeBuffers    = nullptr;
  ResizeBuffers_t oResizeBuffersAlt = nullptr;

  typedef HRESULT(__stdcall* ResizeBuffers1_t)(
    IDXGISwapChain3* pSwapChain,
    UINT             BufferCount,
    UINT             Width,
    UINT             Height,
    DXGI_FORMAT      Format,
    UINT             SwapChainFlags,
    const UINT*      pCreationNodeMask,
    IUnknown* const* ppPresentQueue
  );
  ResizeBuffers1_t oResizeBuffers1    = nullptr;
  ResizeBuffers1_t oResizeBuffers1Alt = nullptr;

  static void* g_HookedPresent        = nullptr;
  static void* g_HookedPresent1       = nullptr;
  static void* g_HookedResizeBuffers  = nullptr;
  static void* g_HookedResizeBuffers1 = nullptr;

  typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
  WNDPROC oWndProc                                          = nullptr;

  HWND                    window                            = nullptr;
  bool                    g_WindowIsUnicode                 = true;
  IDXGISwapChain*         g_pSwapChain                      = nullptr;
  ID3D11Device*           pDevice                           = nullptr;
  ID3D11DeviceContext*    pContext                          = nullptr;
  ID3D11RenderTargetView* mainRenderTargetView              = nullptr;
  bool                    init                              = false;

  ID3D12Device*              g_pd3dDevice                   = nullptr;
  ID3D12DescriptorHeap*      g_pd3dRtvDescHeap              = nullptr;
  ID3D12DescriptorHeap*      g_pd3dSrvDescHeap              = nullptr;
  ID3D12CommandQueue*        g_pd3dCommandQueue             = nullptr;
  ID3D12GraphicsCommandList* g_pd3dCommandList              = nullptr;
  ID3D12CommandAllocator*    g_commandAllocators[16]        = {};
  ID3D12Resource*            g_mainRenderTargetResource[16] = {};
  UINT                       g_NUM_FRAMES_IN_FLIGHT         = 16;
  bool                       initDX12                       = false;

  static bool      s_bInsertKeyDown                         = false;
  static ULONGLONG s_LastWndProcInsertMs                    = 0;
  static bool      s_WasMenuOpen                            = true;
  static bool      s_ImGuiMayHoldCapture                    = false;

  typedef void(__stdcall* ExecuteCommandLists_t)(
    ID3D12CommandQueue* queue, UINT NumCommandLists, ID3D12CommandList* const* ppCommandLists
  );
  ExecuteCommandLists_t oExecuteCommandLists = nullptr;

  bool GetSwapChainDesc(IDXGISwapChain* pSwapChain, DXGI_SWAP_CHAIN_DESC& sd)
  {
    ZeroMemory(&sd, sizeof(sd));
    if (SUCCEEDED(pSwapChain->GetDesc(&sd))) {
      return true;
    }

    IDXGISwapChain1* pSwapChain1 = nullptr;
    if (SUCCEEDED(pSwapChain->QueryInterface(__uuidof(IDXGISwapChain1), (void**) &pSwapChain1))) {
      DXGI_SWAP_CHAIN_DESC1 sd1;
      if (SUCCEEDED(pSwapChain1->GetDesc1(&sd1))) {
        sd.BufferCount       = sd1.BufferCount;
        sd.BufferDesc.Format = sd1.Format;
        sd.BufferDesc.Width  = sd1.Width;
        sd.BufferDesc.Height = sd1.Height;
        sd.SampleDesc        = sd1.SampleDesc;
        sd.BufferUsage       = sd1.BufferUsage;
        sd.SwapEffect        = sd1.SwapEffect;
        sd.Flags             = sd1.Flags;
        pSwapChain1->GetHwnd(&sd.OutputWindow);
        pSwapChain1->Release();
        return true;
      }
      pSwapChain1->Release();
    }
    return false;
  }

  void CleanupRenderTargetDX12()
  {
    for (UINT i = 0; i < 16; i++) {
      if (g_mainRenderTargetResource[i]) {
        g_mainRenderTargetResource[i]->Release();
        g_mainRenderTargetResource[i] = nullptr;
      }
    }
  }

  void CreateRenderTargetDX12(IDXGISwapChain* pSwapChain, ID3D12Device* pDev)
  {
    if (!pSwapChain || !pDev || !g_pd3dRtvDescHeap)
      return;

    DXGI_SWAP_CHAIN_DESC sd;
    if (GetSwapChainDesc(pSwapChain, sd) && sd.BufferCount > 0) {
      g_NUM_FRAMES_IN_FLIGHT = (sd.BufferCount <= 16) ? sd.BufferCount : 16;
    }

    const UINT incSize = pDev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    for (UINT i = 0; i < g_NUM_FRAMES_IN_FLIGHT; i++) {
      ID3D12Resource* pBackBuffer = nullptr;
      if (SUCCEEDED(pSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**) &pBackBuffer)) && pBackBuffer) {
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
        rtvHandle.ptr += (SIZE_T) i * incSize;
        pDev->CreateRenderTargetView(pBackBuffer, nullptr, rtvHandle);
        g_mainRenderTargetResource[i] = pBackBuffer;
      }
    }
  }

  void CleanupRenderTarget()
  {
    if (mainRenderTargetView) {
      mainRenderTargetView->Release();
      mainRenderTargetView = nullptr;
    }
  }

  void CreateRenderTarget(IDXGISwapChain* pSwapChain)
  {
    if (!pSwapChain || !pDevice)
      return;

    ID3D11Texture2D* pBackBuffer = nullptr;
    if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &pBackBuffer)) && pBackBuffer) {
      pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
      pBackBuffer->Release();
    }
  }

  static void EnsureRenderTargetDX11(IDXGISwapChain* pSwapChain)
  {
    if (!pSwapChain || !pDevice)
      return;

    ID3D11Texture2D* pBackBuffer = nullptr;
    if (FAILED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &pBackBuffer)) || !pBackBuffer) {
      return;
    }

    ID3D11Resource* pCurrentResource = nullptr;
    if (mainRenderTargetView) {
      mainRenderTargetView->GetResource(&pCurrentResource);
    }

    if (pCurrentResource != pBackBuffer) {
      CleanupRenderTarget();
      pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
    }

    if (pCurrentResource)
      pCurrentResource->Release();
    pBackBuffer->Release();
  }

  static bool IsMouseMessage(UINT uMsg) { return uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST; }

  static bool IsKeyboardMessage(UINT uMsg)
  {
    return uMsg == WM_KEYDOWN || uMsg == WM_KEYUP || uMsg == WM_CHAR || uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP
        || uMsg == WM_IME_CHAR || uMsg == WM_IME_COMPOSITION;
  }

  static bool ShouldForwardHiddenMessage(UINT uMsg)
  {
    return uMsg == WM_SETFOCUS || uMsg == WM_KILLFOCUS || uMsg == WM_DISPLAYCHANGE || uMsg == WM_SETTINGCHANGE
        || uMsg == WM_DEVICECHANGE || uMsg == WM_DPICHANGED;
  }

  static bool IsProcessWindowFocused()
  {
    HWND fg = GetForegroundWindow();
    if (!fg)
      return false;
    DWORD fgPid = 0;
    GetWindowThreadProcessId(fg, &fgPid);
    return fgPid == GetCurrentProcessId();
  }

  static void PollInsertKeyFallback()
  {
    const bool      physDown = (GetAsyncKeyState(VK_INSERT) & 0x8000) != 0;
    const ULONGLONG now      = GetTickCount64();

    std::lock_guard<std::mutex> lock(g_InsertMutex);
    // Let WndProc drive toggling when it recently saw VK_INSERT to avoid keyup-lag double toggles
    if (now - s_LastWndProcInsertMs <= 150) {
      return;
    }

    if (physDown) {
      if (!s_bInsertKeyDown && IsProcessWindowFocused()) {
        Config.bMenuOpen = !Config.bMenuOpen.load();
      }
      s_bInsertKeyDown = true;
    }
    else {
      s_bInsertKeyDown = false;
    }
  }

  LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  static WNDPROC HookWindowProc(HWND targetWindow)
  {
    g_WindowIsUnicode = (IsWindowUnicode(targetWindow) != FALSE);
    if (g_WindowIsUnicode) {
      return reinterpret_cast<WNDPROC>(
        SetWindowLongPtrW(targetWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc))
      );
    }
    return reinterpret_cast<WNDPROC>(
      SetWindowLongPtrA(targetWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProc))
    );
  }

  static void UnhookWindowProc(HWND targetWindow, WNDPROC origProc)
  {
    if (!targetWindow || !origProc || !IsWindow(targetWindow))
      return;

    LONG_PTR currentProc =
      g_WindowIsUnicode ? GetWindowLongPtrW(targetWindow, GWLP_WNDPROC) : GetWindowLongPtrA(targetWindow, GWLP_WNDPROC);
    if (currentProc == reinterpret_cast<LONG_PTR>(WndProc)) {
      if (g_WindowIsUnicode) {
        SetWindowLongPtrW(targetWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(origProc));
      }
      else {
        SetWindowLongPtrA(targetWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(origProc));
      }
    }
  }

  static LRESULT CallOrigWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    if (g_WindowIsUnicode) {
      return CallWindowProcW(oWndProc, hWnd, uMsg, wParam, lParam);
    }
    return CallWindowProcA(oWndProc, hWnd, uMsg, wParam, lParam);
  }

  LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    if ((uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) && wParam == VK_INSERT) {
      bool toggledClosed = false;
      {
        std::lock_guard<std::mutex> lock(g_InsertMutex);
        s_LastWndProcInsertMs = GetTickCount64();
        if (!s_bInsertKeyDown) {
          const bool next  = !Config.bMenuOpen.load();
          Config.bMenuOpen = next;
          toggledClosed    = !next;
        }
        s_bInsertKeyDown = true;
      }
      if (toggledClosed && GetCapture() == hWnd) {
        ReleaseCapture();
        s_ImGuiMayHoldCapture = false;
      }
      return 1;
    }
    if ((uMsg == WM_KEYUP || uMsg == WM_SYSKEYUP) && wParam == VK_INSERT) {
      std::lock_guard<std::mutex> lock(g_InsertMutex);
      s_bInsertKeyDown      = false;
      s_LastWndProcInsertMs = GetTickCount64();
      return 1;
    }

    const bool menuOpen   = Config.bMenuOpen.load();
    bool       blockInput = false;

    if (!menuOpen && s_ImGuiMayHoldCapture) {
      s_ImGuiMayHoldCapture = false;
      if (GetCapture() == hWnd) {
        ReleaseCapture();
      }
    }

    if (menuOpen || ShouldForwardHiddenMessage(uMsg)) {
      std::lock_guard<std::recursive_mutex> lock(g_ImGuiMutex);
      if (ImGui::GetCurrentContext()) {
        ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

        if (menuOpen) {
          if (
            uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN || uMsg == WM_MBUTTONDOWN || uMsg == WM_XBUTTONDOWN
            || uMsg == WM_LBUTTONDBLCLK || uMsg == WM_RBUTTONDBLCLK || uMsg == WM_MBUTTONDBLCLK
            || uMsg == WM_XBUTTONDBLCLK
          ) {
            s_ImGuiMayHoldCapture = (GetCapture() == hWnd);
          }
          else if (
            uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONUP || uMsg == WM_MBUTTONUP || uMsg == WM_XBUTTONUP
            || uMsg == WM_KILLFOCUS
          ) {
            s_ImGuiMayHoldCapture = (GetCapture() == hWnd);
          }

          ImGuiIO& io = ImGui::GetIO();

          // Block mouse input to the game ONLY if ImGui wants to capture it (e.g. hovering over the menu)
          if (io.WantCaptureMouse && IsMouseMessage(uMsg)) {
            blockInput = true;
          }

          // Block keyboard input to the game ONLY if ImGui is focused on a text input
          if (io.WantCaptureKeyboard && IsKeyboardMessage(uMsg)) {
            blockInput = true;
          }
        }
      }
    }

    if (blockInput) {
      return 1;
    }

    return CallOrigWndProc(hWnd, uMsg, wParam, lParam);
  }

  static void InitImGuiContext(HWND targetWindow)
  {
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    char    path[MAX_PATH];
    HMODULE hMod = NULL;
    GetModuleHandleExA(
      GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR) &GetConfigPath,
      &hMod
    );
    if (hMod && GetModuleFileNameA(hMod, path, MAX_PATH)) {
      std::string fullPath(path);
      size_t      lastSlash = fullPath.find_last_of("\\/");
      if (lastSlash != std::string::npos) {
        g_IniPath      = fullPath.substr(0, lastSlash) + "\\imgui.ini";
        io.IniFilename = g_IniPath.c_str();
      }
    }

    ImGui_ImplWin32_Init(targetWindow);
    ImGui::StyleColorsDark();
    s_WasMenuOpen = Config.bMenuOpen.load();
  }

  static void EnsureWindowAttached(IDXGISwapChain* pSwapChain, HWND targetWindow)
  {
    if (!targetWindow || !IsWindow(targetWindow) || targetWindow == window)
      return;

    const bool shouldSwitch =
      !IsWindow(window) || pSwapChain == g_pSwapChain || (!IsWindowVisible(window) && IsWindowVisible(targetWindow));
    if (!shouldSwitch)
      return;

    std::lock_guard<std::recursive_mutex> lock(g_ImGuiMutex);
    UnhookWindowProc(window, oWndProc);
    if (ImGui::GetCurrentContext()) {
      ImGui_ImplWin32_Shutdown();
    }
    window   = targetWindow;
    oWndProc = HookWindowProc(window);
    if (ImGui::GetCurrentContext()) {
      ImGui_ImplWin32_Init(window);
    }
    g_pSwapChain = pSwapChain;
  }

  static void ShutdownDX11()
  {
    std::lock_guard<std::recursive_mutex> lock(g_ImGuiMutex);
    UnhookWindowProc(window, oWndProc);
    if (ImGui::GetCurrentContext()) {
      ImGui_ImplDX11_Shutdown();
      ImGui_ImplWin32_Shutdown();
      ImGui::DestroyContext();
    }
    CleanupRenderTarget();
    if (pContext) {
      pContext->Release();
      pContext = nullptr;
    }
    pDevice      = nullptr;
    g_pSwapChain = nullptr;
    window       = nullptr;
    oWndProc     = nullptr;
    init         = false;
  }

  static void ShutdownDX12()
  {
    std::lock_guard<std::recursive_mutex> lock(g_ImGuiMutex);
    UnhookWindowProc(window, oWndProc);
    if (ImGui::GetCurrentContext()) {
      ImGui_ImplDX12_Shutdown();
      ImGui_ImplWin32_Shutdown();
      ImGui::DestroyContext();
    }
    CleanupRenderTargetDX12();
    for (UINT i = 0; i < 16; i++) {
      if (g_commandAllocators[i]) {
        g_commandAllocators[i]->Release();
        g_commandAllocators[i] = nullptr;
      }
    }
    if (g_pd3dCommandList) {
      g_pd3dCommandList->Release();
      g_pd3dCommandList = nullptr;
    }
    if (g_pd3dRtvDescHeap) {
      g_pd3dRtvDescHeap->Release();
      g_pd3dRtvDescHeap = nullptr;
    }
    if (g_pd3dSrvDescHeap) {
      g_pd3dSrvDescHeap->Release();
      g_pd3dSrvDescHeap = nullptr;
    }
    g_pd3dDevice = nullptr;
    g_pSwapChain = nullptr;
    window       = nullptr;
    oWndProc     = nullptr;
    initDX12     = false;
  }

  static void BeforeResizeBuffers(IDXGISwapChain* pSwapChain)
  {
    std::lock_guard<std::recursive_mutex> lock(g_RenderMutex);
    if (pSwapChain == g_pSwapChain || g_pSwapChain == nullptr) {
      CleanupRenderTarget();
      if (initDX12)
        CleanupRenderTargetDX12();
    }
  }

  static void AfterResizeBuffers(IDXGISwapChain* pSwapChain, HRESULT hr)
  {
    if (FAILED(hr))
      return;

    std::lock_guard<std::recursive_mutex> lock(g_RenderMutex);
    if (pSwapChain == g_pSwapChain) {
      if (pDevice)
        CreateRenderTarget(pSwapChain);
      if (initDX12 && g_pd3dDevice)
        CreateRenderTargetDX12(pSwapChain, g_pd3dDevice);
    }
  }

  HRESULT __stdcall hkResizeBuffers(
    IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags
  )
  {
    BeforeResizeBuffers(pSwapChain);
    HRESULT hr = oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
    AfterResizeBuffers(pSwapChain, hr);
    return hr;
  }

  HRESULT __stdcall hkResizeBuffersAlt(
    IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags
  )
  {
    BeforeResizeBuffers(pSwapChain);
    HRESULT hr = oResizeBuffersAlt(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
    AfterResizeBuffers(pSwapChain, hr);
    return hr;
  }

  HRESULT __stdcall hkResizeBuffers1(
    IDXGISwapChain3* pSwapChain,
    UINT             BufferCount,
    UINT             Width,
    UINT             Height,
    DXGI_FORMAT      Format,
    UINT             SwapChainFlags,
    const UINT*      pCreationNodeMask,
    IUnknown* const* ppPresentQueue
  )
  {
    BeforeResizeBuffers(pSwapChain);
    HRESULT hr = oResizeBuffers1(
      pSwapChain, BufferCount, Width, Height, Format, SwapChainFlags, pCreationNodeMask, ppPresentQueue
    );
    AfterResizeBuffers(pSwapChain, hr);
    return hr;
  }

  HRESULT __stdcall hkResizeBuffers1Alt(
    IDXGISwapChain3* pSwapChain,
    UINT             BufferCount,
    UINT             Width,
    UINT             Height,
    DXGI_FORMAT      Format,
    UINT             SwapChainFlags,
    const UINT*      pCreationNodeMask,
    IUnknown* const* ppPresentQueue
  )
  {
    BeforeResizeBuffers(pSwapChain);
    HRESULT hr = oResizeBuffers1Alt(
      pSwapChain, BufferCount, Width, Height, Format, SwapChainFlags, pCreationNodeMask, ppPresentQueue
    );
    AfterResizeBuffers(pSwapChain, hr);
    return hr;
  }

  static void SyncMenuVisibilityAndDraw()
  {
    const bool menuOpen = Config.bMenuOpen.load();
    ImGuiIO&   io       = ImGui::GetIO();

    if (menuOpen) {
      io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
    }
    else {
      io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    }

    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (s_WasMenuOpen != menuOpen) {
      if (!menuOpen) {
        io.ClearInputKeys();
        io.ClearInputMouse();
        ImGui::SetWindowFocus(nullptr);
      }
      else {
        ImGui::SetNextWindowFocus();
        ImGui::SetNextWindowCollapsed(false, ImGuiCond_Appearing);
      }
      s_WasMenuOpen = menuOpen;
    }

    DrawMenuUI();

    ImGui::Render();
  }

  void RenderImGui_DX11(IDXGISwapChain* pSwapChain, ID3D11Device* pDeviceArg)
  {
    DXGI_SWAP_CHAIN_DESC sd;
    if (!GetSwapChainDesc(pSwapChain, sd) || !sd.OutputWindow || !IsWindow(sd.OutputWindow))
      return;

    if (init && pDeviceArg != pDevice) {
      ShutdownDX11();
    }

    if (!init) {
      pDevice = pDeviceArg;
      if (!pDevice)
        return;

      pDevice->GetImmediateContext(&pContext);
      window       = sd.OutputWindow;
      g_pSwapChain = pSwapChain;
      CreateRenderTarget(pSwapChain);

      oWndProc = HookWindowProc(window);

      {
        std::lock_guard<std::recursive_mutex> lock(g_ImGuiMutex);
        InitImGuiContext(window);
        ImGui_ImplDX11_Init(pDevice, pContext);
      }

      init = true;
    }
    else {
      EnsureWindowAttached(pSwapChain, sd.OutputWindow);
      if (sd.OutputWindow == window && pSwapChain != g_pSwapChain) {
        g_pSwapChain = pSwapChain;
        CleanupRenderTarget();
      }
    }

    EnsureRenderTargetDX11(pSwapChain);

    {
      std::lock_guard<std::recursive_mutex> lock(g_ImGuiMutex);
      ImGui_ImplDX11_NewFrame();
      SyncMenuVisibilityAndDraw();
    }

    if (mainRenderTargetView) {
      pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
      ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
  }

  void RenderImGui_DX12(IDXGISwapChain* pSwapChain, ID3D12Device* pDev)
  {
    if (!g_pd3dCommandQueue)
      return;

    DXGI_SWAP_CHAIN_DESC sd;
    if (!GetSwapChainDesc(pSwapChain, sd) || !sd.OutputWindow || !IsWindow(sd.OutputWindow))
      return;

    if (initDX12 && pDev != g_pd3dDevice) {
      ShutdownDX12();
    }

    if (!initDX12) {
      g_pd3dDevice                    = pDev;
      window                          = sd.OutputWindow;
      g_pSwapChain                    = pSwapChain;

      g_NUM_FRAMES_IN_FLIGHT          = (sd.BufferCount > 0 && sd.BufferCount <= 16) ? sd.BufferCount : 16;

      D3D12_DESCRIPTOR_HEAP_DESC desc = {};
      desc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
      desc.NumDescriptors             = 16;
      desc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
      desc.NodeMask                   = 1;
      if (pDev->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap)) != S_OK) {
        ShutdownDX12();
        return;
      }

      D3D12_DESCRIPTOR_HEAP_DESC srvdesc = {};
      srvdesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
      srvdesc.NumDescriptors             = 1;
      srvdesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
      if (pDev->CreateDescriptorHeap(&srvdesc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)) != S_OK) {
        ShutdownDX12();
        return;
      }

      for (UINT i = 0; i < 16; i++) {
        HRESULT hrAlloc =
          pDev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_commandAllocators[i]));
        if (hrAlloc != S_OK) {
          ShutdownDX12();
          return;
        }
      }

      HRESULT hrCmdList = pDev->CreateCommandList(
        0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_commandAllocators[0], NULL, IID_PPV_ARGS(&g_pd3dCommandList)
      );
      if (hrCmdList != S_OK) {
        ShutdownDX12();
        return;
      }
      g_pd3dCommandList->Close();

      CreateRenderTargetDX12(pSwapChain, pDev);
      oWndProc = HookWindowProc(window);

      {
        std::lock_guard<std::recursive_mutex> lock(g_ImGuiMutex);
        InitImGuiContext(window);
        ImGui_ImplDX12_InitInfo info      = {};
        info.Device                       = pDev;
        info.CommandQueue                 = g_pd3dCommandQueue;
        info.NumFramesInFlight            = g_NUM_FRAMES_IN_FLIGHT;
        info.RTVFormat                    = sd.BufferDesc.Format;
        info.DSVFormat                    = DXGI_FORMAT_UNKNOWN;
        info.SrvDescriptorHeap            = g_pd3dSrvDescHeap;
        info.LegacySingleSrvCpuDescriptor = g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart();
        info.LegacySingleSrvGpuDescriptor = g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart();
        ImGui_ImplDX12_Init(&info);
      }

      initDX12 = true;
    }
    else {
      EnsureWindowAttached(pSwapChain, sd.OutputWindow);
      if (sd.OutputWindow == window && pSwapChain != g_pSwapChain) {
        g_pSwapChain = pSwapChain;
        CleanupRenderTargetDX12();
      }
    }

    if (!g_mainRenderTargetResource[0]) {
      CreateRenderTargetDX12(pSwapChain, pDev);
    }

    {
      std::lock_guard<std::recursive_mutex> lock(g_ImGuiMutex);
      ImGui_ImplDX12_NewFrame();
      SyncMenuVisibilityAndDraw();
    }

    IDXGISwapChain3* pSwapChain3   = nullptr;
    UINT             backBufferIdx = 0;
    if (SUCCEEDED(pSwapChain->QueryInterface(__uuidof(IDXGISwapChain3), (void**) &pSwapChain3))) {
      backBufferIdx = pSwapChain3->GetCurrentBackBufferIndex();
      pSwapChain3->Release();
    }

    if (
      backBufferIdx >= g_NUM_FRAMES_IN_FLIGHT || !g_mainRenderTargetResource[backBufferIdx]
      || !g_commandAllocators[backBufferIdx]
    ) {
      return;
    }

    ID3D12CommandAllocator* commandAllocator = g_commandAllocators[backBufferIdx];
    commandAllocator->Reset();
    g_pd3dCommandList->Reset(commandAllocator, nullptr);

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource   = g_mainRenderTargetResource[backBufferIdx];
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
    g_pd3dCommandList->ResourceBarrier(1, &barrier);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
    rtvHandle.ptr += (SIZE_T) backBufferIdx * pDev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    g_pd3dCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, NULL);
    g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);

    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);

    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
    g_pd3dCommandList->ResourceBarrier(1, &barrier);
    g_pd3dCommandList->Close();

    ID3D12CommandList* ppCommandLists[] = {g_pd3dCommandList};
    g_pd3dCommandQueue->ExecuteCommandLists(1, ppCommandLists);
  }

  using CmdLists = ID3D12CommandList* const*;

  void __stdcall hkExecuteCommandLists(ID3D12CommandQueue* queue, UINT NumCommandLists, CmdLists ppCommandLists)
  {
    if (!g_pd3dCommandQueue && queue->GetDesc().Type == D3D12_COMMAND_LIST_TYPE_DIRECT) {
      g_pd3dCommandQueue = queue;
    }
    return oExecuteCommandLists(queue, NumCommandLists, ppCommandLists);
  }

  void RenderFrame(IDXGISwapChain* pSwapChain)
  {
    PollInsertKeyFallback();

    std::lock_guard<std::recursive_mutex> lock(g_RenderMutex);

    ID3D11Device* pD11 = nullptr;
    if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**) &pD11))) {
      RenderImGui_DX11(pSwapChain, pD11);
      pD11->Release();
      return;
    }

    // Proton / VKD3D behavior: GetDevice returns ID3D12Device
    ID3D12Device* pD12 = nullptr;
    if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D12Device), (void**) &pD12))) {
      RenderImGui_DX12(pSwapChain, pD12);
      pD12->Release();
      return;
    }

    // Windows Native DXGI behavior: GetDevice returns ID3D12CommandQueue
    ID3D12CommandQueue* pD12CommandQueue = nullptr;
    if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D12CommandQueue), (void**) &pD12CommandQueue))) {
      if (SUCCEEDED(pD12CommandQueue->GetDevice(__uuidof(ID3D12Device), (void**) &pD12))) {
        g_pd3dCommandQueue = pD12CommandQueue;
        RenderImGui_DX12(pSwapChain, pD12);
        pD12->Release();
      }
      pD12CommandQueue->Release();
    }
  }

  HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
  {
    if ((Flags & DXGI_PRESENT_TEST) == 0) {
      RenderFrame(pSwapChain);
    }
    return oPresent(pSwapChain, SyncInterval, Flags);
  }

  HRESULT __stdcall hkPresentAlt(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
  {
    if ((Flags & DXGI_PRESENT_TEST) == 0) {
      RenderFrame(pSwapChain);
    }
    return oPresentAlt(pSwapChain, SyncInterval, Flags);
  }

  HRESULT __stdcall hkPresent1(
    IDXGISwapChain1* pSwapChain, UINT SyncInterval, UINT PresentFlags, const DXGI_PRESENT_PARAMETERS* pPresentParameters
  )
  {
    if ((PresentFlags & DXGI_PRESENT_TEST) == 0) {
      RenderFrame(pSwapChain);
    }
    return oPresent1(pSwapChain, SyncInterval, PresentFlags, pPresentParameters);
  }

  HRESULT __stdcall hkPresent1Alt(
    IDXGISwapChain1* pSwapChain, UINT SyncInterval, UINT PresentFlags, const DXGI_PRESENT_PARAMETERS* pPresentParameters
  )
  {
    if ((PresentFlags & DXGI_PRESENT_TEST) == 0) {
      RenderFrame(pSwapChain);
    }
    return oPresent1Alt(pSwapChain, SyncInterval, PresentFlags, pPresentParameters);
  }

  static void HookSwapChainVTable(IDXGISwapChain* pSwapChain)
  {
    void** pVTable        = *reinterpret_cast<void***>(pSwapChain);
    void*  pPresent       = pVTable[8];
    void*  pResizeBuffers = pVTable[13];

    if (!g_HookedPresent) {
      if (MH_CreateHook(pPresent, (void*) hkPresent, (void**) &oPresent) == MH_OK) {
        g_HookedPresent = pPresent;
      }
    }
    else if (pPresent != g_HookedPresent && !oPresentAlt) {
      MH_CreateHook(pPresent, (void*) hkPresentAlt, (void**) &oPresentAlt);
    }

    if (!g_HookedResizeBuffers) {
      if (MH_CreateHook(pResizeBuffers, (void*) hkResizeBuffers, (void**) &oResizeBuffers) == MH_OK) {
        g_HookedResizeBuffers = pResizeBuffers;
      }
    }
    else if (pResizeBuffers != g_HookedResizeBuffers && !oResizeBuffersAlt) {
      MH_CreateHook(pResizeBuffers, (void*) hkResizeBuffersAlt, (void**) &oResizeBuffersAlt);
    }

    IDXGISwapChain1* pSwapChain1 = nullptr;
    if (SUCCEEDED(pSwapChain->QueryInterface(__uuidof(IDXGISwapChain1), (void**) &pSwapChain1))) {
      void** pVTable1  = *reinterpret_cast<void***>(pSwapChain1);
      void*  pPresent1 = pVTable1[22];
      if (!g_HookedPresent1) {
        if (MH_CreateHook(pPresent1, (void*) hkPresent1, (void**) &oPresent1) == MH_OK) {
          g_HookedPresent1 = pPresent1;
        }
      }
      else if (pPresent1 != g_HookedPresent1 && !oPresent1Alt) {
        MH_CreateHook(pPresent1, (void*) hkPresent1Alt, (void**) &oPresent1Alt);
      }
      pSwapChain1->Release();
    }

    IDXGISwapChain3* pSwapChain3 = nullptr;
    if (SUCCEEDED(pSwapChain->QueryInterface(__uuidof(IDXGISwapChain3), (void**) &pSwapChain3))) {
      void** pVTable3        = *reinterpret_cast<void***>(pSwapChain3);
      void*  pResizeBuffers1 = pVTable3[39];
      if (!g_HookedResizeBuffers1) {
        if (MH_CreateHook(pResizeBuffers1, (void*) hkResizeBuffers1, (void**) &oResizeBuffers1) == MH_OK) {
          g_HookedResizeBuffers1 = pResizeBuffers1;
        }
      }
      else if (pResizeBuffers1 != g_HookedResizeBuffers1 && !oResizeBuffers1Alt) {
        MH_CreateHook(pResizeBuffers1, (void*) hkResizeBuffers1Alt, (void**) &oResizeBuffers1Alt);
      }
      pSwapChain3->Release();
    }
  }

  void Initialize()
  {
    Config.LoadConfig();

    // Dummy DX11 swap chain creation to get the vtable address of Present
    WNDCLASSEXA wc = {sizeof(WNDCLASSEXA),    CS_CLASSDC, DefWindowProcA, 0L,   0L,
                      GetModuleHandleA(NULL), NULL,       NULL,           NULL, NULL,
                      "EvitaniaDummy",        NULL};
    RegisterClassExA(&wc);
    HWND dummyWindow =
      CreateWindowA("EvitaniaDummy", "", WS_OVERLAPPEDWINDOW, 100, 100, 300, 300, NULL, NULL, wc.hInstance, NULL);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount                       = 1;
    sd.BufferDesc.Format                 = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage                       = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow                      = dummyWindow;
    sd.SampleDesc.Count                  = 1;
    sd.Windowed                          = TRUE;
    sd.SwapEffect                        = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL featureLevels[]    = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0};

    IDXGISwapChain*      pDummySwapChain = nullptr;
    ID3D11Device*        pDummyDevice    = nullptr;
    ID3D11DeviceContext* pDummyContext   = nullptr;

    HRESULT hr                           = D3D11CreateDeviceAndSwapChain(
      NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, featureLevels, 3, D3D11_SDK_VERSION, &sd, &pDummySwapChain,
      &pDummyDevice, NULL, &pDummyContext
    );

    if (FAILED(hr)) {
      hr = D3D11CreateDeviceAndSwapChain(
        NULL, D3D_DRIVER_TYPE_WARP, NULL, 0, featureLevels, 3, D3D11_SDK_VERSION, &sd, &pDummySwapChain, &pDummyDevice,
        NULL, &pDummyContext
      );
    }

    if (SUCCEEDED(hr) && pDummySwapChain) {
      HookSwapChainVTable(pDummySwapChain);

      pDummySwapChain->Release();
      pDummyDevice->Release();
      pDummyContext->Release();
    }
    else {
      MessageBoxA(NULL, "Failed to create D3D11 Device (Menu might not show)", "Evitania Error", MB_OK);
    }

    // Dummy DX12 device creation to get the vtable address of ExecuteCommandLists
    ID3D12Device* pDummyD12Device = nullptr;
    if (SUCCEEDED(D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**) &pDummyD12Device))) {
      D3D12_COMMAND_QUEUE_DESC queueDesc = {};
      queueDesc.Type                     = D3D12_COMMAND_LIST_TYPE_DIRECT;
      queueDesc.Priority                 = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
      queueDesc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
      queueDesc.NodeMask                 = 0;

      ID3D12CommandQueue* pDummyQueue    = nullptr;
      HRESULT             hrQueue =
        pDummyD12Device->CreateCommandQueue(&queueDesc, __uuidof(ID3D12CommandQueue), (void**) &pDummyQueue);
      if (SUCCEEDED(hrQueue)) {
        void** pQueueVTable         = *reinterpret_cast<void***>(pDummyQueue);
        void*  pExecuteCommandLists = pQueueVTable[10];

        MH_CreateHook(pExecuteCommandLists, (void*) hkExecuteCommandLists, (void**) &oExecuteCommandLists);

        IDXGIFactory4* pFactory = nullptr;
        if (SUCCEEDED(CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**) &pFactory))) {
          DXGI_SWAP_CHAIN_DESC1 sd1      = {};
          sd1.BufferCount                = 2;
          sd1.Width                      = 100;
          sd1.Height                     = 100;
          sd1.Format                     = DXGI_FORMAT_R8G8B8A8_UNORM;
          sd1.BufferUsage                = DXGI_USAGE_RENDER_TARGET_OUTPUT;
          sd1.SwapEffect                 = DXGI_SWAP_EFFECT_FLIP_DISCARD;
          sd1.SampleDesc.Count           = 1;

          IDXGISwapChain1* pD12SwapChain = nullptr;
          HRESULT          hrD12 =
            pFactory->CreateSwapChainForHwnd(pDummyQueue, dummyWindow, &sd1, nullptr, nullptr, &pD12SwapChain);
          if (SUCCEEDED(hrD12)) {
            HookSwapChainVTable(pD12SwapChain);
            pD12SwapChain->Release();
          }
          pFactory->Release();
        }

        pDummyQueue->Release();
      }
      pDummyD12Device->Release();
    }

    DestroyWindow(dummyWindow);
    UnregisterClassA("EvitaniaDummy", wc.hInstance);
  }

  void Uninitialize()
  {
    std::lock_guard<std::recursive_mutex> lock(g_RenderMutex);
    if (init) {
      ShutdownDX11();
    }
    if (initDX12) {
      ShutdownDX12();
    }
  }
}  // namespace Menu
