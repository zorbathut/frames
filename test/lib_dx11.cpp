/*  Copyright 2014 Mandible Games
    
    This file is part of Frames.
    
    Please see the COPYING file for detailed licensing information.
    
    Frames is dual-licensed software. It is available under both a
    commercial license, and also under the terms of the GNU General
    Public License as published by the Free Software Foundation, either
    version 3 of the License, or (at your option) any later version.

    Frames is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Frames.  If not, see <http://www.gnu.org/licenses/>. */

#include <initguid.h> // must be before a lot of things, probably including d3d11.h

#include "lib_dx11.h"

#include <frames/renderer_dx11.h>

#include <d3d11.h>
#include <d3d11sdklayers.h>
#include <dxgi.h>
#include <dxgidebug.h>
#include <windows.h>

const wchar_t *const windowClassName = L"FramesLibTester";

static LRESULT CALLBACK HandleMessage(HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam) {
  // Yeah, sure, whatever
  return DefWindowProcW(window_handle, message, wParam, lParam);
}

TestWindowDX11::TestWindowDX11(int width, int height, D3D_FEATURE_LEVEL fl, TestWindowDX11::Mode mode) : TestWindow(width, height),
  m_window(0),
  m_swap(0),
  m_device(0),
  m_context(0),
  m_backBuffer(0),
  m_captureBuffer(0),
  m_renderTarget(0),
  m_dxgiDebug(0),
  m_dxgiInfo(0)
{
  WNDCLASSW window_class;
  window_class.style = CS_OWNDC;  // CS_OWNDC can cause issues if other applications are trying to write to this window, but in a standard game model, that shouldn't ever happen
  window_class.lpfnWndProc = HandleMessage;
  window_class.cbClsExtra = 0;
  window_class.cbWndExtra = 0;
  window_class.hInstance = GetModuleHandle(0);
  window_class.hIcon = 0;
  window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
  window_class.hbrBackground = 0;
  window_class.lpszMenuName = 0;
  window_class.lpszClassName = windowClassName;
  RegisterClassW(&window_class); // this may "fail" because it's already registered

  DWORD window_style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
  DWORD window_style_ex = 0;

  RECT window_rectangle;
  window_rectangle.left = 0;
  window_rectangle.right = width;
  window_rectangle.top = 0;
  window_rectangle.bottom = height;
  EXPECT_TRUE(AdjustWindowRectEx(&window_rectangle, window_style, false, window_style_ex));

  m_window = CreateWindowExW(
    window_style_ex,
    windowClassName,
    L"Frames test window",
    window_style,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    window_rectangle.right - window_rectangle.left,
    window_rectangle.bottom - window_rectangle.top,
    NULL,
    NULL,
    GetModuleHandle(0),
    NULL);
  EXPECT_TRUE(m_window);

  DXGI_SWAP_CHAIN_DESC sd;
  ZeroMemory(&sd, sizeof(sd));
  sd.BufferCount = 1;
  sd.BufferDesc.Width = WidthGet();
  sd.BufferDesc.Height = HeightGet();
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 60;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = m_window;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.Windowed = TRUE;

  D3D_FEATURE_LEVEL FeatureLevelsRequested = fl;
  D3D_FEATURE_LEVEL FeatureLevelsSupported;

  // Our test framework sometimes starts too many directx instances; deal with E_OUTOFMEMORY errors by retrying after a second, at which point we'll hopefully have cleaned up a bit
  while (true) {
    int rv = D3D11CreateDeviceAndSwapChain(NULL,
      mode == MODE_REFERENCE ? D3D_DRIVER_TYPE_REFERENCE : D3D_DRIVER_TYPE_HARDWARE,
      NULL,
      mode == MODE_DEBUG ? D3D11_CREATE_DEVICE_DEBUG : 0,
      &FeatureLevelsRequested,
      1,
      D3D11_SDK_VERSION,
      &sd,
      &m_swap,
      &m_device,
      &FeatureLevelsSupported,
      &m_context);

    if (rv == S_OK) break; // Yay!

    EXPECT_EQ(E_OUTOFMEMORY, rv);
    
    if (rv != E_OUTOFMEMORY) {
      return;
    }

    printf("E_OUTOFMEMORY, trying again in one second . . .\n");
    fflush(stdout);

    HandleEvents();

    Sleep(1000);
  }

  // Rig up debug logging first
  {
    typedef HRESULT(__stdcall *fPtr)(const IID&, void**);
    HMODULE hDll = GetModuleHandleW(L"dxgidebug.dll");
    fPtr DXGIGetDebugInterface = hDll ? (fPtr)GetProcAddress(hDll, "DXGIGetDebugInterface") : 0;

    if (DXGIGetDebugInterface) {
      DXGIGetDebugInterface(__uuidof(IDXGIDebug), (void**)&m_dxgiDebug);
      DXGIGetDebugInterface(__uuidof(IDXGIInfoQueue), (void**)&m_dxgiInfo);

      // TO ADDRESS SPACE CRASH, AND BEYOND
      m_dxgiInfo->SetMessageCountLimit(DXGI_DEBUG_D3D11, -1);
    } else {
      printf("DXGI interfaces unavailable, memory leaks and other DX bugs cannot be tracked");
    }
  }

  EXPECT_EQ(S_OK, m_swap->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_backBuffer));
  m_device->CreateRenderTargetView(m_backBuffer, NULL, &m_renderTarget);
  m_context->OMSetRenderTargets(1, &m_renderTarget, 0);

  D3D11_TEXTURE2D_DESC description;
  m_backBuffer->GetDesc(&description);
  description.BindFlags = 0;
  description.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
  description.Usage = D3D11_USAGE_STAGING;

  EXPECT_EQ(S_OK, m_device->CreateTexture2D(&description, 0, &m_captureBuffer));
}

