#include <Arduino.h>
#include "DFRobot_RTU.h"
#include "DFRobot_BloodOxygen_S.h"
#include "arduinoFFT.h"
#include <FirebaseESP32.h>
#include <WiFi.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

// Insert your network credentials
#define WIFI_SSID "Project"
#define WIFI_PASSWORD "Project1234"

// Insert Firebase project API Key
#define API_KEY "AIzaSyBYaT_O26FWV5Exz66veFS9d918d0oS46k"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "DUProject55@gmail.com"
#define USER_PASSWORD "DUProject5555"

// Insert RTDB URLefine the RTDB URL
#define DATABASE_URL "https://mobile-kareem-default-rtdb.europe-west1.firebasedatabase.app/"

// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variable to save USER UID
String uid;

// Database main path (to be updated in setup with the user UID)
String databasePath;
// Database child nodes
String ECGPath = "/ECG";
String BPMPath = "/BPM";
String Sp02Path = "/SpO2";
String GSRPath = "/GSR";
String TempPath = "/Temp";
String ApenaPath = "/Apnea";

FirebaseJson json;

// Parent Node (to be updated in every loop)
String parentPath;

// Timer variables (send new readings every three minutes)
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay_T = 5000;

// Variables to store ECG and GSR values and DFRobot Temp and BPM and SpO2
float ECG = 0;
float GSR = 0;
float TemP = 0;
int BPM = 0;
int SpO2 = 0;
bool Apena = false;
// #define I2C_COMMUNICATION  //use I2C for communication, but use the serial port for communication if the line of codes were masked
#ifdef I2C_COMMUNICATION
#define I2C_ADDRESS 0x57
DFRobot_BloodOxygen_S_I2C MAX30102(&Wire, I2C_ADDRESS);
#else
/* ---------------------------------------------------------------------------------------------------------------
 *    board   |             MCU                | Leonardo/Mega2560/M0 |    UNO    | ESP8266 | ESP32 |  microbit  |
 *     VCC    |            3.3V/5V             |        VCC           |    VCC    |   VCC   |  VCC  |     X      |
 *     GND    |              GND               |        GND           |    GND    |   GND   |  GND  |     X      |
 *     RX     |              TX                |     Serial1 TX1      |     5     |   5/D6  |  D2   |     X      |
 *     TX     |              RX                |     Serial1 RX1      |     4     |   4/D7  |  D3   |     X      |
 * ---------------------------------------------------------------------------------------------------------------*/
#if defined(ARDUINO_AVR_UNO) || defined(ESP8266)
SoftwareSerial mySerial(4, 5);
DFRobot_BloodOxygen_S_SoftWareUart MAX30102(&mySerial, 9600);
#else
DFRobot_BloodOxygen_S_HardWareUart MAX30102(&Serial2, 9600);
#endif
#endif

// To enable any of ECG , GSR or FFT. please uncomment the following line (remove those "//")

#define ECG_USE
#define GSR_USE
#define FFT_USE

// ECG_Data Variables and Pins

#if defined(ECG_USE) & !defined(FFT_USE)

#define ECG_DATA_PIN 36 // Setup for ECG Output Pin
#define LODPlus 19      // Setup for leads off detection LO +
#define LODMinus 18     // Setup for leads off detection LO -

#endif

// GSR Variables and Pins

#if defined(GSR_USE)

#define GSR_Data_PIN 39
int senVal = 0;
int GSR_Ave = 0;
long Sum = 0;

#endif

#if defined(FFT_USE)

#define CHANNEL 36

#define LODPlus 19  // Setup for leads off detection LO +
#define LODMinus 18 // Setup for leads off detection LO -

const uint16_t samples = 64;         // This value MUST ALWAYS be a power of 2
const float samplingFrequency = 100; // Hz, must be less than 10000 due to ADC
unsigned int sampling_period_us;
unsigned long microseconds;

float vReal[samples];
float vImag[samples];

/* Create FFT object with weighing factor storage */
ArduinoFFT<float> FFT = ArduinoFFT<float>(vReal, vImag, samples, samplingFrequency, true);

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

void PrintVector(float *vData, uint16_t bufferSize, uint8_t scaleType);

#endif

void setup()
{
  Serial.begin(115200);

  Serial.println("Connected to AP");
  while (!MAX30102.begin())
  {
    Serial.println("init fail!");
    delay(1000);
  }
  Serial.println("init success!");
  Serial.println("start measuring...");
  MAX30102.sensorStartCollect();

#if defined(ECG_USE) || defined(FFT_USE)
  pinMode(LODPlus, INPUT);
  pinMode(LODMinus, INPUT);
#endif

#if defined(FFT_USE)
  sampling_period_us = round(1000000 * (1.0 / samplingFrequency));
#endif

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();

  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "")
  {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  // Update database path
  databasePath = "/Data/";

}

