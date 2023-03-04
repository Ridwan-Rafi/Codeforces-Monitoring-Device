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

//IP Address and MAC
IPAddress ip;
char IP[17];
String mac;

//Global User name of Codeforces and target
String cfID = String('~');
int cfTarget = -1, valid = -1;
int baki = -1;
uint8_t target = 0, pre = -1;

#define Button1 D3
#define Button2 D4
#define Button3 D8
bool bt1, bt2, bt3, ok;

//custom Header file
//#include "setupCF.h"
#include "bootLogo.h"
//#include "timeStamp.h"
#include "targetUpdt.h"




//Setup web server at 80 port
ESP8266WebServer webserver(80);



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
  u8g2.drawStr(40, 10, "Welcome");
  char tmpID[100];
  cfID.toCharArray(tmpID, 100);
  u8g2.drawStr(20, 30, tmpID);
  u8g2.sendBuffer();
  delay(1000);
}


int isValid() {
  Firebase.setInt("/Valid/" + mac, -1);
  delay(70);
  u8g2.clearBuffer();
  u8g2.drawStr(45, 20, "Username is");
  u8g2.drawStr(40, 30, "Checking...");
  u8g2.sendBuffer();
  Serial.println("Valid user name cheking checking");

  if (Firebase.success()) {
    while (1) {
      delay(80);
      valid = Firebase.getInt("/Valid/" + mac);
      delay(80);
      Serial.println("Valid = " + String(valid));
      if (Firebase.success()) {
        if (valid == 1 || valid == 0) return valid;
      } else {
        Serial.println("Firebase failed");'
        valid=-2;'
        return -2;
      }
    }
  } else {
    Serial.println("Firebase failed");
    valid=-2;
    return -2;
  }
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
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  delay(50);
  Firebase.setString("/User/" + mac, cfID);
  delay(100);
  while(!Firebase.success())
  {
    Serial.println("Sending user name to firebase");
    Firebase.setString("/User/" + mac, cfID);
  delay(50);
  }

  while (isValid() == 0 || valid == -2) {
    cfID = "~";
    if (valid == -2) {
      u8g2.clearBuffer();
      u8g2.drawStr(0, 0, "No internet");
      u8g2.drawStr(0, 10, " wait..");
    } else if (valid == 0) {
      u8g2.clearBuffer();
      u8g2.drawStr(0, 0, "Wrong username");
      u8g2.drawStr(0, 10, "Insert again");
      u8g2.drawStr(0, 20, "Visit this IP:");
      u8g2.drawStr(0, 30, IP);
    }
    u8g2.sendBuffer();

    webserver.begin();
    Serial.println("Web server has started");
    cfID="~";
    while (cfID == String('~')) {
      webserver.handleClient();
    }
    webserver.stop();

    Firebase.setString("/User/" + mac, cfID);
    Firebase.setInt("/Valid/"+mac,-1);
    delay(50);
  }
}

void getTarget() {
  targetSetup();
  targetDisp();
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
  } else {
    Serial.println("Firebase connected");
    Serial.println(mac);
    String name ;
    //name = Firebase.getString("/User/" + mac);
    //delay(50);
    int trgt = Firebase.getInt("/Limit/" + mac);
    delay(150);

    if (Firebase.success() && name.length() != 0) {
      Serial.println("Old User");
      cfID = name;
      cfTarget = trgt;
      welcome();
    } else {
      Serial.println("New User");
      //Username input
      u8g2.clearBuffer();
      cfUser();
      welcome();
      delay(1000);
      getTarget();
      //String logs;
      Firebase.setString("/User/" + mac, cfID);
      delay(60);
      Firebase.setInt("/Limit/" + mac, cfTarget);
      delay(50);
      //Serial.println(logs);
      if (!Firebase.success()) {
        u8g2.clearBuffer();
        u8g2.drawStr(0, 0, "Something went wrong");
      } else {
        Serial.println("Data saved in Firebase ");
      }
    }
  }
}

void resetting() {
  cfID = "~";
  Serial.println("New User");
  //Username input
  u8g2.clearBuffer();
  cfUser();
  delay(1000);
  getTarget();
  //String logs;
  Serial.println("New user name = " + cfID);
  Firebase.setString("/User/" + mac, cfID);
  delay(50);
  Firebase.setInt("/Limit/" + mac, cfTarget);
  //Serial.println(logs);
  if (!Firebase.success()) {
    u8g2.clearBuffer();
    u8g2.drawStr(0, 0, "Something went wrong");
  } else {
    welcome();
  }
}

int prebaki = -1000000, todaySolve = 0;

void todaySolved() {
  todaySolve = Firebase.getInt("/TodaySolve/" + mac);
  if (Firebase.success()) {
    Serial.println("Target=" + String(cfTarget) + "    Today solve" + String(todaySolve));
    baki = cfTarget - todaySolve;
    if (ok == 1) {
      prebaki = -1000000;
      ok = 0;
    }
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
  } else {
    u8g2.drawStr(20, 55, "No Internet");
    u8g2.sendBuffer();
  }
}

void reset() {
  u8g2.clear();
  u8g2.drawStr(20, 0, "Do you want to");
  u8g2.drawStr(40, 10, "reset?");
  u8g2.setFont(u8g2_font_helvR08_tr);
  u8g2.drawButtonUTF8(20, 48, U8G2_BTN_HCENTER | U8G2_BTN_BW2, 34, 2, 2, "Yes");
  u8g2.drawButtonUTF8(90, 48, U8G2_BTN_HCENTER | U8G2_BTN_BW2, 34, 2, 2, "No");
  u8g2.sendBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  delay(100);
  Serial.println("reset oled printed");
  while (1) {
    if (digitalRead(Button1) == HIGH) {
      delay(100);
      return;
    } else if (digitalRead(Button2) == HIGH) {
      delay(100);
      resetting();
      return;
    }
    delay(2);
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(Button1, INPUT);
  pinMode(Button2, INPUT);
  pinMode(Button3, INPUT);
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
int step;

void loop() {
  step = 3000;
  bt3 = LOW, bt1 = LOW;
  while (step-- && bt1 == LOW && bt3 == LOW) {
    bt1 = digitalRead(Button1);
    bt3 = digitalRead(Button3);
    delay(1);
  }
  Serial.println("Bt1=" + String(bt1) + "  Bt2=" + String(bt2) + "  Bt3=" + String(bt3));
  delay(100);

  if (bt3 == HIGH) {
    reset();
    delay(100);
    Serial.println("Bt1=" + String(bt1) + "  Bt2=" + String(bt2) + "  Bt3=" + String(bt3));
    delay(10000);
  } else if (bt1 == HIGH) {
    getTarget();
  }

  todaySolved();
}