#include <Arduino.h>

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>

#define BOUNCE_WITH_PROMPT_DETECTION
#include <Bounce2.h>

const int c_RECVPIN = 14;
const int c_IRLED = 4;
const int c_LED = 2;
const int c_TRIGGERPIN = 27;

int myShotId = 0xF;
bool alive = true;

IRrecv ir_recv(c_RECVPIN);
IRsend ir_send(c_IRLED);

decode_results results;

Bounce trigger = Bounce(); // Instantiate a Bounce object

int timeLastHit = 0;

void makeShot(); 

void setup() 
{
  ir_send.begin();
  
  Serial.begin(115200);
  ir_recv.enableIRIn();  // Start the receiver 
  delay(50);
  Serial.println();
  Serial.print("IRrecvDemo is now running and waiting for IR message on Pin ");
  Serial.println(c_RECVPIN);
  Serial.print("myShotId = ");
  Serial.print(myShotId);

  trigger.attach (c_TRIGGERPIN,INPUT_PULLUP);
  trigger.interval(25);

  pinMode(c_LED, OUTPUT);
  digitalWrite(c_LED, LOW);
}

void loop() 
{
  if((millis() - timeLastHit) > 1000)
  {
    digitalWrite(c_LED, LOW);
  }
  trigger.update();
  if(trigger.fell())
  {
      makeShot();
  }
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
    timeLastHit = millis();
  }
} 

void makeShot() 
{
  if (alive)
  {
  Serial.println("NEC");
  ir_send.sendNEC(myShotId);
  }
}