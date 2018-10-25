# BeatDetector
poor mans beat detection for audio played through a teensy audio board.

*/ When combined with the teensy audio libraries FFT object it detects low/med/high frequency beats in audio played.
 * 
 * To use create a BeatDetector object passing it a pointer to an AudioAnalyzeFFT256 object created using the teensy audio library 
 * Once per loop call BeatDetectorLoop().
 * To determine if beat was detected just check objects public variables lowBeat/midBeat/highBeat 
 */
 
 /*
 * BEAT DETECTION ALGORITHYM:
 * A running average of the magnitude of the fft signal is kept for each low/med/high bin range. 
 * The maximum magnitude of fft signal seen in the last second is updated every second (or so)
 * The differnce between average and max audio values is calculated and a beat detection threshold is set between these two values determined by a threshold factor that you can set.
 * If the current audio signal is greater than this threshold then it is a beat.
 * Retriggering of beat detection is locked out for a preset amount of time. (see variables somewhere below)
 * set enableSerialBeatDisplay true and use arduino serial plotter to visualise whats going on.
 *
 * I'm  also experimenting  generating a virtual beat signal. This is generated with bpm timing every time a valid bpm is measured
 * Until a valid bpm is measured, virtualbeat will mirror low beat
 * 
 */
 
 Teensy3.2 + Audo board.
 Run 
