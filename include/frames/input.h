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

#ifndef FRAMES_INPUT
#define FRAMES_INPUT

#include <string>
#include <vector>

namespace Frames {
  // Forward declarations
  class Environment;
  template <typename T> class Ptr;
  typedef Ptr<Environment> EnvironmentPtr;

  namespace Input {

    /// Represents the current state of the system's standard modifier keys.
    struct Meta {
      /// Initialize to all values false.
      Meta() : shift(false), ctrl(false), alt(false) { }

      /// Indicates if either "shift" key is pressed.
      bool shift;
      /// Indicates if either "ctrl" key is pressed.
      bool ctrl;
      /// Indicates if either "alt" key is pressed.
      bool alt;
    };

    /// Represents the scancode of a single keyboard key relative to QWERTY layout.
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

    /// Converts a scancode into a human-readable constant string.
    /** This is intended only for internal development and debug purposes; the string returned does not respect locale.
    
    Pointers returned are valid forever. */
    const char *StringFromKey(Key key);

    /// Represents a single input command that can be sent to the Environment.
    /** This is not required to use Frames. It's provided as a convenient method for generic event transport from an OS abstraction layer to a Frames::Environment. */
    class Command {
    public:
      /// Creates a MOUSEDOWN Command.
      static Command CreateMouseDown(int button);
      /// Creates a MOUSEUP Command.
      static Command CreateMouseUp(int button);
      /// Creates a MOUSEWHEEL Command.
      static Command CreateMouseWheel(int delta);
      /// Creates a MOUSEMOVE Command.
      static Command CreateMouseMove(int x, int y);
      /// Creates a MOUSECLEAR Command.
      static Command CreateMouseClear();

      /// Creates a METASET Command.
      static Command CreateMetaSet(const Meta &meta);

      /// Creates a KEYDOWN Command.
      static Command CreateKeyDown(Key key);
      /// Creates a KEYUP Command.
      static Command CreateKeyUp(Key key);
      /// Creates a KEYREPEAT Command.
      static Command CreateKeyRepeat(Key key);
      /// Creates a KEYTEXT Command.
      static Command CreateKeyText(const std::string &type);

      /// Creates a Command initialized to the INVALID state.
      Command();

      /// List of states that this Command can be in.
      enum Type {
        INVALID, ///< Not a valid command.
        MOUSEDOWN, ///< Will call Environment::Input_MouseDown.
        MOUSEUP, ///< Will call Environment::Input_MouseUp.
        MOUSEWHEEL, ///< Will call Environment::Input_MouseWheel.
        MOUSEMOVE, ///< Will call Environment::Input_MouseMove.
        MOUSECLEAR, ///< Will call Environment::Input_MouseClear.
        METASET, ///< Will call Environment::Input_MetaSet.
        KEYDOWN, ///< Will call Environment::Input_KeyDown.
        KEYUP, ///< Will call Environment::Input_KeyUp.
        KEYREPEAT, ///< Will call Environment::Input_KeyRepeat.
        KEYTEXT, ///< Will call Environment::Input_KeyText.
      };
      /// Returns the current state of this Command.
      Type TypeGet() const;

      /// Returns the button involved in this MOUSEDOWN event.
      /** Call only if this is a MOUSEDOWN Command. */
      int MouseDownButtonGet() const;
      /// Returns the button involved in this MOUSEUP event.
      /** Call only if this is a MOUSEUP Command. */
      int MouseUpButtonGet() const;
      /// Returns the delta involved in this MOUSEWHEEL event.
      /** Call only if this is a MOUSEWHEEL Command. */
      int MouseWheelDeltaGet() const;
      /// Returns the X position involved in this MOUSEMOVE event.
      /** Call only if this is a MOUSEMOVE Command. */
      int MouseMoveXGet() const;
      /// Returns the Y position involved in this MOUSEMOVE event.
      /** Call only if this is a MOUSEMOVE Command. */
      int MouseMoveYGet() const;
      
      /// Returns the new meta state involved in this METASET event.
      /** Call only if this is a METASET Command. */
      const Meta &MetaGet() const;

      /// Returns the key involved in this KEYDOWN event.
      /** Call only if this is a KEYDOWN Command. */
      Key KeyDownGet() const;
      /// Returns the key involved in this KEYUP event.
      /** Call only if this is a KEYUP Command. */
      Key KeyUpGet() const;
      /// Returns the key involved in this KEYREPEAT event.
      /** Call only if this is a KEYREPEAT Command. */
      Key KeyRepeatGet() const;
      /// Returns the text involved in this KEYTEXT event.
      /** Call only if this is a KEYTEXT Command. */
      const std::string &KeyTextGet() const;

      /// Feeds this Command into an Environment.
      /** Returns true if this event was consumed by the Environment, false otherwise. */
      bool Process(Environment *env) const;

      /// Feeds this Command into an Environment.
      /** Returns true if this event was consumed by the Environment, false otherwise. */
      bool Process(const EnvironmentPtr &env) const;
        
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
        Key m_keyDown;
        Key m_keyUp;
        Key m_keyRepeat;
      };

      // These can't be. :( stupid constructor
      Meta m_meta;
      std::string m_keyText;
    };

    /// Represents a stream of input commands that can be sent to the Environment.
    /** This is basically a std::vector<Command> with a helper function. More complicated Command processing should be done with your own tools. */
    class Sequence {
    public:
      /// Adds a Command to the end of this Sequence.
      void Queue(const Command &element);

      /// Processes all Commands in order.
      /** There is no way to detect whether a specific Command was consumed by the Environment or not; if you need this functionality, you should Process the Commands on your own. */
      void Process(Environment *env) const;

      /// Processes all Commands in order.
      /** There is no way to detect whether a specific Command was consumed by the Environment or not; if you need this functionality, you should Process the Commands on your own. */
      void Process(const EnvironmentPtr &env) const;

      /// Gets the Command queue.
      const std::vector<Command> &GetQueue() const { return m_queue; }

    private:
      std::vector<Command> m_queue;
    };
  }
}

#endif
