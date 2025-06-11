// Arduino Music Box by dnvr07

#include <SD.h>          // SD card library
#include <SPI.h>         // SPI communication (used by SD card)
#include <TMRpcm.h>      // Audio playback library (speaker_play_tmrpcm)
#define SD_ChipSelectPin 10 // Define SD card CS (Chip Select) pin

#include <ServoTimer2.h>  ///#include <Servo.h> // Use this instead of <Servo.h> 

TMRpcm tmrpcm; // Creates TMRpcm object for audio playback
ServoTimer2 mymotor;
const int IRpin = 6;  // IR sensor connected here
const int LEDpin = 7; // LED connected here

bool systemRunning = false;  // Track if music+servo+led are running

void setup() {
  pinMode(IRpin, INPUT);    // Setup IR sensor pin
  pinMode(LEDpin, OUTPUT);  // Setup LED pin

  Serial.begin(9600);       // Initialize Serial Monitor
  tmrpcm.speakerPin = 9;    

  if (!SD.begin(SD_ChipSelectPin)) { // Initialize SD card
    Serial.println("SD Module Error!");
    while (true); // Makes the program "do nothing" id SD failed to initialized
  }

  Serial.println("SD Module Initializing...");
  tmrpcm.setVolume(5); // Set playback volume (0 to 7, higher = louder)

  mymotor.attach(8); // Control continuous servo motor at pin 8
  mymotor.write(1500); // Send a 1500 µs pulse, this is the neutral (stop) signal for a continuous rotation servo.

  digitalWrite(LEDpin, LOW); // Sets the light off as its default state
}

void loop() {
  int irState = digitalRead(IRpin);

  if (irState == HIGH && !systemRunning) { // If IR is not obstructed == Box opened: start LED + servo + music
    Serial.println("Box opened! Starting system...");
    if (SD.exists("song.wav")) {
      tmrpcm.play("song.wav");
      tmrpcm.loop(true);
    } else {
      Serial.println("SONG NOT FOUND");
    }
    digitalWrite(LEDpin, HIGH); // Turn LED ON
    mymotor.write(1550); // Start servo rotation. Send a 1550 µs pulse, the slowest I came without the servo stopping.
    
    systemRunning = true; // Tags the system state as "running" to enable change-in-state commands to run
  } 


  else if (irState == LOW && systemRunning) { // If IR is obstructed == Box closed: turn off LED + servo + music
    Serial.println("Box closed! Stopping system...");

    digitalWrite(LEDpin, LOW);  // Turn LED OFF
    mymotor.write(1500); // Stop servo
    tmrpcm.stopPlayback();

    systemRunning = false; // Tags the system state as "not running" to enable change-in-state commands to run
  }
  delay(1000); // Action delay to prevent jittering
}
