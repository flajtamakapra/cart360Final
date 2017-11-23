#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	//ofSetVerticalSync(true);
	ofBackground(54, 54, 54);
	ofSetFrameRate(30);

    // Servo move
    serial.listDevices();
    vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
    
    int baud = 9600;
    serial.setup(0, baud);


    // open an outgoing connection to HOST:PORT
    // sender.setup(HOST, PORT);

    int ticksPerBuffer = 8;
    bufferSize = BUFFER_SIZE; //pdBlock-64
    inChan  = 2;
	outChan = 0;
	sampleRate = SR;

    int midiMin = 21;
    int midiMax = 108;
    
    filterBank.setup(bufferSize, midiMin, midiMax, inChan, BANDWITH, sampleRate, 1.0);
    filterBank.setColor(ofColor::orange);

	soundStreamIn.setup(this, outChan, inChan, sampleRate, bufferSize, ticksPerBuffer);	
    soundStreamIn.stop();



    phase = 0;
    updateWaveform(32);
    ofSoundStreamSetup(2,0);

    frequency = 0;
    targetFrequency = frequency;

}


//--------------------------------------------------------------

void ofApp::update(){

    ofScopedLock waveformLock(waveformMutex);
    updateWaveform(ofMap(ofSignedNoise(0.5, 1), -1, 1, 3, 64));
    //frequency = ofMap(ofGetMouseY(), 0, ofGetHeight(), 60, 700, true);
    frequency = ofLerp(frequency, targetFrequency, 0.1);
    
/*    myByte = serial.readByte();
    cout<<"My byte: "<<myByte<<endl;
*/    //if(ofGetKeyPressed()) {
    if(isPlaying) {
        volume = ofLerp(volume, 0.3, 0.2);
    }
    else {
        volume = ofLerp(volume, 0, 0.06);

    }

}

void ofApp::updateWaveform(int waveformResolution) {
    waveform.resize(waveformResolution);
    // "waveformStep" maps a full oscillation of sin() to the size 
    // of the waveform lookup table
    float waveformStep = (M_PI * 2.) / (float) waveform.size();
    
    for(int i = 0; i < waveform.size(); i++) {
        //waveform[i] = sin(i * waveformStep);
        waveform[i] = ofSignedNoise(i * waveformStep, ofGetElapsedTimef());

       
    }
}

//--------------------------------------------------------------
void ofApp::draw(){


    ofSetColor(225);
    ofNoFill();
    
	//Draw FilterBank
	{
        ofPushStyle();
        ofPushMatrix();
        ofTranslate (100,250,0);
        filterBank.draw(800,400);
        ofPopMatrix();
        ofPopStyle();
	}
	ofSetColor(225);

	string reportString =  "Sampling Rate: "+ ofToString(SR) +"\nBuffer size: "+ ofToString(bufferSize);
	ofDrawBitmapString(reportString, 10, 700);

    if(isPlaying){

        if(!global.empty()){
            if(global.front().size() <= 0){

                // if noise, play noise
                targetFrequency = 0;
                global.pop();
            }
            else {
                targetFrequency = ofMap(vectorRdm(global.front()), 0, 100, 0, 500);
                cout<<targetFrequency<<endl;
                global.pop();
                
            }
        }else isPlaying = false;
    }

}

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    
    //Analyze Input Buffer with ofxFilterbank
    filterBank.analyze(input);
    
}

void ofApp::audioOut(float * output, int bufferSize, int nChannels) {
    ofScopedLock waveformLock(waveformMutex);
    
    float sampleRate = 44100;
    float phaseStep = frequency / sampleRate;
    
    for(int i = 0; i < bufferSize * nChannels; i += nChannels) {
        phase += phaseStep;
        int waveformIndex = (int)(phase * waveform.size()) % waveform.size();
        output[i] = waveform[waveformIndex] * volume;
        output[i+1] = waveform[waveformIndex] * volume;

        
        
    }
}

void ofApp::play(){

    isPlaying = true;
   
}

float ofApp::vectorAvg(vector<int> notes){
    float avg;
    for(auto i: notes){
        avg+=notes[i];
    }
    return avg/notes.size();
}
int ofApp::vectorRdm(vector<int> notes){
    
    return notes[ofRandom(0, notes.size())];
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){
    if( key == 's'){
        if(!sPressed){
            soundStreamIn.start();
            sPressed = true;
        }


        vector<int> notes = filterBank.getNotes();
        notesInTime.push_back(notes);
        global.push(notes);

    }
}


//--------------------------------------------------------------
void ofApp::exit(){
    soundStreamIn.stop();
    soundStreamIn.close();
    filterBank.exit();
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key == 's') {
        sPressed = false;
        soundStreamIn.stop();
        ofSleepMillis(2000);
        play();
    }
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

