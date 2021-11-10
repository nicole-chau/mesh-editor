#include "skeleton.h"

Skeleton::Skeleton(OpenGLContext *mp_context)
    : Drawable(mp_context), root(nullptr), joints{}, representedJoint(nullptr)
{}

Skeleton::~Skeleton() {
    destroy();
}

void Skeleton::create() {
    std::vector<GLuint> idx;
    std::vector<glm::vec4> pos;
    std::vector<glm::vec4> color;

    float numVerts = 360.f / 30.f;
    int maxIdx = 0;

    for (int i = 0; i < joints.size(); ++i) {
        glm::vec4 jointPos = joints.at(i)->getOverallTransformation()[3];

        // lines
        if (joints.at(i)->parent) {
            pos.push_back(jointPos);
            pos.push_back(joints.at(i)->parent->getOverallTransformation()[3]);

            color.push_back(glm::vec4(1, 0, 1, 1));
            color.push_back(glm::vec4(1, 1, 0, 1));

            idx.push_back(pos.size() - 2);
            idx.push_back(pos.size() - 1);
        }
    }

    maxIdx = pos.size() - 1;

    for (int i = 0; i < joints.size(); ++i) {
        glm::vec4 jointPos = joints.at(i)->getOverallTransformation()[3];
        glm::mat4 jointMat = joints.at(i)->getOverallTransformation();

        glm::mat4 trans1 = glm::mat4(glm::vec4(1.f, 0, 0, 0),
                                     glm::vec4(0.f, 1, 0, 0),
                                     glm::vec4(0.f, 0, 1, 0),
                                     glm::vec4(glm::vec3(jointPos) * -1.f, 1.f));
        glm::mat4 trans2 = glm::mat4(glm::vec4(1.f, 0, 0, 0),
                                     glm::vec4(0.f, 1, 0, 0),
                                     glm::vec4(0.f, 0, 1, 0),
                                     jointPos);

        // circle x
        glm::vec3 axisX = glm::mat3(joints.at(i)->getOverallTransformation())
                          * glm::vec3(1.f, 0.f, 0.f);
        glm::vec4 offsetX = glm::translate(jointMat, glm::vec3(0, 0.5, 0.f))[3];
        pos.push_back(offsetX);

        for (int j = 0; j < numVerts - 1; ++j) {
            glm::mat4 rot = glm::rotate(glm::mat4(1.f), glm::radians(30.f), axisX);
            offsetX = trans2 * rot * trans1 * offsetX;
            pos.push_back(offsetX);
        }

        int start = maxIdx + 1;
        for (int j = 0; j < numVerts; ++j) {
            if (joints.at(i).get() == representedJoint) {
                color.push_back(glm::vec4(1, 0, 1, 1));
            } else {
                color.push_back(glm::vec4(1, 0, 0, 1));
            }

            idx.push_back(start + j);

            if (j == numVerts - 1) {
                idx.push_back(start);
            } else {
              idx.push_back(start + j + 1);
            }
        }

        maxIdx += numVerts;

        // circle y
        glm::vec3 axisY = glm::mat3(joints.at(i)->getOverallTransformation())
                          * glm::vec3(0.f, 1.f, 0.f);
        glm::vec4 offsetY = glm::translate(jointMat, glm::vec3(0, 0, 0.5))[3];
        pos.push_back(offsetY);

        for (int j = 0; j < numVerts - 1; ++j) {
            glm::mat4 rot = glm::rotate(glm::mat4(1.f), glm::radians(30.f), axisY);
            offsetY = trans2 * rot * trans1 * offsetY;
            pos.push_back(offsetY);
        }

        start = maxIdx + 1;
        for (int j = 0; j < numVerts; ++j) {
            if (joints.at(i).get() == representedJoint) {
                color.push_back(glm::vec4(1, 1, 0, 1));
            } else {
                color.push_back(glm::vec4(0, 1, 0, 1));
            }

            idx.push_back(start + j);

            if (j == numVerts - 1) {
                idx.push_back(start);
            } else {
              idx.push_back(start + j + 1);
            }
        }

        maxIdx += numVerts;

        // circle z
        glm::vec3 axisZ = glm::mat3(joints.at(i)->getOverallTransformation())
                          * glm::vec3(0.f, 0.f, 1.f);
        glm::vec4 offsetZ = glm::translate(jointMat, glm::vec3(0.f, 0.5, 0.f))[3];
        pos.push_back(offsetZ);

        for (int j = 0; j < numVerts - 1; ++j) {
            glm::mat4 rot = glm::rotate(glm::mat4(1.f), glm::radians(30.f), axisZ);
            offsetZ = trans2 * rot * trans1 * offsetZ;
            pos.push_back(offsetZ);
        }

        start = maxIdx + 1;
        for (int j = 0; j < numVerts; ++j) {
            if (joints.at(i).get() == representedJoint) {
                color.push_back(glm::vec4(0, 1, 1, 1));
            } else {
                color.push_back(glm::vec4(0, 0, 1, 1));
            }

            idx.push_back(start + j);

            if (j == numVerts - 1) {
                idx.push_back(start);
            } else {
              idx.push_back(start + j + 1);
            }
        }

        maxIdx += numVerts;
    }

    count = idx.size();

    generateIdx();
    bindIdx();
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    bindPos();
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateCol();
    bindCol();
    mp_context->glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec4), color.data(), GL_STATIC_DRAW);
}

void Skeleton::updateJoint(Joint * j) {
    representedJoint = j;
}

GLenum Skeleton::drawMode() {
    return GL_LINES;
}

std::vector<glm::mat4> Skeleton::getTransMats() {
    std::vector<glm::mat4> transMats;
    for (uPtr<Joint>& j : joints) {
        transMats.push_back(j->getOverallTransformation());
    }
    return transMats;
}

std::vector<glm::mat4> Skeleton::getBindMats() {
    std::vector<glm::mat4> bindMats;
    for (uPtr<Joint>& j : joints) {
        bindMats.push_back(j->setBindMatrix());
    }
    return bindMats;
}
