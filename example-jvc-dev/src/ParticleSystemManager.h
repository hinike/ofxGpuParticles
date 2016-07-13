#pragma once

#include "ofMain.h"
#include "GpuParticleSystem.h"
#include "MyParticleSystem.h"

class ParticleSystemManager
{
public:
    
    
    vector<BaseParticleSystem*>particleSystems;
    BaseParticleSystem* currentSystem;
    int currentIndex;
    ParticleSystemManager()
    {
        currentSystem = NULL;
        currentIndex = 0;
    }
    
    
    
    void setup()
    {

        GpuParticleSystem* particleSystem = new GpuParticleSystem();
        particleSystem->setup();
        particleSystem->createParticles(1000, 1000);
        particleSystems.push_back(particleSystem);
        
        
        MyParticleSystem* myParticleSystem = new MyParticleSystem();
        myParticleSystem->setup();
        myParticleSystem->createParticles(1000, 1000);
        particleSystems.push_back(myParticleSystem);
        
        currentIndex = 0;
        
        currentSystem = particleSystems[currentIndex];
    }
    
    void nextSystem()
    {
        if(currentIndex+1 < particleSystems.size())
        {
            currentIndex++;
        }else
        {
            currentIndex = 0;
        }
        currentSystem = particleSystems[currentIndex];
    }
    
    void update()
    {
        if(!currentSystem) return;
        
        currentSystem->update();
        
    }
    
    
    void draw()
    {
        if(!currentSystem) return;
        currentSystem->draw();
       
    }
    
    
};