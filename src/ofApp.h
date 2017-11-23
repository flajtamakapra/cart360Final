
/*
Libraries used: 
Leo Zimmerman's ofxFilterbank
https://github.com/leozimmerman/ofxFilterbank



*/

#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxFilterbank.h"
#include "ofxOsc.h"
#include "ofSerial.h"
#include <queue>

#define BANDWITH  1.0
#define BUFFER_SIZE 512
#define LIN2dB(x) (double)(20. * log10(x))
#define SR 44100

#define HOST "localhost"
#define PORT 12345


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void audioIn(float * input, int bufferSize, int nChannels);
        void audioOut(float * input, int bufferSize, int nChannels);
        


        void play();
        float vectorAvg(vector<int> notes);
        int vectorRdm(vector<int>  notes);


        ofSerial serial;

        //------- TEST waveform
        void updateWaveform(int waveFormResolution);
        vector<float> waveform;
        float frequency;
        ofMutex waveformMutex;

        //-------

		ofSoundStream soundStreamIn;
		ofSoundStream soundStream1;

		ofxFilterbank filterBank;
		//ofxOscSender sender;

		int	sampleRate;
		int bufferSize;
		int inChan;
		int outChan;

		bool sPressed = false;

		// Stock all notes
		vector <vector<int>> notesInTime;
		queue<vector<int>> global;
		
		

		//------------------------------
		// Sound out
		float 	pan;
		//int		sampleRate;
		bool 	bNoise;
		float 	volume;

		vector <float> lAudio;
		vector <float> rAudio;
		
		//------------------- for the simple sine wave synthesis
		float 	targetFrequency;
		float 	phase;
		vector<float> phases;
		float 	phaseAdder;
		float 	phaseAdderTarget;

	    bool isPlaying = false;
	    int myByte = 0;

};

#endif

