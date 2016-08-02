#pragma once

#include "ofMain.h"
#include "GpuParticleSystem.h"
#include "TexturedParticleSystem.h"
#include "ParticleSystem3D.h"
#include "GeoParticleSystem.h"
#include "GeoParticleSystemTextured.h"
#include "GeoParticleSystemVideo.h"
#include "GeoParticleSystemBox.h"
#include "TexturedPerParticleSystem.h"

class ParticleSystemManager
{
public:
    
    
    vector<BaseParticleSystem*>particleSystems;
    BaseParticleSystem* currentSystem;
    int currentIndex;
    ofEasyCam* camera;

    ParticleSystemManager()
    {
        currentSystem = NULL;
        currentIndex = 0;
        camera = NULL;
    }
    
    
    
    void setup()
    {
        
      
        
#if 0      
        GeoParticleSystemVideo* geoParticleSystemVideo = new GeoParticleSystemVideo();
        geoParticleSystemVideo->setup();
        geoParticleSystemVideo->createParticles(geoParticleSystemVideo->videoPlayer.getWidth(),
                                                   geoParticleSystemVideo->videoPlayer.getHeight());
        particleSystems.push_back(geoParticleSystemVideo);
#endif      
        
        
#if 0       
        GeoParticleSystemTextured* geoParticleSystemTextured = new GeoParticleSystemTextured();
        geoParticleSystemTextured->setup();
        geoParticleSystemTextured->createParticles(geoParticleSystemTextured->particleTexture.getWidth(),
                                                   geoParticleSystemTextured->particleTexture.getHeight());
        particleSystems.push_back(geoParticleSystemTextured);

        GpuParticleSystem* particleSystem = new GpuParticleSystem();
        particleSystem->setup();
        particleSystem->createParticles(1000, 1000);
        particleSystems.push_back(particleSystem);
        
        TexturedParticleSystem* texturedParticleSystem = new TexturedParticleSystem();
        texturedParticleSystem->setup();
        texturedParticleSystem->createParticles(texturedParticleSystem->particleTexture.getWidth(), texturedParticleSystem->particleTexture.getHeight());
        particleSystems.push_back(texturedParticleSystem);
        
    
        ParticleSystem3D* particleSystem3d = new ParticleSystem3D();
        particleSystem3d->setup();
        particleSystem3d->createParticles(particleSystem3d->particleTexture.getWidth(), particleSystem3d->particleTexture.getHeight());
        particleSystems.push_back(particleSystem3d);
        
        
        GeoParticleSystem* geoParticleSystem = new GeoParticleSystem();
        geoParticleSystem->setup();
        geoParticleSystem->createParticles(500, 500);
        particleSystems.push_back(geoParticleSystem);

   
        GeoParticleSystemBox* geoParticleSystemBox = new GeoParticleSystemBox();
        geoParticleSystemBox->setup();
        geoParticleSystemBox->camera = camera;
        geoParticleSystemBox->createParticles(geoParticleSystemBox->particleTexture.getWidth(), 
                                              geoParticleSystemBox->particleTexture.getHeight());
        particleSystems.push_back(geoParticleSystemBox);
 #endif

#if 1
        TexturedPerParticleSystem* texturedPerParticleSystem = new TexturedPerParticleSystem();
        texturedPerParticleSystem->setup();
        texturedPerParticleSystem->createParticles(texturedPerParticleSystem->particleTexture.getWidth(),
                                                   texturedPerParticleSystem->particleTexture.getHeight());
        particleSystems.push_back(texturedPerParticleSystem);
#endif
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