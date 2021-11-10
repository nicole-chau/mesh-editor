#include "joint.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

Joint::Joint()
    : id(jointPrevId++),
      name(""),
      parent(nullptr),
      children{},
      pos(glm::vec3(0.f)),
      rotation(glm::quat(0.f, 0.f, 0.f, 0.f)),
      bindMatrix(glm::inverse(getOverallTransformation()))
{
    QTreeWidgetItem::setText(0, this->name);
}

Joint::Joint(QString name,
             Joint* parent,
             glm::vec3 pos,
             glm::quat rotation)
    : id(jointPrevId++),
      name(name),
      parent(parent),
      children{},
      pos(pos),
      rotation(rotation),
      bindMatrix(glm::inverse(getOverallTransformation()))
{
    QTreeWidgetItem::setText(0, this->name);
}

Joint::~Joint() {}

glm::mat4 Joint::getLocalTransformation() {
    glm::mat4 rotMat = glm::mat4_cast(rotation);

    glm::mat4 transMat = glm::mat4(glm::vec4(1, 0, 0, 0),
                                   glm::vec4(0, 1, 0, 0),
                                   glm::vec4(0, 0, 1, 0),
                                   glm::vec4(pos, 1));

    return transMat * rotMat;
}

glm::mat4 Joint::getOverallTransformation() {
    if (this->parent) {
        glm::mat4 overallTrans(1.f);
        return this->parent->getOverallTransformation() * this->getLocalTransformation();
    } else {
        return this->getLocalTransformation();
    }
}

void Joint::addChild(Joint* j) {
    QTreeWidgetItem::addChild(j);
    children.push_back(j);
}

void Joint::setName(QString name) {
    this->name = name;
    QTreeWidgetItem::setText(0, this->name);
}

glm::mat4 Joint::setBindMatrix() {
    this->bindMatrix = glm::inverse(getOverallTransformation());
    return this->bindMatrix;
}

