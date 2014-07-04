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

#include <boost/bimap.hpp>

namespace Frames {
  class Environment;
  typedef Ptr<Environment> EnvironmentPtr;
  class Frame;
  class Layout;
  class VerbBase;

  namespace detail {
    class CharacterInfo;
    class FontInfo;
    class Renderer;
    class TextManager;
    class TextureBacking;
    typedef Ptr<TextureBacking> TextureBackingPtr;
    class TextureChunk;
    typedef Ptr<TextureChunk> TextureChunkPtr;
    class Renderer;
  }

  /// Coordinator class for almost all Frames state. Every Frames-using program will contain at least one Environment.
  /** A Frames::Environment is a self-contained Frames instance. It handles input, owns a hierarchy of \ref Frame "Frames", manages resources, and contains all the configuration data needed to use Frames.
  Every Frames-using program will contain at least one Environment.
  
  While Environment is not locked to any individual thread, it is fundamentally single-threaded.
  It is undefined behavior to call any Environment function, or any function on a Frame owned by an Environment, while any other such function is being run in another thread.
  However, multiple Environments can be used in parallel without issue. In most cases, it is also valid to call an Environment or Frame function while handling an event dispatch.
  
  Environment will clean itself up entirely when it is destroyed, including destroying all child \ref Frame "Frames", cleaning up all stored resources and event handles, cleaning up any hooks in scripting languages, and so forth.*/
  class Environment : public Refcountable<Environment> {
  public:
    /// Initialize an Environment.
    /** See Configuration::Local for parameter details.*/
    static EnvironmentPtr Create(const Configuration::Local &config);

    // ==== Updates to the state of the environment

    // Mouse update functions

    /// Informs the environment that the mouse pointer has moved.
    /** Takes coordinates in pixelspace. Mouse movement happens immediately and may trigger Layout::Event::MouseOver/\ref Layout::Event::MouseOut "MouseOut"/\ref Layout::Event::MouseMove "MouseMove"/\ref Layout::Event::MouseMoveoutside "MouseMoveoutside" events. */
    void Input_MouseMove(int x, int y);

    /// Informs the environment that a mouse button has been pressed.
    /** Button state changes happen immediately and may trigger \ref Layout::Event::MouseLeftDown "Layout::Event::Mouse*Down" events.
    All integers are valid and will trigger the appropriate \ref Layout::Event::MouseButtonDown "Layout::Event::MouseButton*" event.
    However, 0 will also trigger \ref Layout::Event::MouseLeftDown "Layout::Event::MouseLeft*" events, 1 will also trigger \ref Layout::Event::MouseRightDown "Layout::Event::MouseRight*" events, and 2 will also trigger \ref Layout::Event::MouseMiddleDown "Layout::Event::MouseMiddle*" events.
    
    Returns true if the event was consumed by a Frame.*/
    bool Input_MouseDown(int button);

    /// Informs the environment that a mouse button has been released.
    /** Button state changes happen immediately and may trigger \ref Layout::Event::MouseLeftUp "Layout::Event::Mouse*Up", \ref Layout::Event::MouseLeftUpoutside "Layout::Event::Mouse*Upoutside", and \ref Layout::Event::MouseLeftClick "Layout::Event::Mouse*Click" events.
    All integers are valid and will trigger the appropriate \ref Layout::Event::MouseButtonUp "Layout::Event::MouseButton*" event.
    However, 0 will also trigger \ref Layout::Event::MouseLeftUp "Layout::Event::MouseLeft*" events, 1 will also trigger \ref Layout::Event::MouseRightUp "Layout::Event::MouseRight*" events, and 2 will also trigger \ref Layout::Event::MouseMiddleUp "Layout::Event::MouseMiddle*" events.
    
    Returns true if the event was consumed by a Frame.*/
    bool Input_MouseUp(int button);

    /// Informs the environment that the mouse wheel has been rotated.
    /** Wheel state changes happen immediately and may trigger Layout::Event::MouseWheel events. 
    
    Returns true if the event was consumed by a Frame.*/
    bool Input_MouseWheel(int delta);

    /// Informs the environment that the mouse no longer exists in the scene.
    /** Mouse movement happens immediately and may trigger Layout::Event::MouseOut events. */
    void Input_MouseClear();

    /// Gets the environment's last known mouse position.
    /** If MouseClear() has been called, this may be inaccurate. */ // TODO come up with a better system >:(
    const Vector &Input_MouseGet() const { return m_mouse; }

    /// Sets the environment's current metakey flags.
    /** This does not trigger events, merely updates the key meta state intended for event handlers. */
    void Input_MetaSet(const Input::Meta &meta) { m_lastMeta = meta; }

    /// Gets the environment's current metakey flags.
    /** This is intended for input handlers to check if the standard meta buttons - shift, ctrl, and alt - are pressed or not. */
    const Input::Meta &Input_MetaGet() const { return m_lastMeta; }

    /// Informs the environment that a key has been pressed.
    /** Keypresses happen immediately and may trigger Layout::Event::KeyDown events.
    
    Returns true if the event was consumed by a Frame.*/
    bool Input_KeyDown(const Input::Key &key);

