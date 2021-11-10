#pragma once
#include "smartpointerhelp.h"
#include "drawable.h"
#include <vector>
#include "joint.h"

class Skeleton : public Drawable
{
private:
    Joint* root;
    std::vector<uPtr<Joint>> joints;
    Joint* representedJoint;

    friend class MyGL;
    friend class MainWindow;
public:
    Skeleton(OpenGLContext *mp_context);
    virtual ~Skeleton();

    void create() override;
    void updateJoint(Joint*);
    GLenum drawMode() override;
    std::vector<glm::mat4> getTransMats();
    std::vector<glm::mat4> getBindMats();

};

