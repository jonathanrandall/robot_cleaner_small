
#ifndef HTML_STUFF_H_
#define HTML_STUFF_H_


#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "motor_drive_stuff.h"

typedef struct test_struct  {
  String sliderValue;
  String variable;
  
} test_struct ;

test_struct myData;

//extern uint8_t  motor_speed;
//extern void update_speed();
//extern void robot_stop();
//extern void robot_back();
//extern void robot_fwd();
//extern void robot_right();
//extern void robot_left();
//extern uint8_t robo;


const char* ssid = "ssid";
const char* password = "pwd";

String sliderValue = "000";
String variable;



// setting PWM properties
//const int freq = 5000;
//const int ledChannel = 0;
//const int resolution = 8;

const char* PARAM_INPUT = "value";
const char* PARAM_CMD = "go";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
  <head>
    <title>ESP32-CAM Robot</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body { font-family: Arial; text-align: center; margin:0px auto; padding-top: 30px;}
    .slider { -webkit-appearance: none; margin: 14px; width: 360px; height: 25px; background: #FFD65C;
      outline: none; -webkit-transition: .2s; transition: opacity .2s;}
    .slider::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 35px; height: 35px; background: #003249; cursor: pointer;}
    .slider::-moz-range-thumb { width: 35px; height: 35px; background: #003249; cursor: pointer; } 
      table { margin-left: auto; margin-right: auto; }
      td { padding: 8 px; }
      .button {
        background-color: #2f4468;
        border: none;
        color: white;
        padding: 10px 20px;
        text-align: center;
        text-decoration: none;
        display: inline-block;
        font-size: 18px;
        margin: 6px 3px;
        cursor: pointer;
        -webkit-touch-callout: none;
        -webkit-user-select: none;
        -khtml-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;
        user-select: none;
        -webkit-tap-highlight-color: rgba(0,0,0,0);
      }
      img {  width: auto ;
        max-width: 100%% ;
        height: auto ; 
      }
    </style>
  </head>
  <body>
    <h1>ESP32 Robot DB1</h1>
    <img src="" id="photo" >
  <p><span id="textSliderValue">%SLIDERVALUE%</span></p>
  <p><input type="range" onchange="updateSliderPWM(this)" id="pwmSlider" min="0" max="255" value="%SLIDERVALUE%" step="1" class="slider"></p>
    <table>
      <tr><td colspan="3" align="center"><button class="button" onmousedown="toggleCheckbox('forward');" ontouchstart="toggleCheckbox('forward');" onmouseup="toggleCheckbox('stop');" ontouchend="toggleCheckbox('stop');">Forward</button></td></tr>
      <tr><td align="center"><button class="button" onmousedown="toggleCheckbox('left');" ontouchstart="toggleCheckbox('left');" onmouseup="toggleCheckbox('stop');" ontouchend="toggleCheckbox('stop');">Left</button></td><td align="center"><button class="button" onmousedown="toggleCheckbox('stop');" ontouchstart="toggleCheckbox('stop');">Stop</button></td><td align="center"><button class="button" onmousedown="toggleCheckbox('right');" ontouchstart="toggleCheckbox('right');" onmouseup="toggleCheckbox('stop');" ontouchend="toggleCheckbox('stop');">Right</button></td></tr>
      <tr><td colspan="3" align="center"><button class="button" onmousedown="toggleCheckbox('backward');" ontouchstart="toggleCheckbox('backward');" onmouseup="toggleCheckbox('stop');" ontouchend="toggleCheckbox('stop');">Backward</button></td></tr>                   
    </table>
   <script>
   function toggleCheckbox(x) {
     var xhr = new XMLHttpRequest();
     xhr.open("GET", "/action?go=" + x, true);
     xhr.send();
   }
   window.onload = document.getElementById("photo").src = "http://192.168.1.185" + ":81/stream";
   function updateSliderPWM(element) {
  var sliderValue = document.getElementById("pwmSlider").value;
  document.getElementById("textSliderValue").innerHTML = sliderValue;
  console.log(sliderValue);
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/slider?value="+sliderValue, true);
  xhr.send();
}
  </script>
  </body>
</html>
)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if (var == "SLIDERVALUE"){
    return sliderValue;
  }
  return String();
}

void start_server(){
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/slider?value=<inputMessage>
  server.on("/slider", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    if (request->hasParam(PARAM_INPUT)) {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      myData.sliderValue = inputMessage;
      sliderValue =  myData.sliderValue;
      motor_speed = (uint8_t) sliderValue.toInt(); //ledcWrite(ledChannel, sliderValue.toInt());
      Serial.println(motor_speed);
//      Serial.println("request here3");
//      ledcWrite(motorPWMChannnelA, 100);
      update_speed();
//      Serial.println("request here4");
//      if(!robot_send()) Serial.println("something wrong with robot send speed");
    }
    else {
      inputMessage = "No message sent";
    }
//    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

  // Send a GET request to <ESP_IP>/slider?value=<inputMessage>
  server.on("/action", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    // GET input1 value on <ESP_IP>/slider?value=<inputMessage>
    if (request->hasParam(PARAM_CMD)) {
      inputMessage = request->getParam(PARAM_CMD)->value();
      myData.variable = inputMessage.c_str();
      variable = myData.variable;
      if (variable== "stop"){
        robot_stop();
      }
      if (variable== "backward"){
        robot_back();
        robo = 1;
      }
      if (variable== "forward"){
        robot_fwd();
        robo = 1;
      }
      if (variable== "left"){
//        Serial.println("left");
        robot_left();
        robo = 1;
      }
      if (variable== "right"){
        robot_right();
        robo = 1;
      }
    }
    else {
      inputMessage = "No message sent";
    }
//    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });
  
  // Start server
  server.begin();
}

#endif