void loop()
{

  if ((SpO2 < 94 && BPM < 50) || (GSR < 1000.0))
    Apena = true;

  else
    Apena = false;
#if defined(ECG_USE) & !defined(FFT_USE)
  if ((digitalRead(LODMinus) == 1) || (digitalRead(LODPlus) == 1))
  {
    Serial.print("ECG_Alert ");
    Serial.println('!');
  }

  else
  {
    // send the value of analog input 0 to serial:
    Serial.print("ECG: ");
    Serial.println(analogRead(ECG_DATA_PIN));
  }

#endif

  for (int i = 0; i < 10; i++) // Average the 10 measurements to remove the glitch
  {
    senVal = analogRead(GSR_Data_PIN);
    Sum += senVal;
  }

  GSR_Ave = Sum / 10;
  GSR = GSR_Ave;
  Sum = 0;

  MAX30102.getHeartbeatSPO2();

#if defined(FFT_USE)

  if ((digitalRead(LODMinus) == 1) || (digitalRead(LODPlus) == 1))
  {
    Serial.print("ECG_Alert ");
    Serial.println('!');
  }

  microseconds = micros();
  for (int i = 0; i < samples; i++)
  {
    vReal[i] = analogRead(CHANNEL);
    vImag[i] = 0;
    while (micros() - microseconds < sampling_period_us)
    {
      // empty loop
    }
    microseconds += sampling_period_us;
  }

  /* Print the results of the sampling according to time */
  Serial.println("Data:");
  PrintVector(vReal, samples, SCL_TIME);
  FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward); /* Weigh data */
  Serial.println("Weighed data:");
  PrintVector(vReal, samples, SCL_TIME);
  FFT.compute(FFTDirection::Forward); /* Compute FFT */
  Serial.println("Computed Real values:");
  PrintVector(vReal, samples, SCL_INDEX);
  Serial.println("Computed Imaginary values:");
  PrintVector(vImag, samples, SCL_INDEX);
  FFT.complexToMagnitude(); /* Compute magnitudes */
  Serial.println("Computed magnitudes:");
  PrintVector(vReal, (samples >> 1), SCL_FREQUENCY);
  float x = FFT.majorPeak();
  ECG = x;
  Serial.println(ECG, 6); // Print out what frequency is the most dominant.

#endif

  // Heart_Rate_Oxygen_Temperature
  SpO2 = MAX30102._sHeartbeatSPO2.SPO2;
  BPM = MAX30102._sHeartbeatSPO2.Heartbeat;
  TemP = MAX30102.getTemperature_C();
  Serial.print("SPO2 is : ");
  Serial.print(SpO2);
  Serial.println("%");
  Serial.print("heart rate is : ");
  Serial.print(BPM);
  Serial.println("Times/min");
  Serial.print("Temperature value of the board is : ");
  Serial.print(TemP);
  Serial.println(" ℃");

  // GSR
  Serial.print("GSR value: ");
  Serial.println(GSR);
  delay(500);
  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay_T || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();

    parentPath = databasePath;

    json.set(TempPath.c_str(), String(TemP));
    json.set(BPMPath.c_str(), String(BPM));
    json.set(Sp02Path.c_str(), String(SpO2));
    json.set(GSRPath.c_str(), String(GSR));
    json.set(ECGPath.c_str(), String(ECG*500));
    json.set(ApenaPath.c_str(), Apena);
    Serial.printf("Set json... %s\n", Firebase.set(fbdo, parentPath.c_str(), json) ? "ok" : fbdo.errorReason().c_str());
  }
  // The sensor updates the data every 4 seconds
  //  delay(4000);
  // Serial.println("stop measuring...");
  // MAX30102.sensorEndCollect();
  /* For C++98 compiler, shipped with Arduino IDE version 1.6.6 or less:

  Telemetry data[TELEMETRY_SIZE] = {
    Telemetry( TEMPERATURE_KEY, 42.2 ),
    Telemetry( HUMIDITY_KEY,    80 ),
  };

  */

  // Uploads new telemetry to ThingsBoard using MQTT.
  // See https://thingsboard.io/docs/reference/mqtt-api/#telemetry-upload-api
  // for more details

  /* For C++98 compiler, shipped with Arduino IDE version 1.6.6 or less:

  Attribute attributes[ATTRIBUTES_SIZE] = {
    Attribute( DEVICE_TYPE_KEY,  SENSOR_VALUE ),
    Attribute( ACTIVE_KEY,       true     ),
  };

  */

  // Publish attribute update to ThingsBoard using MQTT.
  // See https://thingsboard.io/docs/reference/mqtt-api/#publish-attribute-update-to-the-server
  // for more details
}

#if defined(FFT_USE)
void PrintVector(float *vData, uint16_t bufferSize, uint8_t scaleType)
{
  for (uint16_t i = 0; i < bufferSize; i++)
  {
    float abscissa;
    /* Print abscissa value */
    switch (scaleType)
    {
    case SCL_INDEX:
      abscissa = (i * 1.0);
      break;
    case SCL_TIME:
      abscissa = ((i * 1.0) / samplingFrequency);
      break;
    case SCL_FREQUENCY:
      abscissa = ((i * 1.0 * samplingFrequency) / samples);
      break;
    }
    Serial.print(abscissa, 6);
    if (scaleType == SCL_FREQUENCY)
      Serial.print("Hz");
    Serial.print(" ");
    Serial.println(vData[i], 4);
  }
  Serial.println();
}

#endif