//
//  main.cpp
//  LearnCG
//
//  Created by leezy on 16/4/13.
//  Copyright (c) 2016年 leezy. All rights reserved.
//

#include <iostream>
#include "EffectWater.h"
#include "ShadowMap.h"
#include "ShadowVolume.h"
#include "WavingGrass.h"
#include "ShadowPCF.h"


int main(int argc, const char * argv[]) {
    // insert code here...
    
    ShadowPCF demo;
    if ( demo.Init() ) {
        demo.RunLoop();
    }
    
    return 0;
}
