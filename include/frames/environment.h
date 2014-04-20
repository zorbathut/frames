// Environment class

#ifndef FRAMES_ENVIRONMENT
#define FRAMES_ENVIRONMENT

#include "frames/configuration.h"
#include "frames/detail.h"
#include "frames/input.h"
#include "frames/noncopyable.h"
#include "frames/vector.h"

#include <deque>
#include <vector>
#include <set>
#include <map>

struct lua_State;

namespace Frames {
  class Frame;
  class Layout;
  class VerbBase;

  namespace detail {
    class Renderer;
    class TextManager;
    class TextureManager;
  }

  /// Coordinator class for almost all Frames state. Every Frames-using program will contain at least one Environment.
  /**
  A Frames::Environment is a self-contained Frames instance. It handles input, owns a hierarchy of \ref Frame "Frames", manages resources, and contains all the configuration data needed to use Frames. Every Frames-using program will contain at least one Environment.
  
  While Environment is not locked to any individual thread, it is fundamentally single-threaded. It is undefined behavior to call any Environment function, or any function on a Frame owned by an Environment, while any other such function is being run in another thread. However, multiple Environments can be used in parallel without issue. In most cases, it is also valid to call an Environment or Frame function while handling an event dispatch.*/
  class Environment : detail::Noncopyable {
  public:
    /// Initialize an Environment with default values.
    /**
    Equivalent to calling Environment(const Configuration &config); with a default-constructed Configuration.*/
    Environment();

    /// Initialize an Environment with custom values.
    /**
    See Configuration for details.*/
    Environment(const Configuration &config);

    /// Completely shuts down an Environment.
    /**
    Immediately destroys all child \ref Frame "Frames", cleans up all stored resources and event handles, cleans up any hooks in scripting languages, and so forth.*/
    ~Environment();

    // ==== Updates to the state of the environment
    
    // Mouse update functions

    /// Informs the environment that the mouse pointer has moved.
    /**
    Takes coordinates in pixelspace. Mouse movement happens immediately and may trigger Layout::Event::MouseOver/\ref Layout::Event::MouseOut "MouseOut"/\ref Layout::Event::MouseMove "MouseMove"/\ref Layout::Event::MouseMoveoutside "MouseMoveoutside" events. */
    void MouseMove(int x, int y);

    /// Informs the environment that a mouse button has been pressed.
    /**
    Button state changes happen immediately and may trigger \ref Layout::Event::MouseLeftDown "Layout::Event::Mouse*Down" events.
    All integers are valid and will trigger the appropriate \ref Layout::Event::MouseButtonDown "Layout::Event::MouseButton*" event.
    However, 0 will also trigger \ref Layout::Event::MouseLeftDown "Layout::Event::MouseLeft*" events, 1 will also trigger \ref Layout::Event::MouseRightDown "Layout::Event::MouseRight*" events, and 2 will also trigger \ref Layout::Event::MouseMiddleDown "Layout::Event::MouseMiddle*" events. */
    bool MouseDown(int button);

    /// Informs the environment that a mouse button has been released.
    /**
    Button state changes happen immediately and may trigger \ref Layout::Event::MouseLeftUp "Layout::Event::Mouse*Up", \ref Layout::Event::MouseLeftUpoutside "Layout::Event::Mouse*Upoutside", and \ref Layout::Event::MouseLeftClick "Layout::Event::Mouse*Click" events.
    All integers are valid and will trigger the appropriate \ref Layout::Event::MouseButtonUp "Layout::Event::MouseButton*" event.
    However, 0 will also trigger \ref Layout::Event::MouseLeftUp "Layout::Event::MouseLeft*" events, 1 will also trigger \ref Layout::Event::MouseRightUp "Layout::Event::MouseRight*" events, and 2 will also trigger \ref Layout::Event::MouseMiddleUp "Layout::Event::MouseMiddle*" events. */
    bool MouseUp(int button);

    /// Informs the environment that the mouse wheel has been rotated.
    /**
    Wheel state changes happen immediately and may trigger Layout::Event::MouseWheel events. */
    bool MouseWheel(int delta);

