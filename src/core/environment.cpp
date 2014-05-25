
#include "frames/environment.h"

#include "frames/detail_format.h"
#include "frames/frame.h"
#include "frames/renderer.h"
#include "frames/renderer_opengl.h"
#include "frames/text_manager.h"
#include "frames/texture.h"
#include "frames/texture_chunk.h"

namespace Frames {
  /*static*/ EnvironmentPtr Environment::Create() {
    return Create(Configuration::Local());
  }

  /*static*/ EnvironmentPtr Environment::Create(const Configuration::Local &config) {
    return EnvironmentPtr(new Environment(config));
  }

  void Environment::ResizeRoot(int x, int y) {
    m_root->WidthDefaultSet((float)x);
    m_root->HeightDefaultSet((float)y);
  }
  
  void Environment::Input_MouseMove(int ix, int iy) {
    // convert to internal floatingpoint
    float x = (float)ix;
    float y = (float)iy;

    Layout *updated = ProbeAsMouse(x, y);
    m_mouse = Vector(x, y);

    bool mouseover = false;
    if (updated != m_over) {
      Layout *last = m_over;
      m_over = updated; // NOTE: ORDER IS IMPORTANT. Either MouseOut or MouseOver can destroy frames. We need to set m_over *first*, in case either of our calls destroy the new frame and m_over needs to be cleared!

      if (last) {
        last->EventTrigger(Layout::Event::MouseOut);
      }

      mouseover = true;
    }

    // Do our mousemove messages
    if (m_over) {
      m_over->EventTrigger(Layout::Event::MouseMove, Vector(x, y));
    }

    // Do our mousemoveout messages
    for (std::map<int, Layout *>::const_iterator itr = m_buttonDown.begin(); itr != m_buttonDown.end(); ++itr) {
      if (itr->second && itr->second != m_over) {
        itr->second->EventTrigger(Layout::Event::MouseMoveoutside, Vector(x, y));
      }
    }

    if (mouseover && m_over) {
      m_over->EventTrigger(Layout::Event::MouseOver);
    }
  }

  bool Environment::Input_MouseDown(int button) {
    if (m_buttonDown[button]) {
      LogError(detail::Format("Received a mouse down message for button %d while in the middle of a click. Fabricating fake MouseUp message in order to preserve ordering guarantees.", button));
      Input_MouseUp(button);
    }

    if (m_over) {
      m_buttonDown[button] = m_over;

      m_over->EventTrigger(Layout::Event::MouseButtonDown, button);
      if (button == 0) {
        m_over->EventTrigger(Layout::Event::MouseLeftDown);
      } else if (button == 1) {
        m_over->EventTrigger(Layout::Event::MouseRightDown);
      } else if (button == 2) {
        m_over->EventTrigger(Layout::Event::MouseMiddleDown);
      }

      return true;
    }
    return false;
  }

  bool Environment::Input_MouseUp(int button) {
    // be careful: any event can cause m_over or any m_buttonDown to be cleared
    bool consumed = false;

    if (m_over) {
      consumed = true;
      
      m_over->EventTrigger(Layout::Event::MouseButtonUp, button);
      if (m_over) {
        if (button == 0) {
          m_over->EventTrigger(Layout::Event::MouseLeftUp);
        } else if (button == 1) {
          m_over->EventTrigger(Layout::Event::MouseRightUp);
        } else if (button == 2) {
          m_over->EventTrigger(Layout::Event::MouseMiddleUp);
        }
      }

      if (m_over && m_buttonDown[button] == m_over) {
        m_over->EventTrigger(Layout::Event::MouseButtonClick, button);
        if (m_over) {
          if (button == 0) {
            m_over->EventTrigger(Layout::Event::MouseLeftClick);
          } else if (button == 1) {
            m_over->EventTrigger(Layout::Event::MouseRightClick);
          } else if (button == 2) {
            m_over->EventTrigger(Layout::Event::MouseMiddleClick);
          }
        }
      }
    }

    Layout *mbd = m_buttonDown[button];
    if (mbd && mbd != m_over) {
      mbd->EventTrigger(Layout::Event::MouseButtonUpoutside, button);

      if (mbd) {
        if (button == 0) {
          mbd->EventTrigger(Layout::Event::MouseLeftUpoutside);
        } else if (button == 1) {
          mbd->EventTrigger(Layout::Event::MouseRightUpoutside);
        } else if (button == 2) {
          mbd->EventTrigger(Layout::Event::MouseMiddleUpoutside);
        }
      }
    }

    m_buttonDown[button] = 0; // We explicitly do *not* erase items from this set ever! This lets us iterate over the set while removing elements from it.

    return consumed;
  }

  bool Environment::Input_MouseWheel(int delta) {
    if (m_over) {
      m_over->EventTrigger(Layout::Event::MouseWheel, delta);
      return true;
    } else {
      return false;
    }
  }

  void Environment::Input_MouseClear() {
    if (m_over) {
      m_over->EventTrigger(Layout::Event::MouseOut);
      m_over = 0;
    }
  }

