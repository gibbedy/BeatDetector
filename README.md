# BeatDetector
 Beat detection for audio played through a teensy audio board.

 When combined with the teensy audio libraries FFT object it detects low/med/high frequency beats in audio played.
  
 To use, create a BeatDetector object and pass it a pointer to an AudioAnalyzeFFT256 object created using the teensy audio library.
 Once per loop call BeatDetectorLoop().
 To determine if beat was detected just check the beatDector objects public variables lowBeat/midBeat/highBeat 
 
 

  BEAT DETECTION ALGORITHYM:
 * A running average of the magnitude of the fft signal is kept for each low/med/high bin range. 
 * The maximum magnitude of fft signal seen in the last second is updated every second
 * The differnce between average and max audio values is calculated and a beat detection threshold is set between these two values determined by a threshold factor that you can set.
 * If the current audio signal is greater than this threshold then it is a beat.
 * Retriggering of beat detection is locked out for a preset amount of time. (see variables somewhere below)
 * set enableSerialBeatDisplay true and use arduino serial plotter to visualise whats going on.
 * I'm  also experimenting  generating a virtual beat signal. This is generated with bpm timing every time a valid bpm is measured
 * Until a valid bpm is measured, virtualbeat will mirror low beat

 Teensy3.2 + Audo board.
 compile audioAnalyzeDetectBeat.ino with defines for how audio is to be played (line in/mic/sd card playback)
 
 Note:
 I don't think I implemented mid/high or if I did its dodgy.
 Comments that make sense mean that section of code was probably copied/pasted from someone else.
 Comments may not be relevant as I was constantly changing values to get what looked ok.
 This is not an example of good code. I'm a novice.
 
 If you have a teensy3.2 and audio board and an sdcard and a wav file (in correct format) you could play through usb audio and test this without any other hardware.
 
 In arduino IDE open up serial plotter to see what is going on.
 
 https://www.youtube.com/watch?v=E-PomXjd1eQ
 
 
 I believe my inspiration and probably allot of code came from this guy:
 https://github.com/saikoLED/TeensyLED/tree/master/Examples/TeensyLED_Audio_DMX_Master
 so thank you :).
 
