#include "mygl.h"
#include <la.h>
#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      m_geomSquare(this),
      m_progLambert(this), m_progFlat(this), m_progSkeleton(this),
      m_mesh(this),
      m_skeleton(this),
      mp_selectedComponent(nullptr),
      mp_selectedJoint(nullptr),
      m_glCamera(),
      m_vertDisplay(this),
      m_faceDisplay(this),
      m_halfEdgeDisplay(this)
{
    setFocusPolicy(Qt::StrongFocus);
}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    //m_geomSquare.destroy();

    m_mesh.destroy();
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instances of Cylinder and Sphere.
    //m_geomSquare.create();

    m_mesh.buildCube();
    m_mesh.create();

    // Create and set up the diffuse shader
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");
    // create and set up skeleton
    m_progSkeleton.create(":/glsl/skeleton.vert.glsl", ":/glsl/skeleton.frag.glsl");


    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);

    emit sig_sendVertexData(&m_mesh);
    emit sig_sendFaceData(&m_mesh);
    emit sig_sendHalfEdgeData(&m_mesh);
}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_glCamera = Camera(w, h);
    glm::mat4 viewproj = m_glCamera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setViewProjMatrix(viewproj);
    m_progFlat.setViewProjMatrix(viewproj);
    m_progSkeleton.setViewProjMatrix(viewproj);

    printGLErrorLog();
}

//This function is called by Qt any time your GL window is supposed to update
//For example, when the function update() is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_progFlat.setViewProjMatrix(m_glCamera.getViewProj());
    m_progLambert.setViewProjMatrix(m_glCamera.getViewProj());
    m_progSkeleton.setViewProjMatrix(m_glCamera.getViewProj());

    m_progLambert.setCamPos(m_glCamera.eye);
    m_progFlat.setModelMatrix(glm::mat4(1.f));

    //Create a model matrix. This one rotates the square by PI/4 radians then translates it by <-2,0,0>.
    //Note that we have to transpose the model matrix before passing it to the shader
    //This is because OpenGL expects column-major matrices, but you've
    //implemented row-major matrices.
    //glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-2,0,0)) * glm::rotate(glm::mat4(), 0.25f * 3.14159f, glm::vec3(0,1,0));
    //Send the geometry's transformation matrix to the shader
    //m_progLambert.setModelMatrix(model);
    //Draw the example sphere using our lambert shader
    //m_progLambert.draw(m_geomSquare);

    glm::mat4 model = glm::mat4();
    m_progLambert.setModelMatrix(model);
    m_progSkeleton.setModelMatrix(model);

    m_progSkeleton.setOverallTransMat(m_skeleton.getTransMats());

    if (!m_mesh.vertices.at(0)->influencers.empty()) {
        m_progSkeleton.draw(m_mesh);
    } else {
        m_progLambert.draw(m_mesh);
    }

    // planarity test
//    for (const uPtr<Face>& f : m_mesh.faces) {
//        if (!m_mesh.testPlanarity(f.get())) {
//            m_mesh.triangulate(f.get());
//            update();
//            m_mesh.destroy();
//            m_mesh.create();

//            emit sig_sendFaceData(&m_mesh);
//            emit sig_sendHalfEdgeData(&m_mesh);
//        }
//    }

    //Now do the same to render the cylinder
    //We've rotated it -45 degrees on the Z axis, then translated it to the point <2,2,0>
//    model = glm::translate(glm::mat4(1.0f), glm::vec3(2,2,0)) * glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0,0,1));
//    m_progLambert.setModelMatrix(model);
//    m_progLambert.draw(m_geomSquare);

    glDisable(GL_DEPTH_TEST);

    if (mp_selectedComponent) {
        if (dynamic_cast<Vertex*>(mp_selectedComponent)) {
            m_vertDisplay.create();
            m_progFlat.draw(m_vertDisplay);
        } else if (dynamic_cast<Face*>(mp_selectedComponent)) {
            m_faceDisplay.create();
            m_progFlat.draw(m_faceDisplay);
        } else if (dynamic_cast<HalfEdge*>(mp_selectedComponent)) {
            m_halfEdgeDisplay.create();
            m_progFlat.draw(m_halfEdgeDisplay);
        }
    }

    if (!m_skeleton.joints.empty()) {
        m_skeleton.create();
        m_progFlat.draw(m_skeleton);
    }

    glEnable(GL_DEPTH_TEST);
}


