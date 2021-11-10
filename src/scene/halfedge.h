#pragma once

#include "vertex.h"
#include "face.h"
#include <glm/glm.hpp>
#include <QListWidgetItem>

class Vertex;
class Face;

class HalfEdge : public QListWidgetItem
{
private:
    HalfEdge* next;
    HalfEdge* sym;
    Vertex* vertex;
    Face* face;
    int id;
    QString name;

    friend class Mesh;
    friend class FaceDisplay;
    friend class HalfEdgeDisplay;
    friend class MyGL;
    friend class MainWindow;
public:
    static int edgePrevId;

    HalfEdge();
    void setVertex(Vertex *v);
    void setSym(HalfEdge *e);
};

inline int HalfEdge::edgePrevId = 0;

