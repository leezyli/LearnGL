//
//  Model.cpp
//  LearnCG
//
//  Created by leezy on 16/5/14.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include "Model.h"
#include "assimp/assimp/postprocess.h"



void Model::MeshEntry::Init(const std::vector<Vertex>& Vertices,
                            const std::vector<GLushort> &Indices,
                            GLuint PrimitiveType, GLuint MaterialIndex)
{
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mIBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*Vertices.size(), &Vertices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*Indices.size(), &Indices[0], GL_STATIC_DRAW);
    
    mIndexNum = static_cast<GLuint>(Indices.size());
    //mPrimitiveType = PrimitiveType;
    mMaterialIndex = MaterialIndex;
}

Model::Model()
{
    memset(mVAO, 0, sizeof(mVAO));
}

Model::~Model()
{
    glDeleteVertexArrays(2, mVAO);
    
    for (int i = 0; i < mMeshs.size(); ++i) {
        if ( mMeshs[i].mVBO )
            glDeleteBuffers(1, &mMeshs[i].mVBO);
        if ( mMeshs[i].mIBO )
            glDeleteBuffers(1, &mMeshs[i].mIBO);
    }
    
    for (int i = 0; i < mTextures.size(); ++i) {
        if ( mTextures[i] ) {
            delete mTextures[i];
            mTextures[i] = NULL;
        }
    }
}

bool Model::Load(std::string const& Path, uint32_t eFlag)
{
    Assimp::Importer oImporter;
    aiScene const* const pScene = oImporter.ReadFile(Path.c_str(),
                                            aiProcess_Triangulate|aiProcess_GenSmoothNormals|aiProcess_GenUVCoords);
    if ( pScene )
    {
        glGenVertexArrays(2, mVAO);
    }
    if ( EModelFlag_Normal & eFlag ) {
        if ( !LoadFromScene(pScene) )
            return false;
    }
    if ( EModelFlag_TriangleAdjacency & eFlag ) {
        if ( !LoadFromSceneTS(pScene) )
            return false;
    }
    
    mFlag = eFlag;
    return LoadFromMaterials(pScene, Path);
}

void Model::Render(EMODEL_RENDER eRender)
{
    switch (eRender) {
        case EModelRender_Normal:
            RenderForNormal();
            break;
        case EModelRender_TriangleAdjacency:
            RenderForTriangleAdjacency();
            break;
        default:
            break;
    }
}

void Model::RenderForNormal()
{
    glBindVertexArray(mVAO[0]);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    
    for (uint32_t i = 0; i < mMeshs.size(); ++i) {
        uint32_t MaterialIndex = mMeshs[i].mMaterialIndex;
        
        if ( MaterialIndex < mTextures.size() && mTextures[MaterialIndex] ) {
            mTextures[MaterialIndex]->Bind(GL_TEXTURE0);
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, mMeshs[i].mVBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)sizeof(glm::vec3));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(glm::vec3)*2));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mMeshs[i].mIBO);
        
        glDrawElements(GL_TRIANGLES, mMeshs[i].mIndexNum, GL_UNSIGNED_SHORT, 0);
        
    }
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glBindVertexArray(0);
}

void Model::RenderForTriangleAdjacency()
{
    if ( !mVAO[1] )
        return;
    
    glBindVertexArray(mVAO[1]);
    glDrawElements(GL_TRIANGLES_ADJACENCY, mTSIndNum, GL_UNSIGNED_SHORT, NULL);
    glBindVertexArray(0);
}

bool Model::LoadFromScene(const aiScene *const pScene)
{
    if ( !pScene )
        return false;
   
    mMeshs.resize(pScene->mNumMeshes);
    for (uint32_t i = 0; i < pScene->mNumMeshes; ++i) {
        const aiMesh* const pMesh = pScene->mMeshes[i];
        if ( !LoadFromMesh(i, pMesh) )
            return false;
    }
    
    return true;
}

bool Model::LoadFromSceneTS(const aiScene *const pScene)
{
    if ( !pScene )
        return false;
    
    int32_t totalNumVers = 0;
    int32_t totalNumInds = 0;
    for (uint32_t i = 0; i < pScene->mNumMeshes; ++i) {
        const aiMesh* const pMesh = pScene->mMeshes[i];
        if ( !LoadFromMeshTS_Step0(i, totalNumVers, pMesh) )
            return false;
        
        totalNumVers += pMesh->mNumVertices;
        totalNumInds += pMesh->mNumFaces*3;
    }
    
    totalNumVers = 0;
    for (uint32_t i = 0; i < pScene->mNumMeshes; ++i) {
        const aiMesh* const pMesh = pScene->mMeshes[i];
        if ( !LoadFromMeshTS_Step1(i, totalNumVers, pMesh) )
            return false;
        totalNumVers += pMesh->mNumVertices;
    }
   
    GLuint VBO = 0;
    GLuint IBO = 0;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &IBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*mTSVers.size(), &mTSVers[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*mTSInds.size(), &mTSInds[0], GL_STATIC_DRAW);
    
    glBindVertexArray(mVAO[1]);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), NULL);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &IBO);
    
    mTSIndNum = static_cast<int>(mTSInds.size());
    mTSVers.clear();
    mTSInds.clear();
    
    return true;
}