void MyGL::keyPressEvent(QKeyEvent *e)
{
    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        m_glCamera.RotateAboutUp(-amount);
    } else if (e->key() == Qt::Key_Left) {
        m_glCamera.RotateAboutUp(amount);
    } else if (e->key() == Qt::Key_Up) {
        m_glCamera.RotateAboutRight(-amount);
    } else if (e->key() == Qt::Key_Down) {
        m_glCamera.RotateAboutRight(amount);
    } else if (e->key() == Qt::Key_1) {
        m_glCamera.fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        m_glCamera.fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
        m_glCamera.TranslateAlongLook(amount);
    } else if (e->key() == Qt::Key_S) {
        m_glCamera.TranslateAlongLook(-amount);
    } else if (e->key() == Qt::Key_D) {
        m_glCamera.TranslateAlongRight(amount);
    } else if (e->key() == Qt::Key_A) {
        m_glCamera.TranslateAlongRight(-amount);
    } else if (e->key() == Qt::Key_Q) {
        m_glCamera.TranslateAlongUp(-amount);
    } else if (e->key() == Qt::Key_E) {
        m_glCamera.TranslateAlongUp(amount);
    } else if (e->key() == Qt::Key_R) {
        m_glCamera = Camera(this->width(), this->height());

    } else if (e->key() == Qt::Key_N) {
        HalfEdge* edge = dynamic_cast<HalfEdge*>(mp_selectedComponent);
        if (edge) {
            slot_selectHalfEdge(edge->next);
        }
    } else if (e->key() == Qt::Key_M) {
        HalfEdge* edge = dynamic_cast<HalfEdge*>(mp_selectedComponent);
        if (edge) {
            slot_selectHalfEdge(edge->sym);
        }
    } else if (e->key() == Qt::Key_F) {
        HalfEdge* edge = dynamic_cast<HalfEdge*>(mp_selectedComponent);
        if (edge) {
            slot_selectFace(edge->face);
        }
    } else if (e->key() == Qt::Key_V) {
        HalfEdge* edge = dynamic_cast<HalfEdge*>(mp_selectedComponent);
        if (edge) {
            slot_selectVertex(edge->vertex);
        }
    } else if ((e->modifiers() & Qt::ShiftModifier) && (e->key() == Qt::Key_H)) {
        Face* f = dynamic_cast<Face*>(mp_selectedComponent);
        if (f) {
            slot_selectHalfEdge(f->edge);
        }
    } else if (e->key() == Qt::Key_H) {
        Vertex* v = dynamic_cast<Vertex*>(mp_selectedComponent);
        if (v) {
            slot_selectHalfEdge(v->edge);
        }
    }

    m_glCamera.RecomputeAttributes();
//    m_glCamera.recomputePolarAttributes();
    update();  // Calls paintGL, among other things
}

void MyGL::slot_selectVertex(QListWidgetItem *item) {
    mp_selectedComponent = item;
    m_vertDisplay.updateVertex(dynamic_cast<Vertex*>(item));
    update();
}

void MyGL::slot_selectFace(QListWidgetItem *item) {
    mp_selectedComponent = item;
    m_faceDisplay.updateFace(dynamic_cast<Face*>(item));
    update();
}

void MyGL::slot_selectHalfEdge(QListWidgetItem *item) {
    mp_selectedComponent = item;
    m_halfEdgeDisplay.updateHalfEdge(dynamic_cast<HalfEdge*>(item));
    update();
}