  bool Environment::Input_KeyDown(const Input::Key &key) {
    if (m_focus) {
      m_focus->EventTrigger(Layout::Event::KeyDown, key);
      return true;
    }
    return false;
  }

  bool Environment::Input_KeyText(const std::string &type) {
    if (m_focus) {
      m_focus->EventTrigger(Layout::Event::KeyText, type);
      return true;
    }
    return false;
  }

  bool Environment::Input_KeyRepeat(const Input::Key &key) {
    if (m_focus) {
      m_focus->EventTrigger(Layout::Event::KeyRepeat, key);
      return true;
    }
    return false;
  }

  bool Environment::Input_KeyUp(const Input::Key &key) {
    if (m_focus) {
      m_focus->EventTrigger(Layout::Event::KeyUp, key);
      return true;
    }
    return false;
  }

  void Environment::FocusSet(Layout *layout) {
    if (layout && layout->EnvironmentGet() != this) {
      LogError("Attempted to set focus to frame with incorrect environment");
    } else {
      m_focus = layout;
    }
  }

  void Environment::Render(const Layout *root) {
    Performance perf(this, "Environment.Render", Color(0.3f, 0.5f, 0.3f));

    if (!root) {
      root = m_root;
    }

    if (root->EnvironmentGet() != this) {
      Configuration::Get().LoggerGet()->LogError("Attempt to render a frame through an unrelated environment");
      return;
    }

    {
      Performance perf(this, "Environment.Render.Resolve", Color(1, 0, 0));
      // We want to batch up events if possible (todo: is this the case? which is faster - flushing as they go, or flushing all at once?) so, two nested loops
      while (!m_invalidated.empty()) {
        while (!m_invalidated.empty()) {
          Layout *layout = m_invalidated.front();
          m_invalidated.pop_front();

          layout->Resolve();
        }

        // send events here
      }
    }

    {
      Performance perf(this, "Environment.Render.Process", Color(0.5f, 0.8f, 0.5f));

      {
        Performance perf(this, "Environment.Render.Process.Begin", Color(0.4f, 0.2f, 0.2f));
        m_renderer->Begin((int)m_root->WidthGet(), (int)m_root->HeightGet());
      }

      {
        Performance perf(this, "Environment.Render.Process.Render", Color(0.8f, 0.6f, 0.6f));
        root->Render(m_renderer);
      }

      {
        Performance perf(this, "Environment.Render.Process.End", Color(0.4f, 0.2f, 0.2f));
        m_renderer->End();
      }
    }
  }

  Layout *Environment::ProbeAsMouse(float x, float y) {
    // TODO: de-invalidate
    return m_root->ProbeAsMouse(x, y);
  }
  
  Environment::Performance::Performance(Environment *env, const char *name, const Color &color) : m_env(env) {
    m_handle = m_env->ConfigurationGet().PerformanceGet()->Push(name, color);
  }
  Environment::Performance::~Performance() {
    m_env->ConfigurationGet().PerformanceGet()->Pop(m_handle);
  }

  unsigned int Environment::RegisterFrame() {
    if (m_counter == -1) {
      LogError("Frame counter exceeded 4 billion, this maybe should be increased to a 64-bit integer");
    }
    return m_counter++;
  }

  Environment::Environment(const Configuration::Local &config) {
    // init pointers to zero
    m_over = 0;
    m_focus = 0;

    m_renderer = 0;
    m_text_manager = 0;

    m_config = config;

    // Reset config values to defaults, if uninitialized
    if (!m_config.LoggerGet()) {
      m_config.LoggerSet(Configuration::LoggerPtr(new Configuration::Logger()));
    }

    if (!m_config.PerformanceGet()) {
      m_config.PerformanceSet(Configuration::PerformancePtr(new Configuration::Performance()));
    }

    if (!m_config.ClipboardGet()) {
      m_config.ClipboardSet(Configuration::ClipboardPtr(new Configuration::Clipboard()));
    }

    if (!m_config.TextureFromIdGet()) {
      m_config.TextureFromIdSet(Configuration::TextureFromIdPtr(new Configuration::TextureFromId()));
    }

    if (!m_config.StreamFromIdGet()) {
      m_config.StreamFromIdSet(Configuration::StreamFromIdPtr(new Configuration::StreamFromId()));
    }

    if (!m_config.PathFromIdGet()) {
      m_config.PathFromIdSet(Configuration::PathFromIdPtr(new Configuration::PathFromId()));
    }

    if (!m_config.TextureFromStreamGet()) {
      m_config.TextureFromStreamSet(Configuration::TextureFromStreamPtr(new Configuration::TextureFromStream()));
    }

    m_counter = 0;

    m_root = new Layout(this, "Root");

    m_renderer = new detail::RendererOpengl(this);
    m_text_manager = new detail::TextManager(this);
  }

  Environment::~Environment() {
    m_root->zinternalObliterate();

    // this flushes everything out of memory
    while (!m_invalidated.empty()) {
      Layout *layout = m_invalidated.front();
      m_invalidated.pop_front();

      layout->Resolve();
    }

    delete m_text_manager;
    delete m_renderer;
  }

