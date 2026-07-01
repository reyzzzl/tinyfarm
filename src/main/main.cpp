#include <Arduino.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <LiquidCrystal_I2C.h>

// config pin
#define PIN_ONEWIRE   18
#define PIN_LIGHT     4
#define PIN_SMOKE     16
#define PIN_LAMP      19
#define PIN_BUZZER    2
#define PIN_SDA       21
#define PIN_SCL       22

// lcd
#define LCD_ADDR      0x27
#define LCD_COLS      16
#define LCD_ROWS      2

// thresholds
// TODO: move to eprom so its adjustable at runtime
#define THRESH_TEMP   60.0f
#define THRESH_DARK   5
#define THRESH_SMOKE  7
#define OFFSET_TEMP   13.0f  // FIXME: per-unit mybe calibration needed

// debounce intevals ms
#define DEB_LIGHT     2000
#define DEB_SMOKE     3000
#define DEB_TEMP      5000
#define INTERVAL_TEMP 2000
#define INTERVAL_SENS 300
#define INTERVAL_LCD  1000

// buzzer config
#define BUZZ_ON_MS    200
#define BUZZ_OFF_MS   100
#define BUZZ_PULSES   5

// calibration sampls
#define CALIB_SAMPLES 50
#define LIGHT_SAMPLES 20
#define SMOKE_SAMPLES 10

OneWire wire(PIN_ONEWIRE);
DallasTemperature ds(&wire);
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

struct State {
    float temp      = 0;
    bool dark       = false;
    bool smoke      = false;
    bool tempHigh   = false;
    bool fire       = false;
    bool sensorOk   = false;
    bool calibrated = false;
    int  baseline   = 0;
};

struct Alarm {
    bool active  = false;
    bool state   = false;
    bool done    = false;
    int  count   = 0;
    unsigned long lastBuzz = 0;
};

struct Timing {
    unsigned long temp   = 0;
    unsigned long sensor = 0;
    unsigned long light  = 0;
    unsigned long smoke  = 0;
    unsigned long tempCh = 0;
    unsigned long lcd    = 0;
};

State s;
Alarm a;
Timing t;

// lcd
void lcdInit() {
    Wire.begin(PIN_SDA, PIN_SCL);
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("go to boot");
    lcd.setCursor(0, 1); lcd.print("Tinyfarm gooo");
    delay(2000);
    lcd.clear();
}

void lcdUpdate() {
    unsigned long now = millis();
    if (now - t.lcd < INTERVAL_LCD) return;
    t.lcd = now;

    lcd.setCursor(0, 0);
    if (s.fire)      lcd.print("alarm           ");
    else if (s.smoke) lcd.print("smoke detected  ");
    else if (s.tempHigh) lcd.print("high temp       ");
    else if (s.dark) lcd.print("dark - lamp on  ");
    else             lcd.print("all clear       ");

    lcd.setCursor(0, 1);
    char buf[17];
    if (s.sensorOk) sprintf(buf, "%.1fC L:%s S:%s", s.temp, s.dark ? "on" : "of", s.smoke ? "y" : "n");
    else            sprintf(buf, "--C  L:%s S:%s",         s.dark ? "on" : "of", s.smoke ? "y" : "n");
    lcd.print(buf);
}

// sensors
void calibrateLight() {
    int total = 0;
    for (int i = 0; i < CALIB_SAMPLES; i++) { total += digitalRead(PIN_LIGHT); delay(100); }
    s.baseline   = total;
    s.calibrated = true;
    Serial.printf("[calib] light baseline: %d\n", s.baseline);
}

bool readLight() {
    if (!s.calibrated) return false;
    int r = 0;
    for (int i = 0; i < LIGHT_SAMPLES; i++) {
        if (digitalRead(PIN_LIGHT) == HIGH) r++;
        delay(50);
    }
    return r >= THRESH_DARK;
}

