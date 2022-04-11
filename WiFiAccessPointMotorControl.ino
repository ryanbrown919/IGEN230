/*
  WiFiAccessPoint.ino creates a WiFi access point and provides a web server on it.

  Steps:
  1. Connect to the access point "yourAp"
  2. Point your web browser to http://192.168.4.1/H to turn the LED on or http://192.168.4.1/L to turn it off
     OR
     Run raw TCP "GET /H" and "GET /L" on PuTTY terminal with 192.168.4.1 as IP address and 80 as port

  Created for arduino-esp32 on 04 July, 2018
  by Elochukwu Ifediora (fedy0)
*/

#include <WiFi.h>
#include <WiFiAP.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define LED_BUILTIN 2   // Set the GPIO pin where you connected your test LED or comment this line out if your dev board has a built-in LED

const char* ssid = "ESPMONKE";
const char* password = "monkemode";
const int xaxis = 33;
const int CCW = 16;
const int CW = 17;
const int AccPower = 13;


const char* PARAM_INPUT_1 = "output";
const char* PARAM_INPUT_2 = "state";



// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
    <title>ESP Web Server</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    
    <style>
        html {
            font-family: Arial;
            display: inline-block;
            text-align: center;
        }

        h2 {
            font-size: 3.0rem;
        }
        h3 {
            font-size: 2.0rem;
        }
        p {
            font-size: 3.0rem;
        }
        body {
            max-width: device-width;
            margin: 0px auto;
            padding-bottom: 25px;
            display: inline-block;
            align-content: center;
        }
        html {
    font-family: Arial;
    display: inline-block;
    text-align: center;
}



.switch {
    position: relative;
    display: inline-block;
    width: 120px;
    height: 68px
}

@media only screen and (min-device-width: 1024px) {

    #openswitches {
        float: left;
        padding: 10px 30px 30px 30px;
        background-color: white;
        height: 400px;
        border-radius: 10px;
        border: 3px solid darkgray;
        margin-left: 5px;
    }

    #directbuttons {
        float: left;
        padding: 10px 30px 30px 30px;
        background-color: white;
        width: 300px;
        height: 400px;
        margin-left: 5px;
    }

    #closingswitches {
        float: left;
        padding: 10px 30px 30px 30px;
        height: 400px;
        border-radius: 10px;
        border: 3px solid darkgray;
        margin-left: 5px;
    }
}

@media only screen and (max-device-width: 479px) {
    #openswitches {
        align-content: center;
        padding: 10px 30px 30px 30px;
        background-color: white;
        height: 400px;
        border-radius: 10px;
        border: 3px solid darkgray;
        margin-left: 5px;
    }

    #directbuttons {
        float: left;
        padding: 10px 30px 30px 30px;
        background-color: white;
        width: 300px;
        height: 400px;
        margin-left: 90px;
    }

    #closingswitches {
        float: left;
        padding: 10px 30px 30px 30px;
        height: 400px;
        border-radius: 10px;
        border: 3px solid darkgray;
        margin-left: 5px;
    }
}

.switch input {
    display: none
}

.slider {
    position: absolute;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background-color: #ccc;
    border-radius: 6px
}

    .slider:before {
        position: absolute;
        content: "";
        height: 52px;
        width: 52px;
        left: 8px;
        bottom: 8px;
        background-color: #fff;
        -webkit-transition: .4s;
        transition: .4s;
        border-radius: 3px
    }

input:checked + .slider {
    background-color: #b30000
}

    input:checked + .slider:before {
        -webkit-transform: translateX(52px);
        -ms-transform: translateX(52px);
        transform: translateX(52px)
    }

.container {
    display: block;
    align-content: center;
    position: relative;
    padding-left: 35px;
    margin-bottom: 12px;
    cursor: pointer;
    font-size: 22px;
    -webkit-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
}

    /* Hide the browser's default checkbox */
    .container input {
        position: absolute;
        opacity: 0;
        cursor: pointer;
        height: 0;
        width: 0;
    }

/* Create a custom checkbox */
.checkmark {
    position: absolute;
    top: 0;
    left: 0;
    height: 25px;
    width: 25px;
    background-color: #eee;
    border: 1px solid darkgray;
    border-radius: 5px
}