bool Model::LoadFromMesh(int index, const aiMesh *const pMesh)
{
    if ( !pMesh )
        return false;
    
    std::vector<Vertex> Vertices;
    std::vector<GLushort> Indices;
    
    for (uint32_t i = 0; i < pMesh->mNumVertices; ++i) {
        const aiVector3D& Position = pMesh->mVertices[i];
        const aiVector3D& Normal = pMesh->mNormals[i];
        const aiVector3D& TexCoord = pMesh->HasTextureCoords(0)?pMesh->mTextureCoords[0][i]:aiVector3D(0,0,0);
        
        Vertex v(glm::vec3(Position.x, Position.y, Position.z),
                 glm::vec3(Normal.x, Normal.y, Normal.z),
                 glm::vec2(TexCoord.x, TexCoord.y));
        Vertices.push_back(v);
    }
    
    for (uint32_t i = 0; i < pMesh->mNumFaces; ++i) {
        const aiFace& Face = pMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }
    
    mMeshs[index].Init(Vertices, Indices, pMesh->mPrimitiveTypes,pMesh->mMaterialIndex);
    return true;
}

bool Model::LoadFromMeshTS_Step0(int index, int verBais, const aiMesh *const pMesh)
{
    if ( !pMesh )
        return false;
    
    for (uint32_t i = 0; i < pMesh->mNumVertices; ++i) {
        const aiVector3D& Position = pMesh->mVertices[i];
        mTSVers.push_back(glm::vec3(Position.x, Position.y, Position.z));
    }
    
    for (uint32_t i = 0; i < pMesh->mNumFaces; ++i) {
        const aiFace& Face = pMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        int in0 = Face.mIndices[0];
        int in1 = Face.mIndices[1];
        int in2 = Face.mIndices[2];
        
        const aiVector3D& p0 = pMesh->mVertices[in0];
        const aiVector3D& p1 = pMesh->mVertices[in1];
        const aiVector3D& p2 = pMesh->mVertices[in2];
        
        glm::vec3 v0(p0.x, p0.y, p0.z);
        glm::vec3 v1(p1.x, p1.y, p1.z);
        glm::vec3 v2(p2.x, p2.y, p2.z);
        
        mEdge[std::make_pair(v0, v1)] = in2+verBais;
        mEdge[std::make_pair(v1, v2)] = in0+verBais;
        mEdge[std::make_pair(v2, v0)] = in1+verBais;
    }
    
    return true;
}

bool Model::LoadFromMeshTS_Step1(int index, int verBais, const aiMesh *const pMesh)
{
    if ( !pMesh )
        return false;

    for (uint32_t i = 0; i < pMesh->mNumFaces; ++i) {
        const aiFace& Face = pMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        
        int in0 = Face.mIndices[0];
        int in1 = Face.mIndices[1];
        int in2 = Face.mIndices[2];
        
        const aiVector3D& p0 = pMesh->mVertices[in0];
        const aiVector3D& p1 = pMesh->mVertices[in1];
        const aiVector3D& p2 = pMesh->mVertices[in2];
        
        glm::vec3 v0(p0.x, p0.y, p0.z);
        glm::vec3 v1(p1.x, p1.y, p1.z);
        glm::vec3 v2(p2.x, p2.y, p2.z);

        EdgeMap::iterator it0 = mEdge.find(std::make_pair(v1, v0));
        EdgeMap::iterator it1 = mEdge.find(std::make_pair(v2, v1));
        EdgeMap::iterator it2 = mEdge.find(std::make_pair(v0, v2));
        if ( it0 != mEdge.end() && it1 != mEdge.end() && it2 != mEdge.end() ) {
            mTSInds.push_back(in0+verBais);
            mTSInds.push_back(it0->second);
            mTSInds.push_back(in1+verBais);
            mTSInds.push_back(it1->second);
            mTSInds.push_back(in2+verBais);
            mTSInds.push_back(it2->second);
        } else {
            assert(0);
        }
    }
    
    return true;
}

bool Model::LoadFromMaterials(aiScene const* const pScene, std::string const& Path)
{
    if ( !pScene )
        return false;
    
    std::string Dir;
    size_t Pos = Path.rfind('/');
    if ( std::string::npos != Pos ) {
        Dir = Path.substr(0, Pos);
    }
    
    mTextures.resize(pScene->mNumMaterials);
    for (uint32_t i = 0; i < pScene->mNumMaterials; ++i) {
        const aiMaterial* const pMaterial = pScene->mMaterials[i];
        if ( pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0 ) {
            aiString aiPath;
            if ( pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiPath) == AI_SUCCESS ) {
                std::string Path(aiPath.data);
                std::string AssetsPath = Dir + "/" + Path;
                
                mTextures[i] = new Texture2D;
                if ( !mTextures[i]->Load(AssetsPath) ) {
                    printf("Model::LoadFromMaterials!Load Texture failed.");
                    delete mTextures[i];
                    mTextures[i] = NULL;
                    return false;
                }
            }
        }
    }
    return true;
}