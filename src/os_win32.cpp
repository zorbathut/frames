
#define _WIN32_WINNT 0x0600 // needed for XBUTTON, MOUSEWHEEL, MOUSEHWHEEL
#define WINVER 0x0400 // needed for MAPVK_VSC_TO_VK_EX

#include "frames/os_win32.h"

#include <windows.h>
#include <winuser.h>

#include <cstdio>

#ifndef MAPVK_VSC_TO_VK_EX  // Not in the MingW headers for some reason, so we'll just introduce it here
#define MAPVK_VSC_TO_VK_EX 3
#endif

namespace Frames {

  static const Key::Type c_keyIndex[] = {
    Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, // 0x00 - 0x07
    Key::Backspace, Key::Tab, Key::INVALID, Key::INVALID, Key::INVALID, Key::Return, Key::INVALID, Key::INVALID, // 0x08 - 0x0f
    Key::INVALID, Key::INVALID, Key::INVALID, Key::Pause, Key::LockCaps, Key::INVALID, Key::INVALID, Key::INVALID, // 0x10 - 0x17
    Key::INVALID, Key::INVALID, Key::INVALID, Key::Escape, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, // 0x18 - 0x1f
    Key::Space, Key::PageUp, Key::PageDown, Key::End, Key::Home, Key::Left, Key::Up, Key::Right, // 0x20 - 0x27
    Key::Down, Key::INVALID, Key::INVALID, Key::INVALID, Key::Printscreen, Key::Insert, Key::Delete, Key::INVALID, // 0x28 - 0x2f
    Key::Num0, Key::Num1, Key::Num2, Key::Num3, Key::Num4, Key::Num5, Key::Num6, Key::Num7, // 0x30 - 0x37
    Key::Num8, Key::Num9, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, // 0x38 - 0x3f
    Key::INVALID, Key::A, Key::B, Key::C, Key::D, Key::E, Key::F, Key::G, // 0x40 - 0x47
    Key::H, Key::I, Key::J, Key::K, Key::L, Key::M, Key::N, Key::O, // 0x48 - 0x4f
    Key::P, Key::Q, Key::R, Key::S, Key::T, Key::U, Key::V, Key::W, // 0x50 - 0x57
    Key::X, Key::Y, Key::Z, Key::SystemLeft, Key::SystemRight, Key::Apps, Key::INVALID, Key::INVALID, // 0x58 - 0x5f
    Key::Numpad0, Key::Numpad1, Key::Numpad2, Key::Numpad3, Key::Numpad4, Key::Numpad5, Key::Numpad6, Key::Numpad7,  // 0x60 - 0x67
    Key::Numpad8, Key::Numpad9, Key::Multiply, Key::Add, Key::INVALID, Key::Subtract, Key::Decimal, Key::Divide, // 0x68 - 0x6f
    Key::F1, Key::F2, Key::F3, Key::F4, Key::F5, Key::F6, Key::F7, Key::F8, // 0x70 - 0x77
    Key::F9, Key::F10, Key::F11, Key::F12, Key::F13, Key::F14, Key::F15, Key::F16, // 0x78 - 0x7f
    Key::F17, Key::F18, Key::F19, Key::F20, Key::F21, Key::F22, Key::F23, Key::F24, // 0x80 - 0x87
    Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, // 0x88 - 0x8f
    Key::LockNum, Key::LockScroll, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, // 0x90 - 0x97
    Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, // 0x98 - 0x9f
    Key::ShiftLeft, Key::ShiftRight, Key::ControlLeft, Key::ControlRight, Key::AltLeft, Key::AltRight, Key::INVALID, Key::INVALID, // 0xa0 - 0xa7
    Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, // 0xa8 - 0xaf
    Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, // 0xb0 - 0xb7
    Key::INVALID, Key::INVALID, Key::Semicolon, Key::Equal, Key::Comma, Key::Dash, Key::Period, Key::Slash, // 0xb8 - 0xbf
    Key::Tilde, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, // 0xc0 - 0xc7
    Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, // 0xc8 - 0xcf
    Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, // 0xd0 - 0xd7
    Key::INVALID, Key::INVALID, Key::INVALID, Key::BracketLeft, Key::Backslash, Key::BracketRight, Key::Quote, Key::INVALID, // 0xd8 - 0xdf
    Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, // 0xe0 - 0xe7
    Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, // 0xe8 - 0xef
    Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, // 0xf0 - 0xf7
    Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, Key::INVALID, // 0xf8 - 0xff
  };
  