/* On mouse-over, add a grey background color */
.container:hover input ~ .checkmark {
    background-color: #ccc;
}

/* When the checkbox is checked, add a blue background */
.container input:checked ~ .checkmark {
    background-color: #2196F3;
}

/* Create the checkmark/indicator (hidden when not checked) */
.checkmark:after {
    content: "";
    position: absolute;
    display: none;
}

/* Show the checkmark when checked */
.container input:checked ~ .checkmark:after {
    display: block;
}

/* Style the checkmark/indicator */
.container .checkmark:after {
    left: 9px;
    top: 5px;
    width: 5px;
    height: 10px;
    border: solid white;
    border-width: 0 3px 3px 0;
    -webkit-transform: rotate(45deg);
    -ms-transform: rotate(45deg);
    transform: rotate(45deg);
}

button {
    font-size: 100px;
}

.block {
    display: block;
    width: 100px;
    border: none;
    background-color: white;
    padding: 14px 28px;
    font-size: 30px;
    cursor: pointer;
    text-align: center;
    border: 3px solid black;
    border-radius: 15px;
    padding: 20px;
    margin-bottom: 30px;
}

    .block:hover {
        background-color: #ccc;
        transition: 0.3s;
    }

    .block:after {
        background-color: lightblue;
        transition: 0.3s;
    }

.buttoncontainer {
    display: block;
    width: 85px;
    position: relative;
    padding: 14px 28px;
    padding: 20px;
    margin-bottom: 20px;
    cursor: pointer;
    font-size: 30px;
    border: solid black 2px;
    border-radius: 15px;
    -webkit-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
}

    .buttoncontainer:hover {
        background-color: lightgray;
        transition: 0.3s;
    }

    /* Hide the browser's default radio button */
    .buttoncontainer input {
        position: absolute;
        opacity: 0;
        cursor: pointer;
    }

/* Create a custom radio button */
.buttoncheckmark {
    position: absolute;
    top: 0;
    left: 0;
    height: 20px;
    width: 200px;
    border-radius: 10px;
}




/* When the radio button is checked, add a blue background */
.buttoncontainer input:checked ~ .buttoncheckmark {
    background-color: black;
    opacity: 10;
    width: 100px;
    height: 50px;
    border-radius: 15px;
}

/* Create the indicator (the dot/circle - hidden when not checked) */
.buttoncheckmark:after {
    content: "";
    position: absolute;
    display: none;
}

.time {
    margin-top: 20px;
}

</style>
        
</head>
<body>
    <h2>Blind Automation</h2>
    <div id="directbuttons">
        <h3> Tilt </h3>
       
        
        %BUTTONS%
    </div>
    <div id="openswitches">
        <h3> Open </h3>

        %OPENSWITCHES%
    </div>

        

    <div id="closingswitches">
        <h3> Close </h3>

      %CLOSINGSWITCHES%

    </div>



        <script>
            function toggleCheckbox(element) {
                var xhr = new XMLHttpRequest();
                if (element.checked) { xhr.open("GET", "/update?output=" + element.id + "&state=1", true); }
                else { xhr.open("GET", "/update?output=" + element.id + "&state=0", true); }
                xhr.send();
            }
        </script>
