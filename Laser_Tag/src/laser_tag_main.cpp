#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>

const int c_RECVPIN = 14;
const int c_IRLED = 4;
const int c_LED = 2;
const int c_BUTTONPIN = 27;

int myShotId = 0x0;
bool alive = true;

volatile int interruptCounter = 0;
int numberOfInterrupts = 0;

IRrecv ir_recv(c_RECVPIN);
IRsend ir_send(c_IRLED);

decode_results results;

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

void makeShot(); 
void IRAM_ATTR handleInterrupt();

void setup() {
  ir_send.begin();
  
  Serial.begin(115200);
  ir_recv.enableIRIn();  // Start the receiver 
  delay(50);
  Serial.println();
  Serial.print("IRrecvDemo is now running and waiting for IR message on Pin ");
  Serial.println(c_RECVPIN);
  Serial.print("myShotId = ");
  Serial.print(myShotId);

  pinMode(c_BUTTONPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(c_BUTTONPIN), handleInterrupt, FALLING);

  pinMode(c_LED, OUTPUT);
  digitalWrite(c_LED, LOW);
}

void loop() {
  if (ir_recv.decode(&results))
  {
    if (results.value != myShotId)
    {
    // print() & println() can't handle printing long longs. (uint64_t)
    serialPrintUint64(results.value, HEX);
    Serial.println("");
    digitalWrite(c_LED, HIGH);
    alive = false;
    }
    ir_recv.resume();  // Receive the next value
  }
  if(interruptCounter>0){
 
      portENTER_CRITICAL(&mux);
      interruptCounter--;
      portEXIT_CRITICAL(&mux);
      makeShot();
  }
}

void IRAM_ATTR handleInterrupt() {
  portENTER_CRITICAL_ISR(&mux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&mux);
}

void makeShot() 
{
  if (alive)
  {
  Serial.println("NEC");
  ir_send.sendNEC(myShotId);
  }
}