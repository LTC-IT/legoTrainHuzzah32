#include <SPI.h>  //Serial Library


#include <WiFi.h>
#include "wificonfig.h"

#include <PowerFunctions.h>   //Power Functions Library


// BEGIN - SD Card Featherwing code

#include "FS.h"
#include "SD.h"
#include "SPI.h"

String indexHTML, loginHTML, functionalityHTML, mysiteCSS, logoPNG;
File myFile;
WiFiClient client;

String readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\n", path);
  char c;
  String tempHTML = "";

  File file = fs.open(path);
  if (!file) {
    Serial.print("Failed to open file for reading: ");
    Serial.println(path);
    return "";
  }

  while (file.available()) {
    c = file.read();
    tempHTML += c;
  }
  file.close();
  return tempHTML;
}

void setupSD() {
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("SD Started");
}

void loadHTML() {
  indexHTML = readFile(SD, "/index.html");
  loginHTML = readFile(SD, "/login.html");
  functionalityHTML = readFile(SD, "/functionality.html");
  mysiteCSS = readFile(SD, "/mysite.css");
  logoPNG = readFile(SD, "/logo.png");

}

// END - SD Card Featherwing code

//IR Channels
#define CH1 0x0
#define CH2 0x1
#define CH3 0x2
#define CH4 0x3

//IR Transmission
#define IR_TRANS_IN   A5  //IR Trans PIN
#define IR_DEBUG_OFF  0  //IR Debug Mode Off
#define IR_DEBUG_ON   1  //IR Debug Mode On

//Call PowerFunctions parameters
PowerFunctions pf(IR_TRANS_IN, CH1, IR_DEBUG_ON);


WiFiServer server(80);

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;


// Single and dual motor control is defined
void step(uint8_t output, uint8_t pwm, uint16_t time) {
  pf.combo_pwm(output, pwm);
  pf.single_pwm(output, pwm);
}
// Single increment for speed is defined
void increment(uint8_t output) {
  pf.single_increment(output);
}
// Single decrement for speed is defined
void decrement(uint8_t output) {
  pf.single_decrement(output);
}