    /// Informs the environment that the mouse no longer exists in the scene.
    /**
    Mouse movement happens immediately and may trigger Layout::Event::MouseOut events. */
    void MouseClear();

    bool KeyDown(const Key &key);
    bool KeyType(const std::string &type);
    bool KeyRepeat(const Key &key);
    bool KeyUp(const Key &key);

    // "Hey, the resolution has been changed/the window has been resized"
    void ResizeRoot(int x, int y);

    // beep boop more docs
    bool Input(const Input &ie); // Usable as a single-package class, intended for the OS helper functions or for people who want a single data pathway
    
    // ==== State as of the current event that's being handled
    const Vector &GetMouse() const { return m_mouse; }
    bool IsShift() const;
    bool IsCtrl() const;
    bool IsAlt() const;
    
    // ==== Focus
    Layout *GetFocus() { return m_focus; }
    const Layout *GetFocus() const { return m_focus; }
    void SetFocus(Layout *layout);
    void ClearFocus();

    // ==== Rendering
    void Render(const Layout *root = 0);
    
    // ==== Introspection
    Layout *GetRoot() { return m_root; }
    Layout *GetLayoutUnder(float x, float y);

    const Configuration &GetConfiguration() { return m_config; }

    // ==== Scripting languages
    // Lua
    void LuaRegister(lua_State *L, bool hasErrorHandle = false); // if error handle is true, top element of the stack will be popped
    template<typename T> void LuaRegisterFrame(lua_State *L); // needed only if you create third-party frames - Frame will automatically register its internal frame types
    void LuaRegisterEvent(lua_State *L, VerbBase *feb);
    void LuaUnregister(lua_State *L);

    // ==== Internal only, do not call below this line (TODO make more private)
    detail::TextManager *GetTextManager() { return m_text_manager; }
    detail::TextureManager *GetTextureManager() { return m_texture_manager; }

    void LogError(const std::string &log) { if (m_config.logger) m_config.logger->LogError(log); }
    void LogDebug(const std::string &log) { if (m_config.logger) m_config.logger->LogDebug(log); }

    class Performance {
    public:
      Performance(Environment *env, float r, float g, float b);
      ~Performance();

    private:
      Environment *m_env;
      void *m_handle;
    };
  private:
    friend class Layout;
    friend class Frame;

    // Unique ID code
    unsigned int RegisterFrame();
    unsigned int m_counter;

    // Utility functions and parameters
    void Init(const Configuration &config);
    template<typename T> void LuaRegisterFrameLookup(lua_State *L);

    void MarkInvalidated(Layout *layout);
    void UnmarkInvalidated(Layout *layout); // This is currently very slow.
    std::deque<Layout *> m_invalidated;

    // Layout sanity
    void LayoutStack_Push(const Layout *layout, Axis axis, float pt);
    void LayoutStack_Push(const Layout *layout, Axis axis);
    void LayoutStack_Pop();
    void LayoutStack_Error();
    struct LayoutStack_Entry {
      const Layout *layout;
      Axis axis;
      float point;
    };
    std::vector<LayoutStack_Entry> m_layoutStack;
    
    // Maintenance
    void DestroyingLayout(Layout *layout);

    // Configuration
    Configuration m_config;

    // Managers
    detail::Renderer *m_renderer;
    detail::TextManager *m_text_manager;
    detail::TextureManager *m_texture_manager;

    // Root
    Layout *m_root;
    
    Layout *m_over;
    Layout *m_focus;
    std::map<int, Layout *> m_buttonDown;
    Vector m_mouse;
    Key m_lastEvent; // stores the shift/ctrl/alt states

    // Lua
    class LuaStackChecker {
    public:
      LuaStackChecker(lua_State *L, Environment *env, int offset = 0);
      ~LuaStackChecker();

    private:
      int m_depth;
      lua_State *m_L;
      Environment *m_env;
    };
    std::set<lua_State *> m_lua_environments;
    void Lua_PushErrorHandler(lua_State *L);
    static int luaF_errorDefault(lua_State *L);
  };
}

#endif
