// Mercedes W211 CANBUS tiprtronic gear shifter
// Michal Michalski, 2020-10-3


#include <SPI.h>
#include "mcp_can.h"

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 10;
const byte inPark = 8;
const byte inReverse = 7;
const byte inNeutral = 6;
const byte inDrive = 5;
const byte gearUp = 9;
const byte gearDown = 10;

MCP_CAN CAN(SPI_CS_PIN); // Set CS pin

unsigned char len = 0;
unsigned char buf[8];
char str[20];
byte currentGearSelection;

int digMinus = 3;
int digPlus = 8;
int dig23 = 4;
int dig25 = 5;
int dig29 = 6;
int dig31 = 7;


void setup()
{
    Serial.begin(115200);
    while (!Serial);

    while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println(" Init CAN BUS Shield again");
        delay(100);
    }
    Serial.println("CAN BUS Shield init ok!");

     /*
     * set mask, set both the mask to 0x3ff
     */
    CAN.init_Mask(0, 0, 0x3ff);                         // there are 2 mask in mcp2515, you need to set both of them
    CAN.init_Mask(1, 0, 0x3ff);

    CAN.init_Filt(0, 0, 0x230);    //230 is the tipronic gearbox ID

    //initialize digital output
    pinMode(dig23, OUTPUT);
    pinMode(dig25, OUTPUT);
    pinMode(dig29, OUTPUT);
    pinMode(dig31, OUTPUT);
    pinMode(digPlus, OUTPUT);
    pinMode(digMinus, OUTPUT);

    //set startup value for digital output PARK
    digitalWrite(dig23, HIGH);
    digitalWrite(dig25, HIGH);
    digitalWrite(dig29, HIGH);
    digitalWrite(dig31, LOW);
    currentGearSelection = 8;

    //Set values for manual override
    digitalWrite(digPlus, LOW);
    digitalWrite(digMinus, LOW);
}


void loop()
{
  //re initilize this everytime (reduces the chance of it getting modified globally).
  byte canMessage;

  //Get the can bus message.
  canMessage = getCanMessage();

  //When the gear lever is in correct postion.
  if(canMessage != 0)
  {
    //Save the last correct position of the gear lever if not tiptronic + or -
    if(canMessage != gearUp || canMessage != gearDown)
    {
       //Save the gear lever positon.
       currentGearSelection = canMessage;

       //Make sure that the we no longer send the gear up/down
       digitalWrite(digPlus, LOW);
       digitalWrite(digMinus, LOW);
    }

    setDigitalOutput(canMessage);
  }
  //when the gear leaver is in no mans land.
  else
  {
    //When the gear lever is in no mans land set last know position.
    setDigitalOutput(currentGearSelection);
  }

    delay(200);
}


//Method to isolate the code that gets the can bus message
byte getCanMessage()
{ 
  byte receivedCanMessage = 0;
  if (CAN_MSGAVAIL == CAN.checkReceive())  // check if get data
  {
        CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf

        unsigned long canId = CAN.getCanId();
        Serial.println("-----------------------------");
        Serial.println(canId, HEX);
        Serial.print("Gear selector:      ");
        Serial.println("Content:   ");
        receivedCanMessage = buf[0];
        Serial.print(receivedCanMessage);
        Serial.println();
  }
  return receivedCanMessage;
}


//Set digital outputs 
void setDigitalOutput(byte gearSelection)
{
  if(gearSelection == inPark){
  Serial.print("PARK");
  digitalWrite(dig23, HIGH);
  digitalWrite(dig25, HIGH);
  digitalWrite(dig29, HIGH);
  digitalWrite(dig31, LOW);
  }
  else if(gearSelection == inNeutral){
  Serial.print("NEUTRAL");
  digitalWrite(dig23, LOW);
  digitalWrite(dig25, HIGH);
  digitalWrite(dig29, HIGH);
  digitalWrite(dig31, HIGH);
  }
  else if(gearSelection == inReverse){
  Serial.print("REVERSE");
  digitalWrite(dig23, HIGH);
  digitalWrite(dig25, LOW);
  digitalWrite(dig29, HIGH);
  digitalWrite(dig31, HIGH);
  }
  else if(gearSelection == inDrive) {
  Serial.print("DRIVE");
  digitalWrite(dig23, LOW);
  digitalWrite(dig25, LOW);
  digitalWrite(dig29, HIGH);
  digitalWrite(dig31, LOW);
  }
  else if(gearSelection == gearUp){
  Serial.print("GEAR UP");
  digitalWrite(digPlus, HIGH);
  }
  else if(gearSelection == gearDown){
  Serial.print("GEAR DOWN");
  digitalWrite(digMinus, HIGH);
  }
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
