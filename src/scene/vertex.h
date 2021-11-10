#pragma once

#include "halfedge.h"
#include <glm/glm.hpp>
#include <QListWidgetItem>

class HalfEdge;

class Vertex : public QListWidgetItem {
private:
    glm::vec3 pos;
    HalfEdge* edge;
    int id;
    QString name;
    std::vector<std::pair<int, float>> influencers; // pair of joint ID and weight

    friend class Mesh;
    friend class HalfEdge;
    friend class VertexDisplay;
    friend class FaceDisplay;
    friend class HalfEdgeDisplay;
    friend class MyGL;
    friend class MainWindow;

public:
    static int vertexPrevId;

    Vertex();
    Vertex(glm::vec3 pos);
};

inline int Vertex::vertexPrevId = 0;