void MyGL::slot_changeVertexX(double x) {
    Vertex *v = dynamic_cast<Vertex*>(mp_selectedComponent);

    if (v) {
        v->pos.x = x;
        m_mesh.destroy();
        m_mesh.create();
        update();
    }
}

void MyGL::slot_changeVertexY(double y) {
    Vertex *v = dynamic_cast<Vertex*>(mp_selectedComponent);

    if (v) {
        v->pos.y = y;
        m_mesh.destroy();
        m_mesh.create();
        update();
    }
}

void MyGL::slot_changeVertexZ(double z) {
    Vertex *v = dynamic_cast<Vertex*>(mp_selectedComponent);

    if (v) {
        v->pos.z = z;
        m_mesh.destroy();
        m_mesh.create();
        update();
    }
}

void MyGL::slot_changeFaceRed(double r) {
    Face *f = dynamic_cast<Face*>(mp_selectedComponent);

    if (f) {
        f->color.r = r;
        m_mesh.destroy();
        m_mesh.create();
        update();
    }
}

void MyGL::slot_changeFaceGreen(double g) {
    Face *f = dynamic_cast<Face*>(mp_selectedComponent);

    if (f) {
        f->color.g = g;
        m_mesh.destroy();
        m_mesh.create();
        update();
    }
}

void MyGL::slot_changeFaceBlue(double b) {
    Face *f = dynamic_cast<Face*>(mp_selectedComponent);

    if (f) {
        f->color.b = b;
        m_mesh.destroy();
        m_mesh.create();
        update();
    }
}

void MyGL::slot_changeJointX(double x) {
    if (mp_selectedJoint) {
        Joint *j = dynamic_cast<Joint*>(mp_selectedJoint);

        if (j) {
            glm::vec4 newPos = glm::vec4(x, 0.f, 0.f, 1.f);
            newPos = j->setBindMatrix() * newPos;
            j->pos.x = j->pos.x + newPos.x;

            m_progSkeleton.setOverallTransMat(m_skeleton.getTransMats());

            m_skeleton.destroy();
            m_skeleton.create();
            update();
        }
    }
}

void MyGL::slot_changeJointY(double y) {
    Joint *j = dynamic_cast<Joint*>(mp_selectedJoint);

    if (j) {
        glm::vec4 newPos = glm::vec4(0.f, y, 0.f, 1.f);
        newPos = j->setBindMatrix() * newPos;
        j->pos.y = j->pos.y + newPos.y;

        m_progSkeleton.setOverallTransMat(m_skeleton.getTransMats());

        m_skeleton.destroy();
        m_skeleton.create();
        update();
    }
}

void MyGL::slot_changeJointZ(double z) {
    Joint *j = dynamic_cast<Joint*>(mp_selectedJoint);

    if (j) {
        glm::vec4 newPos = glm::vec4(0.f, 0.f, z, 1.f);
        newPos = j->setBindMatrix() * newPos;
        j->pos.z = j->pos.z + newPos.z;

        m_progSkeleton.setOverallTransMat(m_skeleton.getTransMats());

        m_skeleton.destroy();
        m_skeleton.create();
        update();
    }
}

void MyGL::slot_splitEdge() {
    HalfEdge *e = dynamic_cast<HalfEdge*>(mp_selectedComponent);

    if (e) {
        m_mesh.splitEdge(e);
        m_mesh.destroy();
        m_mesh.create();
        update();

        emit sig_sendVertexData(&m_mesh);
        emit sig_sendHalfEdgeData(&m_mesh);
    }
}

void MyGL::slot_triangulate() {
    Face *f = dynamic_cast<Face*>(mp_selectedComponent);

    if (f) {
        m_mesh.triangulate(f);
        m_mesh.destroy();
        m_mesh.create();
        update();

        emit sig_sendFaceData(&m_mesh);
        emit sig_sendHalfEdgeData(&m_mesh);
    }
}

