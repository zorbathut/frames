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

#include "frames/input.h"

#include "frames/configuration.h"
#include "frames/environment.h"

namespace Frames {
  const char *Input::StringFromKey(Key key) {
    switch (key) {
      case Input::INVALID: Configuration::Get().LoggerGet()->LogError("Attempted to retrieve string from invalid key"); return "(invalid)"; // TODO: global assert
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

      default: Configuration::Get().LoggerGet()->LogError("Attempted to retrieve string from out-of-bounds key"); return "(unknown)";  // TODO: global assert
    }
  }

  /*static*/ Input::Command Input::Command::CreateMouseDown(int button) {
    Command element;
    element.m_type = Command::MOUSEDOWN;
    element.m_mouseDownButton = button;
    return element;
  }
  /*static*/ Input::Command Input::Command::CreateMouseUp(int button) {
    Command element;
    element.m_type = Command::MOUSEUP;
    element.m_mouseUpButton = button;
    return element;
  }
  /*static*/ Input::Command Input::Command::CreateMouseWheel(int delta) {
    Command element;
    element.m_type = Command::MOUSEWHEEL;
    element.m_mouseWheelDelta = delta;
    return element;
  }
  /*static*/ Input::Command Input::Command::CreateMouseMove(int x, int y) {
    Command element;
    element.m_type = Command::MOUSEMOVE;
    element.m_mouseMoveX = x;
    element.m_mouseMoveY = y;
    return element;
  }
  /*static*/ Input::Command Input::Command::CreateMouseClear() {
    Command element;
    element.m_type = Command::MOUSECLEAR;
    return element;
  }

  /*static*/ Input::Command Input::Command::CreateMetaSet(const Meta &meta) {
    Command element;
    element.m_type = Command::METASET;
    element.m_meta = meta;
    return element;
  }

  /*static*/ Input::Command Input::Command::CreateKeyDown(Key key) {
    Command element;
    element.m_type = Command::KEYDOWN;
    element.m_keyDown = key;
    return element;
  }
  /*static*/ Input::Command Input::Command::CreateKeyUp(Key key) {
    Command element;
    element.m_type = Command::KEYUP;
    element.m_keyUp = key;
    return element;
  }
  /*static*/ Input::Command Input::Command::CreateKeyRepeat(Key key) {
    Command element;
    element.m_type = Command::KEYREPEAT;
    element.m_keyRepeat = key;
    return element;
  }
  /*static*/ Input::Command Input::Command::CreateKeyText(const std::string &type) {
    Command element;
    element.m_type = Command::KEYTEXT;
    element.m_keyText = type;
    return element;
  }

  Input::Command::Command() : m_type(INVALID) { }

  Input::Command::Type Input::Command::TypeGet() const {
    return m_type;
  }

  int Input::Command::MouseDownButtonGet() const {
    if (m_type != Command::MOUSEDOWN) {
      Configuration::Get().LoggerGet()->LogError("Attempted to retrieve mouse button from non-MOUSEDOWN Input::Command");
      return -1;
    }
    return m_mouseDownButton;
  }
  int Input::Command::MouseUpButtonGet() const {
    if (m_type != Command::MOUSEUP) {
      Configuration::Get().LoggerGet()->LogError("Attempted to retrieve mouse button from non-MOUSEUP Input::Command");
      return -1;
    }
    return m_mouseUpButton;
  }
  int Input::Command::MouseWheelDeltaGet() const {
    if (m_type != Command::MOUSEWHEEL) {
      Configuration::Get().LoggerGet()->LogError("Attempted to retrieve mousewheel delta from non-MOUSEWHEEL Input::Command");
      return 0;
    }
    return m_mouseWheelDelta;
  }
  int Input::Command::MouseMoveXGet() const {
    if (m_type != Command::MOUSEMOVE) {
      Configuration::Get().LoggerGet()->LogError("Attempted to retrieve mousemove X from non-MOUSEMOVE Input::Command");
      return 0;
    }
    return m_mouseMoveX;
  }
  int Input::Command::MouseMoveYGet() const {
    if (m_type != Command::MOUSEMOVE) {
      Configuration::Get().LoggerGet()->LogError("Attempted to retrieve mousemove Y from non-MOUSEMOVE Input::Command");
      return 0;
    }
    return m_mouseMoveY;
  }
        
  const Input::Meta &Input::Command::MetaGet() const {
    if (m_type != Command::METASET) {
      Configuration::Get().LoggerGet()->LogError("Attempted to retrieve meta from non-METASET Input::Command");
      return m_meta;  // not like we have a more sensible alternative
    }
    return m_meta;
  }

  Input::Key Input::Command::KeyDownGet() const {
    if (m_type != Command::KEYDOWN) {
      Configuration::Get().LoggerGet()->LogError("Attempted to retrieve key from non-KEYDOWN Input::Command");
      return Input::INVALID;
    }
    return m_keyDown;
  }
  Input::Key Input::Command::KeyUpGet() const {
    if (m_type != Command::KEYUP) {
      Configuration::Get().LoggerGet()->LogError("Attempted to retrieve key from non-KEYUP Input::Command");
      return Input::INVALID;
    }
    return m_keyUp;
  }
  Input::Key Input::Command::KeyRepeatGet() const {
    if (m_type != Command::KEYREPEAT) {
      Configuration::Get().LoggerGet()->LogError("Attempted to retrieve key from non-KEYREPEAT Input::Command");
      return Input::INVALID;
    }
    return m_keyRepeat;
  }

  static const std::string cEmptyString;
  const std::string &Input::Command::KeyTextGet() const {
    if (m_type != Command::KEYTEXT) {
      Configuration::Get().LoggerGet()->LogError("Attempted to retrieve keytext from non-KEYTEXT Input::Command");
      return cEmptyString;  // need to return a non-local string
    }
    return m_keyText;
  }

  bool Input::Command::Process(Environment *env) const {
    switch (TypeGet()) {
    case MOUSEDOWN:
      return env->Input_MouseDown(MouseDownButtonGet());

    case MOUSEUP:
      return env->Input_MouseUp(MouseUpButtonGet());

    case MOUSEWHEEL:
      return env->Input_MouseWheel(MouseWheelDeltaGet());

    case MOUSEMOVE:
      env->Input_MouseMove(MouseMoveXGet(), MouseMoveYGet());
      return true;

    case MOUSECLEAR:
      env->Input_MouseClear();
      return true;

    case METASET:
      env->Input_MetaSet(MetaGet());
      return true;

    case KEYDOWN:
      return env->Input_KeyDown(KeyDownGet());

    case KEYUP:
      return env->Input_KeyUp(KeyUpGet());

    case KEYREPEAT:
      return env->Input_KeyRepeat(KeyRepeatGet());

    case KEYTEXT:
      return env->Input_KeyText(KeyTextGet());

    default:
      // TODO assert
      return true; // sure, whatever
    }
  }

  void Input::Sequence::Queue(const Input::Command &element) {
    m_queue.push_back(element);
  }

  void Input::Sequence::Process(Environment *env) const {
    for (int i = 0; i < (int)m_queue.size(); ++i)
    {
      m_queue[i].Process(env);
    }
  }
}

