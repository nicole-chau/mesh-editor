#version 150

in vec4 vs_Pos;
in vec4 vs_Col;
in vec2 vs_Weights; // 2 joint influencers
in ivec2 vs_JointIDs;

uniform mat4 u_ViewProj;
uniform mat4 u_Model;
uniform mat4[100] u_OverallTransformMats;
uniform mat4[100] u_BindMats;

out vec4 fs_Col;
out vec2 fs_Weights;
out mat4 fs_OverallTrans1;
out mat4 fs_OverallTrans2;

out vec2 fs_JointIDs;

vec4 findBlendedPos() {
    vec4 result = vec4(0.f);
    for (int i = 0; i < 2; ++i) {
        result += u_OverallTransformMats[vs_JointIDs[i]]
                  * u_BindMats[vs_JointIDs[i]] * vs_Pos * vs_Weights[i];
    }

    return result;
}

void main() {
    fs_Col = vs_Col;

    gl_Position = u_ViewProj * findBlendedPos();
}


