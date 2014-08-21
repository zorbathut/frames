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

#ifndef FRAMES_TEST_LIB_DX11
#define FRAMES_TEST_LIB_DX11

#include "lib.h"

#include <d3d11.h>
#include <dxgi.h>
#include <dxgidebug.h>
#include <windows.h>

class TestWindowDX11 : public TestWindow {
public:
  enum Mode {
    MODE_HAL,
    MODE_DEBUG,
    MODE_REFERENCE,
  };

  TestWindowDX11(int width, int height, D3D_FEATURE_LEVEL fl, Mode mode);
  ~TestWindowDX11();

  virtual void Swap() FRAMES_OVERRIDE;
  virtual void HandleEvents() FRAMES_OVERRIDE;

  virtual Frames::Configuration::RendererPtr RendererGet() FRAMES_OVERRIDE;

  virtual void ClearRenderTarget() FRAMES_OVERRIDE;
  virtual std::vector<unsigned char> Screenshot() FRAMES_OVERRIDE;

private:
  HWND m_window;
  IDXGISwapChain *m_swap;
  ID3D11Device *m_device;
  ID3D11DeviceContext *m_context;
  ID3D11Texture2D *m_backBuffer;
  ID3D11Texture2D *m_captureBuffer;
  ID3D11RenderTargetView *m_renderTarget;

  // DXGI debugging
  IDXGIDebug *m_dxgiDebug;
  IDXGIInfoQueue *m_dxgiInfo;
};

#endif
