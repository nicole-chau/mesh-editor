#include "halfedge.h"

HalfEdge::HalfEdge()
    : next(nullptr), sym(nullptr), vertex(nullptr), face(nullptr),
      id(edgePrevId++), name("Edge ")
{
    name.append(QString::number(id));
    QListWidgetItem::setText(name);
}

void HalfEdge::setVertex(Vertex *v) {
    vertex = v;
    v->edge = this;
}

void HalfEdge::setSym(HalfEdge *e) {
    sym = e;
    e->sym = this;
}
