#include "ofApp.h"



//--------------------------------------------------------------
void ofApp::setup()
{
   // ofBackground(0);
    ofSetVerticalSync(false);
    ofSetFrameRate(90);
    
    doControlPanelDraw = true;
    particleSystemManager.camera = &cam;
    particleSystemManager.setup();
    
    controlPanel.particleSystemManager = &particleSystemManager;
    controlPanel.cam = &cam;
    
    
    
    //cam.disableMouseInput();
}

//--------------------------------------------------------------
void ofApp::update()
{
    particleSystemManager.update();
}



//--------------------------------------------------------------
void ofApp::draw()
{
    //particlesController.particleTexture.draw(0, 0);

    cam.begin();
    
    particleSystemManager.draw();
    cam.end();
    
    if(doControlPanelDraw)
    {
        controlPanel.draw();

    }
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'g')
    {
        doControlPanelDraw = !doControlPanelDraw;
    }
    if(key == '1')
    {
        cam.reset();
    }
    if(key == ' ')
    {
        particleSystemManager.nextSystem();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}