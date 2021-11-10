#include "mesh.h"
#include "mygl.h"

#include <map>

Mesh::Mesh(OpenGLContext *mp_context)
    : Drawable(mp_context), faces{}, vertices{}, edges{}
{}

Mesh::Mesh(OpenGLContext *mp_context,
           std::vector<uPtr<Face>> faces,
           std::vector<uPtr<Vertex>> vertices,
           std::vector<uPtr<HalfEdge>> edges)
    : Drawable(mp_context),
      faces(std::move(faces)),
      vertices(std::move(vertices)),
      edges(std::move(edges))
{}

Mesh::~Mesh() {
    destroy();
}

int Mesh::countEdges(Face* f) {
    HalfEdge *e = f->edge;

    int count = 0;
    do {
        ++count;
        e = e->next;
    } while (e != f->edge);

    return count;
}

void Mesh::create() {
    std::vector<GLuint> idx;
    std::vector<glm::vec4> pos;
    std::vector<glm::vec4> nor;
    std::vector<glm::vec4> color;
    std::vector<glm::vec2> weight;
    std::vector<glm::ivec2> jointID;

    // get pos, nor and color
    for (const uPtr<Face>& f : faces) {
         HalfEdge *e = f->edge;

         do {
            pos.push_back(glm::vec4(e->vertex->pos, 1));
            color.push_back(glm::vec4(e->face->color, 1));
            if (!e->vertex->influencers.empty()) {
                weight.push_back(glm::vec2(e->vertex->influencers.at(0).second,
                                           e->vertex->influencers.at(1).second));
                jointID.push_back(glm::ivec2(e->vertex->influencers.at(0).first,
                                            e->vertex->influencers.at(1).first));
            }

            // calculate normal
            glm::vec3 a = e->vertex->pos;
            glm::vec3 b = e->next->vertex->pos;
            glm::vec3 c = e->next->next->vertex->pos;
            glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
            nor.push_back(glm::vec4(normal, 1));

            e = e->next;
         } while (e != f->edge);
    }

    // get indices
    int numVertsSoFar = 0;
    for (uPtr<Face>& f : faces) {
        int count = countEdges(f.get());

        for (int i = 0; i < count - 2; ++i) {
            idx.push_back(numVertsSoFar);
            idx.push_back(i + 1 + numVertsSoFar);
            idx.push_back(i + 2 + numVertsSoFar);
        }
        numVertsSoFar += count;
    }

    count = idx.size();

    generateIdx();
    bindIdx();
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    bindPos();
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

    generateNor();
    bindNor();
    mp_context->glBufferData(GL_ARRAY_BUFFER, nor.size() * sizeof(glm::vec4), nor.data(), GL_STATIC_DRAW);

    generateCol();
    bindCol();
    mp_context->glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(glm::vec4), color.data(), GL_STATIC_DRAW);

    if (!weight.empty()) {
        generateWeight();
        bindWeight();
        mp_context->glBufferData(GL_ARRAY_BUFFER, weight.size() * sizeof(glm::vec2), weight.data(), GL_STATIC_DRAW);
    }

    if (!jointID.empty()) {
        generateJointID();
        bindJointID();
        mp_context->glBufferData(GL_ARRAY_BUFFER, jointID.size() * sizeof(glm::ivec2), jointID.data(), GL_STATIC_DRAW);
    }
}

GLenum Mesh::drawMode() {
    return GL_TRIANGLES;
}

