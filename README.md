# Music FFT
This simple Arduino program uses the [ArduinoFFT library](https://github.com/kosme/arduinoFFT) and an audio jack breakout to trigger a pin everytime a bass beat is detected. The trigger then can be used together with the [ESP32-Smart](https://github.com/joshwong382/ESP32-Smart) program.

## Setup
Connect the tip of a standard 3.5" audio jack to `MUSIC_PIN`, default is pin 35.
The trigger then can be accessed using `TRIGGER_PIN`, defualt is pin 32.

## Parameters
`LOWPASS_FREQ`: Maximum bass frequency
`SAMPLE_BATCH_SIZE`: How many samples to collect before processing. Note: The higher this number, the higher the delay.
`TRIGGER_AMPLITUDE`: This parameter adjusts the sensitivity of the trigger.
`TRIGGER_BREAK_MS`: Prevents multiple triggers in this period of time.

## Limitations
Currently, I am using a low-pass filter at 35Hz before audio is even passed into the ESP32. Without a pre-low-pass it may cause a lot more unintended triggers.
