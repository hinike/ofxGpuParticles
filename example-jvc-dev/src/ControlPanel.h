#pragma once
#include "ofMain.h"
#include "ofxImGui.h"
#include "ParticlesController.h"

class ControlPanel
{
public:
    
    ofxImGui gui;
    ParticlesController* particlesController;
    
    
    ofEasyCam* cam;
    bool camDisableMouseInput;
    
    bool showParticlesWindow;
    int particlesWidth;
    int particlesHeight;
    ControlPanel()
    {
        showParticlesWindow = true;
        particlesController = NULL;
        camDisableMouseInput = false;
        particlesWidth = 1000;
        particlesHeight = 1000;
        gui.setup();
    }
    
    
    void draw()
    {
        gui.begin();
        if(!ImGui::Begin("Particles", &showParticlesWindow))
        {
            ImGui::End();
        }else
        {
            if(ImGui::SliderInt("particlesWidth", &particlesWidth, 100, 5000))
            {
                particlesController->createParticles(particlesWidth, particlesHeight);
            }
            
            if(ImGui::SliderInt("particlesHeight", &particlesHeight, 100, 5000))
            {
                particlesController->createParticles(particlesWidth, particlesHeight);
            }
            
            
            ImGui::SliderFloat("magnitudeFactor", &particlesController->magnitudeFactor, 0.0f, 1000.0f);
            ImGui::SliderFloat("radiusSquared", &particlesController->radiusSquared, 0.0f, 80000.0f);

            ImGui::SliderFloat("gravity X", &particlesController->gravity.x, -1000.0f, 1000.0f);
            ImGui::SliderFloat("gravity Y", &particlesController->gravity.y, -1000.0f, 1000.0f);
            ImGui::SliderFloat("gravity Z", &particlesController->gravity.z, -1000.0f, 1000.0f);
            
            if(ImGui::Button("Reset Particles"))
            {
                particlesWidth = 1000;
                particlesHeight = 1000;
                particlesController->createParticles(particlesWidth, particlesHeight);

            }
            
            if(ImGui::Button("reset Gravity"))
            {
                particlesController->gravity.set(0, 0, 0);
            }
            
            
            if(ImGui::Button("Toggle Cam Mouse"))
            {
                camDisableMouseInput = !camDisableMouseInput;
                if(camDisableMouseInput)
                {
                    cam->disableMouseInput();
                    cam->reset();
                }else
                {
                    cam->enableMouseInput();
                }
            }
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            ImGui::End();
        }
        gui.end();
        
    }
};