void Mesh::buildCube() {
    int numHE = 24;
    int numVert = 8;
    int numFace = 6;

    for (int i = 0; i < numHE; ++i) {
        edges.push_back(mkU<HalfEdge>());
    }

    for (int i = 0; i < numVert; ++i) {
        vertices.push_back(mkU<Vertex>());
    }

    for (int i = 0; i < numFace; ++i) {
        faces.push_back(mkU<Face>());
    }

    /** VERTICES **/
    // front vertices: upper left clockwise
    vertices[0]->pos = glm::vec3(-0.5, 0.5, 0.5);
    vertices[1]->pos = glm::vec3(-0.5, -0.5, 0.5);
    vertices[2]->pos = glm::vec3(0.5, -0.5, 0.5);
    vertices[3]->pos = glm::vec3(0.5, 0.5, 0.5);

    // back vertices
    vertices[4]->pos = glm::vec3(0.5, 0.5, -0.5);
    vertices[5]->pos = glm::vec3(0.5, -0.5, -0.5);
    vertices[6]->pos = glm::vec3(-0.5, -0.5, -0.5);
    vertices[7]->pos = glm::vec3(-0.5, 0.5, -0.5);

    // assign vertex edge pointers
    for (int i = 0; i < 8; ++i) {
        vertices[i]->edge = edges[i].get();
    }

    /** EDGES **/
    // top, left, bottom, right
    // assign next pointers
    for (int i = 0; i < 4; ++i) {
        edges[i]->next = edges[i + 1].get();
        edges[i + 4]->next = edges[i + 5].get();
        edges[i + 8]->next = edges[i + 9].get();
        edges[i + 12]->next = edges[i + 13].get();
        edges[i + 16]->next = edges[i + 17].get();
        edges[i + 20]->next = edges[i + 21].get();
    }

    edges[3]->next = edges[0].get();
    edges[7]->next = edges[4].get();
    edges[11]->next = edges[8].get();
    edges[15]->next = edges[12].get();
    edges[19]->next = edges[16].get();
    edges[23]->next = edges[20].get();

    // assign sym pointers
    edges[0]->setSym(edges[18].get());
    edges[1]->setSym(edges[11].get());
    edges[2]->setSym(edges[20].get());
    edges[3]->setSym(edges[13].get());
    edges[4]->setSym(edges[16].get());
    edges[5]->setSym(edges[15].get());
    edges[6]->setSym(edges[22].get());
    edges[7]->setSym(edges[9].get());
    edges[8]->setSym(edges[17].get());
    edges[10]->setSym(edges[21].get());
    edges[12]->setSym(edges[19].get());
    edges[14]->setSym(edges[23].get());

    // assign vertex pointers
    // front and back half edges
    for (int i = 0; i < 4; ++i) {
//        edges[i]->setVertex(vertices[i].get());
//        edges[i + 4]->setVertex(vertices[i + 4].get());
        edges[i]->vertex = vertices[i].get();
        edges[i + 4]->vertex = vertices[i + 4].get();
    }

    // left half edges
    edges[8]->vertex = vertices[7].get();
    edges[9]->vertex = vertices[6].get();
    edges[10]->vertex = vertices[1].get();
    edges[11]->vertex = vertices[0].get();

    // right half edges
    edges[12]->vertex = vertices[3].get();
    edges[13]->vertex = vertices[2].get();
    edges[14]->vertex = vertices[5].get();
    edges[15]->vertex = vertices[4].get();

    // top half edges
    edges[16]->vertex = vertices[7].get();
    edges[17]->vertex = vertices[0].get();
    edges[18]->vertex = vertices[3].get();
    edges[19]->vertex = vertices[4].get();

    // bottom half edges
    edges[20]->vertex = vertices[1].get();
    edges[21]->vertex = vertices[6].get();
    edges[22]->vertex = vertices[5].get();
    edges[23]->vertex = vertices[2].get();

    // assign face pointers
    int edgeNum = 0;
    for (int f = 0; f < numFace; ++f) {
        for (int e = 0; e < 4; ++e) {
            edges[edgeNum]->face = faces[f].get();
            edgeNum++;
        }
    }

    /** FACES **/
    // front, back, left, right, top, bottom
    for (int i = 0; i < numFace; ++i) {
        faces[i]->edge = edges[4 * i].get();
    }

    faces[0]->color = glm::vec3(0, 1, 0);
    faces[1]->color = glm::vec3(1, 0, 0);
    faces[2]->color = glm::vec3(0, 0, 1);
    faces[3]->color = glm::vec3(1, 1, 0);
    faces[4]->color = glm::vec3(0, 1, 1);
    faces[5]->color = glm::vec3(1, 0, 1);
}

