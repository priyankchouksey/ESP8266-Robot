#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

AsyncWebServer  web(80); // Web Server
AsyncWebSocket  ws("/ws"); //Websocket

String program;
uint32_t fw_time = 3000;
uint32_t turn_time = 1000;
uint32_t runFor = 0;
uint32_t timeStart = 0;

int currentMode = 0;
int currentProgIndex = 0;

const int M1_P = 0;
const int M1_N = 2;

const int M2_P = 4;
const int M2_N = 5;

const int M3_P = 12;
const int M3_N = 14;

const int M4_P = 13;
const int M4_N = 15;

void setup() {
  Serial.begin(115200);
  SPIFFS.begin();
  WiFi.mode(WIFI_AP);
  String ssid = "ESPRobot";
  WiFi.softAP(ssid.c_str());
  initWeb();
  
  pinMode(M1_P, OUTPUT); // M1 +
  pinMode(M1_N, OUTPUT); // M1 -
  
  pinMode(M2_P, OUTPUT); // M1 +
  pinMode(M2_N, OUTPUT); // M1 -

  pinMode(M3_P, OUTPUT); // M1 +
  pinMode(M3_N, OUTPUT); // M1 -

  pinMode(M4_P, OUTPUT); // M1 +
  pinMode(M4_N, OUTPUT); // M1 -
}

void loop() {
  // put your main code here, to run repeatedly:
  switch (currentMode)
  {
    case 0:{ /* Stopped */
      /* code */
//      motor_stop();
      program = "";
      break;
    }
    case 1:{ /* Play Mode */
      /* do nothing, the handler already did this. */
      break;
    }
    case 2:{ /* Program Mode */
      /* code */
      char command;
      if(timeStart == 0) { /* is first execution */
        timeStart = millis();
        currentProgIndex = 0;
        command = program.charAt(currentProgIndex);
        processProgram(command);
      } else if ((millis() - timeStart) >= runFor){
        currentProgIndex++;
        if (currentProgIndex >= program.length()){
          currentMode = 0;
          motor_stop();
        } else {
          command = program.charAt(currentProgIndex);
          timeStart = millis();
          processProgram(command);
        }
      }
      break;
    }
  }

}

void initWeb() {
  ws.onEvent(wsEvent);
  web.addHandler(&ws);
  DefaultHeaders::Instance().addHeader(F("Access-Control-Allow-Origin"), "*");
  web.serveStatic("/", SPIFFS, "/www/").setDefaultFile("index.html");
  web.begin();
}

void wsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
        AwsEventType type, void * arg, uint8_t *data, size_t len) {
          char* temp = reinterpret_cast<char*>(data);
          Serial.println(temp);
      char command;

  switch (type) {
    case WS_EVT_DATA: {
        DynamicJsonBuffer jsonBuffer;
        JsonObject &json = jsonBuffer.parseObject(reinterpret_cast<char*>(data));
        currentMode = json["mode"].as<int>();
        switch (currentMode)
        {
          case 0 /* Stop current operation */:{
            /* code */
            motor_stop();
           break;
          }
          case 1 /* is Play Mode */: {
            /* code */
            command = json["command"];
            processProgram(command);
            break;
          }
          case 2 /* Is Program Mode */:{
            /* code */
            program = json["program"].as<String>();
            break;
          }
        }
        uint16_t val = json["value"].as<int>();
      break;
    }
    case WS_EVT_CONNECT:
      Serial.print(F("* WS Connect - "));
      Serial.println(client->id());
      break;
    case WS_EVT_DISCONNECT:
      Serial.print(F("* WS Disconnect - "));
      Serial.println(client->id());
      break;
    case WS_EVT_PONG:
      Serial.println(F("* WS PONG *"));
      break;
    case WS_EVT_ERROR:
      Serial.println(F("** WS ERROR **"));
      break;
  }
}

void motor_forward(){
  Serial.println("Moving forward");
    digitalWrite(M1_P, 1);
    digitalWrite(M1_N, 0);
    digitalWrite(M2_P, 1);
    digitalWrite(M2_N, 0);
  }
void motor_stop(){
  Serial.println("Stopped");
    digitalWrite(M1_P, 0);
    digitalWrite(M1_N, 0);
    digitalWrite(M2_P, 0);
    digitalWrite(M2_N, 0);
  }
void motor_back(){
  Serial.println("Moving Back");
    digitalWrite(M1_P, 0);
    digitalWrite(M1_N, 1);
    digitalWrite(M2_P, 0);
    digitalWrite(M2_N, 1);
  }

void motor_left(){
  Serial.println("Moving Left");
    digitalWrite(M1_P, 0);
    digitalWrite(M1_N, 1);
    digitalWrite(M2_P, 1);
    digitalWrite(M2_N, 0);
  }
void motor_right(){
  Serial.println("Moving right");
    digitalWrite(M1_P, 1);
    digitalWrite(M1_N, 0);
    digitalWrite(M2_P, 0);
    digitalWrite(M2_N, 1);
  }

void processProgram(char command){
  if (command == 'F'){
    runFor = fw_time;
    motor_forward();
  } else if (command == 'B'){
    runFor = fw_time;
    motor_back();
  } else if (command == 'R'){
    runFor = turn_time;
    motor_right();
  } else if (command == 'L'){
    runFor = turn_time;
    motor_left();
  }
}
