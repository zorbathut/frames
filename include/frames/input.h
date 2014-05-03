// Input enums and structs

#ifndef FRAMES_INPUT
#define FRAMES_INPUT

#include <string>
#include <vector>

namespace Frames {
  // Forward declarations
  class Environment;

  namespace Input {
    struct Meta {
      bool shift;
      bool ctrl;
      bool alt;
    };

    enum Key {
      INVALID,
      // Letters
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
      // Numbers
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
      // Numpad numerics
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
      // Numpad non-numerics
      Add,
      Subtract,
      Multiply,
      Divide,
      Decimal,
      // Editing keys
      Left,
      Right,
      Up,
      Down,
      PageUp,
      PageDown,
      End,
      Home,
      Insert,
      Delete,
      // F-keys
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
      F16,
      F17,
      F18,
      F19,
      F20,
      F21,
      F22,
      F23,
      F24,
      // Meta keys
      ControlLeft,
      ShiftLeft,
      AltLeft,
      SystemLeft,
      ControlRight,
      ShiftRight,
      AltRight,
      SystemRight,
      Apps,
      LockCaps,
      LockScroll,
      LockNum,
      Printscreen,
      Pause,
      // Punctuation
      Escape,
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
    };

    const char *StringFromKey(Key key);

    class Command {
    public:
      enum Type { INVALID, MOUSEDOWN, MOUSEUP, MOUSEWHEEL, MOUSEMOVE, MOUSECLEAR, META, KEYDOWN, KEYUP, KEYREPEAT, TYPE };

      // Mouse
      static Command CreateMouseDown(int button);
      static Command CreateMouseUp(int button);
      static Command CreateMouseWheel(int delta);
      static Command CreateMouseMove(int x, int y);
      static Command CreateMouseClear();

      // Key
      static Command CreateMeta(const Meta &meta);

      static Command CreateKeyDown(Key key);
      static Command CreateKeyUp(Key key);
      static Command CreateKeyRepeat(Key key);
      static Command CreateType(const std::string &type);

      Command();

      Type TypeGet() const;

      int MouseDownButtonGet() const;
      int MouseUpButtonGet() const;
      int MouseWheelDeltaGet() const;
      int MouseMoveXGet() const;
      int MouseMoveYGet() const;
        
      const Meta &MetaGet() const;

      Key KeyDownGet() const;
      Key KeyUpGet() const;
      Key KeyRepeatGet() const;

      const std::string &TextGet() const;

      bool Process(Environment *env) const;
        
    private:
      Type m_type;

      // Union everything that can be unioned
      union {
        int m_mouseDownButton;
        int m_mouseUpButton;
        int m_mouseWheelDelta;
        struct {
          int m_mouseMoveX;
          int m_mouseMoveY;
        };
        Meta m_meta;
        Key m_keyDown;
        Key m_keyUp;
        Key m_keyRepeat;
      };

      // This can't be. :(
      // also this is confusing with the item "type" but whatever, internal API
      std::string m_text;
    };

    // Used to send full input events into Frames
    class Sequence {
    public:
      void Queue(const Command &element);

      void Process(Environment *env) const;

      const std::vector<Command> &GetQueue() const { return m_queue; }

    private:
      std::vector<Command> m_queue;
    };
  }
}

#endif