void Mesh::splitEdge(HalfEdge *he1) {
    HalfEdge *he2 = he1->sym;
    Vertex* v1 = he1->vertex;
    Vertex* v2 = he2->vertex;

    uPtr<Vertex> v3 = mkU<Vertex>();

    v3->pos = (v1->pos + v2->pos) * 0.5f;
    uPtr<HalfEdge> he1b = mkU<HalfEdge>();
    uPtr<HalfEdge> he2b = mkU<HalfEdge>();

    he1b->vertex = v1;
    he1b->face = he1->face;
    he2b->vertex = v2;
    he2b->face = he2->face;

    v3->edge = he1;

    he1b->next = he1->next;
    he2b->next = he2->next;
    he1->next = he1b.get();
    he2->next = he2b.get();
    he1->vertex = v3.get();
    he2->vertex = v3.get();

    he2b->setSym(he1);
    he1b->setSym(he2);

    vertices.push_back(std::move(v3));
    edges.push_back(std::move(he1b));
    edges.push_back(std::move(he2b));

    Drawable::destroy();
    create();
}

void Mesh::triangulate(Face *f) {
    int numEdges = countEdges(f);

    if (numEdges > 3) {
        HalfEdge* he0 = f->edge;

        uPtr<HalfEdge> heA = mkU<HalfEdge>();
        uPtr<HalfEdge> heB = mkU<HalfEdge>();

        heA->vertex = he0->vertex;

        // find edge that points to the vertex that bounds heA
        // heB points to the vertex that bounds heA
        HalfEdge* e = he0;

        do {
            e = e->next;
        } while (e->next->next->vertex != heA->vertex);

        heB->vertex = e->vertex;

//        heA->sym = heB.get();
//        heB->sym = heA.get();
        heA->setSym(heB.get());

        uPtr<Face> f2 = mkU<Face>();
        heA->face = f2.get();

        // assign half edges to new face f2
        HalfEdge* curr = he0->next;
        do {
            curr->face = f2.get();
            curr = curr ->next;
        } while (curr != e);

        heB->face = f;
        f2->edge = heA.get();

        heB->next = e->next;
        e->next = heA.get();
        heA->next = he0->next;
        he0->next = heB.get();

        f2->color = f->color;

        triangulate(f2.get());

        faces.push_back(std::move(f2));
        edges.push_back(std::move(heA));
        edges.push_back(std::move(heB));
    }
}

bool Mesh::testPlanarity(Face* f) {
    int numEdges = countEdges(f);
    if (numEdges == 3) {
        return true;
    } else {

        // get positions of vertices of face
        std::vector<glm::vec3> vertices;

        HalfEdge *curr = f->edge;

        do {
            vertices.push_back(curr->vertex->pos);
            curr = curr->next;
        } while (curr != f->edge);

        // get equation of plane that 3 points lie on
        glm::vec3 p1 = vertices[0];
        glm::vec3 p2 = vertices[1];
        glm::vec3 p3 = vertices[2];

        glm::vec3 p12 = p2 - p1;
        glm::vec3 p13 = p3 - p1;

        glm::vec3 nor = glm::cross(p12, p13);

        // check if a point lies on the plane
        for (int i = 2; i < vertices.size(); ++i) {
            glm::vec3 p = vertices[i];

            float result = nor.x * (p.x - p1.x)
                           + nor.y * (p.y - p1.y)
                           + nor.z * (p.z - p1.z);

            if (fabs(result) > 0.001) {
                return false;
            }
        }

        return true;
    }
}

