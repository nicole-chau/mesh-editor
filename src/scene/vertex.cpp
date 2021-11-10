#include "vertex.h"

Vertex::Vertex()
    : pos(glm::vec3()), edge(nullptr), id(vertexPrevId++), name("Vertex ")
{
    name.append(QString::number(id));
    QListWidgetItem::setText(name);
}

Vertex::Vertex(glm::vec3 pos)
    : pos(pos), edge(nullptr), id(vertexPrevId++), name("Vertex ")
{
    name.append(QString::number(id));
    QListWidgetItem::setText(name);
}
