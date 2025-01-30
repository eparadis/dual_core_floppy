// ed paradis 2025
// hardware details
// board: Raspberry Pi Pico 2
// wiring:
#define DENSITY_PIN 2 // IDC 2
#define INDEX_PIN 3   // IDC 8
#define SELECT_PIN 4  // IDC 12
#define MOTOR_PIN 5   // IDC 16
#define DIR_PIN 6     // IDC 18
#define STEP_PIN 7    // IDC 20
#define WRDATA_PIN 8  // IDC 22 (not used during read)
#define WRGATE_PIN 9  // IDC 24 (not used during read)
#define TRK0_PIN 10   // IDC 26
#define PROT_PIN 11   // IDC 28
#define READ_PIN 12   // IDC 30
#define SIDE_PIN 13   // IDC 32
#define READY_PIN 14  // IDC 34
// documentation:
//   multicore processing with the Pico Arduino core:
//   https://arduino-pico.readthedocs.io/en/latest/multicore.html

volatile bool core2_ready = false;

void setup() {
  // first core setup
  // the first core handles USB, and therefore our serial communications channel
  // a such, it will be the "lead" core
  
  // initialize serial
  Serial.begin(115200);
  while( !Serial) {
    delay(50);
  }

  // set up any pins the first core will use
  pinMode(LED_BUILTIN, OUTPUT);
  // TODO set up floppy interface pins that the first core will use
  
  // announce we're ready
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Core 1 ready");

  // wait for core 2 to be ready
  int count = 0;
  while(!core2_ready) {
    count += 1;
    Serial.printf("Waiting for core2... %d\n", count);
    delay(500);
    // TODO after some max count, exit to a failsafe state
  }
}

void loop() {
  // first core loop

  // print our status
  Serial.printf("Status: (none yet...)\n");

  // throttle this loop
  delay(500); // TODO it would be better to look at millis() or use a timer to keep this loop more regular
}

void setup1() {
  // second core setup

  // this core will handle data streams to and from the floppy, so initialize those pins here
  // TODO initialize the pins to the floppy
  // index pulse pin
  // motor control pin
  // read data pin

  // turn on the motor and check to see if we get an index pulse
  // TODO index pulse motor check

  // signal that core 2 is ready
  core2_ready = true;
}

void loop1() {
  // second core loop
  // it is synchronized to the index pulse

  // wait for the index pulse

  // now execute whatever command we had from the last run
  // the command write whatever data the produce to whever it needs to go; they don't return it for this loop to write somewhere
  // though I suppose they could return an error code or a retry counter...

  // was the command to recalibrate the pulses lengths? then do that
  // TODO if( command == CMD_RECALIBRATE) 
  // was the command to ... do something else? well then do whatever that thing is

  // we should be done doing whatever we were going to do, so check for a new command from the first core
  // TODO get the latest command from the first core

  
  


}

// naive software impl to read a pulse
// REF: floppy.cafe/mfm.html
int read_symbol() {
  int count = 0;
  // count while the read pin is low
  while( digitalReadFast(READ_PIN) == LOW) {
    count += 1;
  }
  // and count while the read pin is high
  while( digitalReadFast(READ_PIN) == HIGH) {
    count += 1;
  }

  if( counter < SHORT_PERIOD) {
    return 0;
  } else if( counter < MID_PERIOD) {
    return 1;
  } else {
    return 3; // "long period"
  }
}