TestWindowDX11::~TestWindowDX11() {
  if (m_renderTarget) {
    m_renderTarget->Release();
  }

  if (m_captureBuffer) {
    m_captureBuffer->Release();
  }

  if (m_backBuffer) {
    m_backBuffer->Release();
  }

  if (m_swap) {
    m_swap->Release();
  }

  if (m_device) {
    m_device->Release();
  }

  if (m_context) {
    m_context->Release();
  }

  if (m_window) {
    DestroyWindow(m_window);
  }

  HandleEvents();

  UnregisterClassW(windowClassName, GetModuleHandle(0));

  // Test for memory leaks
  if (m_dxgiDebug) {
    m_dxgiDebug->ReportLiveObjects(DXGI_DEBUG_D3D11, DXGI_DEBUG_RLO_DETAIL);

    std::string dump;

    for (int i = 0; i < m_dxgiInfo->GetNumStoredMessagesAllowedByRetrievalFilters(DXGI_DEBUG_D3D11); ++i) {
      SIZE_T len;
      m_dxgiInfo->GetMessage(DXGI_DEBUG_D3D11, i, 0, &len);
      DXGI_INFO_QUEUE_MESSAGE *msg = (DXGI_INFO_QUEUE_MESSAGE *)malloc(len);
      m_dxgiInfo->GetMessage(DXGI_DEBUG_D3D11, i, msg, &len);
      dump += std::string(msg->pDescription) + "\n";
      free(msg);
    }

    if (!dump.empty()) {
      ADD_FAILURE() << dump;
    }
  }

  if (m_dxgiDebug) {
    m_dxgiDebug->Release();
  }

  if (m_dxgiInfo) {
    m_dxgiInfo->Release();
  }
}

void TestWindowDX11::Swap() {
  m_swap->Present(0, 0);
}

void TestWindowDX11::HandleEvents() {
  MSG msg;
  while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

Frames::Configuration::RendererPtr TestWindowDX11::RendererGet() {
  return Frames::Configuration::RendererDX11(m_context);
}

static const float black[4] = { 0, 0, 0, 1 };
void TestWindowDX11::ClearRenderTarget() {
  m_context->ClearRenderTargetView(m_renderTarget, black);
}

std::vector<unsigned char> TestWindowDX11::Screenshot() {
  m_context->CopyResource(m_captureBuffer, m_backBuffer);
  D3D11_MAPPED_SUBRESOURCE map;
  m_context->Map(m_captureBuffer, 0, D3D11_MAP_READ, 0, &map);

  std::vector<unsigned char> pixels;
  pixels.resize(WidthGet() * HeightGet() * 4);
  unsigned char *write = &pixels[0];

  const unsigned char *row = (const unsigned char *)map.pData;
  for (int i = 0; i < HeightGet(); ++i) {
    memcpy(write, row, WidthGet() * 4);
    write += WidthGet() * 4;
    row += map.RowPitch;
  }

  m_context->Unmap(m_captureBuffer, 0);

  ClampScreenshotAlpha(&pixels);

  return pixels;
}
