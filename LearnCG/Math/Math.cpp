//
//  Math.cpp
//  LearnCG
//
//  Created by leezy on 16/7/25.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include "Math.h"
#include <iostream>


void Util::Math::CalcAxisVector(const std::vector<glm::vec3> & v, Eigen::Vector3f &R, Eigen::Vector3f &S, Eigen::Vector3f &T)
{
    Eigen::Vector3f aveVector(0, 0, 0);
    
    // 1. 计算向量平均值
    for (size_t i = 0; i < v.size(); ++i)
    {
        aveVector += Util::Internal::ToEigenVector3f(v[i]);
    }
    aveVector /= v.size();
    
    // 2. 构造协方差矩阵
    float xx = 0, yy = 0, zz = 0;
    float xy = 0, xz = 0, yz = 0;
    
    size_t n = v.size();
    for (size_t i = 0; i < n; ++i) {
        xx += (v[i].x - aveVector.x())*(v[i].x - aveVector.x());
    }
    for (size_t i = 0; i < n; ++i) {
        yy += (v[i].y - aveVector.y())*(v[i].y - aveVector.y());
    }
    for (size_t i = 0; i < n; ++i) {
        zz += (v[i].z - aveVector.z())*(v[i].z - aveVector.z());
    }
    for (size_t i = 0; i < n; ++i) {
        xy += (v[i].x - aveVector.x())*(v[i].y - aveVector.y());
    }
    for (size_t i = 0; i < n; ++i) {
        xz += (v[i].x - aveVector.x())*(v[i].z - aveVector.z());
    }
    for (size_t i = 0; i < n; ++i) {
        yz += (v[i].y - aveVector.y())*(v[i].z - aveVector.z());
    }
    xx /= n; yy /= n; zz /= n;
    xy /= n; yz /= n; xz /= n;
    
    Eigen::Matrix3f cotMat;
    cotMat << xx, xy, xz, xy, yy, yz, xz, yz, zz;
    
    // 3. 求协方差矩阵的特征向量
    Eigen::EigenSolver<Eigen::Matrix3f> solver(cotMat);
    
    Eigen::Vector3f a = solver.eigenvectors().col(0).real();
    Eigen::Vector3f b = solver.eigenvectors().col(1).real();
    Eigen::Vector3f c = solver.eigenvectors().col(2).real();
    float an = a.squaredNorm();
    float bn = b.squaredNorm();
    float cn = c.squaredNorm();
    
    if ( an >= bn ) {
        if ( an >= cn ) {
            if ( bn >= cn ) {
                R = a; S = b; T = c;
            } else {
                R = a; S = c; T = b;
            }
        } else { // cn > an > bn
            R = c; S = a; T = b;
        }
    } else {
        if ( bn >= cn ) {
            if ( an >= cn ) { // bn > an > cn
                R = b; S = a; T = c;
            } else {
                R = b; S = c; T = a;
            }
        } else { // cn > bn > an
            R = c; S = b; T = a;
        }
    }
    
    //std::cout << "eigenvectors:\n" << R << "\n" << S << "\n" << T << std::endl;
}