void Mesh::subdivide(std::vector<Vertex*> *sharpVertices,
                     std::vector<Face*> &sharpFaces,
                     std::vector<HalfEdge*> *sharpEdges) {
    std::vector<uPtr<Vertex>> newVertices;
    std::vector<uPtr<HalfEdge>> newHalfEdges;
    std::vector<uPtr<Face>> newFaces;

    // sharp face treats all vertices and edges as sharp
    for (Face* f : sharpFaces) {
        HalfEdge* e = f->edge;
        do {
            sharpVertices->push_back(e->vertex);
            sharpEdges->push_back(e);
            sharpEdges->push_back(e->sym);
            e = e->next;
        } while (e != f->edge);
    }

    // for a given centroid c, maps c->edge->face to c
    std::map<Face*, Vertex*> centroids;
    // for a given midpoint mp, maps mp->edge->next to mp
    std::map<HalfEdge*, Vertex*> midpoints;

    // compute centroid for each face
    for (const uPtr<Face>& f : faces) {
        computeCentroid(f.get(), &centroids, &newVertices);
    }

    // compute smoothed midpoint of each edge
    for (const uPtr<HalfEdge>& e : edges) {
        computeMidpoint(e.get(), centroids, &midpoints, &newVertices, &newHalfEdges, sharpEdges);
    }

    // smooth vertices
    for (const uPtr<Vertex>& v : vertices) {
        // find number of sharp edges it is adjacent to
        HalfEdge* e = v->edge;
        int numSharpEdges = 0;
        std::vector<Vertex*> adjacentVertices;
        do {
            if (std::find(sharpEdges->begin(), sharpEdges->end(), e) != sharpEdges->end()) {
                numSharpEdges++;
                adjacentVertices.push_back(e->sym->next->vertex);
            }
            e = e->next->sym;
        } while (e != v->edge);

        if (std::find(sharpVertices->begin(), sharpVertices->end(), v.get()) == sharpVertices->end()
            && numSharpEdges == 2) {
            v.get()->pos = 0.75f * v.get()->pos
                           + 0.125f * adjacentVertices.at(0)->pos
                           + 0.125f * adjacentVertices.at(1)->pos;
        } else if (std::find(sharpVertices->begin(), sharpVertices->end(), v.get()) == sharpVertices->end()
                    && numSharpEdges < 2) {
            computeSmoothedVertex(v.get(), centroids, midpoints);
        }
    }

    // split each original face into N quadrangle faces
    for (const uPtr<Face>& f: faces) {
        // temporarily stores a pair containing an edge e1 and e2 = e1->next
        // such that e1->next points to a midpoint (e1 and e2 are edges on original face)
        std::vector<std::pair<HalfEdge*, HalfEdge*>> originalEdges;
        HalfEdge* curr = f->edge;

        do {
            if (midpoints.find(curr->next) == midpoints.end()) {
                originalEdges.push_back(std::pair<HalfEdge*, HalfEdge*>(curr, curr->next));
            }
            curr = curr->next;
        } while (curr != f->edge);

        splitFace(centroids.find(f.get())->second,
                  &newFaces, &newHalfEdges, originalEdges);

        assignSym(&originalEdges);
    }

    for (uPtr<Vertex>& v : newVertices) {
        vertices.push_back(std::move(v));
    }

    for (uPtr<HalfEdge>& e : newHalfEdges) {
        edges.push_back(std::move(e));
    }

    faces.clear();
    for (uPtr<Face>& f : newFaces) {
        faces.push_back(std::move(f));
    }
}

void Mesh::computeCentroid(Face* f,
                           std::map<Face*, Vertex*> *centroids,
                           std::vector<uPtr<Vertex>> *newVertices) {
    uPtr<Vertex> centroid = mkU<Vertex>();

    HalfEdge *e = f->edge;
    glm::vec3 avgPos;
    int numVertices = 0;

    do {
        avgPos += e->vertex->pos;
        numVertices++;
        e = e->next;
    } while (e != f->edge);

    avgPos /= numVertices;

    centroid->pos = avgPos;

    centroids->insert(std::pair<Face*, Vertex*>(f, centroid.get()));
    newVertices->push_back(std::move(centroid));
}

