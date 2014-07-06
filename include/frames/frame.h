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

#ifndef FRAMES_FRAME
#define FRAMES_FRAME

#include "frames/color.h"
#include "frames/layout.h"

namespace Frames {
  /// Basic component of Frames UIs, dynamically createable and movable.
  /** The Frame is the fundamental building block of the UI. It represents a rectangular area in space, aligned along the Scene axes. A Frame has exactly one parent and any number of children, each of which are also Frames.

  Frame itself is generally invisible, although BackgroundSet is provided for prototyping and debug output. If Frame is set to non-visible, all of its children will also be hidden. */
  class Frame : public Layout {
    FRAMES_DECLARE_RTTI();
    friend class Environment;

  public:
    /// Creates a new Frame.
    static Frame *Create(Layout *parent, const std::string &name);

    // --------- PinSet

    /// Creates or redefines a single \ref basicspinning "pin".
    /** It is generally recommended to use the other PinSet overloads before this one. See \ref basicspinning for more information. */
    inline void PinSet(Axis axis, float mypt, const Layout *link, float theirpt, float offset = 0.f) { return zinternalPinSet(axis, mypt, link, theirpt, offset); }
    /// Clears a single \ref basicspinning "pin".
    inline void PinClear(Axis axis, float mypt) { return zinternalPinClear(axis, mypt); }
    /// Clears a single \ref basicspinning "pin".
    inline void PinClear(Anchor anchor) { return zinternalPinClear(anchor); }
    /// Clears all \ref basicspinning "pins" on an axis.
    inline void PinClearAll(Axis axis) { return zinternalPinClearAll(axis); }

    // --------- PinSet variants

    /// PinSet variant for \ref basicspinning "pinning" two \ref Anchor "Anchors".
    inline void PinSet(Anchor myanchor, const Layout *link, Anchor theiranchor) { return zinternalPinSet(myanchor, link, theiranchor); }
    /// PinSet variant for \ref basicspinning "pinning" two \ref Anchor "Anchors" with an offset.
    inline void PinSet(Anchor myanchor, const Layout *link, Anchor theiranchor, const Vector &ofs) { return zinternalPinSet(myanchor, link, theiranchor, ofs); }
    /// PinSet variant for \ref basicspinning "pinning" two \ref Anchor "Anchors" with an offset.
    inline void PinSet(Anchor myanchor, const Layout *link, Anchor theiranchor, float xofs, float yofs) { return zinternalPinSet(myanchor, link, theiranchor, xofs, yofs); }
    /// PinSet variant for \ref basicspinning "pinning" an Anchor to a relative point.
    inline void PinSet(Anchor myanchor, const Layout *link, const Vector &their) { return zinternalPinSet(myanchor, link, their); }
    /// PinSet variant for \ref basicspinning "pinning" an Anchor to a relative point with an offset.
    inline void PinSet(Anchor myanchor, const Layout *link, const Vector &their, const Vector &ofs) { return zinternalPinSet(myanchor, link, their, ofs); }
    /// PinSet variant for \ref basicspinning "pinning" an Anchor to a relative point with an offset.
    inline void PinSet(Anchor myanchor, const Layout *link, const Vector &their, float xofs, float yofs) { return zinternalPinSet(myanchor, link, their, xofs, yofs); }
    /// PinSet variant for \ref basicspinning "pinning" an Anchor to a relative point.
    inline void PinSet(Anchor myanchor, const Layout *link, float theirx, float theiry) { return zinternalPinSet(myanchor, link, theirx, theiry); }
    /// PinSet variant for \ref basicspinning "pinning" an Anchor to a relative point with an offset.
    inline void PinSet(Anchor myanchor, const Layout *link, float theirx, float theiry, const Vector &ofs) { return zinternalPinSet(myanchor, link, theirx, theiry, ofs); }
    /// PinSet variant for \ref basicspinning "pinning" an Anchor to a relative point with an offset.
    inline void PinSet(Anchor myanchor, const Layout *link, float theirx, float theiry, float xofs, float yofs) { return zinternalPinSet(myanchor, link, theirx, theiry, xofs, yofs); }
    /// PinSet variant for \ref basicspinning "pinning" a relative point to an Anchor.
    inline void PinSet(const Vector &my, const Layout *link, Anchor theiranchor) { return zinternalPinSet(my, link, theiranchor); }
    /// PinSet variant for \ref basicspinning "pinning" a relative point to an Anchor with an offset.
    inline void PinSet(const Vector &my, const Layout *link, Anchor theiranchor, const Vector &ofs) { return zinternalPinSet(my, link, theiranchor, ofs); }
    /// PinSet variant for \ref basicspinning "pinning" a relative point to an Anchor with an offset.
    inline void PinSet(const Vector &my, const Layout *link, Anchor theiranchor, float xofs, float yofs) { return zinternalPinSet(my, link, theiranchor, xofs, yofs); }
    /// PinSet variant for \ref basicspinning "pinning" two relative points.
    inline void PinSet(const Vector &my, const Layout *link, const Vector &their) { return zinternalPinSet(my, link, their); }
    /// PinSet variant for \ref basicspinning "pinning" two relative points with an offset.
    inline void PinSet(const Vector &my, const Layout *link, const Vector &their, const Vector &ofs) { return zinternalPinSet(my, link, their, ofs); }
    /// PinSet variant for \ref basicspinning "pinning" two relative points with an offset.
    inline void PinSet(const Vector &my, const Layout *link, const Vector &their, float xofs, float yofs) { return zinternalPinSet(my, link, their, xofs, yofs); }
    /// PinSet variant for \ref basicspinning "pinning" two relative points.
    inline void PinSet(const Vector &my, const Layout *link, float theirx, float theiry) { return zinternalPinSet(my, link, theirx, theiry); }
    /// PinSet variant for \ref basicspinning "pinning" two relative points with an offset.
    inline void PinSet(const Vector &my, const Layout *link, float theirx, float theiry, const Vector &ofs) { return zinternalPinSet(my, link, theirx, theiry, ofs); }
    /// PinSet variant for \ref basicspinning "pinning" two relative points with an offset.
    inline void PinSet(const Vector &my, const Layout *link, float theirx, float theiry, float xofs, float yofs) { return zinternalPinSet(my, link, theirx, theiry, xofs, yofs); }
    /// PinSet variant for \ref basicspinning "pinning" a relative point to an Anchor.
    inline void PinSet(float myx, float myy, const Layout *link, Anchor theiranchor) { return zinternalPinSet(myx, myy, link, theiranchor); }
    /// PinSet variant for \ref basicspinning "pinning" a relative point to an Anchor with an offset.
    inline void PinSet(float myx, float myy, const Layout *link, Anchor theiranchor, const Vector &ofs) { return zinternalPinSet(myx, myy, link, theiranchor, ofs); }
    /// PinSet variant for \ref basicspinning "pinning" a relative point to an Anchor with an offset.
    inline void PinSet(float myx, float myy, const Layout *link, Anchor theiranchor, float xofs, float yofs) { return zinternalPinSet(myx, myy, link, theiranchor, xofs, yofs); }
    /// PinSet variant for \ref basicspinning "pinning" two relative points.
    inline void PinSet(float myx, float myy, const Layout *link, const Vector &their) { return zinternalPinSet(myx, myy, link, their); }
    /// PinSet variant for \ref basicspinning "pinning" two relative points with an offset.
    inline void PinSet(float myx, float myy, const Layout *link, const Vector &their, const Vector &ofs) { return zinternalPinSet(myx, myy, link, their, ofs); }
    /// PinSet variant for \ref basicspinning "pinning" two relative points with an offset.
    inline void PinSet(float myx, float myy, const Layout *link, const Vector &their, float xofs, float yofs) { return zinternalPinSet(myx, myy, link, their, xofs, yofs); }
    /// PinSet variant for \ref basicspinning "pinning" two relative points.
    inline void PinSet(float myx, float myy, const Layout *link, float theirx, float theiry) { return zinternalPinSet(myx, myy, link, theirx, theiry); }
    /// PinSet variant for \ref basicspinning "pinning" two relative points with an offset.
    inline void PinSet(float myx, float myy, const Layout *link, float theirx, float theiry, const Vector &ofs) { return zinternalPinSet(myx, myy, link, theirx, theiry, ofs); }
    /// PinSet variant for \ref basicspinning "pinning" two relative points with an offset.
    inline void PinSet(float myx, float myy, const Layout *link, float theirx, float theiry, float xofs, float yofs) { return zinternalPinSet(myx, myy, link, theirx, theiry, xofs, yofs); }

