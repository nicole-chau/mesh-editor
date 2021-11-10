#include "mainwindow.h"
#include <ui_mainwindow.h>
#include "cameracontrolshelp.h"
#include <scene/mesh.h>
#include "mygl.h"

#include <QFileDialog>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mygl->setFocus();

    /** Display mesh components **/

    connect(ui->mygl, SIGNAL(sig_sendVertexData(Mesh*)),
            this, SLOT(slot_addVertex(Mesh*)));

    connect(ui->mygl, SIGNAL(sig_sendFaceData(Mesh*)),
            this, SLOT(slot_addFace(Mesh*)));

    connect(ui->mygl, SIGNAL(sig_sendHalfEdgeData(Mesh*)),
            this, SLOT(slot_addHalfEdge(Mesh*)));

    connect(ui->mygl, SIGNAL(sig_sendJointData(Skeleton*)),
            this, SLOT(slot_addJointToTreeWidget(Skeleton*)));

    /** Select mesh component **/

    connect(ui->vertsListWidget, SIGNAL(itemPressed(QListWidgetItem*)),
            ui->mygl, SLOT(slot_selectVertex(QListWidgetItem*)));

    connect(ui->facesListWidget, SIGNAL(itemPressed(QListWidgetItem*)),
            ui->mygl, SLOT(slot_selectFace(QListWidgetItem*)));

    connect(ui->halfEdgesListWidget, SIGNAL(itemPressed(QListWidgetItem*)),
            ui->mygl, SLOT(slot_selectHalfEdge(QListWidgetItem*)));

    connect(ui->jointTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
            this, SLOT(slot_selectJoint(QTreeWidgetItem*, int)));

    /** Display mesh component data when selected **/

    connect(ui->vertsListWidget, SIGNAL(itemPressed(QListWidgetItem*)),
                this, SLOT(slot_setPosValue(QListWidgetItem*)));

    connect(ui->facesListWidget, SIGNAL(itemPressed(QListWidgetItem*)),
                this, SLOT(slot_setColorValue(QListWidgetItem*)));

    /** Mesh component editing functions **/

    connect(ui->splitEdgeButton, SIGNAL(clicked(bool)),
            ui->mygl, SLOT(slot_splitEdge()));

    connect(ui->triangulateButton, SIGNAL(clicked(bool)),
            ui->mygl, SLOT(slot_triangulate()));

    connect(ui->vertPosXSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_changeVertexX(double)));

    connect(ui->vertPosYSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_changeVertexY(double)));

    connect(ui->vertPosZSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_changeVertexZ(double)));

    connect(ui->faceRedSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_changeFaceRed(double)));

    connect(ui->faceGreenSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_changeFaceGreen(double)));

    connect(ui->faceBlueSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_changeFaceBlue(double)));

    connect(ui->jointPosXSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_changeJointX(double)));

    connect(ui->jointPosYSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_changeJointY(double)));

    connect(ui->jointPosZSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_changeJointZ(double)));

    connect(ui->jointRotXPos, SIGNAL(clicked(bool)),
            this, SLOT(slot_rotateJointXPos()));

    connect(ui->jointRotXNeg, SIGNAL(clicked(bool)),
            this, SLOT(slot_rotateJointXNeg()));

    connect(ui->jointRotYPos, SIGNAL(clicked(bool)),
            this, SLOT(slot_rotateJointYPos()));

    connect(ui->jointRotYNeg, SIGNAL(clicked(bool)),
            this, SLOT(slot_rotateJointYNeg()));

    connect(ui->jointRotZPos, SIGNAL(clicked(bool)),
            this, SLOT(slot_rotateJointZPos()));

    connect(ui->jointRotZNeg, SIGNAL(clicked(bool)),
            this, SLOT(slot_rotateJointZNeg()));

    /** Mesh editing functions **/

    connect(ui->subdivideButton, SIGNAL(clicked(bool)),
            this, SLOT(slot_subdivide()));

    connect(ui->skinningButton, SIGNAL(clicked(bool)),
            ui->mygl, SLOT(slot_skinning()));

    /** Load files **/

    connect(ui->loadObjButton, SIGNAL(clicked(bool)),
            ui->mygl, SLOT(slot_loadObj()));

    connect(ui->loadJsonButton, SIGNAL(clicked(bool)),
            ui->mygl, SLOT(slot_loadJson()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_actionCamera_Controls_triggered()
{
    CameraControlsHelp* c = new CameraControlsHelp();
    c->show();
}

void MainWindow::slot_addJointToTreeWidget(Skeleton *s) {
    ui->jointTreeWidget->addTopLevelItem(s->root);
}

void MainWindow::slot_selectJoint(QTreeWidgetItem *item, int column) {
    Joint* j = dynamic_cast<Joint*>(item);
    ui->mygl->mp_selectedJoint = j;
    ui->mygl->m_skeleton.updateJoint(j);
    ui->mygl->update();

    ui->jointPosXSpinBox->setValue(j->getOverallTransformation()[3].x);
    ui->jointPosYSpinBox->setValue(j->getOverallTransformation()[3].y);
    ui->jointPosZSpinBox->setValue(j->getOverallTransformation()[3].z);

    ui->jointRotX->setNum(glm::degrees(glm::eulerAngles(j->rotation).x));
    ui->jointRotY->setNum(glm::degrees(glm::eulerAngles(j->rotation).y));
    ui->jointRotZ->setNum(glm::degrees(glm::eulerAngles(j->rotation).z));
}

void MainWindow::slot_setPosValue(QListWidgetItem* item) {
    if (item && dynamic_cast<Vertex*>(item)) {
        Vertex* v = dynamic_cast<Vertex*>(item);
        ui->vertPosXSpinBox->setValue(v->pos.x);
        ui->vertPosYSpinBox->setValue(v->pos.y);
        ui->vertPosZSpinBox->setValue(v->pos.z);
    }
}

void MainWindow::slot_setColorValue(QListWidgetItem* item) {
    if (item && dynamic_cast<Face*>(item)) {
        const Face* f = dynamic_cast<Face*>(item);
        ui->faceRedSpinBox->setValue(f->color.r);
        ui->faceGreenSpinBox->setValue(f->color.g);
        ui->faceBlueSpinBox->setValue(f->color.b);
    }
}

void MainWindow::slot_rotateJointXPos() {
    Joint *j = dynamic_cast<Joint*>(ui->mygl->mp_selectedJoint);

    if (j) {
        glm::quat newRot = ui->mygl->computeQuaternion(5.f, 1.f, 0.f, 0.f);
        j->rotation = newRot * j->rotation;

        ui->mygl->m_skeleton.destroy();
        ui->mygl->m_skeleton.create();
        ui->mygl->update();

        ui->jointRotX->setNum(glm::degrees(glm::eulerAngles(j->rotation).x));
    }
}

void MainWindow::slot_rotateJointXNeg() {
    Joint *j = dynamic_cast<Joint*>(ui->mygl->mp_selectedJoint);

    if (j) {
        glm::quat newRot = ui->mygl->computeQuaternion(-5.f, 1.f, 0.f, 0.f);
        j->rotation = newRot * j->rotation;

        ui->mygl->m_skeleton.destroy();
        ui->mygl->m_skeleton.create();
        ui->mygl->update();

        ui->jointRotX->setNum(glm::degrees(glm::eulerAngles(j->rotation).x));
    }
}

void MainWindow::slot_rotateJointYPos() {
    Joint *j = dynamic_cast<Joint*>(ui->mygl->mp_selectedJoint);

    if (j) {
        glm::quat newRot = ui->mygl->computeQuaternion(5.f, 0.f, 1.f, 0.f);
        j->rotation = newRot * j->rotation;

        ui->mygl->m_skeleton.destroy();
        ui->mygl->m_skeleton.create();
        ui->mygl->update();

        ui->jointRotY->setNum(glm::degrees(glm::eulerAngles(j->rotation).y));
    }
}

void MainWindow::slot_rotateJointYNeg() {
    Joint *j = dynamic_cast<Joint*>(ui->mygl->mp_selectedJoint);

    if (j) {
        glm::quat newRot = ui->mygl->computeQuaternion(-5.f, 0.f, 1.f, 0.f);
        j->rotation = newRot * j->rotation;

        ui->mygl->m_skeleton.destroy();
        ui->mygl->m_skeleton.create();
        ui->mygl->update();

        ui->jointRotY->setNum(glm::degrees(glm::eulerAngles(j->rotation).y));
    }
}

void MainWindow::slot_rotateJointZPos() {
    Joint *j = dynamic_cast<Joint*>(ui->mygl->mp_selectedJoint);

    if (j) {
        glm::quat newRot = ui->mygl->computeQuaternion(5.f, 0.f, 0.f, 1.f);
        j->rotation = newRot * j->rotation;

        ui->mygl->m_skeleton.destroy();
        ui->mygl->m_skeleton.create();
        ui->mygl->update();

        ui->jointRotZ->setNum(glm::degrees(glm::eulerAngles(j->rotation).z));
    }
}

void MainWindow::slot_rotateJointZNeg() {
    Joint *j = dynamic_cast<Joint*>(ui->mygl->mp_selectedJoint);

    if (j) {
        glm::quat newRot = ui->mygl->computeQuaternion(-5.f, 0.f, 0.f, 1.f);
        j->rotation = newRot * j->rotation;

        ui->mygl->m_skeleton.destroy();
        ui->mygl->m_skeleton.create();
        ui->mygl->update();

        ui->jointRotZ->setNum(glm::degrees(glm::eulerAngles(j->rotation).z));
    }
}

void MainWindow::slot_addVertex(Mesh* m) {
    for (const uPtr<Vertex> &v : m->vertices) {
        v.get()->setFlags(v.get()->flags() | Qt::ItemIsUserCheckable);
        v.get()->setCheckState(Qt::Unchecked);
        ui->vertsListWidget->addItem(v.get());
    }
}

void MainWindow::slot_addFace(Mesh* m) {
    for (const uPtr<Face> &f : m->faces) {
        f.get()->setFlags(f.get()->flags() | Qt::ItemIsUserCheckable);
        f.get()->setCheckState(Qt::Unchecked);
        ui->facesListWidget->addItem(f.get());
    }
}

void MainWindow::slot_addHalfEdge(Mesh* m) {
    for (const uPtr<HalfEdge> &he : m->edges) {
        he.get()->setFlags(he.get()->flags() | Qt::ItemIsUserCheckable);
        he.get()->setCheckState(Qt::Unchecked);
        ui->halfEdgesListWidget->addItem(he.get());
    }
}

void MainWindow::slot_subdivide() {
    std::vector<Vertex*> sharpVertices;
    std::vector<Face*> sharpFaces;
    std::vector<HalfEdge*> sharpEdges;

    for (int i = 0; i < ui->vertsListWidget->count(); ++i) {
        if (ui->vertsListWidget->item(i)->checkState() == Qt::Checked) {
            sharpVertices.push_back(dynamic_cast<Vertex*>(ui->vertsListWidget->item(i)));
        }
    }

    for (int i = 0; i < ui->facesListWidget->count(); ++i) {
        if (ui->facesListWidget->item(i)->checkState() == Qt::Checked) {
            sharpFaces.push_back(dynamic_cast<Face*>(ui->facesListWidget->item(i)));
        }
    }

    for (int i = 0; i < ui->halfEdgesListWidget->count(); ++i) {
        if (ui->halfEdgesListWidget->item(i)->checkState() == Qt::Checked) {
            HalfEdge* e = dynamic_cast<HalfEdge*>(ui->halfEdgesListWidget->item(i));
            sharpEdges.push_back(e);
            if (std::find(sharpEdges.begin(), sharpEdges.end(), e->sym) == sharpEdges.end()) {
                sharpEdges.push_back(e->sym);
            }
            e->sym->setCheckState(Qt::Checked);
        }
    }

    bool isBound = !ui->mygl->m_mesh.vertices.at(0)->influencers.empty();

    ui->mygl->m_mesh.subdivide(&sharpVertices, sharpFaces, &sharpEdges);
    ui->mygl->m_mesh.destroy();
    ui->mygl->m_mesh.create();
    ui->mygl->mp_selectedComponent = nullptr;
    ui->mygl->update();

    emit ui->mygl->sig_sendVertexData(&ui->mygl->m_mesh);
    emit ui->mygl->sig_sendFaceData(&ui->mygl->m_mesh);
    emit ui->mygl->sig_sendHalfEdgeData(&ui->mygl->m_mesh);

    // recalculate mesh bind if mesh was previously bound to skeleton
    if (isBound) {
        ui->mygl->slot_skinning();
    }
}
