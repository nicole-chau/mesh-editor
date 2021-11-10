#pragma once

#include "drawable.h"
#include "halfedge.h"

class HalfEdgeDisplay : public Drawable {
protected:
    HalfEdge *representedEdge;

public:
    HalfEdgeDisplay(OpenGLContext *mp_context);
    void create() override;
    void updateHalfEdge(HalfEdge*);
    GLenum drawMode() override;
};
