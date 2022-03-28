/*
  Titre      : DS18
  Auteur     : Duvalier Tsagmo
  Date       : 22/03/2022
  Description: Une description du programme
  Version    : 0.0.1
*/

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "WIFI_NINA_Connector.h"
#include "MQTTConnector.h"

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 9 // Lower resolution

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

String adresseCapteur; // variable pour contenir l'adresse d'un capteur
String adresseMacCapteur; // variable pour contenir l'adresse d'un capteur

byte MacAdressCapteur[6];


int numberOfDevices; // Number of temperature devices found
float tempC= 0; // Variable globale pour la température
DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address
void printAddress(DeviceAddress deviceAddress);
void setup(void)
{
  // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");

    wifiConnect();
	MQTTConnect();

  // Start up the library
  sensors.begin();
  
  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();
  
  // locate devices on the bus
  Serial.print("Locating devices...");
  
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");
  
  // Loop through each device, print out address
  for(int i=0;i<numberOfDevices; i++)
  {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i))
	{
		Serial.print("Found device ");
		Serial.print(i, DEC);
		Serial.print(" with address: ");
		printAddress(tempDeviceAddress);
		Serial.println();
		
		Serial.print("Setting resolution to ");
		Serial.println(TEMPERATURE_PRECISION, DEC);
		
		// set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
		sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
		
		Serial.print("Resolution actually set to: ");
		Serial.print(sensors.getResolution(tempDeviceAddress), DEC); 
		Serial.println();
	}else{
		Serial.print("Found ghost device at ");
		Serial.print(i, DEC);
		Serial.print(" but could not detect address. Check power and cabling");
	}
  }
  
// Get address Mac
	WiFi.macAddress(MacAdressCapteur);
    adresseMacCapteur = "";
    // Création de la chaine et envoi de l'adresse MAC sur T.B
	for (int i = 0; i < 5; i++)
	{
		adresseMacCapteur = adresseMacCapteur + String(MacAdressCapteur[i], HEX);
		
	}
    // Envoi de l'adresse mac sur TB
	appendPayloadMac("adresseMacCapteur" ,adresseMacCapteur);
	sendPayload();

}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  // method 1 - slower
  //Serial.print("Temp C: ");
  //Serial.print(sensors.getTempC(deviceAddress));
  //Serial.print(" Temp F: ");
  //Serial.print(sensors.getTempF(deviceAddress)); // Makes a second call to getTempC and then converts to Fahrenheit

  // method 2 - faster
  float tempC = sensors.getTempC(deviceAddress);
  if(tempC == DEVICE_DISCONNECTED_C) 
  {
    Serial.println("Error: Could not read temperature data");
    return;
  }
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print(" Temp F: ");
  Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit
}

void loop(void)
{ 
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  
  
  // Loop through each device, print out temperature data
  for(int i=0;i<numberOfDevices; i++)
  {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i))
	{
		// Output the device ID
		Serial.print("Temperature for device: ");
		Serial.println(i,DEC);
		tempC = sensors.getTempC(tempDeviceAddress);
		// It responds almost immediately. Let's print out the data
		// printTemperature(tempDeviceAddress); // Use a simple function to print out the data

        // Envoi des données sur TB
        printAddress(tempDeviceAddress);
		appendPayload(adresseCapteur, tempC);
		sendPayload();

    } 
	//else ghost device! Check your power requirements and cabling
  }
  delay(1000);
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
    adresseCapteur = "";
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    adresseCapteur = adresseCapteur + String(deviceAddress[i], HEX);
  }
}
/*

void setup(void)
{
    Serial.begin(9600);
    wifiConnect(); // Branchement au réseau WIFI
    MQTTConnect(); // Branchement au broker MQTT
    sensors.begin();
}


void loop(void)
{
    sensors.requestTemperatures();

    Serial.print("Capteur 1: ");
    printTemperature(sensor1);

    idCapteur1 = sensor1[7];
    idCapteur2 = sensor2[7];

    Serial.print("Capteur 2: ");
    printTemperature(sensor2);

    Serial.println();

    appendPayload(idCapteur1, sensors.getTempC(sensor1));
    appendPayload(idCapteur2, sensors.getTempC(sensor2));
    sendPayload();

    delay(1000);
}

*/