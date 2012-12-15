#include <SPI.h>
#include <Ethernet.h>
#include <HTTPClient.h>
#include <aJSON.h>

// Temperature conversion constants (Steinhart-Hart Equation)
const float a = 0.000546403;
const float b = 0.000195178;
const float c = 0.000000234256;

#define probe 0
long temps[6] = {0};

//  Ethernet Variables
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF };  // MAC address for the ethernet controller.
byte serverIP[] = { 192,168,1,110 };
char* serverHost = "meatgeek.192.168.1.110.xip.io";
#define FEED_URI            "/readings.json"
EthernetClient client;
boolean lastConnected = false;
                                                
void setup()
{
  Serial.begin(9600);
  analogReference(DEFAULT);
  
  Ethernet.begin(mac);
}

void loop() {

  if(temps[5] == 0)
  {
    // add temp to array
    for (int count=0; count<6; count++)
    {
      if (temps[count] == 0)
      {
        // store raw resistance
        temps[count] = analogRead(probe);
        // Serial.print("resistance=");
        // Serial.println(temps[count]);
    
        // stop looping
        count = 6;
      }
    }
  }
  if(temps[5] != 0)
  {
    // average and post temp
    long averageResistance = average(temps);
    float averageTemp = convertToF(averageResistance);
    char averageTempString[7];
    dtostrf(averageTemp, 6, 2, averageTempString);
    
//    Serial.print("averageTemp=");
//    Serial.println(averageTemp);
    
    // clear temps array // todo leaky?
    memset(temps, 0, sizeof(temps));
    
    if(!client.connected() ) {

      Serial.print("sending ");
      HTTPClient client( serverHost, serverIP );

//      couldn't get the json post to work
//      http_client_parameter meatgeek_APIHeader[] = {
//        { "X-MeatGeekApiKey", "77"  },
//        { "Accept", "application/json"},
//        { "Content-type", "application/json"},
//        { NULL, NULL }
//      };
//      aJsonObject *root,*reading;
//      root=aJson.createObject();
//      aJson.addItemToObject(root, "reading", reading = aJson.createObject());
//      aJson.addNumberToObject(reading,"temperature", averageTemp);  
//      char *params = aJson.print(root);
//      FILE* result = client.putURI( FEED_URI, NULL, params, meatgeek_APIHeader );

      http_client_parameter meatgeek_APIHeader[] = {
        { "X-MeatGeekApiKey", "77"  },
        { NULL, NULL }
      };
      
      http_client_parameter parameters[] = {
        { "reading[temperature]", averageTempString},
        { NULL,NULL }
      };
    
      FILE* result = client.postURI( FEED_URI, parameters, "", meatgeek_APIHeader );
    
      int returnCode = client.getLastReturnCode();
      
      if (result!=NULL) {
        client.closeStream(result);  // this is very important -- be sure to close the STREAM
      } 
      else {
        Serial.println("failed to connect");
      }
      
      if (returnCode==200 || returnCode==201) {
        Serial.println("data uploaded");
      } 
      else {
        Serial.print("ERROR: Server returned ");
        Serial.println(returnCode);
      }
    }
    
    
    // store the state of the connection for next time through
    // the loop:
    lastConnected = client.connected();
  }

  // take temp every 10 seconds
  delay(10000);
}

long average(long array[])
{
  long sum = 0;
  int length = sizeof(array);
  for (int count=0;count<length;count++)
  {
    sum += array[count];
  }
  
  return sum / length;
}

// props to matthew.c.fox@gmail.com
// http://foxandsqueal.com/?p=45
float convertToF(long resistance)
{
  resistance = resistance*10000/(1024-resistance); //wtf
  float logcubed=log(resistance);
  logcubed = logcubed * logcubed * logcubed;
  float kelvin = 1.0 / (a + b * log(resistance) + c * (logcubed));
  float temp = (kelvin - 273.15) * 9.0/5.0 + 32.0;
  return temp;
}
