// Input enums and structs

#ifndef FRAME_INPUT
#define FRAME_INPUT

#include <string>

namespace Frame {
  namespace Key {
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
  }

  // Used for internal events
  struct KeyEvent {
    KeyEvent() : key(Key::INVALID), shift(false), ctrl(false), alt(false) { };

    Key::Key key;
    bool shift;
    bool ctrl;
    bool alt;
  };
  
  // Used to send full input events into Frames
  struct InputEvent {
    InputEvent() : mode(MODE_NULL), mouseKnown(false), metaKnown(false) { }
    
    // this block is mutually exclusive
    void SetModeKeyDown(const Key::Key &kev) { mode = MODE_KEYDOWN; key = kev; }
    void SetModeKeyUp(const Key::Key &kev) { mode = MODE_KEYUP; key = kev; }
    void SetModeKeyRepeat(const Key::Key &kev) { mode = MODE_KEYREPEAT; key = kev; }
    void SetModeMouseDown(int button) { mode = MODE_MOUSEDOWN; mouseAmount = button; }
    void SetModeMouseUp(int button) { mode = MODE_MOUSEUP; mouseAmount = button; }
    void SetModeMouseWheel(int delta) { mode = MODE_MOUSEWHEEL; mouseAmount = delta; }
    void SetModeType(const std::string &intype) { mode = MODE_TYPE; type = intype; }
    void SetModeClear() { mode = MODE_NULL; }
    
    // this block is mutually exclusive
    void SetMouseposMove(int x, int y) { mouseKnown = true; mouseValid = true; mouseX = x; mouseY = y; }
    void SetMouseposInvalid() { mouseKnown = true; mouseValid = false; }  // "not in the frame"
    void SetMouseposUnknown() { mouseKnown = false; }  // "this event doesn't know where it is"
    
    // each key is independent
    void SetMeta(bool shift, bool ctrl, bool alt) { metaKnown = true; metaShift = shift; metaCtrl = ctrl; metaAlt = alt; }
    void SetMetaUnknown() { metaKnown = false; }
    
    // accessors. TODO: more asserts
    enum Mode {MODE_NULL, MODE_KEYDOWN, MODE_KEYUP, MODE_KEYREPEAT, MODE_MOUSEDOWN, MODE_MOUSEUP, MODE_MOUSEWHEEL, MODE_TYPE};
    Mode GetMode() const { return mode; }
    
    const Key::Key &GetKey() const { return key; }
    const std::string &GetType() const { return type; }
    int GetMouseButton() const { return mouseAmount; }
    int GetMouseWheelDelta() const { return mouseAmount; }
    
    bool GetMouseposKnown() const { return mouseKnown; }
    bool GetMouseposValid() const { return mouseValid; }
    int GetMouseposX() const { return mouseX; }
    int GetMouseposY() const { return mouseY; }
    
    bool GetMetaKnown() const { return metaKnown; }
    bool GetMetaShift() const { return metaShift; }
    bool GetMetaCtrl() const { return metaCtrl; }
    bool GetMetaAlt() const { return metaAlt; }
    
  private:
    Mode mode;
    
    Key::Key key;
    std::string type;
    
    int mouseAmount;
    
    bool mouseKnown;
    bool mouseValid;
    int mouseX;
    int mouseY;
    
    bool metaKnown;
    bool metaShift;
    bool metaCtrl;
    bool metaAlt;
  };
};

#endif