void connectToWifi() {
  Serial.println ("Connecting to....");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void wifiserver() {
  client = server.available();   // listen for incoming clients
  if (client) {                             // if you get a client,
    //Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // "send" the HTML code to the browser
            //client.println(htmlInitialPage);
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
          String currentCommand = currentLine;
          currentCommand.trim();


          if (currentCommand == "GET /speed=7 HTTP/1.1") {
            Serial.println("Speed 7");
            step(RED, PWM_FWD7, 0); // The step function is run and speed is set to 7
            client.println(functionalityHTML);

          }
          if (currentCommand == "GET /speed=6 HTTP/1.1") {
            Serial.println("Speed 6");
            step(RED, PWM_FWD6, 0);
          }



          if (currentCommand == "GET /speed=5 HTTP/1.1") {
            Serial.println("Speed 5");
            step(RED, PWM_FWD5, 0);
            client.println("<p><h2>Current speed = 5</h2></p>");

          }
          if (currentCommand == "GET /speed=4 HTTP/1.1") {
            Serial.println("Speed 4");
            step(RED, PWM_FWD4, 0);
            client.println("<p><h2>Current speed = 4</h2></p>");

          }
          if (currentCommand == "GET /speed=3 HTTP/1.1") {
            Serial.println("Speed 3");
            step(RED, PWM_FWD3, 0);
            client.println("<p><h2>Current speed = 3 < / h2 > < / p > ");
          }
          if (currentCommand == "GET /speed=2 HTTP/1.1") {
            Serial.println("Speed 2");
            step(RED, PWM_FWD2, 0);
            client.println("<p><h2>Current speed = 2 < / h2 > < / p > ");
          }
          if (currentCommand == "GET /speed=1 HTTP/1.1") {
            Serial.println("Speed 1");
            step(RED, PWM_FWD1, 0);
            client.println("<p><h2>Current speed = 1 < / h2 > < / p > ");
          }
          if (currentCommand == "GET /brake") {
            Serial.println("Stopped");
            step(RED, PWM_BRK, 0);
            client.println("<p><h2>Status: BRAKED < / h2 > < / p > ");
          }
          if (currentCommand == "GET /speed=-7 HTTP/1.1") {
            Serial.println("Backwards Speed 7");
            step(RED, PWM_REV7, 0);
            client.println("<p><h2>Current speed = -7 < / h2 > < / p > ");
          }
          if (currentCommand == "GET /speed=-6 HTTP/1.1") {
            Serial.println("Backwards Speed 6");
            step(RED, PWM_REV6, 0);
            client.println("<p><h2>Current speed = -6 < / h2 > < / p > ");
          }
          if (currentCommand == "GET /speed=-5 HTTP/1.1") {
            Serial.println("Backwards Speed 5");
            step(RED, PWM_REV5, 0);
            client.println("<p><h2>Current speed = -5 < / h2 > < / p > ");
          }
          if (currentCommand == "GET /speed=-4 HTTP/1.1") {
            Serial.println("Backwards Speed 4");
            step(RED, PWM_REV4, 0);
            client.println("<p><h2>Current speed = -4 < / h2 > < / p > ");
          }
          if (currentCommand == "GET /speed=-3 HTTP / 1.1") {
            Serial.println("Backwards Speed 3");
            step(RED, PWM_REV3, 0);
            client.println("<p><h2>Current speed = -3 < / h2 > < / p > ");
          }
          if (currentCommand == "GET /speed=-2 HTTP / 1.1") {
            Serial.println("Backwards Speed 2");
            step(RED, PWM_REV2, 0);
            client.println("<p><h2>Current speed = -2 < / h2 > < / p > ");
          }
          if (currentCommand == "GET /speed=-1 HTTP / 1.1") {
            Serial.println("Backwards Speed 1");
            step(RED, PWM_REV1, 0);
            client.println("<p><h2>Current speed = -1 < / h2 > < / p > ");
          }





          /*
                    if (currentCommand.startsWith("GET / ") && currentCommand.endsWith("HTTP / 1.1")) {
                      Serial.println(currentCommand);
                    }
                    if (currentCommand == "GET / login.html HTTP / 1.1") {
                      client.println(loginHTML);
                    }

                    if (currentCommand == "GET / mysite.css HTTP / 1.1") {
                      client.println(mysiteCSS);
                    }

                    if (currentCommand == "GET / logo.png HTTP / 1.1") {
                      client.println(logoPNG);
                    }

                    if (currentCommand == "GET / index.html HTTP / 1.1" || currentCommand == "GET / HTTP / 1.1")  {
                      client.println(indexHTML);
                    }

                    if (currentCommand == "GET / functionality.html HTTP / 1.1") {
                      client.println(functionalityHTML);
                    }

                    if (currentCommand == "GET / ledOn HTTP / 1.1") {
                      digitalWrite(LED_BUILTIN, HIGH);
                      client.println(functionalityHTML);
                    }

                    if (currentCommand == "GET / ledOff HTTP / 1.1") {
                      digitalWrite(LED_BUILTIN, LOW);
                      client.println(functionalityHTML);
                    }

                    if (currentCommand == "GET / tf1RedOn HTTP / 1.1") {
                      digitalWrite(tf1Red, HIGH);
                      client.println(functionalityHTML);
                    }
                    if (currentCommand == "GET / tf1RedOff HTTP / 1.1") {
                      digitalWrite(tf1Red, LOW);
                      client.println(functionalityHTML);
                    }
                    if (currentCommand == "GET / tf1YellowOn HTTP / 1.1") {
                      digitalWrite(tf1Yellow, HIGH);
                      client.println(functionalityHTML);
                    }
                    if (currentCommand == "GET / tf1YellowOff HTTP / 1.1") {
                      digitalWrite(tf1Yellow, LOW);
                      client.println(functionalityHTML);
                    }
                    if (currentCommand == "GET / tf1GreenOn HTTP / 1.1") {
                      Serial.println("Green On");
                      digitalWrite(tf1Green, HIGH);
                      client.println(functionalityHTML);
                    }
                    if (currentCommand == "GET / tf1GreenOff HTTP / 1.1") {
                      Serial.println("Green Off");
                      digitalWrite(tf1Green, LOW);
                      client.println(functionalityHTML);
                    }

                    if (currentCommand == "GET / tf1Cycle HTTP / 1.1") {
                      Serial.println("Traffic Light Cycle");
                      cycleLights();
                      client.println(functionalityHTML);
                    }
          */

        }
      }
    }
    // close the connection:
    client.stop();
    //Serial.println("Client Disconnected.");
  }
}


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  // Connect to Wi-Fi network with SSID and password.
  connectToWifi();
  setupSD();
  loadHTML();
}

