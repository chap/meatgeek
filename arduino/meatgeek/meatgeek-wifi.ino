#include <SPI.h>
#include <WiFi.h>
// #include <HTTPClient.h>
// #include <aJSON.h>

char ssid[] = "???";     //  your network SSID (name) 
char pass[] = "???";    // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

// Temperature conversion constants (Steinhart-Hart Equation)
const float a = 0.000546403;
const float b = 0.000195178;
const float c = 0.000000234256;

#define meatProbe 0
#define airProbe 1
long meatTemps[6] = {0};
long airTemps[6] = {0};

//  Ethernet Variables

byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF };  // MAC address for the ethernet controller.
#define FEED_URI  "/readings.json"

boolean production = false;

WiFiClient client;
boolean lastConnected = false;
                                                
void setup()
{
  Serial.begin(9600);
  analogReference(DEFAULT);
  
  // attempt to connect to an open network:
  Serial.print("Attempting to connect to WPA network: ");
  Serial.println(ssid);
  status = WiFi.begin(ssid, pass);

  // if you're not connected, stop here:
  if ( status != WL_CONNECTED) { 
    Serial.println("Couldn't get a wifi connection");
    while(true);
  } 
  // if you are connected :
  else {
      Serial.print("You're connected to the network");
      //printCurrentNet();
      //printWifiData();
  }
}

void loop() {

  if(meatTemps[5] == 0)
  {
    // add temp to array
    for (int count=0; count<6; count++)
    {
      if (meatTemps[count] == 0)
      {
        // store raw resistance
        meatTemps[count] = analogRead(meatProbe);
    
        // stop looping
        count = 6;
      }
    }
  }
  if(airTemps[5] == 0)
  {
    // add temp to array
    for (int count=0; count<6; count++)
    {
      if (airTemps[count] == 0)
      {
        // store raw resistance
        airTemps[count] = analogRead(airProbe);
    
        // stop looping
        count = 6;
      }
    }
  }
  if(meatTemps[5] != 0 && airTemps[5] != 0)
  {
    // average and post temp
    long averageMeatResistance = average(meatTemps);
    float averageMeatTemp = convertToF(averageMeatResistance);
    String averageMeatTempString = floatToString(averageMeatTemp, sizeof(averageMeatTemp));
    
    long averageAirResistance = average(airTemps);
    float averageAirTemp = convertToF(averageAirResistance);
    String averageAirTempString = floatToString(averageAirTemp, sizeof(averageAirTemp));
    
    Serial.print("averageMeatTemp=");
    Serial.println(averageMeatTempString);
    
    Serial.print("averageAirTemp=");
    Serial.println(averageAirTempString);
    
    // clear temps array // todo leaky?
    memset(meatTemps, 0, sizeof(meatTemps));
    memset(airTemps, 0, sizeof(airTemps));
    
    if(!client.connected() ) {
      Serial.print("sending ");
      
      // someday I'll get this working
//      aJsonObject *root,*reading;
//      root=aJson.createObject();
//      aJson.addItemToObject(root, "reading", reading = aJson.createObject());
//      aJson.addNumberToObject(reading,"temperature", averageTemp);  
//      String body = aJson.print(root);
      
      
      postTemperature(averageMeatTempString, averageAirTempString);
    }
    
    // store the state of the connection for next time through
    lastConnected = client.connected();
  }

  // take temp every 10 seconds
  delay((production ? 10000 : 1000));
}

void postTemperature(String meatTemp, String airTemp)
{

  if (client.connect((production ? "meatgeek.herokuapp.com" : "meatgeek.192.168.1.110.xip.io"), 80)) {
    Serial.println("connected");
    client.println("POST /readings HTTP/1.1");
    client.println( production ? "Host: meatgeek.herokuapp.com" : "Host: meatgeek.192.168.1.110.xip.io" );
    client.println("Accept: application/json");
    //    client.println("Content-type: application/json");
    client.println("Authorization: Token token=\"???\"");
    client.print("Content-Length: ");
    // calculate the length of the sensor reading in bytes:
    // 95 bytes for 'readings[][temperature]=&readings[][probe_id]=1&readings[][temperature]=&readings[][probe_id]=2' + number of digits of the data:
    int thisLength = 95 + meatTemp.length() + airTemp.length();
    client.println(thisLength);
    client.println("Connection: close");
    
    client.println();
    client.print("readings[][temperature]=");
    client.print(meatTemp);
    client.print("&readings[][probe_id]=1&readings[][temperature]=");
    client.print(airTemp);
    client.println("&readings[][probe_id]=2");
    
  } else {
    Serial.println("connection failed");
  }
  
  
  // int returnCode = client.getLastReturnCode();
  
  if (client.connected()) {
    client.stop();  // this is very important -- be sure to close the STREAM
  } else {
    Serial.println("failed to connect");
  }
  
//  if (returnCode==200 || returnCode==201) {
//    Serial.println("data uploaded");
//  } 
//  else
//  {
//    Serial.print("ERROR: Server returned ");
//    Serial.println(returnCode);
//  }
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

String floatToString(double number, uint8_t digits) 
{ 
  String resultString = "";
  // Handle negative numbers
  if (number < 0.0)
  {
     resultString += "-";
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;
  
  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  resultString += int_part;

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    resultString += "."; 

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    resultString += toPrint;
    remainder -= toPrint; 
  } 
  return resultString;
}