void Mesh::computeMidpoint(HalfEdge* e, std::map<Face*, Vertex*> &centroids,
                           std::map<HalfEdge*, Vertex*> *midpoints,
                           std::vector<uPtr<Vertex>> *newVertices,
                           std::vector<uPtr<HalfEdge>> *newHalfEdges,
                           std::vector<HalfEdge*> *sharpEdges) {
    // if midpoint has not already been computed for the corresponding edge
    if (midpoints->find(e->next) == midpoints->end()) {
        // find bounding vertices v1 and v2
        Vertex* v1 = e->vertex;

        HalfEdge* curr = e;
        do {
            curr = curr->next;
        } while (curr->next->vertex != v1);

        Vertex* v2 = curr->vertex;

        Face* f1 = e->face;
        Face* f2 = e->sym->face;

        // edge is incident to 2 faces
        uPtr<Vertex> midpoint = mkU<Vertex>();
        if (f2) {
            midpoint.get()->pos = (v1->pos + v2->pos
                            + centroids.at(f1)->pos + centroids.at(f2)->pos) / 4.f;

        // edge is incident to 1 face
        } else {
            midpoint.get()->pos = (v1->pos + v2->pos + centroids.at(f1)->pos) / 3.f;
        }

        // update half edges (similar logic as splitEdge function)
        HalfEdge *he2 = e->sym;
        uPtr<HalfEdge> he1b = mkU<HalfEdge>();
        uPtr<HalfEdge> he2b = mkU<HalfEdge>();

        he1b->setVertex(v1);
        he1b->face = e->face;
        he2b->setVertex(v2);
        he2b->face = he2->face;

        he1b->next = e->next;
        he2b->next = he2->next;
        e->next = he1b.get();
        he2->next = he2b.get();

        e->setVertex(midpoint.get());
        he2->setVertex(midpoint.get());

        he2b->setSym(e);
        he1b->setSym(he2);

        midpoints->insert(std::pair<HalfEdge*, Vertex*>(he1b.get(), midpoint.get()));
        midpoints->insert(std::pair<HalfEdge*, Vertex*>(he2b.get(), midpoint.get()));

        // check if original edge was a sharp edge
        if (std::find(sharpEdges->begin(), sharpEdges->end(), e) != sharpEdges->end()) {
            sharpEdges->push_back(he1b.get());
            sharpEdges->push_back(he2b.get());
        }

        newVertices->push_back(std::move(midpoint));
        newHalfEdges->push_back(std::move(he1b));
        newHalfEdges->push_back(std::move(he2b));
    }
}

void Mesh::computeSmoothedVertex(Vertex* v, std::map<Face*, Vertex*> &centroids,
                                 std::map<HalfEdge*, Vertex*> &midpoints) {
    HalfEdge* e = v->edge;

    float n = 0; // number of adjacent midpoints
    glm::vec3 sumMidpoints;
    glm::vec3 sumCentroids;

    // find all adjacent midpoints and centroids by traversing sym pointers
    do {
        glm::vec3 pos = midpoints.find(e)->second->pos;
        sumMidpoints += midpoints.find(e)->second->pos;
        sumCentroids += centroids.find(e->face)->second->pos;
        n++;
        e = e->next->sym;
    } while (e != v->edge);

    v->pos = ((n - 2) * v->pos / n) + ((sumMidpoints + sumCentroids) / (n * n));
}

void Mesh::splitFace(Vertex* centroid,
                     std::vector<uPtr<Face>> *newFaces,
                     std::vector<uPtr<HalfEdge>> *newHalfEdges,
                     std::vector<std::pair<HalfEdge*, HalfEdge*>> &originalEdges) {
    for (std::pair<HalfEdge*, HalfEdge*> e : originalEdges) {
        HalfEdge* e0 = e.second; // edge pointing to midpoint
        uPtr<Face> newFace = mkU<Face>();
        newFace.get()->edge = e0;
        newFace.get()->color = e0->face->color;

        uPtr<HalfEdge> newE1 = mkU<HalfEdge>();
        uPtr<HalfEdge> newE2 = mkU<HalfEdge>();

        // find edge pointing to e0
        HalfEdge* e3 = e0;
        for (int i = 0; i < originalEdges.size(); ++i) {
            if (originalEdges[i].second == e0) {
                e3 = originalEdges[i].first;
                break;
            }
        }

        // build ring
        e0->next = newE1.get();
        newE1.get()->next = newE2.get();
        newE2.get()->next = e3;

        // assign vertices
        newE1.get()->setVertex(centroid);
        newE2.get()->setVertex(e3->sym->vertex);

        // assign faces
        e0->face = newFace.get();
        newE1.get()->face = newFace.get();
        newE2.get()->face = newFace.get();
        e3->face = newFace.get();

        newHalfEdges->push_back(std::move(newE1));
        newHalfEdges->push_back(std::move(newE2));
        newFaces->push_back(std::move(newFace));
    }
}

void Mesh::assignSym(std::vector<std::pair<HalfEdge*, HalfEdge*>> *originalEdges) {
    for (int i = 0; i < originalEdges->size() - 1; ++i) {
        originalEdges->at(i).second->next->setSym(originalEdges->at(i + 1).second->next->next);
    }

    originalEdges->at(originalEdges->size() - 1).second->next->
            setSym(originalEdges->at(0).second->next->next);
}