</body>
</html>

)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if(var == "OPENSWITCHES"){
    String buttons = "";
     buttons += "<label class=\"container\">Monday<input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"0\" " + outputState(0) + "><span class=\"checkmark\"></span></label>";
     buttons += "<label class=\"container\">Tuesday<input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"1\" " + outputState(1) + "><span class=\"checkmark\"></span></label>";
     buttons += "<label class=\"container\">Wednesday<input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"2\" " + outputState(2) + "><span class=\"checkmark\"></span></label>";
     buttons += "<label class=\"container\">Thursday<input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"3\" " + outputState(3) + "><span class=\"checkmark\"></span></label>";
     buttons += "<label class=\"container\">Friday<input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"4\" " + outputState(4) + "<span class=\"checkmark\"></span></label>";
     buttons += "<label class=\"container\">Saturday<input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"5\" " + outputState(5) + "><span class=\"checkmark\"></span></label>";
     buttons += "<label class=\"container\">Sunday<input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"6\" " + outputState(6)+ "><span class=\"checkmark\"></span></label>";
     //buttons += "<label for=\"appt\">Select a time: </label><input type=\"time\" id=\"17\" name=\"appt\" class=\"time\"><span class=\"time\"></span></label>";
    return buttons;
  }  
  else if(var == "CLOSINGSWITCHES"){
    String buttons = "";
    buttons += "<label class=\"container\">Monday<input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"7\" " + outputState(7) + "><span class=\"checkmark\"></span></label>";
     buttons += "<label class=\"container\">Tuesday<input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"8\" " + outputState(8) + "><span class=\"checkmark\"></span></label>";
     buttons += "<label class=\"container\">Wednesday<input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"9\" " + outputState(9) + "><span class=\"checkmark\"></span></label>";
     buttons += "<label class=\"container\">Thursday<input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"10\" " + outputState(10) + "><span class=\"checkmark\"></span></label>";
     buttons += "<label class=\"container\">Friday<input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"11\" " + outputState(11) + "><span class=\"checkmark\"></span></label>";
     buttons += "<label class=\"container\">Saturday<input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"12\" " + outputState(12) + "><span class=\"checkmark\"></span></label>";
     buttons += "<label class=\"container\">Sunday<input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"13\" " + outputState(13) + "><span class=\"checkmark\"></span></label>";
     buttons += "<label for=\"appt\">Select a time: </label><input type=\"time\" id=\"18\" name=\"appt\" class=\"time\"><span class=\"time\"></span></label>";
    return buttons;
  }
  else if(var == "BUTTONS"){
    String buttons = "";
    buttons += "<button>Hello</button>";
    
    
    return buttons;
    
  }
  
  return String();
}


String outputState(int output){

  if(digitalRead(output)){
    return "checked";
  }
  else {
    return "";
  }
}



void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(xaxis, INPUT);
  pinMode(CCW, OUTPUT);
  pinMode(CW, OUTPUT);
  pinMode(AccPower, OUTPUT);
  digitalWrite(AccPower, HIGH);
  
  

  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {

    
    
    int angle;
    
    String inputMessage1;
    int i1;
    String inputMessage2;
    int i2;
    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
      inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      i1 = inputMessage1.toInt();
      i2 = inputMessage2.toInt();
      //angle = analogRead(xaxis)
      digitalWrite(inputMessage1.toInt(), inputMessage2.toInt());

      if (i1 == 14){
        //forwards
        while (analogRead(xaxis) > 1500){
            digitalWrite(CCW, HIGH);               // Tilts the blinds forwards
            digitalWrite(CW, LOW);
            digitalWrite(2, HIGH);
            //angle = analogRead(xaxis);
            //Serial.print(angle);
          }
                    digitalWrite(CCW, LOW);
          digitalWrite(CW, LOW);
          digitalWrite(2, LOW);
        }
        
      
      else if (i1 == 15){
        // Flat
        if (analogRead(xaxis) < 1800) {
            while (analogRead(xaxis) < 1800){
              digitalWrite(CCW, LOW);       // Tilts to middle from forwards
              digitalWrite(CW, HIGH);
              //angle = analogRead(xaxis);
            }
                      digitalWrite(CCW, LOW);
          digitalWrite(CW, LOW);
          }
          else if (analogRead(xaxis) >= 1860){
            while (analogRead(xaxis) >= 1860){
              digitalWrite(CCW, HIGH);        // Tilts to middle from backwards
              digitalWrite(CW, LOW);
              //angle = analogRead(xaxis);
            }
                      digitalWrite(CCW, LOW);
          digitalWrite(CW, LOW);
          }
        
      }
      else if (i1 == 16){
        // Backwards
        while (analogRead(xaxis) < 2180){
            digitalWrite(CCW, LOW);         // Tilts backwards
            digitalWrite(CW, HIGH);
            digitalWrite(2, HIGH);
            //angle = analogRead(xaxis);
            //Serial.print(angle);
            
          }
          digitalWrite(2, LOW);
          digitalWrite(CCW, LOW);
          digitalWrite(CW, LOW);

          
      }
      
    
    else {
      inputMessage1 = "No message sent";
      inputMessage2 = "No message sent";
    }

  




  
  server.begin();

  Serial.println("Server started");
}
  });

}
void loop() {

}
