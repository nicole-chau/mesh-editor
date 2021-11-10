#include "vertexdisplay.h"

VertexDisplay::VertexDisplay(OpenGLContext *mp_context)
    : Drawable(mp_context), representedVertex(nullptr)
{}

void VertexDisplay::create() {
    if (representedVertex) {
    std::vector<GLuint> idx;
    std::vector<glm::vec4> pos;
    std::vector<glm::vec4> color;

    idx.push_back(0);

    pos.push_back(glm::vec4(representedVertex->pos, 1));
    color.push_back(glm::vec4(1, 1, 1, 1));

    count = idx.size();

    generateIdx();
    bindIdx();
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec4), color.data(), GL_STATIC_DRAW);
    }
}

void VertexDisplay::updateVertex(Vertex* v) {
    representedVertex = v;
}

GLenum VertexDisplay::drawMode() {
    return GL_POINTS;
}
