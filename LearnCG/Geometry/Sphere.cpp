//
//  Sphere.cpp
//  LearnCG
//
//  Created by leezy on 16/6/18.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include "Sphere.h"


#define RENDER_NORMAL 0
#define RENDER_TRAIGNLE_ADJANCENCY 1

#define EXP 0.0000001

Sphere::Sphere()
{
    memset(mVAO, 0, sizeof(mVAO));
    mVBO = 0;
    memset(mIBO, 0, sizeof(mIBO));
    mInit = false;
    
    mLan = 30;
    mLong = 30;
    mFaces = 0;
    mAdjFaces = 0;
}

Sphere::~Sphere()
{
    if ( mVAO[0] )
        glDeleteVertexArrays(2, mVAO);
    if ( mIBO[0] )
        glDeleteBuffers(2, mIBO);
    if ( mVBO )
        glDeleteBuffers(1, &mVBO);
}

void Sphere::SetRadius(float aRad)
{
    mR = aRad;
}

void Sphere::SetLantitude(int aLantitude)
{
    mLan = aLantitude;
}

void Sphere::SetLongitude(int aLongitude)
{
    mLong = aLongitude;
}

void Sphere::Render(ESPHERE_RENDER eRender)
{
    if ( !mInit ) {
        mInit = Init();
        if ( !mInit )
            return;
    }
    
    if ( ESPHERE_RenderNormal == eRender ) {
        glBindVertexArray(mVAO[RENDER_NORMAL]);
        glDrawElements(GL_TRIANGLES, mFaces*3, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    } else if ( ESPHERE_RenderTriangleAdjancency == eRender ) {
        glBindVertexArray(mVAO[RENDER_TRAIGNLE_ADJANCENCY]);
        glDrawElements(GL_TRIANGLES_ADJACENCY, mAdjFaces, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    }
}

bool Sphere::Init()
{
    mFaces = 4*mLong*mLan; // 4*Long+4*Long*(Lan-1)
    
    std::vector<Vertex> vertices(mFaces*3);
    
    double phi_r = glm::pi<float>()/2;
    double theta_r = 2*glm::pi<float>();
    double phi_d = glm::pi<float>()/(mLan+1);
    double theta_d = glm::pi<float>()/mLong;

    int N = 0;
    for (double phi = -phi_r; phi + phi_d <= phi_r; phi+=phi_d) {
        for (double theta = 0; theta + theta_d - EXP <= theta_r; theta+=theta_d) {
            float cos0 = cosf(phi), sin0 = sinf(phi);
            float cos1 = cosf(phi+phi_d), sin1 = sinf(phi+phi_d);
            float cos2 = cosf(theta), sin2 = sinf(theta);
            float cos3 = cosf((theta+theta_d+EXP>=theta_r)?0:(theta+theta_d)), sin3 = sinf((theta+theta_d+EXP>=theta_r)?0:(theta+theta_d));
            
            if ( phi == -phi_r ) {
                glm::vec3 v0(cos0, 0, sin0);
                glm::vec3 v1(cos1*cos2, cos1*sin2, sin1);
                glm::vec3 v3(cos1*cos3, cos1*sin3, sin1);
                
                vertices[N++].Position = v0*mR;
                vertices[N++].Position = v1*mR;
                vertices[N++].Position = v3*mR;
            } else if ( phi + phi_d + EXP >= phi_r ){
                glm::vec3 v0(cos0*cos2, cos0*sin2, sin0);
                glm::vec3 v1(cos1, 0, sin1);
                glm::vec3 v2(cos0*cos3, cos0*sin3, sin0);
                
                vertices[N++].Position = v0*mR;
                vertices[N++].Position = v1*mR;
                vertices[N++].Position = v2*mR;
            } else {
                glm::vec3 v0(cos0*cos2, cos0*sin2, sin0);
                glm::vec3 v1(cos1*cos2, cos1*sin2, sin1);
                glm::vec3 v2(cos0*cos3, cos0*sin3, sin0);
                glm::vec3 v3(cos1*cos3, cos1*sin3, sin1);
                
                vertices[N++].Position = v0*mR;
                vertices[N++].Position = v1*mR;
                vertices[N++].Position = v2*mR;
                vertices[N++].Position = v2*mR;
                vertices[N++].Position = v1*mR;
                vertices[N++].Position = v3*mR;
            }
        }
    }
    
    for (size_t i = 0; i < N; ++i) {
        vertices[i].Normal = vertices[i].Position;
    }
    
    std::vector<GLushort> indices(N);
    for (size_t i = 0; i < N; ++i) {
        indices[i] = i;
    }
    
    glGenBuffers(1, &mVBO);
    glGenBuffers(2, mIBO);
    glGenVertexArrays(2, mVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertices.size(), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    Init_RenderNormal(vertices, indices);
    Init_RenderTriangleAdjancency(vertices, indices);
    return true;
}

bool Sphere::Init_RenderNormal(const std::vector<Vertex> &vertices, const std::vector<GLushort> &indices)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO[RENDER_NORMAL]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*indices.size(), &indices[0], GL_STATIC_DRAW);
    
    glBindVertexArray(mVAO[RENDER_NORMAL]);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)sizeof(glm::vec3));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO[RENDER_NORMAL]);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return true;
}

