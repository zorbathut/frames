// The Frame class

#ifndef FRAMES_FRAME
#define FRAMES_FRAME

#include "frames/color.h"
#include "frames/layout.h"

namespace Frames {
  class Frame : public Layout {
    friend class Environment;

  public:
    static Frame *Create(const std::string &name, Layout *parent);

    static const char *GetStaticType();
    virtual const char *GetType() const { return GetStaticType(); }

    // --------- SetPin

    /// Creates or redefines a single anchor \ref layoutbasics "link".
    /** It is generally recommended to use the other SetPin overloads before this one. See \ref layoutbasics for more information. */
    inline void SetPin(Axis axis, float mypt, const Layout *link, float theirpt, float offset = 0.f) { return zinternalSetPin(axis, mypt, link, theirpt, offset); }
    /// Clears a single \ref layoutbasics "link".
    inline void ClearPin(Axis axis, float mypt) { return zinternalClearPin(axis, mypt); }
    /// Clears a single \ref layoutbasics "link".
    inline void ClearPin(Anchor anchor) { return zinternalClearPin(anchor); }
    /// Clears all \ref layoutbasics "links" on an axis.
    inline void ClearPinAll(Axis axis) { return zinternalClearPinAll(axis); }

    // --------- SetPin variants

    /// SetPin variant for \ref layoutbasics "linking" two \ref Anchor "Anchors".
    inline void SetPin(Anchor myanchor, const Layout *link, Anchor theiranchor) { return zinternalSetPin(myanchor, link, theiranchor); }
    /// SetPin variant for \ref layoutbasics "linking" two \ref Anchor "Anchors" with an offset.
    inline void SetPin(Anchor myanchor, const Layout *link, Anchor theiranchor, float xofs, float yofs) { return zinternalSetPin(myanchor, link, theiranchor, xofs, yofs); }
    /// SetPin variant for \ref layoutbasics "linking" an Anchor to a relative point.
    inline void SetPin(Anchor myanchor, const Layout *link, float theirx, float theiry) { return zinternalSetPin(myanchor, link, theirx, theiry); }
    /// SetPin variant for \ref layoutbasics "linking" an Anchor to a relative point with an offset.
    inline void SetPin(Anchor myanchor, const Layout *link, float theirx, float theiry, float xofs, float yofs) { return zinternalSetPin(myanchor, link, theirx, theiry, xofs, yofs); }
    /// SetPin variant for \ref layoutbasics "linking" a relative point to an Anchor.
    inline void SetPin(float myx, float myy, const Layout *link, Anchor theiranchor) { return zinternalSetPin(myx, myy, link, theiranchor); }
    /// SetPin variant for \ref layoutbasics "linking" a relative point to an Anchor with an offset.
    inline void SetPin(float myx, float myy, const Layout *link, Anchor theiranchor, float xofs, float yofs) { return zinternalSetPin(myx, myy, link, theiranchor, xofs, yofs); }
    /// SetPin variant for \ref layoutbasics "linking" two relative points.
    inline void SetPin(float myx, float myy, const Layout *link, float theirx, float theiry) { return zinternalSetPin(myx, myy, link, theirx, theiry); }
    /// SetPin variant for \ref layoutbasics "linking" two relative points with an offset.
    inline void SetPin(float myx, float myy, const Layout *link, float theirx, float theiry, float xofs, float yofs) { return zinternalSetPin(myx, myy, link, theirx, theiry, xofs, yofs); }

    /// Sets the size of an axis.
    inline void SetSize(Axis axis, float size) { return zinternalSetSize(axis, size); }
    /// Sets the width.
    inline void SetWidth(float size) { return zinternalSetWidth(size); }
    /// Sets the height.
    inline void SetHeight(float size) { return zinternalSetHeight(size); }
    /// Clears the size of an axis.
    inline void ClearSize(Axis axis) { return zinternalClearSize(axis); }

    /// Clears all links and sizes.
    inline void ClearConstraintAll() { return zinternalClearConstraintAll(); }

    /// Sets the parent.
    /** New parent must be non-null and a member of the same environment. */
    inline void SetParent(Layout *layout) { return zinternalSetParent(layout); }

    /// Sets the name.
    inline void SetName(const std::string &name) { return zinternalSetName(name); }

    /// Sets the layer.
    /** Layers are used to determine frame render and input order. Higher-numbered layers are layered on top of lower-numbered layers. All floating-point values are acceptable besides infinities and NaN. */
    inline void SetLayer(float layer) { return zinternalSetLayer(layer); }
    /// Gets the layer.
    inline float GetLayer() const { return zinternalGetLayer(); }

    /// Sets the implementation flag.
    /** The implementation flag is used to create new frame types that are composited out of subframes. All frames with this flag set will be rendered before frames without the flag set. In addition, frames with this flag will not be returned from GetChildren() (TODO: nyi!). */
    inline void SetImplementation(bool implementation) { return zinternalSetImplementation(implementation); }
    /// Gets the implementation flag.
    inline bool GetImplementation() const { return zinternalGetImplementation(); }

    /// Destroys this frame and all its children.
    /** Also destroys all \ref layoutbasics "links" to and from these layouts. It is undefined behavior to refer to this frame or any of its children after this function is called. */
    inline void Obliterate() { return zinternalObliterate(); }

    void SetBackground(const Color &color);
    const Color &GetBackground() const { return m_bg; }

    void SetFocus(bool focus);
    bool GetFocus() const;

  protected:
    Frame(const std::string &name, Layout *parent);
    virtual ~Frame();

    virtual void RenderElement(detail::Renderer *renderer) const;

    void luaF_Register(lua_State *L) const { luaF_RegisterWorker(L, GetStaticType()); Layout::luaF_Register(L); }

    static void luaF_RegisterFunctions(lua_State *L);

  private:
    Color m_bg;

    // Lua bindings
    static int luaF_SetPin(lua_State *L);

    static int luaF_SetWidth(lua_State *L);
    static int luaF_SetHeight(lua_State *L);

    static int luaF_SetParent(lua_State *L);
    static int luaF_GetParent(lua_State *L);

    static int luaF_SetLayer(lua_State *L);
    static int luaF_GetLayer(lua_State *L);

    static int luaF_SetImplementation(lua_State *L);
    static int luaF_GetImplementation(lua_State *L);

    static int luaF_SetVisible(lua_State *L);
    static int luaF_GetVisible(lua_State *L);

    static int luaF_SetAlpha(lua_State *L);
    static int luaF_GetAlpha(lua_State *L);

    static int luaF_SetBackground(lua_State *L);
    static int luaF_GetBackground(lua_State *L);

    static int luaF_SetFocus(lua_State *L);
    static int luaF_GetFocus(lua_State *L);

    static int luaF_Obliterate(lua_State *L);
  };
}

#endif