bool readSmoke() {
    int r = 0;
    for (int i = 0; i < SMOKE_SAMPLES; i++) {
        if (digitalRead(PIN_SMOKE) == LOW) r++;
        delay(50);
    }
    Serial.printf("[smoke] low count: %d/%d\n", r, SMOKE_SAMPLES);
    return r >= THRESH_SMOKE;
}

float readTemp() {
    ds.requestTemperatures();
    float v = ds.getTempCByIndex(0);
    if (v == DEVICE_DISCONNECTED_C || v < -55 || v > 125) {
        s.sensorOk = false;
        return -127.0f;
    }
    s.sensorOk = true;
    return v - OFFSET_TEMP;
}

// buzzer logic 

void alarmStart() {
    if (a.active) return;
    a = { true, false, false, 0, millis() };
    digitalWrite(PIN_BUZZER, LOW);
    Serial.println("[alarm] started");
}

void alarmStop() {
    a = {};
    digitalWrite(PIN_BUZZER, LOW);
    Serial.println("[alarm] stopped");
}

void buzzTick() {
    if (!a.active || a.done) return;
    unsigned long now = millis();
    unsigned long interval = a.state ? BUZZ_ON_MS : BUZZ_OFF_MS;
    if (now - a.lastBuzz < interval) return;

    a.lastBuzz = now;
    a.state    = !a.state;
    digitalWrite(PIN_BUZZER, a.state ? HIGH : LOW);
    if (!a.state && ++a.count >= BUZZ_PULSES) {
        a.done = true;
        digitalWrite(PIN_BUZZER, HIGH);
    }
}

// main sensor loop
void checkSensors() {
    unsigned long now = millis();
    if (now - t.sensor < INTERVAL_SENS) return;
    t.sensor = now;

    bool dark  = readLight();
    bool smoke = readSmoke();

    if (now - t.temp > INTERVAL_TEMP) {
        s.temp = readTemp();
        t.temp = now;
        if (s.sensorOk) Serial.printf("[temp] %.1fC\n", s.temp);
        else            Serial.println("[temp] sensor error");
    }

    bool hiTemp = s.sensorOk && s.temp > THRESH_TEMP;

    if (dark != s.dark && now - t.light > DEB_LIGHT) {
        s.dark  = dark; t.light = now;
        digitalWrite(PIN_LAMP, s.dark ? HIGH : LOW);
        Serial.printf("[light] %s\n", s.dark ? "dark, lamp on" : "bright, lamp off");
    }

    if (smoke != s.smoke && now - t.smoke > DEB_SMOKE) {
        s.smoke  = smoke; t.smoke = now;
        Serial.printf("[smoke] %s\n", smoke ? "detected" : "clear");
    }

    if (hiTemp != s.tempHigh && now - t.tempCh > DEB_TEMP) {
        s.tempHigh  = hiTemp; t.tempCh = now;
        Serial.printf("[temp] %s\n", s.tempHigh ? "high" : "normal");
    }

    bool danger = s.smoke || s.tempHigh;
    if (danger && !s.fire) {
        s.fire = true;
        alarmStart();
    } else if (!danger && s.fire) {
        s.fire = false;
        alarmStop();
    }

    lcdUpdate();
}

// setup and loop

void setup() {
    Serial.begin(115200);
    Serial.println("[sys] booting...");

    pinMode(PIN_LIGHT,  INPUT_PULLUP);
    pinMode(PIN_SMOKE,  INPUT_PULLUP);
    pinMode(PIN_LAMP,   OUTPUT);
    pinMode(PIN_BUZZER, OUTPUT);
    digitalWrite(PIN_LAMP,   LOW);
    digitalWrite(PIN_BUZZER, LOW);

    lcdInit();
    ds.begin();

    lcd.clear(); lcd.setCursor(0, 0); lcd.print("calibrating...");
    calibrateLight();

    s.temp = readTemp();
    Serial.printf("[temp] init: %s\n", s.sensorOk ? String(s.temp, 1).c_str() : "sensor error");

    Serial.println("[sys] ready");
    delay(2000);
    lcd.clear();
}

void loop() {
    checkSensors();
    buzzTick();
    delay(50);
}