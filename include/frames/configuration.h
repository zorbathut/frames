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

#ifndef FRAMES_CONFIGURATION
#define FRAMES_CONFIGURATION

#include <string>

#include "frames/color.h"
#include "frames/ptr.h"

namespace Frames {
  class Environment;
  class Stream;
  typedef Ptr<Stream> StreamPtr;
  class Texture;
  typedef Ptr<Texture> TexturePtr;

  namespace detail {
    class Renderer;
  }

  namespace Configuration {
    /// Interface for debug logging functionality.
    /** Default behavior outputs to the Debug Log on Windows. */
    class Logger : public Refcountable<Logger> {
    public:
      virtual ~Logger() { }

      /// Called when errors are logged.
      /** Errors include anything that is thoroughly incorrect, including invalid parameters and conflicting input.
      Technically, the entire Frames system is left in an inconsistent and unpredictable state after a single Error, although the code does make an attempt to clean up and avoid crashes. 

      It is strongly recommend that Error messages be treated with similar severity to an assert(), up to and including large visible error messages and/or modal dialogs.

      The implementation of this function must be threadsafe.*/
      virtual void LogError(const std::string &log);

      /// Called when debug output is logged.
      /** This tends to be pretty spammy. It's very helpful for analyzing what's going on internally, but you may want to disable it by default or provide some convenient way of filtering it.

      The implementation of this function must be threadsafe.*/
      virtual void LogDebug(const std::string &log);
    };
    /// Refcounted Logger typedef.
    typedef Ptr<Logger> LoggerPtr;

    /// Interface for clipboard access.
    /** Default behavior uses the standard Windows global clipboard. */
    class Clipboard : public Refcountable<Clipboard> {
    public:
      virtual ~Clipboard() { }

      /// Called to set the clipboard contents to a given plaintext string.
      virtual void Set(const std::string &dat);

      /// Called to retrieve the clipboard text.
      /** If there is no text in the clipboard, returns the empty string. */
      virtual std::string Get();
    };
    /// Refcounted Clipboard typedef.
    typedef Ptr<Clipboard> ClipboardPtr;

    /// Interface for a performance monitoring system.
    /** Push() will be called with a human-readable name and a color ID code intended for use in visual profilers. The void* it returns will be passed into Pop().

    It is guaranteed that Push/Pop calls will be done in LIFO order.

    Default behavior is no-op.*/
    class Performance : public Refcountable<Performance> {
    public:
      virtual ~Performance() { }

      /// Start a profiling block.
      virtual void *Push(const char *name, Color color) { return 0; }

      /// End a profiling block.
      virtual void Pop(void *) { }
    };
    /// Refcounted Performance typedef.
    typedef Ptr<Performance> PerformancePtr;

    /// Interface to create a Texture from a \ref basicsresources "resource ID".
    /** See \ref basicsresources "Resources" for more detail.

    Default behavior is to use the configuration StreamFromId and the configuration TextureFromStream in order to produce the result. */
    class TextureFromId : public Refcountable<TextureFromId> {
    public:
      virtual ~TextureFromId() { }

      /// Returns a new Texture associated with an environment and resource ID.
      virtual TexturePtr Create(Environment *env, const std::string &id);
    };
    /// Refcounted TextureFromId typedef.
    typedef Ptr<TextureFromId> TextureFromIdPtr;

    /// Interface to create a Stream from a \ref basicsresources "resource ID".
    /** See \ref basicsresources "Resources" for more detail.

    Default behavior is to use the configuration PathFromId to create a path, then to create a StreamFile given that path. */
    class StreamFromId : public Refcountable<StreamFromId> {
    public:
      virtual ~StreamFromId() { }

      /// Returns a new Stream associated with an environment and resource ID.
      virtual StreamPtr Create(Environment *env, const std::string &id);
    };
    /// Refcounted StreamFromId typedef.
    typedef Ptr<StreamFromId> StreamFromIdPtr;

    /// Interface to create a path from a \ref basicsresources "resource ID".
    /** See \ref basicsresources "Resources" for more detail.

    Default behavior is to return the given path verbatim. */
    class PathFromId : public Refcountable<PathFromId> {
    public:
      virtual ~PathFromId() { }

      /// Returns a path associated with an environment and resource ID.
      virtual std::string Process(Environment *env, const std::string &id);
    };
    /// Refcounted PathFromId typedef.
    typedef Ptr<PathFromId> PathFromIdPtr;

    /// Interface to create a Texture from a Stream.
    /** See \ref basicsresources "Resources" for more detail.

    Default behavior is to consult each TextureLoader in turn and use the first one that accepts the given Stream. */
    class TextureFromStream : public Refcountable<TextureFromStream> {
    public:
      virtual ~TextureFromStream() { }

      /// Returns a new Texture associated with an environment and created from the given Stream.
      virtual TexturePtr Create(Environment *env, const StreamPtr &stream);
    };
    /// Refcounted TextureFromStream typedef.
    typedef Ptr<TextureFromStream> TextureFromStreamPtr;

