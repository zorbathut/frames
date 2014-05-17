
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

  static const Input::Key c_keyIndex[] = {
    Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, // 0x00 - 0x07
    Input::Backspace, Input::Tab, Input::INVALID, Input::INVALID, Input::INVALID, Input::Return, Input::INVALID, Input::INVALID, // 0x08 - 0x0f
    Input::INVALID, Input::INVALID, Input::INVALID, Input::Pause, Input::LockCaps, Input::INVALID, Input::INVALID, Input::INVALID, // 0x10 - 0x17
    Input::INVALID, Input::INVALID, Input::INVALID, Input::Escape, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, // 0x18 - 0x1f
    Input::Space, Input::PageUp, Input::PageDown, Input::End, Input::Home, Input::Left, Input::Up, Input::Right, // 0x20 - 0x27
    Input::Down, Input::INVALID, Input::INVALID, Input::INVALID, Input::Printscreen, Input::Insert, Input::Delete, Input::INVALID, // 0x28 - 0x2f
    Input::Num0, Input::Num1, Input::Num2, Input::Num3, Input::Num4, Input::Num5, Input::Num6, Input::Num7, // 0x30 - 0x37
    Input::Num8, Input::Num9, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, // 0x38 - 0x3f
    Input::INVALID, Input::A, Input::B, Input::C, Input::D, Input::E, Input::F, Input::G, // 0x40 - 0x47
    Input::H, Input::I, Input::J, Input::K, Input::L, Input::M, Input::N, Input::O, // 0x48 - 0x4f
    Input::P, Input::Q, Input::R, Input::S, Input::T, Input::U, Input::V, Input::W, // 0x50 - 0x57
    Input::X, Input::Y, Input::Z, Input::SystemLeft, Input::SystemRight, Input::Apps, Input::INVALID, Input::INVALID, // 0x58 - 0x5f
    Input::Numpad0, Input::Numpad1, Input::Numpad2, Input::Numpad3, Input::Numpad4, Input::Numpad5, Input::Numpad6, Input::Numpad7,  // 0x60 - 0x67
    Input::Numpad8, Input::Numpad9, Input::Multiply, Input::Add, Input::INVALID, Input::Subtract, Input::Decimal, Input::Divide, // 0x68 - 0x6f
    Input::F1, Input::F2, Input::F3, Input::F4, Input::F5, Input::F6, Input::F7, Input::F8, // 0x70 - 0x77
    Input::F9, Input::F10, Input::F11, Input::F12, Input::F13, Input::F14, Input::F15, Input::F16, // 0x78 - 0x7f
    Input::F17, Input::F18, Input::F19, Input::F20, Input::F21, Input::F22, Input::F23, Input::F24, // 0x80 - 0x87
    Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, // 0x88 - 0x8f
    Input::LockNum, Input::LockScroll, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, // 0x90 - 0x97
    Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, // 0x98 - 0x9f
    Input::ShiftLeft, Input::ShiftRight, Input::ControlLeft, Input::ControlRight, Input::AltLeft, Input::AltRight, Input::INVALID, Input::INVALID, // 0xa0 - 0xa7
    Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, // 0xa8 - 0xaf
    Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, // 0xb0 - 0xb7
    Input::INVALID, Input::INVALID, Input::Semicolon, Input::Equal, Input::Comma, Input::Dash, Input::Period, Input::Slash, // 0xb8 - 0xbf
    Input::Tilde, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, // 0xc0 - 0xc7
    Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, // 0xc8 - 0xcf
    Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, // 0xd0 - 0xd7
    Input::INVALID, Input::INVALID, Input::INVALID, Input::BracketLeft, Input::Backslash, Input::BracketRight, Input::Quote, Input::INVALID, // 0xd8 - 0xdf
    Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, // 0xe0 - 0xe7
    Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, // 0xe8 - 0xef
    Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, // 0xf0 - 0xf7
    Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, Input::INVALID, // 0xf8 - 0xff
  };
  
  static void InputGatherStandard(HWND window, Input::Sequence *sequence) {
    Input::Meta meta;
    meta.shift = (GetKeyState(VK_SHIFT) & 0x80) != 0;
    meta.ctrl = (GetKeyState(VK_CONTROL) & 0x80) != 0;
    meta.alt = (GetKeyState(VK_MENU) & 0x80) != 0;

    sequence->Queue(Input::Command::CreateMetaSet(meta));

    POINT mouse;
    GetCursorPos(&mouse);
    ScreenToClient(window, &mouse);

    sequence->Queue(Input::Command::CreateMouseMove(mouse.x, mouse.y));
  }
  
  bool InputGatherWin32(Input::Sequence *event, HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
      case WM_KEYDOWN:
      case WM_SYSKEYDOWN:
        if (wParam > 0 && wParam < 256) {
          if (wParam == VK_SHIFT || wParam == VK_CONTROL || wParam == VK_MENU) {
            wParam = MapVirtualKey((lParam & 0x00ff0000) >> 16, MAPVK_VSC_TO_VK_EX);
          }
          InputGatherStandard(window_handle, event);
          Input::Key key = c_keyIndex[wParam];
          if (lParam & (1 << 30)) {
            event->Queue(Input::Command::CreateKeyRepeat(key));
          } else {
            event->Queue(Input::Command::CreateKeyDown(key));
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
          event->Queue(Input::Command::CreateKeyUp(c_keyIndex[wParam]));
          return true;
        }
        break;
      case WM_UNICHAR:
        if (wParam != UNICODE_NOCHAR) {
          InputGatherStandard(window_handle, event);
          std::string typed;
          typed += (char)wParam; // TODO: utf8ize
          event->Queue(Input::Command::CreateKeyText(typed));
          return true;
        }
        break;
      case WM_CHAR:
        if (wParam == '\r') {
          InputGatherStandard(window_handle, event);
          event->Queue(Input::Command::CreateKeyText("\n"));
          return true;
        } else if (wParam != '\t' && wParam != '\b' && wParam != '\033') { // windows passes a bunch of nonprintable characters through this way, thanks windows. thwindows.
          InputGatherStandard(window_handle, event);
          std::string typed;
          typed += (char)wParam; // TODO: utf8ize
          event->Queue(Input::Command::CreateKeyText(typed));
          return true;
        }
        break;
      case WM_MOUSEMOVE:
        InputGatherStandard(window_handle, event);
        return true;
        break;
      case WM_LBUTTONDOWN:
        InputGatherStandard(window_handle, event);
        event->Queue(Input::Command::CreateMouseDown(0));
        return true;
      case WM_LBUTTONUP:
        InputGatherStandard(window_handle, event);
        event->Queue(Input::Command::CreateMouseUp(0));
        return true;
      case WM_MBUTTONDOWN:
        InputGatherStandard(window_handle, event);
        event->Queue(Input::Command::CreateMouseDown(1));
        return true;
      case WM_MBUTTONUP:
        InputGatherStandard(window_handle, event);
        event->Queue(Input::Command::CreateMouseUp(1));
        return true;
      case WM_RBUTTONDOWN:
        InputGatherStandard(window_handle, event);
        event->Queue(Input::Command::CreateMouseDown(2));
        return true;
      case WM_RBUTTONUP:
        InputGatherStandard(window_handle, event);
        event->Queue(Input::Command::CreateMouseUp(2));
        return true;
      case WM_XBUTTONDOWN:
        InputGatherStandard(window_handle, event);
        if ((wParam >> 16) == XBUTTON1) {
          event->Queue(Input::Command::CreateMouseDown(3));
        } else if ((wParam >> 16) == XBUTTON2) {
          event->Queue(Input::Command::CreateMouseDown(4));
        }
        return true;
      case WM_XBUTTONUP:
        InputGatherStandard(window_handle, event);
        if ((wParam >> 16) == XBUTTON1) {
          event->Queue(Input::Command::CreateMouseUp(3));
        } else if ((wParam >> 16) == XBUTTON2) {
          event->Queue(Input::Command::CreateMouseUp(4));
        }
        return true;
    }
    return false;
  }
}
