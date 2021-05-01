
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  11
#define SDCARD_SCK_PIN   13

AudioPlaySdWav           playWav;
AudioOutputI2S           audioOutput;
AudioConnection          co1(playWav, 0, audioOutput, 0);
AudioConnection          co2(playWav, 1, audioOutput, 1);
AudioControlSGTL5000     sgtl5000_1;

AudioInputI2S            audioInput;           //xy=105,63
AudioAnalyzePeak     peak_L;
AudioAnalyzePeak     peak_R;
AudioAnalyzeRMS      rms_L;
AudioAnalyzeRMS      rms_R;
AudioConnection ci3(audioInput, 0, rms_L, 0);
AudioConnection ci4(audioInput, 1, rms_R, 0);

bool trigger = false;
int lastTrigger = 0;
int timeout = 1000 * 20;

void setup() {
  Serial.begin(9600);

  AudioMemory(8);

  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
  sgtl5000_1.volume(0.5);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card.");
      delay(1000);
    }
  }
}

void playFile(const char *filename)
{
  Serial.print("Playing file: ");
  Serial.println(filename);

  playWav.play(filename);
}


void loop() {
  delay(50);
  if (rms_L.available() && rms_R.available()) {
    uint8_t leftRMS = rms_L.read() * 100.0;
    uint8_t rightRMS = rms_R.read() * 100.0;
    Serial.print("lRMS: ");
    Serial.println(leftRMS);
    Serial.print("rRMS: ");
    Serial.println(rightRMS);
    if (leftRMS > 10 || rightRMS > 10) {
      trigger = true;
      lastTrigger = millis();
    } else if ((lastTrigger + timeout) < millis()) {
      trigger = false;
    }
    
  }
  if (trigger && !playWav.isPlaying()) playFile("SDTEST1.WAV");
}

