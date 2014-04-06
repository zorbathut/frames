// Frame event definitions. This file needs to be included from any file that defines events.

#ifndef FRAMES_EVENT_DEFINITION
#define FRAMES_EVENT_DEFINITION

// not that this helps the defines, but whatever
namespace Frames {
  #define FRAMES_VERB_DEFINE(eventname, paramlist) \
    Verb<void paramlist> eventname(#eventname);

  #define FRAMES_VERB_DEFINE_BUBBLE(eventname, paramlist) \
    VerbPackage<void paramlist> eventname(#eventname, #eventname "Dive", #eventname "Bubble");
}

#endif
