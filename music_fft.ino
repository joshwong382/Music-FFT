#include "arduinoFFT.h"

#define MUSIC_PIN 35
#define LOWPASS_FREQ 100                                // ignore frequencies above this
#define SAMPLE_BATCH_SIZE 512                           // Must be a power of 2
#define MAX_DETECTION_FREQUENCY 40000                   // Maximum frequency that is going to be procesesd
#define SAMPLING_FREQUENCY (MAX_DETECTION_FREQUENCY*2)  // double the max detection frequency
#define SAMPLING_PERIOD_US round(1000000L / SAMPLING_FREQUENCY)

#define TRIGGER_AMPLITUDE 2500                          // ignore amplitude below noise
#define TRIGGER_PIN 32
#define TRIGGER_BREAK_MS 150

unsigned long last_sample_time;
unsigned long last_trigger_time;
double vReal[SAMPLE_BATCH_SIZE];
double vImag[SAMPLE_BATCH_SIZE];
unsigned current_sample;
arduinoFFT FFT = arduinoFFT();

unsigned getFreq(unsigned sample_index) {
  return round(sample_index * 1.0 * SAMPLING_FREQUENCY / SAMPLE_BATCH_SIZE);
}


unsigned getSampleIndex(double frequency) {
  return ceil(frequency * SAMPLE_BATCH_SIZE / SAMPLING_FREQUENCY);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(TRIGGER_PIN, OUTPUT);
  analogReadResolution(12);
  current_sample = 0;
  last_sample_time = micros();
  last_trigger_time = 0;
}

void loop() {

  if (micros() - last_sample_time < SAMPLING_PERIOD_US) return;
  last_sample_time = micros();

  vReal[current_sample] = double(analogRead(MUSIC_PIN));
  vImag[current_sample] = 0;
  current_sample++;

  if (current_sample < SAMPLE_BATCH_SIZE - 1) return;
  current_sample = 0;

  FFT.Windowing(vReal, SAMPLE_BATCH_SIZE, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLE_BATCH_SIZE, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLE_BATCH_SIZE);

  // get peak amplitude
  double peak = FFT.MajorPeak(vReal, getSampleIndex(LOWPASS_FREQ), LOWPASS_FREQ*2);
  double amplitude = vReal[getSampleIndex(peak)];

  // Reset Data
  for (unsigned i=0; i<SAMPLE_BATCH_SIZE; i++) {
    vReal[i] = 0;
    vImag[i] = 0;
  }

  if (amplitude < TRIGGER_AMPLITUDE) {
    digitalWrite(TRIGGER_PIN, LOW);
    return;
  }

  if (micros() - last_trigger_time < TRIGGER_BREAK_MS * 1000 && !digitalRead(TRIGGER_PIN)) return;

  digitalWrite(TRIGGER_PIN, HIGH);
  last_trigger_time = micros();
}