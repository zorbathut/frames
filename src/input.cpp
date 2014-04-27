
#include "frames/input.h"

#include "frames/environment.h"

namespace Frames {
  const char *Input::StringFromKey(Key key) {
    switch (key) {
      case Input::INVALID: return "(invalid)"; // TODO: global assert
      case Input::A: return "a";
      case Input::B: return "b";
      case Input::C: return "c";
      case Input::D: return "d";
      case Input::E: return "e";
      case Input::F: return "f";
      case Input::G: return "g";
      case Input::H: return "h";
      case Input::I: return "i";
      case Input::J: return "j";
      case Input::K: return "k";
      case Input::L: return "l";
      case Input::M: return "m";
      case Input::N: return "n";
      case Input::O: return "o";
      case Input::P: return "p";
      case Input::Q: return "q";
      case Input::R: return "r";
      case Input::S: return "s";
      case Input::T: return "t";
      case Input::U: return "u";
      case Input::V: return "v";
      case Input::W: return "w";
      case Input::X: return "x";
      case Input::Y: return "y";
      case Input::Z: return "z";
      // Numbers
      case Input::Num0: return "0";
      case Input::Num1: return "1";
      case Input::Num2: return "2";
      case Input::Num3: return "3";
      case Input::Num4: return "4";
      case Input::Num5: return "5";
      case Input::Num6: return "6";
      case Input::Num7: return "7";
      case Input::Num8: return "8";
      case Input::Num9: return "9";
      // Numpad numerics
      case Input::Numpad0: return "Numpad 0";
      case Input::Numpad1: return "Numpad 1";
      case Input::Numpad2: return "Numpad 2";
      case Input::Numpad3: return "Numpad 3";
      case Input::Numpad4: return "Numpad 4";
      case Input::Numpad5: return "Numpad 5";
      case Input::Numpad6: return "Numpad 6";
      case Input::Numpad7: return "Numpad 7";
      case Input::Numpad8: return "Numpad 8";
      case Input::Numpad9: return "Numpad 9";
      // Numpad non-numerics
      case Input::Add: return "Numpad Add";
      case Input::Subtract: return "Numpad Subtract";
      case Input::Multiply: return "Numpad Multiply";
      case Input::Divide: return "Numpad Divide";
      case Input::Decimal: return "Numpad Decimal";
      // Editing keys
      case Input::Left: return "Left";
      case Input::Right: return "Right";
      case Input::Up: return "Up";
      case Input::Down: return "Down";
      case Input::PageUp: return "Page Up";
      case Input::PageDown: return "Page Down";
      case Input::End: return "End";
      case Input::Home: return "Home";
      case Input::Insert: return "Insert";
      case Input::Delete: return "Delete";
      // F-keys
      case Input::F1: return "F1";
      case Input::F2: return "F2";
      case Input::F3: return "F3";
      case Input::F4: return "F4";
      case Input::F5: return "F5";
      case Input::F6: return "F6";
      case Input::F7: return "F7";
      case Input::F8: return "F8";
      case Input::F9: return "F9";
      case Input::F10: return "F10";
      case Input::F11: return "F11";
      case Input::F12: return "F12";
      case Input::F13: return "F13";
      case Input::F14: return "F14";
      case Input::F15: return "F15";
      case Input::F16: return "F16";
      case Input::F17: return "F17";
      case Input::F18: return "F18";
      case Input::F19: return "F19";
      case Input::F20: return "F20";
      case Input::F21: return "F21";
      case Input::F22: return "F22";
      case Input::F23: return "F23";
      case Input::F24: return "F24";
      // Meta keys
      case Input::ControlLeft: return "Left Control";
      case Input::ShiftLeft: return "Left Shift";
      case Input::AltLeft: return "Left Alt";
      case Input::SystemLeft: return "Left System";
      case Input::ControlRight: return "Right Control";
      case Input::ShiftRight: return "Right Shift";
      case Input::AltRight: return "Right Alt";
      case Input::SystemRight: return "Right System";
      case Input::Apps: return "Apps";
      case Input::LockCaps: return "Caps Lock";
      case Input::LockScroll: return "Scroll Lock";
      case Input::LockNum: return "Num Lock";
      case Input::Printscreen: return "Printscreen";
      case Input::Pause: return "Pause";
      // Punctuation
      case Input::Escape: return "Escape";
      case Input::BracketLeft: return "Left Bracket";
      case Input::BracketRight: return "Right Bracket";
      case Input::Semicolon: return "Semicolon";
      case Input::Comma: return "Comma";
      case Input::Period: return "Period";
      case Input::Quote: return "Quote";
      case Input::Slash: return "Slash";
      case Input::Backslash: return "Backslash";
      case Input::Tilde: return "Tilde";
      case Input::Equal: return "Equal";
      case Input::Dash: return "Dash";
      case Input::Space: return "Space";
      case Input::Return: return "Return";
      case Input::Backspace: return "Backspace";
      case Input::Tab: return "Tab";

      default: return "(unknown)";  // TODO: global assert
    }
  }

  void Input::Sequence::Process(Environment *env) {
    if (GetMetaKnown()) {
      env->MetaSet(meta);
    }
    
    if (GetMouseposKnown()) {
      if (GetMouseposValid()) {
        env->MouseMove(GetMouseposX(), GetMouseposY());
      } else {
        env->LogError("No support for invalid mousepos yet");
      }
    }
    
    bool consumed = true;
    
    if (GetMode() == MODE_KEYDOWN) {
      consumed = env->KeyDown(key);
    } else if (GetMode() == MODE_KEYUP) {
      consumed = env->KeyUp(key);
    } else if (GetMode() == MODE_KEYREPEAT) {
      consumed = env->KeyRepeat(key);
    } else if (GetMode() == MODE_MOUSEDOWN) {
      consumed = env->MouseDown(GetMouseButton());
    } else if (GetMode() == MODE_MOUSEUP) {
      consumed = env->MouseUp(GetMouseButton());
    } else if (GetMode() == MODE_MOUSEWHEEL) {
      env->LogError("No support for mousewheel yet");
    } else if (GetMode() == MODE_TYPE) {
      consumed = env->KeyType(GetType());
    }
  }
}

