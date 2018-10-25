/* 
 *  This example is for teensy3.2 and an audio adapter. 
 *  It can be setup for mic or line in audio.
 *  When  music is played low frequency beats are detected and displayed on LED on GPIO pin 4.
 *  When a valid BPM is detected it is indicated by a short flash on  LED attached to GPIO pin 2.
 *  A virtual beat is generated using the valid bpm and this can be seen on pin 5.
 *  
 *  The more frequent the valid bpm's are detected the less virtual beat will wander away from the actual beat.
 */
 
#include "BeatDetector.h"

//#define PLAY_SD
//#define USE_USB_AUDIO   //If using usb audio as output (so music is played through pc speakers).. Also must set "USB Type" to "Audio" in arduino tools menu before compile/upload
#define USE_LINE_IN       //if using mic comment out
#define VALID_BPM_LED 2  //output pin used to show when a valid rpm is detected
#define LOW_BEAT_LED 4  //output pin used for low beat led
#define VIRTUAL_BEAT_LED 5  //output pin used for virtual beat led
#define LED_PULSE_TIME_MS 20 //time that led stays on for to display beat

// GUItool: begin automatically generated code
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioPlaySdWav           playSdWav1;     //xy=105,636
AudioInputI2S            i2s2;           //xy=107,728
AudioMixer4              mixer1;         //xy=392,749
AudioMixer4              mixer3;         //xy=454,656
AudioMixer4              mixer2;         //xy=473,575
AudioAnalyzeFFT256       fft256_1;       //xy=566,713
AudioOutputI2S           i2s1;           //xy=715,631
AudioConnection          patchCord1(playSdWav1, 0, mixer2, 0);
AudioConnection          patchCord2(playSdWav1, 1, mixer3, 0);
AudioConnection          patchCord3(i2s2, 0, mixer2, 1);
AudioConnection          patchCord4(i2s2, 1, mixer3, 1);
AudioConnection          patchCord5(mixer1, fft256_1);
AudioConnection          patchCord6(mixer3, 0, i2s1, 1);
AudioConnection          patchCord7(mixer3, 0, mixer1, 1);
AudioConnection          patchCord9(mixer2, 0, i2s1, 0);
AudioConnection          patchCord10(mixer2, 0, mixer1, 0);

#ifdef USE_USB_AUDIO
  AudioOutputUSB           usb1;           //xy=722,698
  AudioConnection          patchCord11(mixer2, 0, usb1, 0);
  AudioConnection          patchCord8(mixer3, 0, usb1, 1);
#endif

AudioControlSGTL5000     sgtl5000_1;     //xy=146,846
// GUItool: end automatically generated code

// GUItool: end automatically generated code

#ifdef USE_LINE_IN
 const int myInput = AUDIO_INPUT_LINEIN;
#else
 const int myInput = AUDIO_INPUT_MIC;
#endif


elapsedMillis lowBeatLedTimer=0;
elapsedMillis virtualLedTimer=0;
elapsedMillis validBpmLedTimer=0;

BeatDetector beatDetector(fft256_1);

void setup()
{
  beatDetector.enableSerialBeatDisplay=true; //enable serial output for arduino serial monitor.
  pinMode(VALID_BPM_LED,OUTPUT);
  pinMode(LOW_BEAT_LED,OUTPUT);
  pinMode(VIRTUAL_BEAT_LED,OUTPUT);
  
  //Audio Stuff  
   //set gains of stereo to mono mixer
   //I think it needs to be .5 to prevent clipping

   mixer1.gain(0,0.5);
   mixer1.gain(1,0.5);
   mixer1.gain(2,0);
   mixer1.gain(3,0);
   
   mixer2.gain(0,1);
   mixer2.gain(1,1);
   mixer2.gain(2,0);
   mixer2.gain(3,0);  
   
   mixer3.gain(0,1);
   mixer3.gain(1,1);
   mixer3.gain(2,0);
   mixer3.gain(3,0); 

   fft256_1.averageTogether(3);//I this gives me about 115 samples per second 
   
  AudioMemory(20);
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(0.5);
  SPI.setMOSI(7);
  SPI.setSCK(14);
  #ifdef PLAY_SD
   if (!(SD.begin(10))) {
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
   }
  #endif
  
  delay(1000);
  //end audio stuff
}

void loop()
{
     beatDetector.BeatDetectorLoop();
     
     if(beatDetector.lowBeat) //Beat has been detected
     {
      lowBeatLedTimer=0;                        //zero timer so led will be turned on
     }
         
     if(beatDetector.virtualBeat)             //virtual beat generaget from detected Beats and generating a bpm
     {
      virtualLedTimer=0;                        //zero timer so led will be turned on
     }

     if(beatDetector.validBPM)             //valid bpm detected so virtual beat will be syncronised with this beat
     {
      validBpmLedTimer=0;                        //zero timer so led will be turned on
     }

     if(lowBeatLedTimer<LED_PULSE_TIME_MS)      //Beat has been detected above
     {
      digitalWrite(LOW_BEAT_LED,HIGH);          //turn beat detectioin led on
     }
     else
     {
      digitalWrite(LOW_BEAT_LED,LOW);
     }

          if(virtualLedTimer<LED_PULSE_TIME_MS)      //Beat has been detected above
     {
      digitalWrite(VIRTUAL_BEAT_LED,HIGH);          //turn beat detectioin led on
     }
     else
     {
      digitalWrite(VIRTUAL_BEAT_LED,LOW);
     }

     if(validBpmLedTimer<LED_PULSE_TIME_MS)      //validBPM
     {
      digitalWrite(VALID_BPM_LED,HIGH);          //turn beat detectioin led on
     }
     else
     {
      digitalWrite(VALID_BPM_LED,LOW);
     }
  #ifdef PLAY_SD  
     //play wav file from sd card
     if (playSdWav1.isPlaying() == false) {
      Serial.println("Start playing");
      playSdWav1.play("BOOB.WAV");
      delay(1000); // wait for library to parse WAV info    
    }
  #endif
}





