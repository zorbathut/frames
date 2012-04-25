// Input enums and structs

#ifndef FRAMES_INPUT
#define FRAMES_INPUT

namespace Frames {
  namespace Key {
    // This list of keys is copied largely-verbatim from SFML, which did a dang good job with it. It's been modified slightly to fit Frames conventions.
    // There is no guarantee made that this will correspond to SFML in the future.
    //
    // SFML - Simple and Fast Multimedia Library
    // Copyright (C) 2007-2012 Laurent Gomila (laurent.gom@gmail.com)
    //
    // This software is provided 'as-is', without any express or implied warranty.
    // In no event will the authors be held liable for any damages arising from the use of this software.
    //
    // Permission is granted to anyone to use this software for any purpose,
    // including commercial applications, and to alter it and redistribute it freely,
    // subject to the following restrictions:
    //
    // 1. The origin of this software must not be misrepresented;
    //    you must not claim that you wrote the original software.
    //    If you use this software in a product, an acknowledgment
    //    in the product documentation would be appreciated but is not required.
    //
    // 2. Altered source versions must be plainly marked as such,
    //    and must not be misrepresented as being the original software.
    //
    // 3. This notice may not be removed or altered from any source distribution.
    //

    enum Key {
      INVALID,
      A,
      B,
      C,
      D,
      E,
      F,
      G,
      H,
      I,
      J,
      K,
      L,
      M,
      N,
      O,
      P,
      Q,
      R,
      S,
      T,
      U,
      V,
      W,
      X,
      Y,
      Z,
      Num0,
      Num1,
      Num2,
      Num3,
      Num4,
      Num5,
      Num6,
      Num7,
      Num8,
      Num9,
      Escape,
      ControlLeft,
      ShiftLeft,
      AltLeft,
      SystemLeft,
      ControlRight,
      ShiftRight,
      AltRight,
      SystemRight,
      Menu,
      BracketLeft,
      BracketRight,
      Semicolon,
      Comma,
      Period,
      Quote,
      Slash,
      Backslash,
      Tilde,
      Equal,
      Dash,
      Space,
      Return,
      Backspace,
      Tab,
      PageUp,
      PageDown,
      End,
      Home,
      Insert,
      Delete,
      Add,
      Subtract,
      Multiply,
      Divide,
      Left,
      Right,
      Up,
      Down,
      Numpad0,
      Numpad1,
      Numpad2,
      Numpad3,
      Numpad4,
      Numpad5,
      Numpad6,
      Numpad7,
      Numpad8,
      Numpad9,
      F1,
      F2,
      F3,
      F4,
      F5,
      F6,
      F7,
      F8,
      F9,
      F10,
      F11,
      F12,
      F13,
      F14,
      F15,
      Pause,
    };
  }

  struct KeyEvent {
    KeyEvent() : key(Key::INVALID), shift(false), ctrl(false), alt(false) { };

    Key::Key key;
    bool shift;
    bool ctrl;
    bool alt;
  };
};

#endif