void MyGL::slot_loadObj() {
    QString filename = QFileDialog::getOpenFileName(0, QString("Load OBJ"),
                                                    QDir::currentPath() + (QString("../..")),
                                                    QString("*.obj"));

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return;
    }

    m_mesh.faces.clear();
    m_mesh.vertices.clear();
    m_mesh.edges.clear();

    Face::facePrevId = 0;
    Vertex::vertexPrevId = 0;
    HalfEdge::edgePrevId = 0;

    // maps id of bounding vertices to a given half edge
    std::map<std::pair<int, int>, HalfEdge*> symMap;

    while (!file.atEnd()) {
        QString line = file.readLine();

        if (!line.startsWith("#")) {
            // vertex positions
            if (line.startsWith("v ")) {
                QStringList vPos = line.split(" ", Qt::SkipEmptyParts);

                m_mesh.vertices.push_back(mkU<Vertex>());
                m_mesh.vertices.back().get()->pos = glm::vec3(vPos.at(1).toFloat(), vPos.at(2).toFloat(), vPos.at(3).toFloat());
            }

            // faces
            if (line.startsWith("f ")) {
                QStringList faceDef = line.split(" ", Qt::SkipEmptyParts);

                m_mesh.faces.push_back(mkU<Face>());
                Face* currFace = m_mesh.faces.back().get();

                // vertices that face is composed of
                std::vector<Vertex*> faceV;
                // half edges that face is composed of
                std::vector<uPtr<HalfEdge>> faceHE;

                for (int i = 1; i < faceDef.size(); ++i) {
                    int idx = faceDef.at(i).split("/").at(0).toInt() - 1;
                    faceV.push_back(m_mesh.vertices.at(idx).get());
                    faceHE.push_back(mkU<HalfEdge>());
                }

                // link next edge pointers and vertex and face
                for (int i = 0; i < faceHE.size(); ++i) {
                    faceHE.at(i)->setVertex(faceV.at(i));

                    if (i == faceHE.size() - 1) {
                        faceHE.back()->next = faceHE.at(0).get();
                    } else {
                        faceHE.at(i)->next = faceHE.at(i + 1).get();
                    }

                    faceHE.at(i)->face = currFace;
                }

                // assign sym pointers
                for (int i = 0; i < faceHE.size(); ++i) {

                    // find bounding vertex for each edge
                    int v1 = faceHE.at(i).get()->vertex->id;
                    HalfEdge* curr = faceHE.at(i).get();

                    do {
                        curr = curr->next;
                    } while (curr->next != faceHE.at(i).get());

                    int v2 = curr->vertex->id;

                    std::pair<int, int> symPair = std::pair<int, int>(v2, v1);
                    auto searchEdge = symMap.find(symPair);

                    if (searchEdge != symMap.end()) {
                        faceHE.at(i).get()->setSym(searchEdge->second);
                    } else {
                        symMap.insert(std::pair<std::pair<int, int>, HalfEdge*>
                                      (std::pair<int, int>(v1, v2), faceHE.at(i).get()));
                    }
                }

                for (int i = 0; i < faceHE.size(); ++i) {
                    m_mesh.edges.push_back(std::move(faceHE.at(i)));
                }

                currFace->edge = m_mesh.edges.back().get();
            }
        }
    }

    file.close();

    m_mesh.destroy();
    m_mesh.create();
    mp_selectedComponent = nullptr;
    update();

    emit sig_sendVertexData(&m_mesh);
    emit sig_sendFaceData(&m_mesh);
    emit sig_sendHalfEdgeData(&m_mesh);
}

void MyGL::slot_loadJson() {

    QString filename = QFileDialog::getOpenFileName(0, QString("Load JSON"),
                                                    QDir::currentPath() + (QString("../..")),
                                                    QString("*.json"));

    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return;
    }

    QByteArray data = file.readAll();

    QJsonDocument json = QJsonDocument::fromJson(data);

    QJsonObject jsonObj = json.object();

    QJsonObject root = jsonObj["root"].toObject();

    mp_selectedJoint = nullptr;
    m_skeleton.root = nullptr;
    m_skeleton.representedJoint = nullptr;
    for (uPtr<Joint>& j : m_skeleton.joints) {
        j.reset();
    }
    m_skeleton.joints.clear();

    Joint* rootJoint = parseJoint(root, nullptr);

    file.close();

    m_skeleton.root = rootJoint;
    m_skeleton.destroy();
    m_skeleton.create();
    update();

    emit sig_sendJointData(&m_skeleton);
}

