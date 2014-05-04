// The Configuration class

#ifndef FRAMES_CONFIGURATION
#define FRAMES_CONFIGURATION

#include <string>

#include "frames/color.h"
#include "frames/os_gl.h"
#include "frames/ptr.h"

namespace Frames {
  class Environment;
  class Stream;
  class Texture;

  /// All configuration data that needs to be provided for a functioning Environment.
  /** Every Environment contains a Configuration. If a Configuration isn't provided when the Environment is constructed, a default Configuration will be built.
  
  Most Configuration settings are designed as a class that can be inherited from. The class's default behavior is available on request, using standard inheritance behavior. */
  struct Configuration {
  public:
    Configuration();

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

      /// Called when debug output is requested.
      virtual void LogDebug(const std::string &log);
    };

    /// Clipboard access for copy-paste behavior on Text frames.
    /** Default behavior uses the standard Windows global clipboard. */
    class Clipboard : public Refcountable<Clipboard> {
    public:
      virtual ~Clipboard() { }

      /// Called to set the clipboard contents to a given string.
      virtual void Set(const std::string &dat);

      /// Called to retrieve the clipboard contents. Empty string is interpreted as no clipboard contents.
      virtual std::string Get();
    };

    /// Hooks for a performance monitoring system.
    /** Push() will be called with a human-readable name and a color ID code intended for use in visual profilers. It may return an opaque void* that will be provided in Pop().

    It is guaranteed that Push/Pop calls will be done in LIFO order.
    
    Default behavior is a no-op.*/
    class Performance : public Refcountable<Performance> {
    public:
      virtual ~Performance() { }

      // Start a profiling block.
      virtual void *Push(const char *name, Color color) { return 0; }

      // End a profiling block.
      virtual void Pop(void *) {}
    };

    /// Creates a Texture from a resource ID.
    /** See \ref resources "Resources" for more detail.
    
    Default behavior is to use the configuration StreamFromId and the configuration TextureFromStream in order to produce the result. */
    class TextureFromId : public Refcountable<TextureFromId> {
    public:
      virtual ~TextureFromId() {}

      /// Returns a new Texture associated with an environment and resource ID.
      virtual Ptr<Texture> Create(Environment *env, const std::string &id);
    };

    /// Creates a Stream from a resource ID.
    /** See \ref resources "Resources" for more detail.
    
    Default behavior is to use the configuration PathFromId to create a path, then to create a StreamFile given that path. */
    class StreamFromId : public Refcountable<StreamFromId> {
    public:
      virtual ~StreamFromId() {}

      /// Returns a new Stream associated with an environment and resource ID.
      virtual Ptr<Stream> Create(Environment *env, const std::string &id);
    };

    /// Creates a path from a resource ID.
    /** See \ref resources "Resources" for more detail.
    
    Default behavior is to return to given path verbatim. */
    class PathFromId : public Refcountable<PathFromId> {
    public:
      virtual ~PathFromId() {}

      /// Returns a path associated with an environment and resource ID.
      virtual std::string Process(Environment *env, const std::string &id);
    };

    /// Creates a Texture from a Stream.
    /** See \ref resources "Resources" for more detail.

    Default behavior is to consult each TextureLoader in turn and use the first one that accepts the given Stream. */
    class TextureFromStream : public Refcountable<TextureFromStream> {
    public:
      virtual ~TextureFromStream() {}

      /// Returns a new Texture associated with an environment and created from the given Stream.
      virtual Ptr<Texture> Create(Environment *env, const Ptr<Stream> &stream);
    };

    /// Sets the Configuration's Logger module.
    void LoggerSet(const Ptr<Logger> &logger) { m_logger = logger; }
    /// Gets the Configuration's Logger module.
    const Ptr<Logger> &LoggerGet() const { return m_logger;  }

    /// Sets the Configuration's Clipboard module.
    void ClipboardSet(const Ptr<Clipboard> &clipboard) { m_clipboard = clipboard; }
    /// Gets the Configuration's Clipboard module.
    const Ptr<Clipboard> &ClipboardGet() const { return m_clipboard; }

    /// Sets the Configuration's Performance module.
    void PerformanceSet(const Ptr<Performance> &performance) { m_performance = performance; }
    /// Gets the Configuration's Performance module.
    const Ptr<Performance> &PerformanceGet() const { return m_performance; }

    /// Sets the Configuration's TextureFromId module.
    void TextureFromIdSet(const Ptr<TextureFromId> &textureFromId) { m_textureFromId = textureFromId; }
    /// Gets the Configuration's TextureFromId module.
    const Ptr<TextureFromId> &TextureFromIdGet() const { return m_textureFromId; }

    /// Sets the Configuration's StreamFromId module.
    void StreamFromIdSet(const Ptr<StreamFromId> &streamFromId) { m_streamFromId = streamFromId; }
    /// Gets the Configuration's StreamFromId module.
    const Ptr<StreamFromId> &StreamFromIdGet() const { return m_streamFromId; }

    /// Sets the Configuration's PathFromId module.
    void PathFromIdSet(const Ptr<PathFromId> &pathFromId) { m_pathFromId = pathFromId; }
    /// Gets the Configuration's PathFromId module.
    const Ptr<PathFromId> &PathFromIdGet() const { return m_pathFromId; }

    /// Sets the Configuration's TextureFromStream module.
    void TextureFromStreamSet(const Ptr<TextureFromStream> &textureFromStream) { m_textureFromStream = textureFromStream; }
    /// Gets the Configuration's TextureFromStream module.
    const Ptr<TextureFromStream> &TextureFromStreamGet() const { return m_textureFromStream; }

    /// Sets the Configuration's default font ID.
    void FontDefaultIdSet(const std::string &fontDefaultId) { m_fontDefaultId = fontDefaultId; }
    /// Gets the Configuration's default font ID.
    const std::string &FontDefaultIdGet() const { return m_fontDefaultId; }

  private:
    Ptr<Logger> m_logger;
    Ptr<Clipboard> m_clipboard;
    Ptr<Performance> m_performance;

    Ptr<TextureFromId> m_textureFromId;
    Ptr<StreamFromId> m_streamFromId;
    Ptr<PathFromId> m_pathFromId;
    Ptr<TextureFromStream> m_textureFromStream;
    std::string m_fontDefaultId;
  };
}

#endif
