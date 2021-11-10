#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <scene/mesh.h>
#include <scene/skeleton.h>


namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionQuit_triggered();

    void on_actionCamera_Controls_triggered();

    void slot_addJointToTreeWidget(Skeleton* s);
    void slot_selectJoint(QTreeWidgetItem* item, int);

    void slot_setPosValue(QListWidgetItem* item);
    void slot_setColorValue(QListWidgetItem* item);

    void slot_rotateJointXPos();
    void slot_rotateJointXNeg();
    void slot_rotateJointYPos();
    void slot_rotateJointYNeg();
    void slot_rotateJointZPos();
    void slot_rotateJointZNeg();

    void slot_addVertex(Mesh*);
    void slot_addFace(Mesh*);
    void slot_addHalfEdge(Mesh*);

    void slot_subdivide();

private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
