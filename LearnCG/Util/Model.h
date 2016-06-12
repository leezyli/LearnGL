//
//  Model.h
//  LearnCG
//
//  Created by leezy on 16/5/14.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#ifndef __LearnCG__Model__
#define __LearnCG__Model__

#include <stdio.h>
#include <vector>
#include "LearnCG.h"
#include "assimp/assimp/Importer.hpp"
#include "assimp/assimp/scene.h"
#include "Texture2D.h"
#include <map>

enum EMODEL_FLAGS
{
    EModelFlag_Normal = 0x1,
    EModelFlag_TriangleAdjacency = 0x2
};

enum EMODEL_RENDER
{
    EModelRender_Normal,
    EModelRender_TriangleAdjacency
};

class Model
{
    typedef Model this_class;
public:
    Model();
    ~Model();
    
    bool Load(std::string const& Path, uint32_t eFlag = EModelFlag_Normal);
    void Render(EMODEL_RENDER eRender = EModelRender_Normal);
    bool MakeShadowVolume(glm::vec3 const& aLightDir);
protected:
    bool LoadFromScene(aiScene const* const pScene);
    bool LoadFromSceneTS(aiScene const* const pScene);
    bool LoadFromMesh(int index, aiMesh const* const pMesh);
    bool LoadFromMeshTS_Step0(int index, int verBais, aiMesh const* const pMesh);
    bool LoadFromMeshTS_Step1(int index, int VerBais, aiMesh const* const pMesh);
    bool LoadFromMaterials(aiScene const* const pScene, std::string const& Path);
    void RenderForNormal();
    void RenderForTriangleAdjacency();
private:
    
    struct Vertex
    {
        glm::vec3 mPosition;
        glm::vec3 mNormal;
        glm::vec2 mTexCoord;
        
        explicit Vertex(glm::vec3 const& Position,
                        glm::vec3 const& Normal,
                        glm::vec2 const& TexCoord):
        mPosition(Position), mNormal(Normal), mTexCoord(TexCoord){}
    };
    
    struct MeshEntry {
        GLuint mVBO;
        GLuint mIBO;
        GLuint mIndexNum;
        GLuint mMaterialIndex;
        //GLuint mPrimitiveType;
        MeshEntry() {
            mVBO = 0;
            mIBO = 0;
            //mPrimitiveType = 0;
            mMaterialIndex = -1;
        }
        
        void Init(std::vector<Vertex> const& Vertices,
                  std::vector<GLushort> const& Indices,
                  GLuint PrimitiveType,
                  GLuint MaterialIndex);
    };
    
    typedef std::pair<glm::vec3, glm::vec3> TSVer;
    struct LessObj: std::binary_function<TSVer, TSVer, bool>
    {
        bool operator()(const TSVer& l, const TSVer& r) const
        {
            if ( l.first < r.first ) return true;
            if ( l.first == r.first ) {
                if ( l.second < r.second ) return true;
            }
            return false;
        }
    };
    typedef std::map<TSVer, GLushort, LessObj> EdgeMap;
    
    GLuint mVAO[2];
    GLuint mFlag;
    int mTSIndNum;
    EdgeMap mEdge; // <u,v> -> index
    std::vector<MeshEntry> mMeshs;
    std::vector<glm::vec3> mTSVers;
    std::vector<GLushort> mTSInds;
    std::vector<Texture2D*> mTextures;
};

#endif /* defined(__LearnCG__Model__) */
