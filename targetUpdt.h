void targetSetup() {

  u8g2.clearBuffer();
  u8g2.drawStr(15, 4, "Your daily target");
  u8g2.drawTriangle(30, 20, 30, 40, 20, 30);
  u8g2.drawTriangle(90, 20, 90, 40, 100, 30);

  u8g2.setFont(u8g2_font_ncenR18_te);
  u8g2.setCursor(40, 20);
  u8g2.print(target);  
  u8g2.setFont(u8g2_font_helvR08_tr);
  u8g2.drawButtonUTF8(62, 48, U8G2_BTN_HCENTER | U8G2_BTN_BW2, 34, 2, 2, "OK");
  //u8g2.drawButtonUTF8(60, 48, U8G2_BTN_HCENTER | U8G2_BTN_BW2, 34, 2, 2, "OK");
  u8g2.sendBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
}

void targetDisp() {
  ok = false;
  delay(100);
  while (ok == false) {
    bt1 = digitalRead(Button1);
    bt2 = digitalRead(Button2);
    bt3 = digitalRead(Button3);

    if (bt1 == HIGH) {
      target++;
      Serial.println("D3 is on");
    } else if (bt2 == HIGH) {
      digitalWrite(Button2, HIGH);
      target--;
      Serial.println("D4 is on");
    } else if (bt3 == HIGH) {
      digitalWrite(Button3, HIGH);
      Serial.println("D8 is on");
      ok = true;
    } else Serial.println("D8 is off");

    if (pre != target) {
      u8g2.setFont(u8g2_font_ncenR18_te);
      u8g2.setDrawColor(0);
      u8g2.drawBox(31, 20, 59, 20);
      u8g2.sendBuffer();
      u8g2.setDrawColor(1);
      u8g2.setCursor(40, 20);
      u8g2.print(target);
      u8g2.sendBuffer();
      pre = target;
      u8g2.setFont(u8g2_font_6x10_tf);
      delay(100);
    }
    delay(50);
  }

  Firebase.setInt("/Limit/" + mac, target);
  //Serial.println(logs);
  if (!Firebase.success()) {
    u8g2.clearBuffer();
    u8g2.drawStr(0, 0, "Something went wrong");
  } else {
    cfTarget = target;
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenR18_te);
    u8g2.drawStr(30, 10, "Target");
    u8g2.drawStr(35, 32, "Saved");
    u8g2.sendBuffer();
    u8g2.setFont(u8g2_font_6x10_tf);
    ok = 1;
  }

  delay(1000);
}