  void Environment::MarkInvalidated(Layout *layout) {
    m_invalidated.push_back(layout);
  }

  void Environment::UnmarkInvalidated(Layout *layout) {
    std::deque<Layout *>::iterator itr = find(m_invalidated.begin(), m_invalidated.end(), layout);
    if (itr == m_invalidated.end()) {
      LogError("Internal problem, attempted to unmark and failed");
    } else {
      m_invalidated.erase(itr);
    }
  }

  void Environment::LayoutStack_Push(const Layout *layout, Axis axis, float pt) {
    LayoutStack_Entry entry = {layout, axis, pt};
    m_layoutStack.push_back(entry);
  }

  void Environment::LayoutStack_Push(const Layout *layout, Axis axis) {
    LayoutStack_Entry entry = {layout, axis, detail::Undefined};
    m_layoutStack.push_back(entry);
  }

  void Environment::LayoutStack_Pop() {
    m_layoutStack.pop_back();
  }

  void Environment::LayoutStack_Error() {
    if (m_layoutStack.empty()) {
      LogError("Layout loop dependency message received, but stack is empty. This should never happen.");
      return;
    }

    LogError(detail::Format("Layout loop dependency detected, axis %c:", (m_layoutStack[0].axis == X) ? 'X' : 'Y'));
    for (int i = (int)m_layoutStack.size(); i > 0; --i) {
      LayoutStack_Entry entry = m_layoutStack[i - 1];
      if (detail::IsUndefined(entry.point))
        LogError(detail::Format("  %s: size", entry.layout->DebugNameGet()));
      else if (entry.point == 0 && entry.axis == X)
        LogError(detail::Format("  %s: LEFT", entry.layout->DebugNameGet()));
      else if (entry.point == 0 && entry.axis == Y)
        LogError(detail::Format("  %s: TOP", entry.layout->DebugNameGet()));
      else if (entry.point == 0.5)
        LogError(detail::Format("  %s: CENTER", entry.layout->DebugNameGet()));
      else if (entry.point == 1 && entry.axis == X)
        LogError(detail::Format("  %s: RIGHT", entry.layout->DebugNameGet()));
      else if (entry.point == 1 && entry.axis == Y)
        LogError(detail::Format("  %s: BOTTOM", entry.layout->DebugNameGet()));
      else
        LogError(detail::Format("  %s: %f", entry.layout->DebugNameGet(), entry.point));
    }
  }

  void Environment::DestroyingLayout(Layout *layout) {
    if (m_over == layout) {
      m_over = 0;
      // TODO: refresh mouse position based on the new layout?
    }

    if (m_focus == layout) {
      m_focus = 0;
    }

    for (std::map<int, Layout *>::iterator itr = m_buttonDown.begin(); itr != m_buttonDown.end(); ++itr) {
      if (itr->second == layout) {
        itr->second = 0;  // We explicitly do *not* erase items from this set ever! This lets us iterate over the set while removing elements from it.
      }
    }
  }


  detail::TextureChunkPtr Environment::TextureChunkFromId(const std::string &id) {
    if (m_texture.left.count(id)) {
      return detail::TextureChunkPtr(m_texture.left.find(id)->second);
    }

    LogDebug(detail::Format("Attempting to load texture %s", id));

    TexturePtr conf = ConfigurationGet().TextureFromIdGet()->Create(this, id);

    detail::TextureChunkPtr rv = TextureChunkFromConfig(conf);

    if (!rv) {
      LogError(detail::Format("Failed to load texture %s", id));
      return rv; // something went wrong
    }

    m_texture.insert(boost::bimap<std::string, detail::TextureChunk *>::value_type(id, rv.Get()));

    return rv;
  }

  detail::TextureChunkPtr Environment::TextureChunkFromConfig(const TexturePtr &tex, detail::TextureBackingPtr backing /*= 0*/) {
    if (!tex) {
      return detail::TextureChunkPtr();
    }

    if (tex->TypeGet() == Texture::RAW) {
      // time to GL-ize it
      // for now we're just putting it into its own texture

      if (!backing) {
        backing = GetRenderer()->BackingCreate(tex->WidthGet(), tex->HeightGet(), tex->FormatGet());
      }

      std::pair<int, int> origin = backing->AllocateSubtexture(tex->WidthGet(), tex->HeightGet());
      backing->Write(origin.first, origin.second, tex);

      detail::TextureChunkPtr chunk = detail::TextureChunk::Create();
      chunk->Attach(backing, origin.first, origin.second, origin.first + tex->WidthGet(), origin.second + tex->HeightGet());

      return chunk;
    } else {
      LogError("Unknown texture type");
      return detail::TextureChunkPtr();
    }
  }

  void Environment::TextureChunkShutdown(detail::TextureChunk *chunk) {
    if (m_texture.right.count(chunk)) {
      m_texture.right.erase(chunk);
    }
  }
}

