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
    /// Debug logging functionality.
    /** Default behavior outputs to the Debug Log on Windows. */
    class Logger : public Refcountable<Logger> {
    public:
      virtual ~Logger() { }

      /// Called when errors are logged.
      /** Errors include anything that is thoroughly incorrect, including invalid parameters and conflicting input.
      It is strongly recommend that Error messages be treated with the same severity as an assert().
      Technically, the entire Frames system is left in an inconsistent and unpredictable state after a single Error, although the code does make an attempt to clean up and avoid crashes. */
      virtual void LogError(const std::string &log);

      /// Called when debug output is logged.
      /** This tends to be pretty spammy. It's very helpful for analyzing what's going on internally, but you may want to disable it by default or provide some convenient way of filtering it. */
      virtual void LogDebug(const std::string &log);
    };
    /// Refcounted Logger typedef.
    typedef Ptr<Logger> LoggerPtr;

    /// Provides clipboard access for copy-paste behavior on Text frames.
    /** Default behavior uses the standard Windows global clipboard. */
    class Clipboard : public Refcountable<Clipboard> {
    public:
      virtual ~Clipboard() { }

      /// Called to set the clipboard contents to a given string.
      virtual void Set(const std::string &dat);

      /// Called to retrieve the clipboard contents. Empty string is interpreted as no clipboard contents.
      virtual std::string Get();
    };
    /// Refcounted Clipboard typedef.
    typedef Ptr<Clipboard> ClipboardPtr;

    /// Hooks for a performance monitoring system.
    /** Push() will be called with a human-readable name and a color ID code intended for use in visual profilers. It may return an opaque void* that will be provided in Pop().

    It is guaranteed that Push/Pop calls will be done in LIFO order.

    Default behavior is a no-op.*/
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

    /// Creates a Texture from a \ref basicsresources "resource ID".
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

    /// Creates a Stream from a \ref basicsresources "resource ID".
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

    /// Creates a path from a \ref basicsresources "resource ID".
    /** See \ref basicsresources "Resources" for more detail.

    Default behavior is to return to given path verbatim. */
    class PathFromId : public Refcountable<PathFromId> {
    public:
      virtual ~PathFromId() { }

      /// Returns a path associated with an environment and resource ID.
      virtual std::string Process(Environment *env, const std::string &id);
    };
    /// Refcounted PathFromId typedef.
    typedef Ptr<PathFromId> PathFromIdPtr;

    /// Creates a Texture from a Stream.
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

    /// Creates a Renderer.
    /** Create is called once during the initialization of each Frames environment.
    
    Renderer has no default implementation. It must be set manually. */
    class Renderer : public Refcountable<Renderer> {
    public:
      virtual ~Renderer() { }

      /// Returns a new Renderer instance associated to the given Environment.
      virtual detail::Renderer *Create(Environment *env) const = 0;
    };
    /// Refcounted Renderer typedef.
    typedef Ptr<Renderer> RendererPtr;

    /// All configuration data that needs to be provided for a functioning Environment.
    /** Every Environment contains a Configuration. If a Configuration isn't provided when the Environment is constructed, a default Configuration will be built.

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
      void TextureFromIdSet(const TextureFromIdPtr &textureFromId) { m_textureFromId = textureFromId; }
      /// Gets the Configuration's TextureFromId module.
      const TextureFromIdPtr &TextureFromIdGet() const { return m_textureFromId; }

      /// Sets the Configuration's StreamFromId module.
      void StreamFromIdSet(const StreamFromIdPtr &streamFromId) { m_streamFromId = streamFromId; }
      /// Gets the Configuration's StreamFromId module.
      const StreamFromIdPtr &StreamFromIdGet() const { return m_streamFromId; }

      /// Sets the Configuration's PathFromId module.
      void PathFromIdSet(const PathFromIdPtr &pathFromId) { m_pathFromId = pathFromId; }
      /// Gets the Configuration's PathFromId module.
      const PathFromIdPtr &PathFromIdGet() const { return m_pathFromId; }

      /// Sets the Configuration's TextureFromStream module.
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

    /// Configuration data that cannot be associated with a single Environment.
    /** Some Configuration elements don't have a valid Environment attached. This class allows you to configure log handlers for errors and debug messages without a Configuration available. */
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
    void Set(const Global &config);

    /// Gets the global configuration data.
    const Global &Get();
  }
}

#endif
