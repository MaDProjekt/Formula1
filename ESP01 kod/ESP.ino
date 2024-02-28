

#include <ESP8266WiFi.h>
// #include <WiFiClient.h>
// #include <WiFi.h>


const char* ssid     = "F1-M&D";
const char* password = "123456789";


WiFiServer server(80);


String header;


String output26State = "off";
String output27State = "off";


const int output26 = 26;
const int output27 = 27;

int data = 0;

const char DELIMITER = ';';
const char END_CHAR = '|';

String receivedData = ""; // Prijaté dáta
String speed = "";
String DRS = "";
String ERS = "";
String brzda = "";
String rotation = "";

String webSpeed = "";
String webDRS = "";
String webERS = "";
String webReceivedData = "";
String webBrzda = "";
String webRotation = "";

void processIncomingData() {
  // Spracovanie prijatých údajov
  Serial.print("Spracovanie prijatých údajov: ");
  Serial.println(receivedData);
  webReceivedData = receivedData;

  int delimiterIndex = receivedData.indexOf(DELIMITER);
  while (delimiterIndex != -1) {
    Serial.println(String(delimiterIndex));
    // Oddelenie premenných
    String token = receivedData.substring(0, delimiterIndex);
    
    Serial.print("Token: ");
    Serial.println(token);

    // Spracovanie premenných
    if (speed == "") {
      speed = token;
    } else if (DRS == "") {
      DRS = token;
    } else if (ERS == "") {
      ERS = token;
    } else if (brzda == "") {
      brzda = token;
    } else if (rotation == "") {
      rotation = token;
      token = "";
    }

    String combinedString = "Data2:" + speed + "," + DRS + "," + ERS + "," + brzda + "," + rotation;
    Serial.println(combinedString);
    
    // Odstránenie spracovanej časti z retazca
    receivedData = receivedData.substring(delimiterIndex + 1);
    
    // Nájdenie ďalšieho oddelovača
    delimiterIndex = receivedData.indexOf(DELIMITER);
  }

  // Ak sme na konci retazca, znamená to, že sme prijali údaje pre všetky premenné
  //String END_STR = String(END_CHAR);
  // if (receivedData.endsWith(END_STR)) {
  if (receivedData.indexOf(END_CHAR) != -1) {
    // Spracovanie poslednej časti retazca
    receivedData = receivedData.substring(0, receivedData.length() - 1);
    rotation = receivedData;
    
    // Resetovanie premenných pre ďalší prijatý retazec
    webSpeed = speed;
    speed = "";
    webDRS = DRS;
    DRS = "";
    webERS = ERS;
    ERS = "";
    webBrzda = brzda;
    brzda = "";
    webRotation = rotation;
    rotation = "";
    receivedData = "";
    
    
    String combinedString = "Data:" + webSpeed + "," + webDRS + "," + webERS + "," + webBrzda + "," + webRotation;
    Serial.println(combinedString);
  }
}

