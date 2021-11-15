#include <math.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// GPIO pin config (check this)
const int ledPin = 2;
const int warmWhiteLed = 13;
const int coolWhiteLed = 12;
const int PWM_Ch13 = 13;
const int PWM_Ch12 = 12;
const int freq = 5000;
const int res = 8;

static int LEDIndex = 0;
static int offFlag = 0;

// Network credentials
const char* ssid     = "Stream Darkside Imp";
const char* password = "thatyear028";

// NTP client time offsets
const long gmtOffset = -25200;
const int daylightOffset = -3600;

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

// Variables to configure and track timer
volatile int interruptCounter;
int totalInterruptCounter;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
hw_timer_t * timer = NULL;

// Variables of local sunrise and sunset time (in minutes)
const int SUNRISE = 398;
const int SUNSET = 1017;
static int scaledCurrentTime = 0;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

const int coolLEDVal[257] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 
                             32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 
                             64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90, 92, 94, 
                             96, 98, 100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126, 
                             128, 130, 132, 134, 136, 138, 140, 142, 144, 146, 148, 150, 152, 154, 156, 158, 
                             160, 162, 164, 166, 168, 170, 172, 174, 176, 178, 180, 182, 184, 186, 188, 190, 
                             192, 194, 196, 198, 200, 202, 204, 206, 208, 210, 212, 214, 216, 218, 220, 222, 
                             224, 226, 228, 230, 232, 234, 236, 238, 240, 242, 244, 246, 248, 250, 252, 254,
                             254, 252, 250, 248, 246, 244, 242, 240, 238, 236, 234, 232, 230, 228, 226, 224, 
                             222, 220, 218, 216, 214, 212, 210, 208, 206, 204, 202, 200, 198, 196, 194, 192, 
                             190, 188, 186, 184, 182, 180, 178, 176, 174, 172, 170, 168, 166, 164, 162, 160, 
                             158, 156, 154, 152, 150, 148, 146, 144, 142, 140, 138, 136, 134, 132, 130, 128, 
                             126, 124, 122, 120, 118, 116, 114, 112, 110, 108, 106, 104, 102, 100, 98, 96, 
                             94, 92, 90, 88, 86, 84, 82, 80, 78, 76, 74, 72, 70, 68, 66, 64, 
                             62, 60, 58, 56, 54, 52, 50, 48, 46, 44, 42, 40, 38, 36, 34, 32, 
                             30, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10, 8, 6, 4, 2, 0, 0};
                             
const int warmLEDVal[257] = {254, 252, 250, 248, 246, 244, 242, 240, 238, 236, 234, 232, 230, 228, 226, 224, 
                             222, 220, 218, 216, 214, 212, 210, 208, 206, 204, 202, 200, 198, 196, 194, 192, 
                             190, 188, 186, 184, 182, 180, 178, 176, 174, 172, 170, 168, 166, 164, 162, 160, 
                             158, 156, 154, 152, 150, 148, 146, 144, 142, 140, 138, 136, 134, 132, 130, 128, 
                             126, 124, 122, 120, 118, 116, 114, 112, 110, 108, 106, 104, 102, 100, 98, 96, 
                             94, 92, 90, 88, 86, 84, 82, 80, 78, 76, 74, 72, 70, 68, 66, 64, 
                             62, 60, 58, 56, 54, 52, 50, 48, 46, 44, 42, 40, 38, 36, 34, 32, 
                             30, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10, 8, 6, 4, 2, 0, 
                             0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 
                             32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 
                             64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90, 92, 94, 
                             96, 98, 100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126, 
                             128, 130, 132, 134, 136, 138, 140, 142, 144, 146, 148, 150, 152, 154, 156, 158, 
                             160, 162, 164, 166, 168, 170, 172, 174, 176, 178, 180, 182, 184, 186, 188, 190, 
                             192, 194, 196, 198, 200, 202, 204, 206, 208, 210, 212, 214, 216, 218, 220, 222, 
                             224, 226, 228, 230, 232, 234, 236, 238, 240, 242, 244, 246, 248, 250, 252, 254, 0};              

// Function to increment interrupt counter
void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // link LED pins to PWM channels
  pinMode(ledPin, OUTPUT);
  ledcSetup(PWM_Ch13, freq, res);
  ledcSetup(PWM_Ch12, freq, res);
  ledcAttachPin(warmWhiteLed, PWM_Ch13);
  ledcAttachPin(coolWhiteLed, PWM_Ch12);

  // Initialize timer: prescaler
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 100000, true);
  timerAlarmEnable(timer);

  // Connect to WIFI
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize a NTPClient to get time
  timeClient.begin();

  // set GMT +8 -dayLightSavings offset
  timeClient.setTimeOffset(gmtOffset + daylightOffset);
}

void ISR() {
    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);
 
    totalInterruptCounter++;
 
    Serial.print("An interrupt as occurred. Total number: ");
    Serial.println(totalInterruptCounter);

    // The formattedDate format: 2018-05-28T16:00:13Z
    formattedDate = timeClient.getFormattedDate();
    Serial.println(formattedDate);
  
    // Extract date
    int splitT = formattedDate.indexOf("T");
    dayStamp = formattedDate.substring(0, splitT);
    Serial.print("DATE: ");
    Serial.println(dayStamp);
    // Extract time
    timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
    Serial.print("HOUR: ");
    Serial.println(timeStamp);
    // getHours() (from 0 to 23) | getMinutes() (from 0 to 59)

    LEDIndex++;
    if (LEDIndex >= 255) {
      LEDIndex = 0;
      offFlag = 1;
    }
      
    ledcWrite(PWM_Ch13, warmLEDVal[LEDIndex]);
    ledcWrite(PWM_Ch12, coolLEDVal[LEDIndex]);
}

void loop() {
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  if (interruptCounter > 0) {
    ISR();
  }
  if (offFlag) {
    ledcWrite(PWM_Ch13, 0);
    ledcWrite(PWM_Ch12, 0);
    delay(5000);
    offFlag = 0;
  }
}
