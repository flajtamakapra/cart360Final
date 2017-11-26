#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	//ofSetVerticalSync(true);
	ofBackground(54, 54, 54);
    ofSetFrameRate(framerate);

    receiver.setup(PORT);

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


    // ------ Maxim
    ofSoundStreamSetup(2,0, this, sampleRate, bufferSize, 2);
    //frequency = 220;
    modulationFrequency = 0;
    modulationIndex = 0;
    modSpeed = 2;


    // Background initialisation
    backGroundNoise.emplace_back(new Key(50, 0.9, 0.6));

    // Memory notes initialisation
    for(int i = 0 ; i < 10 ; i++){
        memoryNoise.emplace_back(new Key(10, 0.2));
    }

    // Live notes mimic initialisation
    for(int i = 0 ; i < 10 ; i++){
        keys.emplace_back(new Key(10, 0.27));
    }

}

//--------------------------------------------------------------
void ofApp::playSequence() {
    isPlaying = true;
}

//--------------------------------------------------------------
void ofApp::update(){


    //***************************************************************
    // VOICES
    // Turning on background noise (breathing)
    backGroundNoise[0]->on();

    // Memory buffer: will be randomly affected with a range of notes
    queue<vector<int>> memoryNotes;

    // If Someone just talked on the mike and
    // If the live voice buffer is not empty,
    // play live notes
    if(isPlaying){

        // Live voice buffer is empty?
        if(!notesInTime.empty()){

            // Clear the current frame live noise
            for (int i = 0 ; i < 10 ; i++){
               keys[i]->freq(0);
            }

            // Attribuate all the notes recorded and play them
            for(int i = 0 ; i < 10 ; i++){
                if(notesInTime.front().size() == 0) {
                    int n = ofMap(ofSignedNoise(1), -1, 1, 0, 100);
                    keys[i]->freq(n);
                    keys[i]->release(ofRandom(100, 2000));
                    keys[i]->on();
                }else {

                    int n = ofMap(notesInTime.front()[i], 21, 108, 0, 1000 );
                    keys[i]->freq(n);
                    keys[i]->release(ofRandom(100, 2000));
                    keys[i]->on();
                }
            }

            // Pop the current frame buffer
            notesInTime.pop();
        }
        // If live buffer is empty, stop live playing and start memory playback
        else {
            isPlaying = false;
        }
    }
    // If mike is off, turn all keys off
    else {
        for (int i = 0 ; i < keys.size() ; i++){
           keys[i]->off();
        }
        // If global memory contains... memories AND nobody is speaking
        // 1 - Determine a random range in the memory vector
        // 2 - Copy this range in memory buffer queue (memoryNotes)
        // 3 - Play the memory notes range.
        if(!global.empty() && !sPressed){
            unsigned int ref = ofRandom(0, 10); // ESSAYER AVEC OFSIGNEDNOISE()
            unsigned int rd = ofRandom(0,10);
            if (ref == rd) {
                unsigned int indexA = ofRandom(0, global.size());
                unsigned int indexBAdd = ofRandom(15, 100);
                unsigned int indexB;
                if(indexA + indexBAdd > global.size()){
                    indexB = global.size()-1;
                }
                else indexB = indexA + indexBAdd;
                cout<<indexA<<" "<<indexB<<" "<<global.size()<<endl;

                for(int i = indexA ; i < indexB ; i++){
                    memoryNotes.push(global[i]);
                }
            }
            // Play the notes
            if(!memoryNotes.empty()){
                // Attribuate all the notes recorded and play them
                for(int i = 0 ; i < 10 ; i++){

                    if(memoryNotes.front().size() == 0){
                        int n = ofMap(ofSignedNoise(1), -1, 1, 0, 1000 );
                        memoryNoise[i]->freq(n);
                        memoryNoise[i]->release(ofRandom(100, 2000));
                        memoryNoise[i]->on();
                    }
                    else {
                        int n = ofMap(memoryNotes.front()[i], 21, 108, 0, 1000 );
                        memoryNoise[i]->freq(n);
                        memoryNoise[i]->release(ofRandom(100, 2000));
                        memoryNoise[i]->on();

                    }

                }
                memoryNotes.pop();

            }
            // Turn off all memoryNoise keys after playing them
            for (int i = 0 ; i < memoryNoise.size() ; i++){
               memoryNoise[i]->off();
            }
        }
    }
    // END VOICES
    //*********************************************************************

    //*********************************************************************
    // OSC receive: button pressed on the eyes aparatus (rPie and Cam face detection)

    if(receiver.hasWaitingMessages() || sPressed){
        ofxOscMessage m;
        receiver.getNextMessage(m);
        if(m.getAddress() == "/buttonOn" || sPressed){
            cout<<"testOn"<<endl;
            if(!sPressed) {
                sPressed = true;
                soundStreamIn.start();
            }
            int n;

            vector<int> notes = filterBank.getNotes();
            backGroundNoise[0]->vol(0.1);
            cout<<notes.size()<<endl;

            //if(notes.size()>0){
                notesInTime.push(notes);
                global.push_back(notes);

            //}
        }
        if(m.getAddress() == "/buttonOff" && sPressed){
            sPressed = false;
            backGroundNoise[0]->vol(0.5);
            ofSleepMillis(1000);
            soundStreamIn.stop();
            playSequence();
        }
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

}



//--------------------------------------------------------------
// avg2 from stackOverflow
// https://stackoverflow.com/questions/35833360/get-the-average-value-from-a-vector-of-integers
double ofApp::avg2(std::vector<int> const& v) {
    int n = 0;
    double mean = 0.0;
    for (auto x : v) {
        double delta = x - mean;
        mean += delta/++n;
    }
    return mean;
}
int ofApp::vectorRdm(vector<int> notes){
    
    return notes[ofRandom(0, notes.size())];
}
int ofApp::vectorPlusGros(vector<int> notes) {

    int r = 0;
    for (int i = 0 ; i < notes.size() ; i++ ) {
        if(notes[i] > r) r = notes[i];
    }
    return r;

}

void ofApp::shrinkNotes() {

}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){
    if( key == 's'){
        if(!sPressed){
            soundStreamIn.start();
            sPressed = true;
        }
        int n;

        vector<int> notes = filterBank.getNotes();
        backGroundNoise[0]->vol(0.1);

        if(notes.size()>0){
            notesInTime.push(notes);
            global.push_back(notes);

        }

    }
    if( key == 'e'){

        keys[0]->freq(500);
        keys[0]->on();
//        keys[1]->on();
//        keys[2]->on();
//        keys[3]->on();
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
        backGroundNoise[0]->vol(0.5);
        ofSleepMillis(1000);
        soundStreamIn.stop();
        //shrinkNotes();
        playSequence();
    }

    if( key == 'e'){
        keys[0]->off();
        keys[1]->off();
        keys[2]->off();
        keys[3]->off();
    }
}

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){

    //Analyze Input Buffer with ofxFilterbank
    filterBank.analyze(input);

}
//--------------------------------------------------------------

void ofApp::audioOut(float * output, int bufferSize, int nChannels) {
    for(unsigned i = 0 ; i < bufferSize ; i++) {

        currentFrame = backGroundNoise[0]->play(1);
            if(isPlaying){
                for(int j = 0 ; j < keys.size() ; j++){
                      currentFrame += keys[j]->play(0) / (keys.size()+1);
                }
            }else {
                for(int j = 0 ; j < memoryNoise.size() ; j++){
                      currentFrame += memoryNoise[j]->play(0) / (keys.size()+1);
                }
            }


        mix.stereo(currentFrame, outputs, 0.5);
        output[i * nChannels] = outputs[0] * 0.8;
        output[i * nChannels +1] = outputs[1] * 0.8;

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

