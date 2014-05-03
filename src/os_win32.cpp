
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
    Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, // 0x00 - 0x07
    Input::Key::Backspace, Input::Key::Tab, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::Return, Input::Key::INVALID, Input::Key::INVALID, // 0x08 - 0x0f
    Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::Pause, Input::Key::LockCaps, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, // 0x10 - 0x17
    Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::Escape, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, // 0x18 - 0x1f
    Input::Key::Space, Input::Key::PageUp, Input::Key::PageDown, Input::Key::End, Input::Key::Home, Input::Key::Left, Input::Key::Up, Input::Key::Right, // 0x20 - 0x27
    Input::Key::Down, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::Printscreen, Input::Key::Insert, Input::Key::Delete, Input::Key::INVALID, // 0x28 - 0x2f
    Input::Key::Num0, Input::Key::Num1, Input::Key::Num2, Input::Key::Num3, Input::Key::Num4, Input::Key::Num5, Input::Key::Num6, Input::Key::Num7, // 0x30 - 0x37
    Input::Key::Num8, Input::Key::Num9, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, // 0x38 - 0x3f
    Input::Key::INVALID, Input::Key::A, Input::Key::B, Input::Key::C, Input::Key::D, Input::Key::E, Input::Key::F, Input::Key::G, // 0x40 - 0x47
    Input::Key::H, Input::Key::I, Input::Key::J, Input::Key::K, Input::Key::L, Input::Key::M, Input::Key::N, Input::Key::O, // 0x48 - 0x4f
    Input::Key::P, Input::Key::Q, Input::Key::R, Input::Key::S, Input::Key::T, Input::Key::U, Input::Key::V, Input::Key::W, // 0x50 - 0x57
    Input::Key::X, Input::Key::Y, Input::Key::Z, Input::Key::SystemLeft, Input::Key::SystemRight, Input::Key::Apps, Input::Key::INVALID, Input::Key::INVALID, // 0x58 - 0x5f
    Input::Key::Numpad0, Input::Key::Numpad1, Input::Key::Numpad2, Input::Key::Numpad3, Input::Key::Numpad4, Input::Key::Numpad5, Input::Key::Numpad6, Input::Key::Numpad7,  // 0x60 - 0x67
    Input::Key::Numpad8, Input::Key::Numpad9, Input::Key::Multiply, Input::Key::Add, Input::Key::INVALID, Input::Key::Subtract, Input::Key::Decimal, Input::Key::Divide, // 0x68 - 0x6f
    Input::Key::F1, Input::Key::F2, Input::Key::F3, Input::Key::F4, Input::Key::F5, Input::Key::F6, Input::Key::F7, Input::Key::F8, // 0x70 - 0x77
    Input::Key::F9, Input::Key::F10, Input::Key::F11, Input::Key::F12, Input::Key::F13, Input::Key::F14, Input::Key::F15, Input::Key::F16, // 0x78 - 0x7f
    Input::Key::F17, Input::Key::F18, Input::Key::F19, Input::Key::F20, Input::Key::F21, Input::Key::F22, Input::Key::F23, Input::Key::F24, // 0x80 - 0x87
    Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, // 0x88 - 0x8f
    Input::Key::LockNum, Input::Key::LockScroll, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, // 0x90 - 0x97
    Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, // 0x98 - 0x9f
    Input::Key::ShiftLeft, Input::Key::ShiftRight, Input::Key::ControlLeft, Input::Key::ControlRight, Input::Key::AltLeft, Input::Key::AltRight, Input::Key::INVALID, Input::Key::INVALID, // 0xa0 - 0xa7
    Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, // 0xa8 - 0xaf
    Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, // 0xb0 - 0xb7
    Input::Key::INVALID, Input::Key::INVALID, Input::Key::Semicolon, Input::Key::Equal, Input::Key::Comma, Input::Key::Dash, Input::Key::Period, Input::Key::Slash, // 0xb8 - 0xbf
    Input::Key::Tilde, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, // 0xc0 - 0xc7
    Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, // 0xc8 - 0xcf
    Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, // 0xd0 - 0xd7
    Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::BracketLeft, Input::Key::Backslash, Input::Key::BracketRight, Input::Key::Quote, Input::Key::INVALID, // 0xd8 - 0xdf
    Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, // 0xe0 - 0xe7
    Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, // 0xe8 - 0xef
    Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, // 0xf0 - 0xf7
    Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, Input::Key::INVALID, // 0xf8 - 0xff
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
