void targetSetup() {
  pinMode(D3, INPUT);
  pinMode(D4, INPUT);
  pinMode(D6, INPUT);

  u8g2.clearBuffer();
  u8g2.drawStr(15, 4, "Your daily target");
  u8g2.drawTriangle(30, 20, 30, 40, 20, 30);
  u8g2.drawTriangle(90, 20, 90, 40, 100, 30);

  u8g2.setFont(u8g2_font_helvR08_tr);
  u8g2.drawButtonUTF8(62, 48, U8G2_BTN_HCENTER|U8G2_BTN_BW2, 34,  2,  2, "OK" );
  //u8g2.drawButtonUTF8(60, 48, U8G2_BTN_HCENTER | U8G2_BTN_BW2, 34, 2, 2, "OK");
  u8g2.sendBuffer();
}

uint8_t target=0, pre=-1;
void targetDisp() {
  bool b1=true, b2=true, b3=true,ok=false;
  while (ok==false) {
    b1 = digitalRead(D3);
    b2 = digitalRead(D4);
    b3 = digitalRead(D6);

    if (b1 == LOW) {
      target++;
      Serial.println("D3 is on");
    } else Serial.println("D3 is off");

    if (b2 == LOW) {
      target--;
      Serial.println("D4 is on");
    } else Serial.println("D4 is off");

    if(b3==LOW){
      Serial.println("D6 is on");
      ok=true;
    }
    else Serial.println("D6 is off");

    if (pre != target) {
      u8g2.setFont(u8g2_font_ncenR18_te);
      u8g2.setDrawColor(0);
      u8g2.drawBox(31,20,59,20);
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

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenR18_te);
  u8g2.drawStr(30,10,"Target");
  u8g2.drawStr(35,32,"Saved");
  u8g2.sendBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  delay(1000);
}
