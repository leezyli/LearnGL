//
//  BoundingVolume.cpp
//  LearnCG
//
//  Created by leezy on 16/7/25.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include "BoundingVolume.h"
#include "UtilCG.h"
#include "Camera.h"
#include "Pipeline.h"
#include "Math.h"

BoundingBoxDrawer::BoundingBoxDrawer()
{
    mVAO = 0;
    mProg = INVALID_HANDLE;
}

BoundingBoxDrawer::~BoundingBoxDrawer()
{
    if ( INVALID_HANDLE != mProg )
        glDeleteProgram(mProg);
    
    if ( mVAO )
        glDeleteVertexArrays(1, &mVAO);
}

bool BoundingBoxDrawer::LoadShader()
{
    std::string VShader = g_AssetsPath+"/BoundingVolume/Box.vs";
    std::string FShader = g_AssetsPath+"/BoundingVolume/Box.fs";
    mProg = UtilCG::Shader::LoadShaders(VShader.c_str(), FShader.c_str());
    return INVALID_HANDLE != mProg;
}

bool BoundingBoxDrawer::Make(BoundingBox &box)
{
    Eigen::Matrix3f mat[8];
    // left, top, near
    mat[0] << box.left.x, box.left.y, box.left.z, box.top.x, box.top.y, box.top.z, box.near.x, box.near.y, box.near.z;
    // left, top, far
    mat[1] << box.left.x, box.left.y, box.left.z, box.top.x, box.top.y, box.top.z, box.far.x, box.far.y, box.far.z;
    // left, bottom, near
    mat[2] << box.left.x, box.left.y, box.left.z, box.bottom.x, box.bottom.y, box.bottom.z, box.near.x, box.near.y, box.near.z;
    // left, bottom, far
    mat[3] << box.left.x, box.left.y, box.left.z, box.bottom.x, box.bottom.y, box.bottom.z, box.far.x, box.far.y, box.far.z;
    // right, top, near
    mat[4] << box.right.x, box.right.y, box.right.z, box.top.x, box.top.y, box.top.z, box.near.x, box.near.y, box.near.z;
    // right, top, far
    mat[5] << box.right.x, box.right.y, box.right.z, box.top.x, box.top.y, box.top.z, box.far.x, box.far.y, box.far.z;
    // right, bottom, near
    mat[6] << box.right.x, box.right.y, box.right.z, box.bottom.x, box.bottom.y, box.bottom.z, box.near.x, box.near.y, box.near.z;
    // right, bottom, far
    mat[7] << box.right.x, box.right.y, box.right.z, box.bottom.x, box.bottom.y, box.bottom.z, box.far.x, box.far.y, box.far.z;
    
    Eigen::Vector3f d[8];
    // left, top, near
    d[0] << box.left.w, box.top.w, box.near.w;
    // left, top, far
    d[1] << box.left.w, box.top.w, box.far.w;
    // left, bottom, near
    d[2] << box.left.w, box.bottom.w, box.near.w;
    // left, bottom, far
    d[3] << box.left.w, box.bottom.w, box.far.w;
    // right, top, near
    d[4] << box.right.w, box.top.w, box.near.w;
    // right, top, far
    d[5] << box.right.w, box.top.w, box.far.w;
    // right, bottom, near
    d[6] << box.right.w, box.bottom.w, box.near.w;
    // right, bottom, far
    d[7] << box.right.w, box.bottom.w, box.far.w;
    
    Eigen::Vector3f Q[8];
    for (int i = 0; i < 8; ++i)
        Q[i] = mat[i].inverse() * (-d[i]);
    
    
    glm::vec3 Position[8];
    for (int i = 0; i < 8; ++i) {
        Position[i] = Util::Internal::ToVector3f(Q[i]);
    }
    
    GLushort Indices[24] = {0};
    Indices[0] = 0; Indices[1] = 1; Indices[2] = 2; Indices[3] = 3;
    Indices[4] = 0; Indices[5] = 2; Indices[6] = 4; Indices[7] = 6;
    Indices[8] = 0; Indices[9] = 4; Indices[10] = 1; Indices[11] = 5;
    Indices[12] = 4; Indices[13] = 6; Indices[14] = 5; Indices[15] = 7;
    Indices[16] = 2; Indices[17] = 3; Indices[18] = 6; Indices[19] = 7;
    Indices[20] = 1; Indices[21] = 5; Indices[22] = 3; Indices[23] = 7;
    
    GLuint XBO[2];
    glGenBuffers(2, XBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, XBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*8, &Position[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, XBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), &Indices[0], GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, XBO[1]);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glDeleteBuffers(2, XBO);
    
    return true;
}

bool BoundingBoxDrawer::Render(const Camera &aCamera, const Pipeline &aPipeline)
{
    if ( !mVAO )
        return false;
    
    if ( INVALID_HANDLE == mProg ) {
        if ( !LoadShader() )
            return false;
    }
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    GLint curProg = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &curProg);
    
    glUseProgram(mProg);
    glClearColor(0, 0, 0, 0);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    GLint gWorldMat = glGetUniformLocation(mProg, "gWorldMat");
    GLint gViewMat = glGetUniformLocation(mProg, "gViewMat");
    GLint gProjMat = glGetUniformLocation(mProg, "gProjMat");
    glUniformMatrix4fv(gWorldMat, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
    glUniformMatrix4fv(gViewMat, 1, GL_FALSE, glm::value_ptr(aCamera.GetViewMat()));
    glUniformMatrix4fv(gProjMat, 1, GL_FALSE, glm::value_ptr(aPipeline.GetProjMat()));
    
    glBindVertexArray(mVAO);
    for (int i = 0; i < 6; ++i) {
        glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(sizeof(GLushort)*i*4));
    }
    glBindVertexArray(0);
    
    glUseProgram(curProg);
    glDisable(GL_BLEND);
    
    return true;
}
