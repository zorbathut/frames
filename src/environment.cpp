
#include "frames/environment.h"

#include "frames/frame.h"

#include <GL/gl.h>

namespace Frames {
  Environment::Environment() : m_renderer(this) {
    Configuration config;
    Init(config);
  }
  Environment::Environment(const Configuration &config) : m_renderer(this) {
    Init(config);
  }
  Environment::~Environment() {
    delete m_config_logger_owned;
  };

  void Environment::ResizeRoot(int x, int y) {
    m_root->SetWidth(x);
    m_root->SetHeight(y);
  }

  void Environment::Render(LayoutPtr root) {
    if (!root) {
      root = m_root;
    }

    if (root->GetEnvironment() != this) {
      GetConfiguration().logger->LogError("Attempt to render a frame through an unrelated environment");
      return;
    }

    // We want to batch up events if possible (todo: is this the case? which is faster - flushing as they go, or flushing all at once?) so, two nested loops
    while (!m_invalidated.empty()) {
      while (!m_invalidated.empty()) {
        LayoutPtr layout = m_invalidated.front();
        m_invalidated.pop_front();

        layout->Resolve();
      }

      // send events here
    }

    // GC step?

    m_renderer.Begin(m_root->GetWidth(), m_root->GetHeight());

    root->Render(&m_renderer);

    m_renderer.End();
  };

  void Environment::Init(const Configuration &config) {
    m_config_logger_owned = 0;

    m_config = config;

    if (!m_config.logger) {
      m_config_logger_owned = new Configuration::Logger();
      m_config.logger = m_config_logger_owned;
    }

    m_root = new Layout(0, this);
    m_root->SetNameStatic("Root");
  }

  void Environment::MarkInvalidated(LayoutPtr layout) {
    m_invalidated.push_back(layout);
  }

  void Environment::LayoutStack_Push(const Layout *layout, Axis axis, float pt) {
    LayoutStack_Entry entry = {layout, axis, pt};
    m_layoutStack.push_back(entry);
  }

  void Environment::LayoutStack_Push(const Layout *layout, Axis axis) {
    LayoutStack_Entry entry = {layout, axis, Utility::Undefined};
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

    LogError(Utility::Format("Layout loop dependency detected, axis %c:", (m_layoutStack[0].axis == X) ? 'X' : 'Y'));
    for (int i = m_layoutStack.size(); i > 0; --i) {
      LayoutStack_Entry entry = m_layoutStack[i - 1];
      if (Utility::IsUndefined(entry.point))
        LogError(Utility::Format("  %s: size", entry.layout->GetNameDebug().c_str()));
      else if (entry.point == 0 && entry.axis == X)
        LogError(Utility::Format("  %s: LEFT", entry.layout->GetNameDebug().c_str()));
      else if (entry.point == 0 && entry.axis == Y)
        LogError(Utility::Format("  %s: TOP", entry.layout->GetNameDebug().c_str()));
      else if (entry.point == 0.5)
        LogError(Utility::Format("  %s: CENTER", entry.layout->GetNameDebug().c_str()));
      else if (entry.point == 1 && entry.axis == X)
        LogError(Utility::Format("  %s: RIGHT", entry.layout->GetNameDebug().c_str()));
      else if (entry.point == 1 && entry.axis == Y)
        LogError(Utility::Format("  %s: BOTTOM", entry.layout->GetNameDebug().c_str()));
      else
        LogError(Utility::Format("  %s: %f", entry.layout->GetNameDebug().c_str(), entry.point));
    }
  }
}

