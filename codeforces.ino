//for wifi
#include <ESP8266WiFi.h>

//Wifi Manager
#include <WiFiManager.h>

//For i2c
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
U8G2_SH1106_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);

//For firebase
#include <FirebaseArduino.h>
#define FIREBASE_HOST "codeforces-fbdd0-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "yXGsqXjQQtYZB4qNNyXtOJqKBG6VxHNhBZJXFKyy"


//For Web server
#include <ESP8266WebServer.h>

//For https reequest
#include <WiFiClientSecure.h>

//#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

//custom Header file
#include "setupCF.h"
#include "bootLogo.h"
#include "timeStamp.h"
#include "targetUpdt.h"

//IP Address and MAC
IPAddress ip;
char IP[17];
String mac;

//Setup web server at 80 port
ESP8266WebServer webserver(80);

//Global User name of Codeforces and target
String cfID = String('~');
int cfTarget = -1;
int baki = -1;


//Web Page for Username input
static const char PROGMEM cfUsernameWeb[] = R"rawliteral(  
<!DOCTYPE html>
<html>
<head>
<title>Codeforces Monitor</title>
</head>
<body>
<form method="post" action="/setUsr" >
<label for='name'>Insert Your Codeforces User Name Here:</label>
<br><br>
<input type='text' id='name' name='usrname' placeholder="Username" required minlength='0' maxlength='800' size='100'></br>
<input type="submit" name="clk_action" value="accept">
</form>
</body>
</html>
)rawliteral";

void handleUserName() {
  webserver.send(200, "text/html", cfUsernameWeb);
}

void handleSetUsr() {
  String str = "Settings Saved ...\r\n", TmpID;

  Serial.print("number of arguments ");
  Serial.println(webserver.args());  // number of arguments
  if (webserver.args() > 0) {
    for (uint8_t i = 0; i < webserver.args(); i++) {
      str += webserver.argName(i) + " = " + webserver.arg(i) + "\r\n";
      TmpID = webserver.arg(i);
      if (webserver.argName(i) == "usrname" && TmpID.length() != 0) {
        cfID = webserver.arg(i);
      }
      Serial.println("Arg " + String(i) + "=" + webserver.argName(i));
      Serial.println("Arg " + String(i) + "=" + webserver.arg(i));
    }
  }
  webserver.send(200, "text/plain", str.c_str());
}

void handleNotFound() {
  webserver.send(404, "text/plain", "Page not found ...");
}

