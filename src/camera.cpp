#include "camera.h"

#include <la.h>
#include <iostream>
#include <math.h>


Camera::Camera():
    Camera(400, 400)
{
    look = glm::vec3(0,0,-1);
    up = glm::vec3(0,1,0);
    right = glm::vec3(1,0,0);

    phi = 0.f;
    theta = 0.f;
    r = 10.f;
}

Camera::Camera(unsigned int w, unsigned int h):
    Camera(w, h, glm::vec3(0,0,10), glm::vec3(0,0,0), glm::vec3(0,1,0))
{}

Camera::Camera(unsigned int w, unsigned int h, const glm::vec3 &e, const glm::vec3 &r, const glm::vec3 &worldUp):
    fovy(45),
    width(w),
    height(h),
    near_clip(0.1f),
    far_clip(1000),
    eye(e),
    ref(r),
    world_up(worldUp),

    phi(0.f), theta(0.f), r(10.f)
{
    RecomputeAttributes();
//    recomputePolarAttributes();
}

Camera::Camera(const Camera &c):
    fovy(c.fovy),
    width(c.width),
    height(c.height),
    near_clip(c.near_clip),
    far_clip(c.far_clip),
    aspect(c.aspect),
    eye(c.eye),
    ref(c.ref),
    look(c.look),
    up(c.up),
    right(c.right),
    world_up(c.world_up),
    V(c.V),
    H(c.H),

    phi(c.phi), theta(c.theta), r(c.r)
{}


void Camera::RecomputeAttributes()
{
    look = glm::normalize(ref - eye);
    right = glm::normalize(glm::cross(look, world_up));
    up = glm::cross(right, look);

    float tan_fovy = tan(glm::radians(fovy/2));
    float len = glm::length(ref - eye);
    aspect = width / static_cast<float>(height);
    V = up*len*tan_fovy;
    H = right*len*aspect*tan_fovy;
}

void Camera::recomputePolarAttributes() {
    glm::vec4 unitEye(0.f, 0.f, 0.f, 1.f);
    glm::vec4 unitForward(0.f, 0.f, 1.f, 0.f);
    glm::vec4 unitUp(0.f, 1.f, 0.f, 0.f);
    glm::vec4 unitRight(1.f, 0.f, 0.f, 0.f);

    glm::mat4 polarMat(1.f);

    polarMat = glm::rotate(polarMat, glm::radians(theta), glm::vec3(0.f, 1.f, 0.f))
               * glm::rotate(polarMat, glm::radians(phi), glm::vec3(1.f, 0.f, 0.f))
               * glm::translate(polarMat, glm::vec3(0.f, 0.f, r));

    eye = glm::vec3(polarMat * unitEye);
    look = glm::vec3(polarMat * unitForward);
    up = glm::vec3(polarMat * unitUp);
    right = glm::vec3(polarMat * unitRight);
}

glm::mat4 Camera::getViewProj()
{
    return glm::perspective(glm::radians(fovy), width / (float)height, near_clip, far_clip) * glm::lookAt(eye, ref, up);
}

void Camera::RotateAboutUp(float deg)
{
//    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(deg), up);
//    ref = ref - eye;
//    ref = glm::vec3(rotation * glm::vec4(ref, 1));
//    ref = ref + eye;
//    RecomputeAttributes();

    theta += deg;
    recomputePolarAttributes();
}
void Camera::RotateAboutRight(float deg)
{
//    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(deg), right);
//    ref = ref - eye;
//    ref = glm::vec3(rotation * glm::vec4(ref, 1));
//    ref = ref + eye;
//    RecomputeAttributes();

    phi += deg;
    recomputePolarAttributes();
}

void Camera::TranslateAlongLook(float amt)
{
//    glm::vec3 translation = look * amt;
//    eye += translation;
//    ref += translation;

    r -= amt;
    recomputePolarAttributes();
}

void Camera::TranslateAlongRight(float amt)
{
    glm::vec3 translation = right * amt;
    eye += translation;
    ref += translation;

}
void Camera::TranslateAlongUp(float amt)
{
    glm::vec3 translation = up * amt;
    eye += translation;
    ref += translation;
}
