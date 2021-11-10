#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include "smartpointerhelp.h"
#include <QTreeWidgetItem>

class Joint : public QTreeWidgetItem {
private:
    int id;
    QString name;
    Joint* parent;
    std::vector<Joint*> children;
    glm::vec3 pos;
    glm::quat rotation;
    glm::mat4 bindMatrix;

    friend class Skeleton;
    friend class MyGL;
    friend class MainWindow;
public:
    static int jointPrevId;

    Joint();
    Joint(QString name,
          Joint* parent,
          glm::vec3 pos,
          glm::quat rotation);
    virtual ~Joint();

    glm::mat4 getLocalTransformation();
    glm::mat4 getOverallTransformation();
    void addChild(Joint* j);
    void setName(QString name);
    glm::mat4 setBindMatrix();
};

inline int Joint::jointPrevId = 0;