bool Sphere::Init_RenderTriangleAdjancency(const std::vector<Vertex> &vertices, const std::vector<GLushort> &indices)
{
    EdgeMap edge;
    UniqueMap uniqueIndex;
    for (size_t i = 0; i < indices.size(); ++i) {
        glm::vec3 const& Position = vertices[indices[i]].Position;
        if ( uniqueIndex.find(Position) == uniqueIndex.end() ) {
            uniqueIndex[Position] = indices[i];
        }
    }

    for (size_t i = 0; i < indices.size(); i += 3) {
        int idx0 = indices[i];
        int idx1 = indices[i+1];
        int idx2 = indices[i+2];
        idx0 = uniqueIndex[vertices[idx0].Position];
        idx1 = uniqueIndex[vertices[idx1].Position];
        idx2 = uniqueIndex[vertices[idx2].Position];
        edge[std::make_pair(idx0, idx1)] = idx2;
        edge[std::make_pair(idx1, idx2)] = idx0;
        edge[std::make_pair(idx2, idx0)] = idx1;
    }
    
    std::vector<GLushort> Inds;
    for (size_t i = 0; i < indices.size(); i += 3) {
        int idx0 = indices[i];
        int idx1 = indices[i+1];
        int idx2 = indices[i+2];
        idx0 = uniqueIndex[vertices[idx0].Position];
        idx1 = uniqueIndex[vertices[idx1].Position];
        idx2 = uniqueIndex[vertices[idx2].Position];
        if ( edge.find(std::make_pair(idx0, idx1)) == edge.end() )
        {
            assert(0);
        }
        if ( edge.find(std::make_pair(idx1, idx2)) == edge.end() )
        {
            assert(0);
        }
        if ( edge.find(std::make_pair(idx2, idx0)) == edge.end() )
        {
            assert(0);
        }
        int idx3 = edge[std::make_pair(idx1, idx0)];
        int idx4 = edge[std::make_pair(idx2, idx1)];
        int idx5 = edge[std::make_pair(idx0, idx2)];
        Inds.push_back(idx0);
        Inds.push_back(idx3);
        Inds.push_back(idx1);
        Inds.push_back(idx4);
        Inds.push_back(idx2);
        Inds.push_back(idx5);
    }
    
    mAdjFaces = static_cast<int>(Inds.size());
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO[RENDER_TRAIGNLE_ADJANCENCY]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*Inds.size(), &Inds[0], GL_STATIC_DRAW);
    
    glBindVertexArray(mVAO[RENDER_TRAIGNLE_ADJANCENCY]);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)sizeof(glm::vec3));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO[RENDER_TRAIGNLE_ADJANCENCY]);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return true;
}