void loop() {
  //  WiFiClient client = server.available();   // Listen for incoming clients
  //  if (client) {                             // If a new client connects,
  //    Serial.println("New Client.");          // print a message out in the serial port
  //    String currentLine = "";                // make a String to hold incoming data from the client
  //    currentTime = millis();
  //    previousTime = currentTime;
  //    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
  //      currentTime = millis();
  //      if (client.available()) {             // if there's bytes to read from the client,
  //        char c = client.read();             // read a byte, then
  //        Serial.write(c);                    // print it out the serial monitor
  //        header += c;
  //        if (c == '\n') {                    // if the byte is a newline character
  //          // if the current line is blank, you got two newline characters in a row.
  //          // that's the end of the client HTTP request, so send a response:
  //          if (currentLine.length() == 0) {
  //            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  //            // and a content-type so the client knows what's coming, then a blank line:
  //            client.println("HTTP / 1.1 200 OK");
  //            client.println("Content - type: text / html");
  //            client.println("Connection: close");
  //            client.println();
  //
  //            // All speeds are available via URL query parameters. Use the IP address of the web server with the relevant URL/code below.
  //            //Change the value at the end of the URL to change the speed.
  //            if (header.indexOf("GET / NX0bqCEob / ZgY7lnh5yt / POJ9DzA7UZVMJi6hNbZvpw = 7") >= 0) {
  //              Serial.println("Speed 7");
  //              step(RED, PWM_FWD7, 0); // The step function is run and speed is set to 7
  //              client.println("<p><h2>Current speed = 7 < / h2 > < / p > ");
  //
  //            } else if (header.indexOf("GET / NX0bqCEob / ZgY7lnh5yt / POJ9DzA7UZVMJi6hNbZvpw = 6") >= 0) {
  //              Serial.println("Speed 6");
  //              step(RED, PWM_FWD6, 0);
  //              client.println("<p><h2>Current speed = 6 < / h2 > < / p > ");
  //
  //            } else if (header.indexOf("GET / NX0bqCEob / ZgY7lnh5yt / POJ9DzA7UZVMJi6hNbZvpw = 5") >= 0) {
  //              Serial.println("Speed 5");
  //              step(RED, PWM_FWD5, 0);
  //              client.println("<p><h2>Current speed = 5 < / h2 > < / p > ");
  //
  //            } else if (header.indexOf("GET / NX0bqCEob / ZgY7lnh5yt / POJ9DzA7UZVMJi6hNbZvpw = 4") >= 0) {
  //              Serial.println("Speed 4");
  //              step(RED, PWM_FWD4, 0);
  //              client.println("<p><h2>Current speed = 4 < / h2 > < / p > ");
  //
  //            } else if (header.indexOf("GET / NX0bqCEob / ZgY7lnh5yt / POJ9DzA7UZVMJi6hNbZvpw = 3") >= 0) {
  //              Serial.println("Speed 3");
  //              step(RED, PWM_FWD3, 0);
  //              client.println("<p><h2>Current speed = 3 < / h2 > < / p > ");
  //
  //            } else if (header.indexOf("GET / NX0bqCEob / ZgY7lnh5yt / POJ9DzA7UZVMJi6hNbZvpw = 2") >= 0) {
  //              Serial.println("Speed 2");
  //              step(RED, PWM_FWD2, 0);
  //              client.println("<p><h2>Current speed = 2 < / h2 > < / p > ");
  //
  //            } else if (header.indexOf("GET / NX0bqCEob / ZgY7lnh5yt / POJ9DzA7UZVMJi6hNbZvpw = 1") >= 0) {
  //              Serial.println("Speed 1");
  //              step(RED, PWM_FWD1, 0);
  //              client.println("<p><h2>Current speed = 1 < / h2 > < / p > ");
  //
  //            } else if (header.indexOf("GET / brake") >= 0) {
  //              Serial.println("Stopped");
  //              step(RED, PWM_BRK, 0);
  //              client.println("<p><h2>Status: BRAKED < / h2 > < / p > ");
  //
  //            } else if (header.indexOf("GET / NX0bqCEob / ZgY7lnh5yt / POJ9DzA7UZVMJi6hNbZvpw = -7") >= 0) {
  //              Serial.println("Backwards Speed 7");
  //              step(RED, PWM_REV7, 0);
  //              client.println("<p><h2>Current speed = -7 < / h2 > < / p > ");
  //
  //            } else if (header.indexOf("GET / NX0bqCEob / ZgY7lnh5yt / POJ9DzA7UZVMJi6hNbZvpw = -6") >= 0) {
  //              Serial.println("Backwards Speed 6");
  //              step(RED, PWM_REV6, 0);
  //              client.println("<p><h2>Current speed = -6 < / h2 > < / p > ");
  //
  //            } else if (header.indexOf("GET / NX0bqCEob / ZgY7lnh5yt / POJ9DzA7UZVMJi6hNbZvpw = -5") >= 0) {
  //              Serial.println("Backwards Speed 5");
  //              step(RED, PWM_REV5, 0);
  //              client.println("<p><h2>Current speed = -5 < / h2 > < / p > ");
  //
  //            } else if (header.indexOf("GET / NX0bqCEob / ZgY7lnh5yt / POJ9DzA7UZVMJi6hNbZvpw = -4") >= 0) {
  //              Serial.println("Backwards Speed 4");
  //              step(RED, PWM_REV4, 0);
  //              client.println("<p><h2>Current speed = -4 < / h2 > < / p > ");
  //
  //            } else if (header.indexOf("GET / NX0bqCEob / ZgY7lnh5yt / POJ9DzA7UZVMJi6hNbZvpw = -3") >= 0) {
  //              Serial.println("Backwards Speed 3");
  //              step(RED, PWM_REV3, 0);
  //              client.println("<p><h2>Current speed = -3 < / h2 > < / p > ");
  //
  //            } else if (header.indexOf("GET / NX0bqCEob / ZgY7lnh5yt / POJ9DzA7UZVMJi6hNbZvpw = -2") >= 0) {
  //              Serial.println("Backwards Speed 2");
  //              step(RED, PWM_REV2, 0);
  //              client.println("<p><h2>Current speed = -2 < / h2 > < / p > ");
  //
  //            } else if (header.indexOf("GET / NX0bqCEob / ZgY7lnh5yt / POJ9DzA7UZVMJi6hNbZvpw = -1") >= 0) {
  //              Serial.println("Backwards Speed 1");
  //              step(RED, PWM_REV1, 0);
  //              client.println("<p><h2>Current speed = -1 < / h2 > < / p > ");
  //            }
  //
  //           // Display the HTML web page
  //            client.println(" < !DOCTYPE html > <html>");
  //            client.println("<head> < meta name = \"viewport\" content=\"width=device-width, initial-scale=1\">");
  //            client.println("<link rel=\"icon\" href=\"data:,\">");
  //            // CSS to style the on/off buttons
  //            // Feel free to change the background-color and font-size attributes to fit your preferences
  //            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}</style>");
  //            //client.println(".button { background-color: #3399ff; border: none; color: white; padding: 16px 40px;");
  //            client.println("<style>text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}</style>");
  //            //client.println(".button2 {background-color: #555555;}</style></head>");
  //            client.println("<style>body { background-color: #A7D4F4;}</style>");
  //
  //            // Web Page Heading
  //            client.println("<body><h3>The</h3> <br> <h1>Transport of Resources Around International Networks</h1> <br> <h3>system</h3> <br> <h1>(T.R.A.I.N)</h1>");
  //            client.println("<!--The Raspberry Pi's username can be found at the station.-->");
  //            client.println("<!--A Virtual Network Connection is required to browse the Raspberry Pi remotely.-->");
  //            client.println("<!--VGhlIHBhc3N3b3JkIGZvciB0aGUgUGkgaXMgdGhlIG1vZGVsIG51bWJlciBvZiB0aGUgdHJhaW4u-->"); // Message encrypted in base64 (password). The train model number is 60051.
  //            client.println("<!--Do you know how to cook? I enjoy baking cakes.-->");
  //            client.println("<br>");
  //            client.println("<!--<br>MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM-->");
  //            client.println("<!--<br>MMMMMMMMMdmmNMMMMMMMMMMMMMmhhhhhhhhhhNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM-->");
  //            client.println("<!--<br>MMMMMMMMysyyhhMMMMMMMMMMMMmh+++++++oyNMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM-->");
  //            client.println("<!--<br>MMMMMMMMMsssyNMMMMMMMMMMMMmd``....-/hNMMMMooyyyyyyyyyyyyyyyyyyyyyNMMMMMs+yyyyyyyyyyyyyyyyyyyyymMMMMM-->");
  //            client.println("<!--<br>MMMMMMMMMsyyhMMMMMMMMMMMMMmd...----/dNMMMMyd:::::hm:::::ydo::::+dNMMMMMhd:::::yd/::::ods:::::mmMMMMM-->");
  //            client.println("<!--<br>MMMMMMdssym++++++++++oshNMmd.------/dNMMMMyh`....hd`....sd/`.../dNMMMMMhd`....sd-....+do`...-mmMMMMM-->");
  //            client.println("<!--<br>MMMMMNyyydsoooooooooooosmhdyyyyyyhhhyNMMMMyh---::hd.---:yd+---:+dNMMMMMhd.---:yd:---:odo---::mmMMMMM-->");
  //            client.println("<!--<br>MMMMMdhhhmooooooooooooosmhdooooosyyyyNMMMMyyhhhhhhyhhhhhhhhhhhhhhNMMMMMhyhhhhhhyhhhhhhhhhhhhhhmMMMMM-->");
  //            client.println("<!--<br>MMMMMdhhhmooooooooooooosmhdoooosyyyyyNMMMMysssssssssyyyyyyyyyyyyyNMMMMMhsssssssssyyyyyyyyyyyyymMMMMM-->");
  //            client.println("<!--<br>MMMMMmmmmmsssssssssssssymddooooyyyyyyNMMMMysssssssyyyyyyyyyyyyyyyNMMMMMhsssssssyyyyyyyyyyyyyyymMMMMM-->");
  //            client.println("<!--<br>MMMMMNmmmNyyyyyyyyyyyyyymmmhhhhddhyyyNMMMMyssssyyyyyyyyyyyyyyyyyyNMMMMMhssssyyyyyyyyyyyyyyyyyymMMMMM-->");
  //            client.println("<!--<br>MMMMMMNNmmNNNNNNNNNNNNNNmysyyyysshdyymdmMMyssyyyyyyyyyyyyyyyyyyyyNMhmMMhssyyyyyyyyyyyyyyyyyyyymMddMM-->");
  //            client.println("<!--<br>MMMMhsdhhhhhhhhhhhhhhhmdsyhhhhhhhsyNdmmmdNhyhhhhhhhhhhhhhhhhhhhhhNmNNdmdyhhhhhhhhhhhhhhhhhhhhhmmmMMM-->");
  //            client.println("<!--<br>MMNysdMMNNNNMMMMMNNNMMMyshhdmmmhhysmMMMMNsyhhhhhhhhhhhhhhhhhhhhhhyhMMMssyhhhhhhhhhhhhhhhhhhhhhyyNMMM-->");
  //            client.println("<!--<br>MMysymMhoddhdNNmyddsyMMdsyhhhhhhhssNMMMMMMNmdmNMMmddNMMNddmMMNddmNMMMMMMmddNMMmddNMMNddmNMNmddNMMMMM-->");
  //            client.println("<!--<br>MMNNNMMsohh+hMMm/yhs+NMMdssyyyyysyNMMMMMMMdhdhhMdhdhhMMhhdhmMhhdhdMMMMMdhdhhMmhdhhNMhhdhdMdhdhhMMMMM-->");
  //            client.println("<!--<br>MMMMMMMMdyymMMMMmhyhNMMMMMNddhdmNMMMMMMMMMMmdmNMMmdmNMMNmdmMMNmdmMMMMMMMmdmNMMmddNMMNmdmMMMmdmNMMMMM-->");
  //            client.println("<!--<br>MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM-->");
  //            client.println("<!--<br>MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM-->");
  //
  //// Uncomment these lines for user-friendly buttons
  //
  //     //Backwards Speed 7 button
  //            //client.println("<a href=\"/NX0bqCEob/ZgY7lnh5yt/POJ9DzA7UZVMJi6hNbZvpw=-7\"><button class=\"button\">Backwards Speed 7</button></a>");
  //     //Backwards Speed 6 button
  //            //client.println("<a href=\"/NX0bqCEob/ZgY7lnh5yt/POJ9DzA7UZVMJi6hNbZvpw=-6\"><button class=\"button\">Backwards Speed 6</button></a>");
  //     //Backwards Speed 5 button
  //            //client.println("<a href=\"/NX0bqCEob/ZgY7lnh5yt/POJ9DzA7UZVMJi6hNbZvpw=-5\"><button class=\"button\">Backwards Speed 5</button></a>");
  //     //Backwards Speed 4 button
  //            //client.println("<a href=\"/NX0bqCEob/ZgY7lnh5yt/POJ9DzA7UZVMJi6hNbZvpw=-4\"><button class=\"button\">Backwards Speed 4</button></a>");
  //     //Backwards Speed 3 button
  //            //client.println("<a href=\"/NX0bqCEob/ZgY7lnh5yt/POJ9DzA7UZVMJi6hNbZvpw=-3\"><button class=\"button\">Backwards Speed 3</button></a>");
  //     //Backwards Speed 2 button
  //            //client.println("<a href=\"/NX0bqCEob/ZgY7lnh5yt/POJ9DzA7UZVMJi6hNbZvpw=-2\"><button class=\"button\">Backwards Speed 2</button></a>");
  //     //Backwards Speed 1 button
  //            //client.println("<a href=\"/NX0bqCEob/ZgY7lnh5yt/POJ9DzA7UZVMJi6hNbZvpw=-1\"><button class=\"button\">Backwards Speed 1</button><br></a>");
  //     //Brakes
  //            //client.println("<a href=\"/brake\"><button class=\"button\">Brakes</button><br></a>");
  //     //Speed 1 button
  //            //client.println("<a href=\"/NX0bqCEob/ZgY7lnh5yt/POJ9DzA7UZVMJi6hNbZvpw=1"><button class=\"button\">Speed 1</button></a>");
  //     //Speed 2 button
  //            //client.println("<a href=\"/NX0bqCEob/ZgY7lnh5yt/POJ9DzA7UZVMJi6hNbZvpw=2"><button class=\"button\">Speed 2</button></a>");
  //     //Speed 3 button
  //            //client.println("<a href=\"/NX0bqCEob/ZgY7lnh5yt/POJ9DzA7UZVMJi6hNbZvpw=3"><button class=\"button\">Speed 3</button></a>");
  //     //Speed 4 button
  //            //client.println("<a href=\"/NX0bqCEob/ZgY7lnh5yt/POJ9DzA7UZVMJi6hNbZvpw=4"><button class=\"button\">Speed 4</button></a>");
  //     //Speed 5 button
  //            //client.println("<a href=\"/NX0bqCEob/ZgY7lnh5yt/POJ9DzA7UZVMJi6hNbZvpw=5"><button class=\"button\">Speed 5</button></a>");
  //     //Speed 6 button
  //            //client.println("<a href=\"/NX0bqCEob/ZgY7lnh5yt/POJ9DzA7UZVMJi6hNbZvpw=6"><button class=\"button\">Speed 6</button></a>");
  //     //Speed 7 button
  //            //client.println("<a href=\"/LPuVyUjuVrIahpvTxNLpwXOSe8qSCgyzP754aLwLupg=7"><button class=\"button\">Speed 7</button></a>");
  //
  //            //Close the body of the HTML page
  //            client.println("</body></html>");
  //
  //            // The HTTP response ends with another blank line
  //            client.println();
  //            // Break out of the while loop
  //            break;
  //          } else { // if you got a newline, then clear currentLine
  //            currentLine = "";
  //          }
  //        } else if (c != '\r') {  // if you got anything else but a carriage return character,
  //          currentLine += c;      // add it to the end of the currentLine
  //        }
  //      }
  //    }
  //    // Clear the header variable
  //    header = "";
  //    // Close the connection
  //    client.stop();
  //  }
  wifiserver();

}

// Uncomment the lines below to print out the WiFi status to the serial monitor

//void printWiFiStatus() {
//  // print the SSID of the network you're attached to:
//  Serial.print("SSID: ");
//  Serial.println(WiFi.SSID());
//
//  // print your WiFi shield's IP address:
//  IPAddress ip = WiFi.localIP();
//  Serial.print("IP Address: ");
//  Serial.println(ip);
//
//  // print the received signal strength:
//  long rssi = WiFi.RSSI();
//  Serial.print("signal strength (RSSI):");
//  Serial.print(rssi);
//  Serial.println(" dBm");
//}
