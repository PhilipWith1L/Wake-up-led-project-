#include <Wire.h>
#include <RTClib.h>
#include <stdio.h>

#define ALR_HOUR 6
#define ALR_MIN 00
#define STOP_PIN 2
#define RED_PIN 3
#define GREEN_PIN 5
#define BLUE_PIN 6

RTC_DS1307 RTC;

float cur_red = 0.0;
float cur_green = 0.0;
float cur_blue = 0.0;

void set_color(byte red, byte green, byte blue) {
  cur_red = red;
  cur_green = green;
  cur_blue = blue;
}

void refresh_color() {
  cur_red = constrain(cur_red, 0.0, 255.0);
  cur_green = constrain(cur_green, 0.0, 255.0);
  cur_blue = constrain(cur_blue, 0.0, 255.0);
  analogWrite(RED_PIN, (byte)cur_red);
  analogWrite(GREEN_PIN, (byte)cur_green);
  analogWrite(BLUE_PIN, (byte)cur_blue);
}

void turn_off() {
  set_color(0, 0, 0);
  refresh_color();
}

void delay_min(int minutes) {
  unsigned long prev_millis;
  
  for(int t = 0; t < minutes; t++) {
    prev_millis = millis();
    while((millis() - prev_millis) < 60000UL) {
      if (digitalRead(STOP_PIN) == LOW)
        return;
    }
  }
}

int fade_to(byte red, byte green, byte blue, unsigned long seconds) {
  unsigned long time_step_ms = (seconds * 1000UL) / 256UL;
  unsigned long prev_millis;
  float dist_red = (float)red - cur_red;
  float dist_green = (float)green - cur_green;
  float dist_blue = (float)blue - cur_blue;
  float incr_red = dist_red / 255.0;
  float incr_green = dist_green / 255.0;
  float incr_blue = dist_blue / 255.0;
  
  if (seconds == 0) {
    set_color(red, green, blue);
    refresh_color();
    return 1;
  }

  for (int step = 0; step < 256; step++) {
    prev_millis = millis();
    while((millis() - prev_millis) < time_step_ms) {
      if (digitalRead(STOP_PIN) == LOW)
        return 0;
    }
    cur_red += incr_red;
    cur_green += incr_green;
    cur_blue += incr_blue;
    refresh_color();
  }
  return 1;
}

void do_wake_up() {
  if (!fade_to(1, 1, 10, 300)) return;     // azul
  if (!fade_to(30, 1, 1, 300)) return;     // rojo
  if (!fade_to(50, 10, 1, 300)) return;    // naranja
  if (!fade_to(75, 25, 1, 180)) return;    // amarillo
  if (!fade_to(150, 35, 1, 180)) return;   // amarillo-blanco
  if (!fade_to(255, 100, 20, 180)) return; // amarillo-blanco
}

void clearLCD(){
  Serial.write(0xFE);
  Serial.write(0x01);
  delay(10);
}

void backlightOn(){
  Serial.write(0x7C);
  Serial.write(157);
  delay(10);
}

void backlightOff(){
  Serial.write(0x7C);
  Serial.write(128);
  delay(10);
}

void setup() {
  Wire.begin();
  RTC.begin();
  pinMode(STOP_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  turn_off();
  backlightOff();
}

void loop() {
  DateTime now;
  char line[32];
  unsigned long prev_millis;

  prev_millis = millis();
  
  now = RTC.now();
  sprintf(line, "Hora: %02d:%02d:%02d  Alarma: %02d:%02d   ", now.hour(), now.minute(), now.second(), ALR_HOUR, ALR_MIN);
  Serial.print(line);
  
  if ((now.hour() == ALR_HOUR) && (now.minute() == ALR_MIN) && (now.second() < 3)) {
    do_wake_up();
    delay_min(10);
    turn_off();
  }
  
  while((millis() - prev_millis) < 1000);
}
