#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    ofSetFrameRate(60);
    ofSetCircleResolution(45);
    
    //open eye tribe tracker
    //tet.startServer();
    tet.close();
    tet.open();
    
    vid.loadMovie("video.mp4");
    vid.setLoopState(OF_LOOP_NONE);
    
    replay.loadImage("replay.png");
    track.loadImage("track.png");
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if (!loading){
    
        if (tracking && !calibrated){
            
            calibrate();
        }
        
        else if (!waiting){
            
            play();
        }
    }
    
    // otherwise wait for user to start video
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackground(0);
    
    if (tet.isCalibrating()){
        tet.drawCalibration();
    }
    
    else if (loading){
        
        ofPushStyle();
        
        ofSetColor(255);
        
        float x = ofGetWidth()*0.5-200;
        float y = 200;
        ofDrawBitmapString("Press arrow up or down to select tracker csv file", x,y);
        y += 20;
        
        x += 20;
        for (int i=0; i< dir.size(); i++){
            y += 20;
            
            if (selection == i){
                ofCircle(x - 15,y-5,5);
            }
            ofDrawBitmapString(dir.getPath(i), x,y);
        }
        x -= 20;
        
        y += 40;
        ofDrawBitmapString("and press SPACE to play", x,y);
        y += 20;
        ofDrawBitmapString("Or press L to exit to tracking mode", x,y);
        
        ofPopStyle();
        
    }
        
    else if (waiting) { // if waiting for user to start video
        
        if (tracking) { track.draw(0,0,ofGetWidth(),ofGetHeight()); }
        else { replay.draw(0,0,ofGetWidth(),ofGetHeight()); }
        
    }
    
    else if (tracking) {
        
        if (drawTracker){
            
            // draw video with tracker shader
            tracker.drawCurrent(vid.getTextureReference(), 0,0, ofGetWidth(), ofGetHeight(), drawTrackerDot, recenter);
        }
        else {
            
            // or draw video normally
            vid.draw(0,0,ofGetWidth(),ofGetHeight());
        }
    }
    
    else { // replay
        
        // replay video with tracking shader
        tracker.replay(vid.getTextureReference(), vidFrame, 0,0, ofGetWidth(), ofGetHeight(), drawTrackerDot, recenter, ease);
    }
    
}

//--------------------------------------------------------------
void ofApp::play(){
    
    vid.update();
    vidFrame = vid.getCurrentFrame();
    vidDone = vid.getIsMovieDone();
    
    
    if (vidDone){ // if video is finished
        
        vid.stop();
        vid.firstFrame(); // reset playhead
        
        if (tracking){ // if was tracking
            
            tracker.save(trackCount); // save tracking as csv
            trackCount++; // increment track counter
            ofLogNotice("EyeTracker","tracking finished, waiting for input");
            
        } else { // if was replaying
            
            tracker.reset(); // clear eye tracking data
            ofLogNotice("EyeTracker","replay finished, waiting for input");
            
        }
        
        tracking = !tracking; // switch modes
        waiting = true; // wait for user to restart video
        
    }
    
    
    // if video is not finished
    
    else {
            
        // if video is not playing yet, start video
        if (!vid.isPlaying()) {  vid.play(); }
    
        // or update tracker if tracking
        else if (tracking) {
            tracker.update(tet.getPoint2dAvg(), vidFrame);
        }
        
    }
}

//--------------------------------------------------------------
void ofApp::calibrate(){
    
    if (tet.isConnected()){
        
        if (!calibrated){ // if NOT calibrated
            
            if (!tet.isCalibrating()){ // if NOT calibrating, start calibration
                
                bool calibration = tet.startCalibrationProcess();
                
                if (calibration){
                    ofLogNotice("ofxEyeTribe", "calibration started");
                } else {
                    ofLogError("ofxEyeTribe", "calibration start error");
                }
            }
            
            // if just finished calibrating, set calibrated true
            
            if (tet.isCalibrationSucceed()){ calibrated = true; }
            
        }
        
    } else {
        ofLogError("ofxEyeTribe", "EyeTribe not connected!");
    }
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    switch(key){
            
        case 't':{
            if (tracking){
                drawTracker = !drawTracker;
                string yesno = "";
                if (!drawTracker){
                    yesno = "not ";
                }
                ofLogNotice("EyeTracker","tracker is " + yesno + "drawing");
            }
            break;
        }
            
        case 'd':{
            drawTrackerDot = !drawTrackerDot;
            string yesno = "";
            if (!drawTrackerDot){
                yesno = "not ";
            }
            ofLogNotice("EyeTracker","tracker is " + yesno + "drawing tracker dot");
            break;
        }
            
        case 'r':{
            recenter = !recenter;
            string yesno = "";
            if (!recenter){
                yesno = "not ";
            }
            ofLogNotice("EyeTracker","tracker is " + yesno + "recentering video");
            break;
        }
            
        case 'e':{
            ease = !ease;
            string yesno = "";
            if (!ease){
                yesno = "not ";
            }
            ofLogNotice("EyeTracker","replay tracker is " + yesno + "easing");
            break;
        }
            
        case ' ':{
            if (loading){
                if (selection < dir.size()){
                    tracker.load(dir.getPath(selection));
                    loading = false;
                }
            }
            else if (waiting){
                waiting = false;
                string tr = "tracking ";
                if (!tracking){
                    tr = "replay ";
                }
                ofLogNotice("EyeTracker","video " + tr + "started");
            }
            break;
        }
            
        case 'l':{
            if (loading){
                tracker.reset();
                loading = false;
                tracking = true;
                waiting = true;
            } else {
                if (vid.isPlaying()){
                    vid.stop();
                    vid.firstFrame();
                }
                selection = 0;
                dir = ofDirectory(""); // bin/data
                dir.allowExt("csv");
                dir.listDir();
                tracking = false;
                waiting = true;
                loading = true;
            }
            break;
        }
            
        case OF_KEY_UP:{
            if (loading && (selection > 0)){
                selection--;
            }
            break;
        }
            
        case OF_KEY_DOWN:{
            if (loading && (selection < dir.size()-1)){
                selection++;
            }
            break;
        }
            
        case 'q':{
            //quit
            ofExit();
            break;
        }
            
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

void ofApp::exit(){
    tet.close();
}
