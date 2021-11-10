#include "face.h"

Face::Face()
    : edge(nullptr), color(glm::vec3(rand() / (float) RAND_MAX,
                                     rand() / (float) RAND_MAX,
                                     rand() / (float) RAND_MAX)),
      id(facePrevId++), name("Face ")
{
    name.append(QString::number(id));
    QListWidgetItem::setText(name);
}
