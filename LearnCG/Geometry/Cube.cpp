//
//  Cube.cpp
//  LearnCG
//
//  Created by leezy on 16/6/4.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include "Cube.h"
#include <vector>


Cube::Cube()
{
    mInit = false;
    memset(mVAO, 0, sizeof(mVAO));
    mVBO = 0;
    memset(mIBO, 0, sizeof(mIBO));
}

Cube::~Cube()
{
    glDeleteVertexArrays(2, mVAO);
    glDeleteBuffers(1, &mVBO);
    glDeleteBuffers(2, mIBO);
}

void Cube::Render(ECUBE_RENDER eRender)
{
    if ( !mInit )
        mInit = Init();
    if ( mInit ) {
        if ( ECUBE_RenderNormal == eRender ) {
            glBindVertexArray(mVAO[0]);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, NULL);
            glBindVertexArray(0);
        } else if ( ECUBE_RenderTriangleAdjacency == eRender ) {
            glBindVertexArray(mVAO[1]);
            glDrawElements(GL_TRIANGLES_ADJACENCY, 72, GL_UNSIGNED_SHORT, NULL);
            glBindVertexArray(0);
        }
    }
}

bool Cube::Init()
{
    std::vector<Vertex> Vertices(24);
    // front
    Vertices[0].Position = glm::vec3(-1, 1, 1);
    Vertices[0].Normal = glm::vec3(0, 0, 1);
    Vertices[0].TexCoord = glm::vec2(0, 0);
    Vertices[1].Position = glm::vec3(-1, -1, 1);
    Vertices[1].Normal = glm::vec3(0, 0, 1);
    Vertices[1].TexCoord = glm::vec2(0, 1);
    Vertices[2].Position = glm::vec3(1, 1, 1);
    Vertices[2].Normal = glm::vec3(0, 0, 1);
    Vertices[2].TexCoord = glm::vec2(1, 0);
    Vertices[3].Position = glm::vec3(1, -1, 1);
    Vertices[3].Normal = glm::vec3(0, 0, 1);
    Vertices[3].TexCoord = glm::vec2(1, 1);
    // right
    Vertices[4].Position = glm::vec3(1, 1, 1);
    Vertices[4].Normal = glm::vec3(1, 0, 0);
    Vertices[4].TexCoord = glm::vec2(0, 0);
    Vertices[5].Position = glm::vec3(1, -1, 1);
    Vertices[5].Normal = glm::vec3(1, 0, 0);
    Vertices[5].TexCoord = glm::vec2(0, 1);
    Vertices[6].Position = glm::vec3(1, 1, -1);
    Vertices[6].Normal = glm::vec3(1, 0, 0);
    Vertices[6].TexCoord = glm::vec2(1, 0);
    Vertices[7].Position = glm::vec3(1, -1, -1);
    Vertices[7].Normal = glm::vec3(1, 0, 0);
    Vertices[7].TexCoord = glm::vec2(1, 1);
    // back
    Vertices[8].Position = glm::vec3(1, 1, -1);
    Vertices[8].Normal = glm::vec3(0, 0, -1);
    Vertices[8].TexCoord = glm::vec2(0, 0);
    Vertices[9].Position = glm::vec3(1, -1, -1);
    Vertices[9].Normal = glm::vec3(0, 0, -1);
    Vertices[9].TexCoord = glm::vec2(0, 1);
    Vertices[10].Position = glm::vec3(-1, 1, -1);
    Vertices[10].Normal = glm::vec3(0, 0, -1);
    Vertices[10].TexCoord = glm::vec2(1, 0);
    Vertices[11].Position = glm::vec3(-1, -1, -1);
    Vertices[11].Normal = glm::vec3(0, 0, -1);
    Vertices[11].TexCoord = glm::vec2(1, 1);
    // left
    Vertices[12].Position = glm::vec3(-1, 1, -1);
    Vertices[12].Normal = glm::vec3(-1, 0, 0);
    Vertices[12].TexCoord = glm::vec2(0, 0);
    Vertices[13].Position = glm::vec3(-1, -1, -1);
    Vertices[13].Normal = glm::vec3(-1, 0, 0);
    Vertices[13].TexCoord = glm::vec2(0, 1);
    Vertices[14].Position = glm::vec3(-1, 1, 1);
    Vertices[14].Normal = glm::vec3(-1, 0, 0);
    Vertices[14].TexCoord = glm::vec2(1, 0);
    Vertices[15].Position = glm::vec3(-1, -1, 1);
    Vertices[15].Normal = glm::vec3(-1, 0, 0);
    Vertices[15].TexCoord = glm::vec2(1, 1);
    // top
    Vertices[16].Position = glm::vec3(-1, 1, -1);
    Vertices[16].Normal = glm::vec3(0, 1, 0);
    Vertices[16].TexCoord = glm::vec2(0, 0);
    Vertices[17].Position = glm::vec3(-1, 1, 1);
    Vertices[17].Normal = glm::vec3(0, 1, 0);
    Vertices[17].TexCoord = glm::vec2(0, 1);
    Vertices[18].Position = glm::vec3(1, 1, -1);
    Vertices[18].Normal = glm::vec3(0, 1, 0);
    Vertices[18].TexCoord = glm::vec2(1, 0);
    Vertices[19].Position = glm::vec3(1, 1, 1);
    Vertices[19].Normal = glm::vec3(0, 1, 0);
    Vertices[19].TexCoord = glm::vec2(1, 1);
    // bottom
    Vertices[20].Position = glm::vec3(1, -1, 1);
    Vertices[20].Normal = glm::vec3(0, -1, 0);
    Vertices[20].TexCoord = glm::vec2(1, 1);
    Vertices[21].Position = glm::vec3(1, -1, -1);
    Vertices[21].Normal = glm::vec3(0, -1, 0);
    Vertices[21].TexCoord = glm::vec2(1, 0);
    Vertices[22].Position = glm::vec3(-1, -1, 1);
    Vertices[22].Normal = glm::vec3(0, -1, 0);
    Vertices[22].TexCoord = glm::vec2(0, 1);
    Vertices[23].Position = glm::vec3(-1, -1, -1);
    Vertices[23].Normal = glm::vec3(0, -1, 0);
    Vertices[23].TexCoord = glm::vec2(0, 0);
    
    GLushort Indices[36] = {
        0, 1, 2, 2, 1, 3,
        4, 5, 6, 6, 5, 7,
        8, 9, 10, 10, 9, 11,
        12, 13, 14, 14, 13, 15,
        16, 17, 18, 18, 17, 19,
        20, 22, 21, 21, 22, 23
    };
    
    int count = sizeof(Indices)/sizeof(GLushort);
    EdgeMap edge;
    for (int i = 0; i < count; i += 3) {
        int idx0 = Indices[i];
        int idx1 = Indices[i+1];
        int idx2 = Indices[i+2];
        edge[std::make_pair(Vertices[idx0].Position, Vertices[idx1].Position)] = idx2;
        edge[std::make_pair(Vertices[idx1].Position, Vertices[idx2].Position)] = idx0;
        edge[std::make_pair(Vertices[idx2].Position, Vertices[idx0].Position)] = idx1;
    }
    
    std::vector<GLushort> Inds;
    for (int i = 0; i < count; i += 3) {
        int idx0 = Indices[i];
        int idx1 = Indices[i+1];
        int idx2 = Indices[i+2];
        int idx3 = edge[std::make_pair(Vertices[idx1].Position, Vertices[idx0].Position)];
        int idx4 = edge[std::make_pair(Vertices[idx2].Position, Vertices[idx1].Position)];
        int idx5 = edge[std::make_pair(Vertices[idx0].Position, Vertices[idx2].Position)];
        Inds.push_back(idx0);
        Inds.push_back(idx3);
        Inds.push_back(idx1);
        Inds.push_back(idx4);
        Inds.push_back(idx2);
        Inds.push_back(idx5);
    }
    
    glGenBuffers(1, &mVBO);
    glGenBuffers(2, mIBO);
    glGenVertexArrays(2, mVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*Vertices.size(), &Vertices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), &Indices[0], GL_STATIC_DRAW);
    
    glBindVertexArray(mVAO[0]);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)12);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)24);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO[0]);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*Inds.size(), &Inds[0], GL_STATIC_DRAW);
    
    glBindVertexArray(mVAO[1]);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)12);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)24);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO[1]);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    return true;
}
