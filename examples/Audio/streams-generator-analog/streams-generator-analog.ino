#include "AudioTools.h"

AudioInfo info(44100, 2, 16);
SineWaveGenerator<int16_t> sineWave(32000);                
GeneratedSoundStream<int16_t> sound(sineWave);             
AnalogAudioStream out; 
StreamCopy copier(out, sound);                             
int pins[] = {PA4, PA5}

// Arduino Setup
void setup(void) {  
  // Open Serial 
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Warning);

  // start the analog output
  auto config = out.defaultConfig(TX_MODE);
  config.setPins(pins);
  config.copyFrom(info); 
  out.begin(config);

  // Setup sine wave
  sineWave.begin(info, N_B4);
  Serial.println("started...");
}

// Arduino loop - copy sound to out 
void loop() {
  copier.copy();
}

