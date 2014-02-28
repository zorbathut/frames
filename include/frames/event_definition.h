// Frame event definitions. This file needs to be included from any file that defines events.

#ifndef FRAMES_EVENT_DEFINITION
#define FRAMES_EVENT_DEFINITION

// not that this helps the defines, but whatever
namespace Frames {
  #define FRAMES_FRAMEEVENT_DEFINE(eventname, paramlist) \
    EventType<void paramlist> eventname(#eventname);

  #define FRAMES_FRAMEEVENT_DEFINE_BUBBLE(eventname, paramlist) \
    EventType<void paramlist> eventname##Dive(#eventname "Dive"); \
    EventType<void paramlist> eventname##Bubble(#eventname "Bubble"); \
    EventType<void paramlist> eventname(#eventname, &eventname##Dive, &eventname##Bubble);
}

#endif
