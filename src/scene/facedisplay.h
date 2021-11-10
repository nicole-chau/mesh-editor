#pragma once

#include "drawable.h"
#include "face.h"

class FaceDisplay : public Drawable {
protected:
    Face *representedFace;

public:
    FaceDisplay(OpenGLContext *mp_context);
    void create() override;
    void updateFace(Face*);
    GLenum drawMode() override;
};
