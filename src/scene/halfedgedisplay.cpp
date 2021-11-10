#include "halfedgedisplay.h"
#include "smartpointerhelp.h"

HalfEdgeDisplay::HalfEdgeDisplay(OpenGLContext *mp_context)
    : Drawable(mp_context), representedEdge(nullptr)
{}

void HalfEdgeDisplay::create() {
    std::vector<GLuint> idx;
    std::vector<glm::vec4> pos;
    std::vector<glm::vec4> color;

    idx.push_back(0);
    idx.push_back(1);

    HalfEdge *e = representedEdge;

    Vertex* v1 = representedEdge->vertex;

    do {
        e = e->next;
    } while (e->next != representedEdge);

    Vertex* v2 = e->vertex;

    pos.push_back(glm::vec4(v1->pos, 1));
    pos.push_back(glm::vec4(v2->pos, 1));

    color.push_back(glm::vec4(1, 1, 0, 1));
    color.push_back(glm::vec4(1, 0, 0, 1));

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

void HalfEdgeDisplay::updateHalfEdge(HalfEdge* he) {
    representedEdge = he;
}

GLenum HalfEdgeDisplay::drawMode() {
    return GL_LINES;
}
