/*
 *  GpuParticles.h
 *
 *  Copyright (c) 2013, Neil Mendoza, http://www.neilmendoza.com
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of Neil Mendoza nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */
#pragma once

#include "ofMain.h"

class GpuParticlesListener
{
public:
    virtual void onParticlesUpdate()=0;
    
};

/**
 * For now, only uses RGBA and TEXTURE_RECTANGLE to make usage simpler
 */
class GpuParticles
{
public:
    int FLOATS_PER_TEXEL = 4;
    
    // you don't have to use these but makes
    // code more readable
    enum DataTextureIndex
    {
        POSITION,
        VELOCITY
    };
    
    
    GpuParticlesListener* listener;

    
    ofFbo fbos[2];
    ofVboMesh mesh;
    ofVboMesh quadMesh;
    int currentReadFbo;
    int textureLocation;
    int width, height, numFloats;
    int numDataTextures;
    
    GpuParticles()
    {
        listener = NULL;
        currentReadFbo = 0;
        textureLocation = 0;
        numDataTextures = 2;
    }
    
    void init(int width_, int height_)
    {
        width = width_;
        height = height_;
        numFloats = width * height * FLOATS_PER_TEXEL;
        
        // fbos
        ofFbo::Settings s;
        s.internalformat = GL_RGBA32F_ARB;
        s.textureTarget = GL_TEXTURE_RECTANGLE_ARB;
        s.minFilter = GL_NEAREST;
        s.maxFilter = GL_NEAREST;
        s.wrapModeHorizontal = GL_CLAMP;
        s.wrapModeVertical = GL_CLAMP;
        s.width = width;
        s.height = height;
        s.numColorbuffers = numDataTextures;
        for (unsigned i = 0; i < 2; ++i)
        {
            fbos[i].allocate(s);
        }
        
        // mesh
        mesh.clear();
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                mesh.addVertex(ofVec3f(200.f * x / (float)width - 100.f, 200.f * y / (float)height - 100.f, -500.f));
                mesh.addTexCoord(ofVec2f(x, y));
            }
        }
        mesh.setMode(OF_PRIMITIVE_POINTS);
        
        quadMesh.addVertex(ofVec3f(-1.f, -1.f, 0.f));
        quadMesh.addVertex(ofVec3f(1.f, -1.f, 0.f));
        quadMesh.addVertex(ofVec3f(1.f, 1.f, 0.f));
        quadMesh.addVertex(ofVec3f(-1.f, 1.f, 0.f));
        
        quadMesh.addTexCoord(ofVec2f(0.f, 0.f));
        quadMesh.addTexCoord(ofVec2f(width, 0.f));
        quadMesh.addTexCoord(ofVec2f(width, height));
        quadMesh.addTexCoord(ofVec2f(0.f, height));
        
        quadMesh.addIndex(0);
        quadMesh.addIndex(1);
        quadMesh.addIndex(2);
        quadMesh.addIndex(0);
        quadMesh.addIndex(2);
        quadMesh.addIndex(3);
        
        quadMesh.setMode(OF_PRIMITIVE_TRIANGLES);
        

    }


    void update()
    {
        
        fbos[1 - currentReadFbo].begin(false);
        glPushAttrib(GL_ENABLE_BIT);
        // we set up no camera model and ignore the modelview and projection matrices
        // in the vertex shader, we make a viewport large enought to ensure the shader
        // is executed for each pixel
        glViewport(0, 0, width, height);
        glDisable(GL_BLEND);
        fbos[1 - currentReadFbo].activateAllDrawBuffers();
        listener->onParticlesUpdate();
        glPopAttrib();
        
        fbos[1 - currentReadFbo].end();
        
        currentReadFbo = 1 - currentReadFbo;
    }
    
    
    void setUniforms(ofShader* shader)
    {
        for (unsigned i = 0; i < fbos[currentReadFbo].getNumTextures(); ++i)
        {
            ostringstream oss;
            oss << "particles" << ofToString(i);
            //ofLogVerbose() << "oss.str(): " << oss.str();
            
            shader->setUniformTexture(oss.str().c_str(), fbos[currentReadFbo].getTexture(i), i + textureLocation);
        }
    }
    
    void loadDataTexture(int idx, float* data,
                         int x = 0, int y = 0, int width = 0, int height = 0)
    {
        if (idx < fbos[currentReadFbo].getNumTextures())
        {
            if (!width) width = this->width;
            if (!height) height = this->height;
            fbos[currentReadFbo].getTexture(idx).bind();
            glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, x, y, width, height, GL_RGBA, GL_FLOAT, data);
            fbos[currentReadFbo].getTexture(idx).unbind();
        }
        else ofLogError() << "Trying to load data from array into non-existent buffer.";
    }
    
    void zeroDataTexture(int idx,
                         int x = 0, int y = 0, int width = 0, int height = 0)
    {
        if (!width) width = this->width;
        if (!height) height = this->height;
        float* zeroes = new float[width * height * FLOATS_PER_TEXEL];
        memset(zeroes, 0, sizeof(float) * width * height * FLOATS_PER_TEXEL);
        loadDataTexture(idx, zeroes, x, y, width, height);
        delete[] zeroes;
    }
   
};

