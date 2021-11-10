#ifndef MYGL_H
#define MYGL_H

#include <openglcontext.h>
#include <utils.h>
#include <shaderprogram.h>
#include <scene/squareplane.h>
#include "camera.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>

#include <scene/mesh.h>
#include <scene/vertexdisplay.h>
#include <scene/facedisplay.h>
#include <scene/halfedgedisplay.h>
#include <scene/joint.h>
#include <scene/skeleton.h>

class MyGL
    : public OpenGLContext
{
    Q_OBJECT
private:
    SquarePlane m_geomSquare;// The instance of a unit cylinder we can use to render any cylinder
    ShaderProgram m_progLambert;// A shader program that uses lambertian reflection
    ShaderProgram m_progFlat;// A shader program that uses "flat" reflection (no shadowing at all)
    ShaderProgram m_progSkeleton;

    Mesh m_mesh;
    Skeleton m_skeleton;

    QListWidgetItem *mp_selectedComponent;
    QTreeWidgetItem *mp_selectedJoint;

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Camera m_glCamera;

    friend class MainWindow;

    glm::quat computeQuaternion(float angle, float axisX, float axisY, float axisZ);
    Joint* parseJoint(QJsonObject o, Joint* parent);

public:
    VertexDisplay m_vertDisplay;
    FaceDisplay m_faceDisplay;
    HalfEdgeDisplay m_halfEdgeDisplay;

    explicit MyGL(QWidget *parent = nullptr);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

protected:
    void keyPressEvent(QKeyEvent *e);

signals:
    void sig_sendVertexData(Mesh*);
    void sig_sendFaceData(Mesh*);
    void sig_sendHalfEdgeData(Mesh*);
    void sig_sendJointData(Skeleton*);

public slots:
    void slot_selectVertex(QListWidgetItem *item);
    void slot_selectFace(QListWidgetItem *item);
    void slot_selectHalfEdge(QListWidgetItem *item);

    void slot_changeVertexX(double x);
    void slot_changeVertexY(double y);
    void slot_changeVertexZ(double z);

    void slot_changeFaceRed(double r);
    void slot_changeFaceGreen(double g);
    void slot_changeFaceBlue(double b);

    void slot_changeJointX(double x);
    void slot_changeJointY(double y);
    void slot_changeJointZ(double z);

    void slot_splitEdge();
    void slot_triangulate();

    void slot_loadObj();
    void slot_loadJson();

    void slot_skinning();
};


#endif // MYGL_H
