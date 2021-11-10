#pragma once
#include "smartpointerhelp.h"
#include "halfedge.h"
#include "drawable.h"
#include <vector>

class Mesh : public Drawable
{
private:
    std::vector<uPtr<Face>> faces;
    std::vector<uPtr<Vertex>> vertices;
    std::vector<uPtr<HalfEdge>> edges;

    int countEdges(Face* f);
    void computeCentroid(Face* f,
                         std::map<Face*, Vertex*> *centroids,
                         std::vector<uPtr<Vertex>> *newVertices);
    void computeMidpoint(HalfEdge* e, std::map<Face*, Vertex*> &centroids,
                         std::map<HalfEdge*, Vertex*> *midpoints,
                         std::vector<uPtr<Vertex>> *newVertices,
                         std::vector<uPtr<HalfEdge>> *newHalfEdges,
                         std::vector<HalfEdge*> *sharpEdges);
    void computeSmoothedVertex(Vertex* v, std::map<Face*, Vertex*> &centroids,
                               std::map<HalfEdge*, Vertex*> &midpoints);
    void splitFace(Vertex* centroid,
                   std::vector<uPtr<Face>> *newFaces,
                   std::vector<uPtr<HalfEdge>> *newHalfEdges,
                   std::vector<std::pair<HalfEdge*, HalfEdge*>> &originalEdges);

    void assignSym(std::vector<std::pair<HalfEdge*, HalfEdge*>> *originalEdges);

    friend class MainWindow;
    friend class MyGL;
public:
    Mesh(OpenGLContext *mp_context);
    Mesh(OpenGLContext *mp_context,
         std::vector<uPtr<Face>> faces,
         std::vector<uPtr<Vertex>> vertices,
         std::vector<uPtr<HalfEdge>> edges);
    virtual ~Mesh();

    void create() override;
    GLenum drawMode() override;
    void buildCube();
    void splitEdge(HalfEdge *he);
    void triangulate(Face *f);
    bool testPlanarity(Face* f);
    void subdivide(std::vector<Vertex*> *sharpVertices,
                   std::vector<Face*> &sharpFaces,
                   std::vector<HalfEdge*> *sharpEdges);
};

