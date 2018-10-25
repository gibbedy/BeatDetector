#include "BeatDetector.h"

BeatDetector::BeatDetector(AudioAnalyzeFFT256 &fft)
{
  this->fft256_1=&fft;
}

bool BeatDetector::BeatDetectorUpdate(float &beatDetected,float &audioValue,float &maxValue,float &total, float * readings,int &readIndex, const int numReadings,float &oldMaxValue, float &thresholdFactor, float &silenceFactor ,elapsedMillis &retrigger, int &retriggerTime)
{
  beatDetected=0.0;
  if(audioValue>maxValue)
    {
      maxValue=audioValue;
    }
       
  // subtract the last reading:
  total = total - readings[readIndex];
  // read value
  readings[readIndex] = audioValue;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
       
    oldMaxValue=.6*oldMaxValue+.4*maxValue;
    oldMaxValue=maxValue;
    maxValue=0;
  }
  
  // calculate the average:
average = total / numReadings;


//what I'm trying to do here is limit the rate of change to the averageSmoothing value.
//This will be effected by rate of fft data as set in setup() fftaverage.
if(oldAverage-average>averageSmoothing)  //only if average calculated is different than old average by a value greater than  smoothing amount do I need to adjust average figure calculated
{
  average = oldAverage-averageSmoothing;
}

if(average-oldAverage>averageSmoothing)
{
  average=oldAverage+averageSmoothing;
}

oldAverage=average;

float    thresholdValue= (oldMaxValue-average)*thresholdFactor+average;       //threshold is set at .8 x the difference between the average rms value and average maximum rms value


  if(enablePlot)
  {

  if(enableSerialBeatDisplay)
  {
    Serial.print(average*100);//x10 for arduino serial plotter.
    Serial.print(",");
    Serial.print(oldMaxValue*100);
    Serial.print(",");
    Serial.print(thresholdValue*100);
    Serial.print(",");
    Serial.print(audioValue*100);
    Serial.print(",");
    
    if(retrigger>retriggerTime)
    {
    Serial.println(1);
    }
    else
    {
      Serial.println(0);
      
    }

  }
  }
 
 /*
//nextion display code... I don't think I'll use this again

  if(enablePlot&&currentPage==1) //only stream variable data when screen is selected as there is a bit of overhead.  
  { 
    //plotCount=0;
    
    if(enablePlot0)
    {
      s0p1.addValue(0, audioValue*480);
    }

    if(enablePlot1)
    {
     s0p1.addValue(1, average*480);
    }

    if(enablePlot2)
    {
      s0p1.addValue(2, thresholdValue*480);
    }

    if(enablePlot3)
    {
      if(retrigger>retriggerTime)
      {
        s0p1.addValue(3, 200);
      }
      else
      {
        s0p1.addValue(3, 0);     
      }
    }
  }
 */ 
  
    if(audioValue>thresholdValue&&audioValue>2*average&&retrigger>retriggerTime) //oldmax/average bit is to detect when there is a section of music without a beat and not detect random beats in whatever is playing.And beat is more than retriggerTime ms away from laste beat detection
    {
    beatDetected=audioValue;
    retrigger=0;
   
    }
    else
    {
      
    }
    return beatDetected;
}

