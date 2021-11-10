#include "facedisplay.h"
#include "smartpointerhelp.h"

FaceDisplay::FaceDisplay(OpenGLContext *mp_context)
    : Drawable(mp_context), representedFace(nullptr)
{}

void FaceDisplay::create() {
    std::vector<GLuint> idx;
    std::vector<glm::vec4> pos;
    std::vector<glm::vec4> color;

    HalfEdge *e = representedFace->edge;

    int numEdges = 0;
    do {
        ++numEdges;
        pos.push_back(glm::vec4(e->vertex->pos, 1));
        e = e->next;
    } while (e != representedFace->edge);

    for (int i = 0; i < numEdges; ++i) {
        idx.push_back(i);
        idx.push_back((i + 1) % numEdges);
    }

    glm::vec4 c = glm::abs(glm::vec4(1, 1, 1, 1) - glm::vec4(representedFace->color, 1));

    for (int i = 0; i < 4; ++i) {
        color.push_back(c);
    }

    count = idx.size();

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec4), color.data(), GL_STATIC_DRAW);
}

void FaceDisplay::updateFace(Face* f) {
    representedFace = f;
}

GLenum FaceDisplay::drawMode() {
    return GL_LINES;
}
