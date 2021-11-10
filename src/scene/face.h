#pragma once

#include "halfedge.h"
#include <glm/glm.hpp>
#include <QListWidgetItem>

class HalfEdge;

class Face : public QListWidgetItem
{
private:
    HalfEdge* edge;
    glm::vec3 color;
    int id;
    QString name;

    friend class Mesh;
    friend class FaceDisplay;
    friend class HalfEdgeDisplay;
    friend class MyGL;
    friend class MainWindow;

public:
    static int facePrevId;

    Face();
};

inline int Face::facePrevId = 0;
