#include "AudioTools.h"

AudioInfo info(44100, 2, 16);
SineWaveGenerator<int16_t> sineWave(32000); 
GeneratedSoundStream<int16_t> sound(sineWave);  
PWMAudioOutput pwm;                  
StreamCopy copier(pwm, sound);    
int pins[] = {PA1, PA5};


void setup() {
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Warning);  

  // setup sine wave
  sineWave.begin(info, N_B4);

  // setup PWM output
  auto config = pwm.defaultConfig();
  config.copyFrom(info);
  config.setPins(pins);
  pwm.begin(config);
}

void loop(){
  copier.copy();
}