Joint* MyGL::parseJoint(QJsonObject o, Joint* parent) {

    glm::vec3 pos = glm::vec3(o.value("pos").toArray().takeAt(0).toDouble(),
                              o.value("pos").toArray().takeAt(1).toDouble(),
                              o.value("pos").toArray().takeAt(2).toDouble());

    glm::quat rot = computeQuaternion(o.value("rot").toArray().takeAt(0).toDouble(),
                                      o.value("rot").toArray().takeAt(1).toDouble(),
                                      o.value("rot").toArray().takeAt(2).toDouble(),
                                      o.value("rot").toArray().takeAt(3).toDouble());

    uPtr<Joint> j = mkU<Joint>(o.value("name").toString(), parent, pos, rot);


    for (QJsonValue v : o.value("children").toArray()) {
        Joint* child = parseJoint(v.toObject(), j.get());
        j.get()->addChild(child);
    }

    m_skeleton.joints.push_back(std::move(j));

    return m_skeleton.joints.back().get();
}

glm::quat MyGL::computeQuaternion(float angle, float axisX, float axisY, float axisZ) {
    float w = cos(glm::radians(angle) / 2);
    float x = sin(glm::radians(angle) / 2) * axisX;
    float y = sin(glm::radians(angle) / 2) * axisY;
    float z = sin(glm::radians(angle) / 2) * axisZ;

    return glm::quat(w, x, y, z);
}

void MyGL::slot_skinning() {
    if (!m_skeleton.joints.empty()) {
        int numInfluencers = 2;

        for (uPtr<Vertex>& v : m_mesh.vertices) {
            Joint* j1 = nullptr;
            Joint* j2 = nullptr;
            float j1Dist = std::numeric_limits<float>::infinity();
            float j2Dist = std::numeric_limits<float>::infinity();
            int j1ID = 0;
            int j2ID = 0;

            if (m_skeleton.joints.size() <= numInfluencers) {
                j1 = m_skeleton.joints.at(0).get();
                j2 = m_skeleton.joints.at(1).get();
                j1Dist = glm::distance(v->pos, glm::vec3(m_skeleton.joints.at(0).get()
                                                         ->getOverallTransformation()[3]));
                j2Dist = glm::distance(v->pos, glm::vec3(m_skeleton.joints.at(1).get()
                                                         ->getOverallTransformation()[3]));
                j2ID = 1;
            } else {
                for (int i = 0; i < m_skeleton.joints.size(); ++i) {
                    Joint* j = m_skeleton.joints.at(i).get();

                    float dist = glm::distance(v->pos, glm::vec3(j->getOverallTransformation()[3]));

                    if (dist < j1Dist) {
                        // j2 is now the second smallest value
                        if (j1) {
                            j2 = j1;
                            j2Dist = j1Dist;
                            j2ID = j1ID;
                        }

                        j1 = j;
                        j1Dist = dist;
                        j1ID = i;
                    } else if (dist < j2Dist) {
                        j2 = j;
                        j2Dist = dist;
                        j2ID = i;
                    }
                }
            }

            float j1Weight = 1 - (j1Dist / (j1Dist + j2Dist));
            float j2Weight = 1 - (j2Dist / (j1Dist + j2Dist));

            v.get()->influencers.push_back(std::pair(j1ID, j1Weight));
            v.get()->influencers.push_back(std::pair(j2ID, j2Weight));
        }

        m_progSkeleton.setBindMat(m_skeleton.getBindMats());

        m_mesh.destroy();
        m_mesh.create();

        update();
    }
}
