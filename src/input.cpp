
#include "frame/input.h"

namespace Frame {
  const char *Key::StringFromKey(Key::Key key) {
    switch (key) {
      case INVALID: return "(invalid)"; // TODO: global assert
      case A: return "a";
      case B: return "b";
      case C: return "c";
      case D: return "d";
      case E: return "e";
      case F: return "f";
      case G: return "g";
      case H: return "h";
      case I: return "i";
      case J: return "j";
      case K: return "k";
      case L: return "l";
      case M: return "m";
      case N: return "n";
      case O: return "o";
      case P: return "p";
      case Q: return "q";
      case R: return "r";
      case S: return "s";
      case T: return "t";
      case U: return "u";
      case V: return "v";
      case W: return "w";
      case X: return "x";
      case Y: return "y";
      case Z: return "z";
      // Numbers
      case Num0: return "0";
      case Num1: return "1";
      case Num2: return "2";
      case Num3: return "3";
      case Num4: return "4";
      case Num5: return "5";
      case Num6: return "6";
      case Num7: return "7";
      case Num8: return "8";
      case Num9: return "9";
      // Numpad numerics
      case Numpad0: return "Numpad 0";
      case Numpad1: return "Numpad 1";
      case Numpad2: return "Numpad 2";
      case Numpad3: return "Numpad 3";
      case Numpad4: return "Numpad 4";
      case Numpad5: return "Numpad 5";
      case Numpad6: return "Numpad 6";
      case Numpad7: return "Numpad 7";
      case Numpad8: return "Numpad 8";
      case Numpad9: return "Numpad 9";
      // Numpad non-numerics
      case Add: return "Numpad Add";
      case Subtract: return "Numpad Subtract";
      case Multiply: return "Numpad Multiply";
      case Divide: return "Numpad Divide";
      case Decimal: return "Numpad Decimal";
      // Editing keys
      case Left: return "Left";
      case Right: return "Right";
      case Up: return "Up";
      case Down: return "Down";
      case PageUp: return "Page Up";
      case PageDown: return "Page Down";
      case End: return "End";
      case Home: return "Home";
      case Insert: return "Insert";
      case Delete: return "Delete";
      // F-keys
      case F1: return "F1";
      case F2: return "F2";
      case F3: return "F3";
      case F4: return "F4";
      case F5: return "F5";
      case F6: return "F6";
      case F7: return "F7";
      case F8: return "F8";
      case F9: return "F9";
      case F10: return "F10";
      case F11: return "F11";
      case F12: return "F12";
      case F13: return "F13";
      case F14: return "F14";
      case F15: return "F15";
      case F16: return "F16";
      case F17: return "F17";
      case F18: return "F18";
      case F19: return "F19";
      case F20: return "F20";
      case F21: return "F21";
      case F22: return "F22";
      case F23: return "F23";
      case F24: return "F24";
      // Meta keys
      case ControlLeft: return "Left Control";
      case ShiftLeft: return "Left Shift";
      case AltLeft: return "Left Alt";
      case SystemLeft: return "Left System";
      case ControlRight: return "Right Control";
      case ShiftRight: return "Right Shift";
      case AltRight: return "Right Alt";
      case SystemRight: return "Right System";
      case Apps: return "Apps";
      case LockCaps: return "Caps Lock";
      case LockScroll: return "Scroll Lock";
      case LockNum: return "Num Lock";
      case Printscreen: return "Printscreen";
      case Pause: return "Pause";
      // Punctuation
      case Escape: return "Escape";
      case BracketLeft: return "Left Bracket";
      case BracketRight: return "Right Bracket";
      case Semicolon: return "Semicolon";
      case Comma: return "Comma";
      case Period: return "Period";
      case Quote: return "Quote";
      case Slash: return "Slash";
      case Backslash: return "Backslash";
      case Tilde: return "Tilde";
      case Equal: return "Equal";
      case Dash: return "Dash";
      case Space: return "Space";
      case Return: return "Return";
      case Backspace: return "Backspace";
      case Tab: return "Tab";

      default: return "(unknown)";  // TODO: global assert
    }
  }
}