    /// Interface to create a Renderer.
    /** Create is called once during the initialization of each Frames environment.
    
    Configuration::Renderer is not intended to be inherited by end-users. Each renderer module has a function which returns a Configuration::RendererPtr.
    
    See Configuration::RendererOpengl, Configuration::RendererDX11, and Configuration::RendererNull. */
    class Renderer : public Refcountable<Renderer> {
    public:
      virtual ~Renderer() { }

      /// Returns a new Renderer instance associated to the given Environment.
      virtual detail::Renderer *Create(Environment *env) const = 0;
    };
    /// Refcounted Renderer typedef.
    typedef Ptr<Renderer> RendererPtr;

    /// Configuration data associated with an Environment.
    /** Every Environment contains an immutable Configuration::Local.

    Most Configuration settings are designed as a class that can be inherited from. The default behavior is available by calling the base class's members. */
    class Local {
    public:
      Local() { }

      /// Sets the Configuration's Logger module.
      void LoggerSet(const LoggerPtr &logger) { m_logger = logger; }
      /// Gets the Configuration's Logger module.
      const LoggerPtr &LoggerGet() const { return m_logger; }

      /// Sets the Configuration's Clipboard module.
      void ClipboardSet(const ClipboardPtr &clipboard) { m_clipboard = clipboard; }
      /// Gets the Configuration's Clipboard module.
      const ClipboardPtr &ClipboardGet() const { return m_clipboard; }

      /// Sets the Configuration's Performance module.
      void PerformanceSet(const PerformancePtr &performance) { m_performance = performance; }
      /// Gets the Configuration's Performance module.
      const PerformancePtr &PerformanceGet() const { return m_performance; }

      /// Sets the Configuration's TextureFromId module.
      /** See \ref basicsresources "Resources" for more detail. */
      void TextureFromIdSet(const TextureFromIdPtr &textureFromId) { m_textureFromId = textureFromId; }
      /// Gets the Configuration's TextureFromId module.
      const TextureFromIdPtr &TextureFromIdGet() const { return m_textureFromId; }

      /// Sets the Configuration's StreamFromId module.
      /** See \ref basicsresources "Resources" for more detail. */
      void StreamFromIdSet(const StreamFromIdPtr &streamFromId) { m_streamFromId = streamFromId; }
      /// Gets the Configuration's StreamFromId module.
      const StreamFromIdPtr &StreamFromIdGet() const { return m_streamFromId; }

      /// Sets the Configuration's PathFromId module.
      /** See \ref basicsresources "Resources" for more detail. */
      void PathFromIdSet(const PathFromIdPtr &pathFromId) { m_pathFromId = pathFromId; }
      /// Gets the Configuration's PathFromId module.
      const PathFromIdPtr &PathFromIdGet() const { return m_pathFromId; }

      /// Sets the Configuration's TextureFromStream module.
      /** See \ref basicsresources "Resources" for more detail. */
      void TextureFromStreamSet(const TextureFromStreamPtr &textureFromStream) { m_textureFromStream = textureFromStream; }
      /// Gets the Configuration's TextureFromStream module.
      const TextureFromStreamPtr &TextureFromStreamGet() const { return m_textureFromStream; }

      /// Sets the Configuration's default font ID.
      void FontDefaultIdSet(const std::string &fontDefaultId) { m_fontDefaultId = fontDefaultId; }
      /// Gets the Configuration's default font ID.
      const std::string &FontDefaultIdGet() const { return m_fontDefaultId; }

      /// Sets the Configuration's Renderer module.
      void RendererSet(const RendererPtr &renderer) { m_renderer = renderer; }
      /// Gets the Configuration's TextureFromStream module.
      const RendererPtr &RendererGet() const { return m_renderer; }

    private:
      LoggerPtr m_logger;
      ClipboardPtr m_clipboard;
      PerformancePtr m_performance;

      TextureFromIdPtr m_textureFromId;
      StreamFromIdPtr m_streamFromId;
      PathFromIdPtr m_pathFromId;
      TextureFromStreamPtr m_textureFromStream;
      
      RendererPtr m_renderer;

      std::string m_fontDefaultId;
    };

    /// Configuration data not associated with an Environment.
    /** Some Configuration elements don't have a valid Environment attached. This class allows you to configure logging handlers for global errors and debug messages. */
    class Global {
    public:
      Global() { }

      /// Sets the Configuration's Logger module.
      void LoggerSet(const LoggerPtr &logger) { m_logger = logger; }
      /// Gets the Configuration's Logger module.
      const LoggerPtr &LoggerGet() const { return m_logger; }

    private:
      LoggerPtr m_logger;
    };


    /// Sets the global configuration data.
    /** Do not call this function while any other Frames function is running in a parallel thread. */
    void Set(const Global &config);

    /// Gets the global configuration data.
    const Global &Get();
  }
}

#endif
