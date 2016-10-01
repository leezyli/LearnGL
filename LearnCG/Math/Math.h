//
//  Math.h
//  LearnCG
//
//  Created by leezy on 16/7/25.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#ifndef __LearnCG__Math__
#define __LearnCG__Math__

#include <stdio.h>
#define GLM_SWIZZLE
#define GLM_FORCE_CXX98
#include <glm/vec2.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/constants.hpp> // glm::pi
#include <glm/gtc/type_ptr.hpp>
#include <glm/geometric.hpp>
#include <Eigen/Eigenvalues>

#include <vector>


namespace Util
{
    namespace Internal
    {
        inline glm::vec3 ToVector3f(Eigen::Vector3f const& v)
        {
            return glm::vec3(v.x(), v.y(), v.z());
        }
        
        inline Eigen::Vector3f ToEigenVector3f(glm::vec3 const& v)
        {
            return Eigen::Vector3f(v.x, v.y, v.z);
        }
    }
    
    namespace Math
    {
        void CalcAxisVector(std::vector<glm::vec3> const& v, Eigen::Vector3f& R, Eigen::Vector3f& S, Eigen::Vector3f& T);
    }
}

#endif /* defined(__LearnCG__Math__) */
