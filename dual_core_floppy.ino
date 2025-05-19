#include <Arduino.h>


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

// Status codes for core2_status
#define STATUS_SETUP 0
#define STATUS_IDLE 1
#define STATUS_OK 2
#define STATUS_ERR_UNRECOGNIZED_COMMAND 3
#define STATUS_RETRY 4
#define STATUS_UNDEFINED 5
volatile int core2_status = STATUS_IDLE;

// Commands
#define CMD_IDLE 0
#define CMD_INDEX_CHECK 1
#define CMD_RECALIBRATE 2
#define CMD_READ 3
#define CMD_WRITE 4
volatile int command = CMD_IDLE;

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
  while(!core2_status != STATUS_IDLE) {
    count += 1;
    Serial.printf("Waiting for core2 to be idle. (core2_status=%d)\n", core2_status);
    delay(500);
    // TODO after some max count, exit to a failsafe state
  }

  Serial.printf("Core 2 status: %d\n", core2_status);
}

void loop() {
  // first core loop.
  // it blocks for user input

  // get a command
  Serial.println("Command? ");
  while( !Serial.available()) {
    // wait for user input
    delay(100);
  }
  String command_str = Serial.readStringUntil('\n');
  if(command_str == "icheck") {
    command = CMD_INDEX_CHECK;
  } else if( command_str == "recal") {
    command = CMD_RECALIBRATE;
  } else if( command_str == "read") {
    command = CMD_READ;
  } else if( command_str == "write") {
    command = CMD_WRITE;
  } else if( command_str == "idle") {
    command = CMD_IDLE;
  } else {
    Serial.println("available commands: icheck, recal, read, write, idle");
    return;
  }

  // report second core status
  Serial.printf("Core 2 status: %d\n", core2_status);
}

void setup1() {
  // second core setup
  core2_status = STATUS_SETUP;

  // this core will handle data streams to and from the floppy, so initialize those pins here
  // TODO initialize the pins to the floppy
  // index pulse pin
  // motor control pin
  // read data pin

  // TODO when do we turn on the motor? and when do we turn it of??

  // signal that core 2 is ready
  core2_ready = true;
  core2_status = STATUS_IDLE;
}

void loop1() {
  // second core loop
  // it is synchronized to the index pulse

  int status = STATUS_UNDEFINED;

  // wait for the index pulse

  // now execute whatever command we had from the last run
  // the commands write whatever data they produce to where ever it needs to go; they don't return it for this loop to write somewhere
  // they do return an error code or a retry counter...

  if( command == CMD_INDEX_CHECK) {
    status = measure_index_pulse(); // see if we get appropriate index pulses
  }
  else if( command == CMD_RECALIBRATE) {
    status = recalibrate_pulse_timings(); // measure a bunch of pulses and set the thresholds for pulse types
  } else if( command == CMD_IDLE) {
    // just do nothing!
    status = STATUS_OK; // can't do nothing any way but right
  } else {
    // unrecognized command. that's an internal error
    status = STATUS_ERR_UNRECOGNIZED_COMMAND;
  }
  // what other commands? reading a sector; reading track marker; writing a sector; writing a track marker

  // we should be done doing whatever we were going to do, so check for a new command from the first core
  // TODO get the latest command from the first core
  if( status == STATUS_RETRY) {
    // don't get a command, actually; just leave it at whatever it was
  } else if( status == STATUS_ERR_UNRECOGNIZED_COMMAND) {
    // TODO how do we shut this whole thing down? tell the first core there's a problem and shutdown all the hardware we control?
  } else {
    command = get_command();
  }

}

int get_command() {
  // TODO: get the next command from the first core
  // For now, just return the command to do nothing specific
  return CMD_IDLE;
}

int measure_index_pulse() {
  // TODO: Replace with actual index pulse measurement logic
  // For now, just return STATUS_OK to avoid compile errors
  return STATUS_OK;
}

int recalibrate_pulse_timings() {
  // TODO: Replace with actual index pulse calibration logic
  // For now, just return STATUS_OK to avoid compile errors
  return STATUS_OK;
}

// naive software impl to read a pulse
// REF: floppy.cafe/mfm.html
// #define SHORT_PERIOD 100
// #define MID_PERIOD 200
// int read_symbol() {
//   int count = 0;
//   // count while the read pin is low
//   while( digitalReadFast(READ_PIN) == LOW) {
//     count += 1;
//   }
//   // and count while the read pin is high
//   while( digitalReadFast(READ_PIN) == HIGH) {
//     count += 1;
//   }

//   if( count < SHORT_PERIOD) {
//     return 0;
//   } else if( count < MID_PERIOD) {
//     return 1;
//   } else {
//     return 3; // "long period"
//   }
// }