void u8g2_prepare(void) {
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

String Ip2String(IPAddress& ipAddress) {
  return String(ipAddress[0]) + String(".") + String(ipAddress[1]) + String(".") + String(ipAddress[2]) + String(".") + String(ipAddress[3]);
}


void connectToWiFi() {

  WiFiManager wm;
  bool res;
  res = wm.autoConnect("Codeforces", "codeforces");  // password protected ap
  u8g2.clearBuffer();

  if (!res) {
    Serial.println("Failed to connect");
    // ESP.restart();
  } else {
    //if you get here you have connected to the WiFi
    Serial.println(F("WiFi connected!"));
    u8g2.drawStr(0, 0, "WiFi connected!");
    u8g2.sendBuffer();
    delay(100);
    u8g2.drawStr(0, 10, "=================");
    u8g2.sendBuffer();
    delay(100);
    Serial.println("IP address: ");
    u8g2.drawStr(0, 20, "IP address:");
    u8g2.sendBuffer();
    ip = WiFi.localIP();
    Serial.println(ip);
    String s = Ip2String(ip);
    s.toCharArray(IP, 17);
    u8g2.drawStr(0, 30, IP);
    u8g2.sendBuffer();
  }

  Serial.println(F("Setup ready"));
  u8g2.drawStr(40, 50, "Setup ready");
}
void welcome() {
  u8g2.clearBuffer();
  char tmpID[100];
  cfID.toCharArray(tmpID, 100);
  u8g2.drawStr(20, 30, tmpID);
  u8g2.sendBuffer();
}




void cfUser() {
  Serial.println("Cf user input");

  u8g2.clearBuffer();
  u8g2.drawStr(0, 0, "Input your Codeforces");
  u8g2.drawStr(0, 10, "username.");
  u8g2.drawStr(0, 20, "Visit this IP:");
  u8g2.drawStr(0, 30, IP);

  u8g2.sendBuffer();
  webserver.on("/", handleUserName);
  webserver.on("/setUsr", handleSetUsr);
  webserver.onNotFound(handleNotFound);

  webserver.begin();
  Serial.println("Web server has started");
  while (cfID == String('~')) {
    webserver.handleClient();
  }
  webserver.stop();
}

void getTarget() {
  targetSetup();
  targetDisp();
  cfTarget = target;
}
void firebaseSetup() {
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  u8g2.clearBuffer();

  if (Firebase.failed()) {
    Serial.println("Firebase failed");
    u8g2.drawStr(0, 0, "Can't connect to the");
    u8g2.sendBuffer();
    u8g2.drawStr(0, 10, "Server. Try again :)");
    u8g2.sendBuffer();
    u8g2.drawStr(0, 30, "*Check your Internet");
    u8g2.sendBuffer();
  } 
  else {
    Serial.println("Firebase connected");
    Serial.println(mac);
    String name = Firebase.getString("/User/" + mac);
    int trgt = Firebase.getInt("/Limit/" + mac);
    if (Firebase.success() && name.length()!=0) {
      Serial.println("Old User");
      cfID = name;
      cfTarget = trgt;
      welcome();
    } else {
      Serial.println("New User");
      //Username input
      u8g2.clearBuffer();
      cfUser();
      delay(1000);
      getTarget();
      //String logs;
      Firebase.setString("/User/" + mac, cfID);
      Firebase.setInt("/Limit/" + mac, cfTarget);
      //Serial.println(logs);
      if (!Firebase.success()) {
        u8g2.clearBuffer();
        u8g2.drawStr(0, 0, "Something went wrong");
      } else {
        welcome();
      }
    }
  }
}

int prebaki = -1000000;
void todaySolved() {
  int todaySolved = Firebase.getInt("/TodaySolve/" + mac);
  Serial.println("Target=" + String(cfTarget) + "    Today solve" + String(todaySolved));
  baki = cfTarget - todaySolved;

  if (prebaki != baki) {
    u8g2.clearBuffer();
    if (baki < 0) {
      u8g2.drawStr(0, 0, "Extra Solved ");
      u8g2.setCursor(75, 0);
      u8g2.print(baki * (-1));
    } else {
      u8g2.drawStr(0, 0, "Remaining ");
      u8g2.setCursor(70, 0);
      u8g2.print(baki);
    }
    u8g2.sendBuffer();
    prebaki = baki;
  }
}

// Codeforces url for connecting
#define TEST_HOST "codeforces.com"
// OPTIONAL - The finferprint of the site you want to connec
//D1:40:5B:67:17:F5:21:9E:4D:C0:52:B6:BA:96:81:0A:7F:8B:13:DA
#define TEST_HOST_FINGERPRINT "D1 40 5B 67 17 F5 21 9E 4D C0 52 B6 BA 96 81 0A 7F 8B 13 DA"
// The finger print will change every few months.

void getJson() {
  //check fingerprint
  client.setFingerprint(TEST_HOST_FINGERPRINT);

  // Opening connection to server (Use 80 as port if HTTP)
  if (!client.connect(TEST_HOST, 443)) {
    Serial.println(F("64->Connection failed"));
    return;
  } else Serial.println(F("67->Connection successfull"));

  // give the esp a breather
  yield();


  // Send HTTP request
  client.print(F("GET "));
  // This is the second half of a request (everything that comes after the base URL)
  client.print("/api/user.status?handle=" + cfID + "&from=1&count=1");  // %2C == ,

  // HTTP 1.0 is ideal as the response wont be chunked
  // But some API will return 1.1 regardless, so we need
  // to handle both.
  client.println(F(" HTTP/1.1"));

  //Headers
  client.print(F("Host: "));
  client.println(TEST_HOST);

  client.println(F("Cache-Control: no-cache"));

  if (client.println() == 0) {
    Serial.println(F("Failed to send request"));
    return;
  } else Serial.println(F("Request sended to host"));
  //delay(100);
  // Check HTTP status
  char http_status[32] = { 0 };
  client.readBytesUntil('\r', http_status, sizeof(http_status));
  Serial.println(http_status);
  // Check if it responded "OK" with either HTTP 1.0 or 1.1
  if (strcmp(http_status, "HTTP/1.1 200 ") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(http_status);
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    return;
  }

  // For APIs that respond with HTTP/1.1 we need to remove
  // the chunked data length values at the start of the body
  //
  // peek() will look at the character, but not take it off the queue
  while (client.available() && client.peek() != '{' && client.peek() != '[') {
    char c = 0;
    client.readBytes(&c, 1);
    Serial.print(c);
    Serial.println("BAD");
  }

  // While the client is still availble read each
  // byte and print to the serial monitor
  char json_data[1000];
  int data_size = 0;
  while (client.available()) {
    char c = 0;
    client.readBytes(&c, 1);
    //Serial.print(c);
    json_data[data_size++] = c;
  }
  json_data[data_size] = '\0';

  Serial.println(json_data);

  const size_t capacity = 2 * JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(3) + JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(12) + 510;
  DynamicJsonBuffer jsonBuffer(capacity);

  const char* json = json_data;  //Json data

  JsonObject& root = jsonBuffer.parseObject(json);

  const char* status = root["status"];  // "OK"

  JsonObject& result_0 = root["result"][0];
  long result_0_id = result_0["id"];                                    // 193893475
  int result_0_contestId = result_0["contestId"];                       // 1795
  long result_0_creationTimeSeconds = result_0["creationTimeSeconds"];  // 1676563521
  int result_0_relativeTimeSeconds = result_0["relativeTimeSeconds"];   // 5421

  JsonObject& result_0_problem = result_0["problem"];
  int result_0_problem_contestId = result_0_problem["contestId"];  // 1795
  const char* result_0_problem_index = result_0_problem["index"];  // "B"
  const char* result_0_problem_name = result_0_problem["name"];    // "Ideal Point"
  const char* result_0_problem_type = result_0_problem["type"];    // "PROGRAMMING"
  int result_0_problem_rating = result_0_problem["rating"];        // 900

  JsonArray& result_0_problem_tags = result_0_problem["tags"];
  const char* result_0_problem_tags_0 = result_0_problem_tags[0];  // "brute force"
  const char* result_0_problem_tags_1 = result_0_problem_tags[1];  // "geometry"
  const char* result_0_problem_tags_2 = result_0_problem_tags[2];  // "greedy"

  JsonObject& result_0_author = result_0["author"];
  int result_0_author_contestId = result_0_author["contestId"];  // 1795

  const char* result_0_author_members_0_handle = result_0_author["members"][0]["handle"];  // "Ridwan_Islam"

  const char* result_0_author_participantType = result_0_author["participantType"];  // "CONTESTANT"
  bool result_0_author_ghost = result_0_author["ghost"];                             // false
  long result_0_author_startTimeSeconds = result_0_author["startTimeSeconds"];       // 1676558100

  const char* result_0_programmingLanguage = result_0["programmingLanguage"];  // "GNU C++17"
  const char* result_0_verdict = result_0["verdict"];                          // "WRONG_ANSWER"
  const char* result_0_testset = result_0["testset"];                          // "TESTS"
  int result_0_passedTestCount = result_0["passedTestCount"];                  // 0
  int result_0_timeConsumedMillis = result_0["timeConsumedMillis"];            // 0
  int result_0_memoryConsumedBytes = result_0["memoryConsumedBytes"];          // 0

  //.................. Json parsing end..........

  Serial.println("Json parsing successfull");
  Serial.print("status= ");
  Serial.println(status);
  Serial.print("Verdict= ");
  Serial.println(result_0_verdict);
  String date = timeStamp(result_0_creationTimeSeconds);
  Serial.println(date);
}

void setup() {
  Serial.begin(9600);
  mac = WiFi.macAddress();
  //Display preparing
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2_prepare();

  bootLogo();
  delay(3000);

  connectToWiFi();
  u8g2.sendBuffer();
  delay(2000);

  loading();
  //Firebase preparing
  delay(3000);
  firebaseSetup();
}

void loop() {
  delay(5000);
  todaySolved();
}