    /// Informs the environment that text has been typed.
    /** Typing happens immediately and may trigger Layout::Event::KeyType events.
    
    Returns true if the event was consumed by a Frame.*/
    bool Input_KeyText(const std::string &text);

    /// Informs the environment that a key has repeated.
    /** Key repeats happen immediately and may trigger Layout::Event::KeyRepeat events.
    
    Returns true if the event was consumed by a Frame.*/
    bool Input_KeyRepeat(const Input::Key &key);

    /// Informs the environment that a key has released.
    /** Key releases happen immediately and may trigger Layout::Event::KeyUp events.
    
    Returns true if the event was consumed by a Frame.*/
    bool Input_KeyUp(const Input::Key &key);
    
    // ==== Focus
    /// Sets the key focus to the given Layout.
    /** The key focus directs all key input to a specific Layout (more commonly, Frame). Without a focus, key input will not be consumed.
    
    Call with a parameter of 0 to clear the key focus.*/
    void FocusSet(Layout *layout);
    /// Gets the current key focus.
    Layout *FocusGet() { return m_focus; }
    /// Gets the current key focus.
    const Layout *FocusGet() const { return m_focus; }
    

    // ==== Rendering
    /// Renders a tree of Frames.
    /** This can be used to render a subtree if the desired subroot is passed as a parameter, otherwise it will start from the root. */
    void Render(const Layout *root = 0);

    /// Informs the environment that the rendering environment has resized.
    /** This must be called whenever the render canvas resizes. It will resize Root immediately. */
    void ResizeRoot(int x, int y);
    
    // ==== Introspection
    /// Returns the root layout.
    /** The root layout is the bottom-most Layout. Everything is a child of this. */
    Layout *RootGet() { return m_root; }
    /// Returns the root layout.
    /** The root layout is the bottom-most Layout. Everything is a child of this. */
    const Layout *RootGet() const { return m_root; }

    /// Returns the layout underneath a given coordinate in the context of mouse input.
    /** This can be used to find out what frame would be hit by a mouse even at a certain coordinate. */
    Layout *ProbeAsMouse(float x, float y);

    /// Returns the environment's Configuration.
    const Configuration::Local &ConfigurationGet() { return m_config; }

    // ==== Logging and debugging
    /// Logs a string to the Logger's Error path.
    void LogError(const std::string &log) { m_config.LoggerGet()->LogError(log); }
    /// Logs a string to the Logger's Debug path.
    void LogDebug(const std::string &log) { m_config.LoggerGet()->LogDebug(log); }

    /// Does performance monitoring of scope blocks.
    /** Monitoring is RAII-managed - merely create an Environment::Performance object with the appropriate scope, then its destructor will finish the performance block at the appropriate time.
    
    Be careful if using weird scope-manipulation techniques - Environment::Performance objects must be strictly LIFO. */
    class Performance {
    public:
      /// Begins a new performance block.
      /** name and color will be passed verbatim to \ref Configuration::Performance "Configuration's Performance" class. */
      Performance(Environment *env, const char *name, const Color &color);
      /// Ends a performance block.
      ~Performance();

    private:
      Environment *m_env;
      void *m_handle;
    };

    /// Returns the environment's active Renderer. This should probably not be used by endusers; this will be fixed later.
    detail::Renderer *RendererGet() const { return m_renderer;  }
  private:
    friend class Layout;
    friend class Frame;

    // For access to manager Get functions
    friend class Text;
    friend class Sprite;
    friend class detail::CharacterInfo;
    friend class detail::FontInfo;
    friend class detail::TextureBacking;
    friend class detail::TextureChunk;

    // REMOVE LATER - REFACTORING TEMPORARY
    friend class detail::Renderer;

    // Refcount destructor access
    friend class Refcountable<Environment>;

    // Private constructor/destructor
    Environment(const Configuration::Local &config);
    ~Environment();

    // Unique ID code
    unsigned int RegisterFrame();
    unsigned int m_counter;

    // Utility functions and parameters
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
    Configuration::Local m_config;

    // Managers
    detail::Renderer *GetRenderer() { return m_renderer; }
    detail::TextManager *GetTextManager() { return m_text_manager; }

    detail::Renderer *m_renderer;
    detail::TextManager *m_text_manager;

    // Root
    Layout *m_root;
    
    // Input states
    Layout *m_over;
    Layout *m_focus;
    std::map<int, Layout *> m_buttonDown;
    Vector m_mouse;
    Input::Meta m_lastMeta;

    // Texture manipulation
    detail::TextureChunkPtr TextureChunkFromId(const std::string &id);
    detail::TextureChunkPtr TextureChunkFromConfig(const TexturePtr &conf, detail::TextureBackingPtr backing = detail::TextureBackingPtr());

    boost::bimap<std::string, detail::TextureChunk *> m_texture; // not refcounted, the refcounting needs to deallocate
    void TextureChunkShutdown(detail::TextureChunk *chunk);
  };
}

#endif