  static void InputGatherStandard(HWND window, Input *event) {
    POINT mouse;
    GetCursorPos(&mouse);
    ScreenToClient(window, &mouse);
    event->SetMouseposMove(mouse.x, mouse.y);
    event->SetMeta((GetKeyState(VK_SHIFT) & 0x80) != 0, (GetKeyState(VK_CONTROL) & 0x80) != 0, (GetKeyState(VK_MENU) & 0x80) != 0);
  }
  
  bool InputGatherWin32(Input *event, HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
      case WM_KEYDOWN:
      case WM_SYSKEYDOWN:
        if (wParam > 0 && wParam < 256) {
          if (wParam == VK_SHIFT || wParam == VK_CONTROL || wParam == VK_MENU) {
            wParam = MapVirtualKey((lParam & 0x00ff0000) >> 16, MAPVK_VSC_TO_VK_EX);
          }
          InputGatherStandard(window_handle, event);
          Key::Type key = c_keyIndex[wParam];
          if (lParam & (1 << 30)) {
            event->SetModeKeyRepeat(key);
          } else {
            event->SetModeKeyDown(key);
          }
          return true;
        }
        break;
      case WM_KEYUP:
      case WM_SYSKEYUP:
        if (wParam > 0 && wParam < 256) {
          if (wParam == VK_SHIFT || wParam == VK_CONTROL || wParam == VK_MENU) {
            wParam = MapVirtualKey((lParam & 0x00ff0000) >> 16, MAPVK_VSC_TO_VK_EX);
          }
          InputGatherStandard(window_handle, event);
          event->SetModeKeyUp(c_keyIndex[wParam]);
          return true;
        }
        break;
      case WM_UNICHAR:
        if (wParam != UNICODE_NOCHAR) {
          InputGatherStandard(window_handle, event);
          std::string typed;
          typed += (char)wParam; // TODO: utf8ize
          event->SetModeType(typed);
          return true;
        }
        break;
      case WM_CHAR:
        if (wParam == '\r') {
          InputGatherStandard(window_handle, event);
          event->SetModeType("\n");
          return true;
        } else if (wParam != '\t' && wParam != '\b' && wParam != '\033') { // windows passes a bunch of nonprintable characters through this way, thanks windows. thwindows.
          InputGatherStandard(window_handle, event);
          std::string typed;
          typed += (char)wParam; // TODO: utf8ize
          event->SetModeType(typed);
          return true;
        }
        break;
      case WM_MOUSEMOVE:
        InputGatherStandard(window_handle, event);
        return true;
        break;
      case WM_LBUTTONDOWN:
        InputGatherStandard(window_handle, event);
        event->SetModeMouseDown(0);
        return true;
      case WM_LBUTTONUP:
        InputGatherStandard(window_handle, event);
        event->SetModeMouseUp(0);
        return true;
      case WM_MBUTTONDOWN:
        InputGatherStandard(window_handle, event);
        event->SetModeMouseDown(1);
        return true;
      case WM_MBUTTONUP:
        InputGatherStandard(window_handle, event);
        event->SetModeMouseUp(1);
        return true;
      case WM_RBUTTONDOWN:
        InputGatherStandard(window_handle, event);
        event->SetModeMouseDown(2);
        return true;
      case WM_RBUTTONUP:
        InputGatherStandard(window_handle, event);
        event->SetModeMouseUp(2);
        return true;
      case WM_XBUTTONDOWN:
        InputGatherStandard(window_handle, event);
        if ((wParam >> 16) == XBUTTON1) {
          event->SetModeMouseDown(3);
        } else if ((wParam >> 16) == XBUTTON2) {
          event->SetModeMouseDown(4);
        }
        return true;
      case WM_XBUTTONUP:
        InputGatherStandard(window_handle, event);
        if ((wParam >> 16) == XBUTTON1) {
          event->SetModeMouseUp(3);
        } else if ((wParam >> 16) == XBUTTON2) {
          event->SetModeMouseUp(4);
        }
        return true;
    }
    return false;
  }
}