void setup() {
  Serial.begin(57600);
  // Initialize the output variables as outputs
  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address v1: ");
  Serial.println(IP);

  
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients 

  while (Serial.available() > 0) {
    char incomingChar = Serial.read();
    if (incomingChar == END_CHAR) {
      // Spracovanie prijatých údajov
      receivedData += incomingChar;
      processIncomingData();
      receivedData = "";
    } else {
      // Pridanie písmena k prijatým údajom
      receivedData += incomingChar;
    }
  }
  

  if (client) {                             
    Serial.println("New Client.");       
    String currentLine = "";             
    while (client.connected()) {           
      if (client.available()) {             
        char c = client.read();           
        Serial.write(c);                    
        header += c;
        if (c == '\n') {                             
          if (currentLine.length() == 0) {       
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
          
            client.println("<!DOCTYPE html>");
            client.println("<html lang=\"en\">");
            client.println("<head>");
            client.println("  <meta charset=\"UTF-8\">");
            client.println("  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
            client.println("  <meta http-equiv=\"refresh\" content=\"2\">");
            client.println("  <title>F1 M&D</title>");
            client.println("  <style>");
            client.println("    h1 {");
            client.println("      color: white;");
            client.println("      font-size: 100px;");
            client.println("      font-style: italic;");
            client.println("    }");
            client.println("    h2 {");
            client.println("      color: #f5f5f5;");
            client.println("    }");
            client.println("    h3 {");
            client.println("      font-size: 30px;");
            client.println("    }");
            client.println("    p{");
            client.println("      color: #f5f5f5;");
            client.println("    }");
            client.println("    .myDiv {");
            client.println("      text-align: center;");
            client.println("    }");
            client.println("    .dataDiv {");
            client.println("      text-align: center;");
            client.println("      margin-top: 10%;");
            client.println("    }");
            client.println("    .textDiv {");
            client.println("      display: flex;");
            client.println("      text-align: center;");
            client.println("      margin-top: 1%;");
            client.println("      margin-left: 20%;");
            client.println("      margin-right: 20%;");
            client.println("    }");
            client.println("    .dataDiv2 {");
            client.println("      text-align: center;");
            client.println("      margin-right: 55%;");
            client.println("      margin-left: 45%;");
            client.println("    }");
            client.println("  </style>");
            client.println("</head>");
            client.println("<body style=\"background-color: rgb(59, 57, 57);\">");
            client.println("  <div class=\"myDiv\">");
            client.println("    <h1>F1 M&D</h1>");
            client.println("  </div>");
            client.println("  <div class=\"dataDiv2\">");
            client.println("    <h2 color: white>Speed: </h2>");
            client.println("<p>" + String(webSpeed) + "</p>");
            client.println("    <h2>DRS: </h2>");
            client.println("<p>" + String(webDRS) + "</p>");
            client.println("    <h2>Turbo: </h2>");
            client.println("    <canvas id=\"ERS\" width=\"100\" height=\"50\" style=\"border:2px solid #f5f5f5;\"></canvas>");
            client.println("  </div>");
            client.println("</div>");
            client.println("  <div class=\"dataContainer\">");
            client.println("    <div class=\"dataDiv\">");
            client.println("        <canvas id=\"Brzda\" width=\"200\" height=\"300\" style=\"border:2px solid #f5f5f5;margin-right: 10%;\"></canvas>");
            client.println("        <canvas id=\"Rotation\" width=\"220\" height=\"200\" style=\"border:2px solid #f5f5f5;\"></canvas>");
            client.println("        <canvas id=\"Plyn\" width=\"200\" height=\"300\" style=\"border:2px solid #f5f5f5;margin-left: 10%;\"></canvas>");
            client.println("      </div>");
            client.println("    <div class=\"textDiv\">");
            client.println("        <h3 style=\"color: white;margin-right:20%\">Brzda</h3>");
            client.println("        <h3 style=\"color: white;margin-right:20%; margin-left:20%\">Otačanie</h3>");
            client.println("        <h3 style=\"color: white;margin-left:20%\">Plyn</h3>");
            client.println("    </div>");
            client.println("  <script>");
            client.println("    // Získanie kontextu kreslenia z canvasu");
            client.println("    var canvasBrzda = document.getElementById(\"Brzda\");");
            client.println("    var contextBrzda = canvasBrzda.getContext(\"2d\");");
            client.println("");
            client.println("    var canvasPlyn = document.getElementById(\"Plyn\");");
            client.println("    var contextPlyn = canvasPlyn.getContext(\"2d\");");
            client.println("");
            client.println("    var canvasRot = document.getElementById(\"Rotation\");");
            client.println("    var contextRot = canvasRot.getContext(\"2d\");");
            client.println("");
            client.println("    var canvasERS = document.getElementById(\"ERS\");");
            client.println("    var contextERS = canvasERS.getContext(\"2d\");");
            client.println("");
            client.println("    // Funkcia na kreslenie obdĺžnika a pridanie textu");
            client.println("    function drawRectWithText(context, x, y, width, height, farba, text) {");
            client.println("      // Nastaviť farbu vyplnenia obdĺžnika");
            client.println("      context.fillStyle = farba;");
            client.println("");
            client.println("      // Nakresliť obdĺžnik pomocou rect() funkcie");
            client.println("      context.fillRect(x, canvasBrzda.height - y - height, width, height);");
            client.println("");
            client.println("      // Pridať text nad canvas");
            client.println("      context.fillStyle = \"#000\"; // čierna farba textu");
            client.println("      context.font = \"28px Arial\"; // veľkosť a typ písma");
            client.println("      context.fillText(text, x + 10, canvasBrzda.height - y - height + 150); // 10 je vzdialenosť textu od vrchu obdĺžnika");
            client.println("    }");
            client.println("");
            client.println("    var cervena = \"#FF0000\";");
            client.println("    var zelena = \"#00ff00\";");
            client.println("    var modra = \"#0000FF\";");
            client.println("");
            client.println("    var x_brzda = 0;");
            client.println("    var y_brzda = 0;");
            client.println("    var width_brzda = 300;");
            client.println("    var height_brzda = " + String(webBrzda) + ";");
            client.println("    ");
            client.println("    var x_plyn = 0;");
            client.println("    var y_plyn = 0;");
            client.println("    var width_plyn = 400;");
            client.println("    var height_plyn = " + String(webSpeed) + ";");
            client.println("");
            client.println("    var x_rot = " + String(webRotation) + ";");
            client.println("    var y_rot = 0;");
            client.println("    var width_rot = 20;");
            client.println("    var height_rot = 400;");
            client.println("");
            client.println("    var x_ers = 0;");
            client.println("    var y_ers = 0;");
            client.println("    var width_ers = " + String(webERS) + ";");
            client.println("    var height_ers = 400;");
            client.println("");
            client.println("    // Volanie funkcie na kreslenie obdĺžnika s textom");
            client.println("    drawRectWithText(contextBrzda, x_brzda, y_brzda, width_brzda, height_brzda, cervena, \"\");");
            client.println("    drawRectWithText(contextPlyn, x_plyn, y_plyn, width_plyn, height_plyn, zelena,\"\" );");
            client.println("    drawRectWithText(contextRot, x_rot, y_rot, width_rot, height_rot, modra, \"\");");
            client.println("    drawRectWithText(contextERS, x_ers, y_ers, width_ers, height_ers, zelena, \"\");");
            client.println("  </script>");
            client.println("</body>");
            client.println("</html>");

           
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') { 
          currentLine += c;   
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