    /// Sets the size of an axis.
    inline void SizeSet(Axis axis, float size) { return zinternalSizeSet(axis, size); }
    /// Sets the width.
    inline void WidthSet(float size) { return zinternalWidthSet(size); }
    /// Sets the height.
    inline void HeightSet(float size) { return zinternalHeightSet(size); }
    /// Clears the size of an axis.
    inline void SizeClear(Axis axis) { return zinternalSizeClear(axis); }

    /// Clears all links and sizes.
    inline void ConstraintClearAll() { return zinternalConstraintClearAll(); }

    /// Sets the parent.
    /** New parent must be non-null and a member of the same environment. */
    inline void ParentSet(Layout *layout) { return zinternalParentSet(layout); }

    /// Sets the name.
    inline void NameSet(const std::string &name) { return zinternalNameSet(name); }

    /// Sets the layer.
    /** Layers are used to determine frame render and input order. Higher-numbered layers are layered on top of lower-numbered layers. All floating-point values are acceptable besides infinities and NaN. */
    inline void LayerSet(float layer) { return zinternalLayerSet(layer); }
    /// Gets the layer.
    inline float LayerGet() const { return zinternalLayerGet(); }

    /// Sets the implementation flag.
    /** The implementation flag is used to create new frame types that are composited out of subframes. All frames with this flag set will be rendered before frames without the flag set. In addition, frames with this flag will not be returned from ChildrenGet() (TODO: nyi!). */
    inline void ImplementationSet(bool implementation) { return zinternalImplementationSet(implementation); }
    /// Gets the implementation flag.
    inline bool ImplementationGet() const { return zinternalImplementationGet(); }

    /// Destroys this frame and all its children.
    /** Also destroys all \ref layoutbasics "links" from these layouts. It is undefined behavior if any other layouts still reference these layouts; it will, however, not cause a crash.
    
    Normally, it is undefined behavior to refer to this frame or any of its children after this function is called.
    
    An exception is if Obliterate() is called during any event handler in this environment.
    In that case, the effect of this call is guaranted to be deferred at least until the last in-flight event finishes with this frame or any of its children.
    The effect may be deferred until all in-flight events have completed entirely. */
    inline void Obliterate() { return zinternalObliterate(); }

    /// Sets the background color.
    /** The background color will be drawn as a solid rectangle of the desired color. This is intended for debugging, although it can also be used for basic UI layout and prototyping. */
    void BackgroundSet(const Color &color);
    /// Gets the background color.
    const Color &BackgroundGet() const { return m_bg; }

  protected:
    /// Creates a new Frame with the given parameters. "parent" must be non-null.
    Frame(Layout *parent, const std::string &name);
    virtual ~Frame();

    /// Renders the Frame background. See Layout::RenderElement for inheritance info.
    virtual void RenderElement(detail::Renderer *renderer) const;

  private:
    Color m_bg;
  };
}

#endif