bool BeatDetector::BeatDetectorLoop()
{

  validBPM=false; //reset these so they are only true for one cycle if set true on last loop. 
  lowBeat=0;
  midBeat=0;
  highBeat=0;
  
  fftDataAvailable=false;
  
  if(fft256_1->available())//sizeof(SEND_DATA_STRUCTURE))   //&&peakMsecs>PEAK_MSECS_INTERVAL)  
  {
	fftDataAvailable=true;
    fftCount++;  //increment fftCount to keep track of number of samples per second. Using for debuggig.
    
    //potValue=map(analogRead(POT_PIN),0,1023,0,127);
    //peakMsecs=0;

    
    FFTLowAverageAudioValue=fft256_1->read(0);
    if(serialPlotLow)
    {
      enablePlot=true;
    }
    BeatDetectorUpdate(lowBeat,FFTLowAverageAudioValue,FFTLowAverageMaxValue,FFTLowAverageTotal,FFTLowAverageReadings,FFTLowAverageReadIndex,FFTLowAverageNumReadings,FFTLowAverageOldMaxValue,FFTLowAverageThresholdFactor,FFTLowAverageSilenceFactor,FFTLowAverageRetrigger,FFTLowAverageRetriggerTime);
    enablePlot=false;

    if(serialPlotMid)
    {
      enablePlot=true;
    }
    
  FFTMidAverageAudioValue=fft256_1->read(25,80);
  BeatDetectorUpdate(midBeat,FFTMidAverageAudioValue,FFTMidAverageMaxValue,FFTMidAverageTotal,FFTMidAverageReadings,FFTMidAverageReadIndex,FFTMidAverageNumReadings,FFTMidAverageOldMaxValue,FFTMidAverageThresholdFactor,FFTMidAverageSilenceFactor,FFTMidAverageRetrigger,FFTMidAverageRetriggerTime);
    enablePlot=false;

    if(serialPlotHigh)
    {
      enablePlot=true;
    }
  FFTHighAverageAudioValue=fft256_1->read(89,127);
  BeatDetectorUpdate(highBeat,FFTHighAverageAudioValue,FFTHighAverageMaxValue,FFTHighAverageTotal,FFTHighAverageReadings,FFTHighAverageReadIndex,FFTHighAverageNumReadings,FFTHighAverageOldMaxValue,FFTHighAverageThresholdFactor,FFTHighAverageSilenceFactor,FFTHighAverageRetrigger,FFTHighAverageRetriggerTime);
    enablePlot=false;
    
  if(lowBeat/*||midBeat||highBeat*/)
  {
	  beatCountTime=beatCountTimer;  //store the current beatCount time so reading doesn't include the time it takes to do other code here. probably not significant 
	  beatCountTimer=0;  //reset beat count timer
	  	  
    validBPM=true;
      //Serial.println(beatCountTime);
      for(int i = BPM_BEAT_LIST_LENGTH-1; i>=0; i--) //move beat times in the FILO beatTimes array one position (discarding the oldest beatTime)
      {
        int deltaBeatTime=beatTimes[i]-beatCountTime;
       
        deltaBeatTime=abs(deltaBeatTime);
        if(deltaBeatTime>BEAT_TOLERANCE) 
        {
          validBPM=false;     //have one or more of the beat times was not consisten with latest beat time recieved so don't update bpm
          #ifdef SERIAL_BPM
            Serial.print("beat of:");
            Serial.print(beatCountTime);
            Serial.print(" out of TOLERANCE by : ");
            Serial.print(deltaBeatTime);
            Serial.println(" ms");
          #endif
        }
      }
      if(validBPM)//beatTime just recorded represents a good indication of bpm therefore update BPM and send it
      {
        bpm=60000/beatCountTime;
		virtualBeatTimer=0; //update the virtual beat count timer on a valid beat
		virtualBeatTime=beatCountTime;
      }
      for(int i = BPM_BEAT_LIST_LENGTH-1; i>0; i--) //move beat times in the FILO beatTimes array one position (discarding the oldest beatTime)
      {
        beatTimes[i] = beatTimes[i-1];
      }
      beatTimes[0]=beatCountTime;                 //insert the newest beat time in the array    
    
       
     if(beatCount<beatsRequired) //increment beat counter
     {
       #ifdef SERIAL_BPM
            Serial.print("Not enough beats only : ");
            Serial.print(beatCount);
            Serial.print("/");
            Serial.println(beatsRequired);
          #endif
       beatCount++; 
     }
          
    musicPlayingStatusTime=0; //reset this timer as manual music status update wont be needed as this info is sent along with beat detected message
    //txdata.deviceNumber=1; //number 1 for led teensy
    //txdata.commandNumber=COMMAND::beatDetected;
    
    //commandData will be already set from nextion popcallback function
    //txdata.fftLowBeat=lowBeat;
    if(validBPM)
    {
      //txdata.fftMidBeat=bpm;
      #ifdef SERIAL_BPM
      Serial.print("Valid BPM: ");
      Serial.println(bpm);
      #endif
      
    }
    else
    {
      //txdata.fftMidBeat=0;//if invalid bpm then send 0 so receiving teensy can ignore it
    }
   // txdata.fftHighBeat=highBeat;
    //txdata.rms1=rms1.read();
    //txdata.sendCount=txdata.sendCount+1;
   // txdata.commandData=musicPlaying;
   // ETout.sendData();
    //digitalWrite(13,!digitalRead(13));  //falsh board led every time message is sent   
    }
  
  }

   if(beatCountTimer>noBeatDuration)//no beats detected in last 2 seconds say
    {
      musicPlaying=false;
	  virtualBeatTime=0;	
      beatCount=0;
      bpm=0; //reset bpm reading and all beat times
    }
  else
  {
    if(beatCount==beatsRequired)
    {
      musicPlaying=true;
    }
  }

  if(musicPlaying==false&&musicWasPlaying==true) //detect when music changes from playing to not playing
  {
    musicStopped=true;
  }
  else
  {
    musicStopped=false;
  }

  musicWasPlaying=musicPlaying;
  /*
  Serial.print("beatcount");
  Serial.println(beatCount);
  Serial.print("musicPlaying");
  Serial.println(musicPlaying);
*/

//if there is a valid bpm signal (virtualBeatTime>0) and it is time to set the virtualBeat (virtualBeatTimer>virtualBeatTime) 
//or a new validbpm was just detected (validBPM&&lowBeat) 
//or there has been no valid bpm signal yet but there was a low beat detected this pass then set virtualBeat true
  if(((virtualBeatTimer>virtualBeatTime/*musicPlaying*/&&virtualBeatTime>0) || (validBPM&&lowBeat)||(virtualBeatTime==0&&lowBeat))&& virtualBeatRetriggerTimer>virtualBeatRetriggerTime) //modify this to fix double pulse issue.
  {

	  virtualBeat=true;
	  virtualBeatTimer=0;
	  virtualBeatRetriggerTimer=0;
  }
  else
  {
	    virtualBeat=false;  
  } 

	 return fftDataAvailable;
	 
  
}

