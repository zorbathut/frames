// Frame event declarations. Theoretically, this file needs to be included from any frame type that defines events.
// Realistically, this file is included from layout.h, which you need to include in order to define new frame types anyway.
// So, y'know, s'all good.

#ifndef FRAMES_EVENT_DECLARATION
#define FRAMES_EVENT_DECLARATION

namespace Frames {
  // we'll add more internal utility functions if/when needed
  #define FRAMES_INTERNAL_FRAMEEVENT_HOOKS(eventname, paramlist, paramlistcomplete) \
      void Event##eventname##Attach(Delegate<void paramlistcomplete> delegate, float order = 0.f); \
      void Event##eventname##Detach(Delegate<void paramlistcomplete> delegate, float order = 0.f / 0.f); \
      static EventId Event##eventname##Id(); \
      typedef void Event##eventname##Functiontype paramlist; \
    private: \
      static char s_event_##eventname##_id; /* the char itself isn't the ID, the address of the char is */ \
    public:

  #define FRAMES_FRAMEEVENT_DECLARE(eventname, paramlist, paramlistcomplete) \
      FRAMES_INTERNAL_FRAMEEVENT_HOOKS(eventname, paramlist, paramlistcomplete) \
    private: \
      void Event##eventname##Trigger paramlist; \
    public:

  #define FRAMES_FRAMEEVENT_DECLARE_BUBBLE(eventname, paramlist, paramlistcomplete) \
      FRAMES_INTERNAL_FRAMEEVENT_HOOKS(eventname, paramlist, paramlistcomplete) \
      FRAMES_INTERNAL_FRAMEEVENT_HOOKS(eventname##Sink, paramlist, paramlistcomplete) \
      FRAMES_INTERNAL_FRAMEEVENT_HOOKS(eventname##Bubble, paramlist, paramlistcomplete) \
    private: \
      void Event##eventname##Trigger paramlist; \
    public:
}

#endif
