#pragma once

#include "drawable.h"
#include "vertex.h"

class VertexDisplay : public Drawable {
protected:
    Vertex *representedVertex;

    friend class MyGL;

public:
    VertexDisplay(OpenGLContext *mp_context);

    // Creates VBO data to make a visual
    // representation of the currently selected Vertex
    void create() override;
    // Change which Vertex representedVertex points to
    void updateVertex(Vertex*);

    GLenum drawMode() override;
};
