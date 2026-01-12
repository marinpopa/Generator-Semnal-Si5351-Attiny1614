/*
 * ATtiny414/1614 + Si5351 + LCD I2C 16x2 + Encoder
 * Optimizat: fără float, texte în PROGMEM, cod compact, afișare cu snprintf()
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <si5351.h>

// ---------------- PINI ENCODER ----------------
#define ENC_A_PIN   PIN_PA3
#define ENC_B_PIN   PIN_PA4
#define ENC_SW_PIN  PIN_PA5

// ---------------- OBIECTE ----------------
LiquidCrystal_I2C lcd(0x27, 16, 2);
Si5351 si5351;

// ---------------- CONSTANTE ----------------
const uint32_t FREQ_MIN = 8000UL;
const uint32_t FREQ_MAX = 150000000UL;

const uint32_t stepList[] = {
  1UL, 10UL, 100UL, 1000UL, 10000UL, 100000UL, 1000000UL
};
const uint8_t STEP_COUNT = sizeof(stepList) / sizeof(stepList[0]);

// ---------------- STRUCTURĂ EEPROM ----------------
struct Settings {
  uint32_t freq[3];
  bool     enabled[3];
  uint8_t  stepIndex;
  int32_t  calibration;
  uint8_t  validFlag;
};

Settings cfg;

// ---------------- VARIABILE MENIU ----------------
volatile int8_t encoderDelta = 0;
uint8_t lastA = 0;
unsigned long lastButtonTime = 0;
const unsigned long debounceTime = 200;

uint8_t currentCLK = 0;
int8_t mainPos = 0;

// ---------------- FUNCȚII LCD OPTIMIZATE ----------------

// Scrie o linie completă (16 caractere)
void lcdPrintLine(uint8_t row, const char* text) {
    char buf[17];
    snprintf(buf, sizeof(buf), "%-16s", text);
    lcd.setCursor(0, row);
    lcd.print(buf);
}

// Formatează frecvența într-un buffer
void formatFreq(char* out, uint32_t f) {

    // MHz: 1.000000MHz ... 160.000000MHz
    if (f >= 1000000UL) {
        uint32_t mhz = f / 1000000UL;
        uint32_t frac = f % 1000000UL;   // 0 ... 999999

        snprintf(out, 17, "%lu.%06luMHz", mhz, frac);
        return;
    }

    // kHz: 8.000kHz ... 999.999kHz
    uint32_t khz = f / 1000UL;
    uint32_t frac = f % 1000UL;          // 0 ... 999

    snprintf(out, 17, "%lu.%03lukHz", khz, frac);
}



// Formatează pasul
void formatStep(char* out, uint32_t st) {
    if (st >= 1000000UL) snprintf(out, 17, "%luMHz", st / 1000000UL);
    else if (st >= 1000UL) snprintf(out, 17, "%lukHz", st / 1000UL);
    else snprintf(out, 17, "%luHz", st);
}

// ---------------- PROTOTIPURI ----------------
void readEncoder();
bool buttonPressed();
void applyAllFrequencies();
void applyOneFrequency(uint8_t clk);
void loadEEPROM();
void saveEEPROM();

void showMainMenu();
void showCLKMenu(uint8_t clk);
void showCLKFreqEdit(uint8_t clk);
void showCLKStepMenu();
void showCLKOnOffMenu(uint8_t clk);
void showCalibrationMenu();

// ---------------- SETUP ----------------
void setup() {
  Wire.begin();
  lcd.init();
  lcd.backlight();

  pinMode(ENC_A_PIN, INPUT_PULLUP);
  pinMode(ENC_B_PIN, INPUT_PULLUP);
  pinMode(ENC_SW_PIN, INPUT_PULLUP);

  loadEEPROM();

  si5351.init(SI5351_CRYSTAL_LOAD_8PF, 25000000UL, cfg.calibration);
  applyAllFrequencies();

  lcdPrintLine(0, "Gen Ready");
  delay(600);
}

// ---------------- LOOP ----------------
void loop() {
  showMainMenu();
}

// ================= EEPROM =================
void loadEEPROM() {
  EEPROM.get(0, cfg);
  if (cfg.validFlag != 0xA5) {
    cfg.freq[0] = cfg.freq[1] = cfg.freq[2] = 1000000UL;
    cfg.enabled[0] = cfg.enabled[1] = cfg.enabled[2] = true;
    cfg.stepIndex = 3;
    cfg.calibration = 0;
    cfg.validFlag = 0xA5;
    saveEEPROM();
  }
}

void saveEEPROM() {
  EEPROM.put(0, cfg);
}

// ================= ENCODER / BUTON =================
void readEncoder() {
  static uint8_t oldState = 3;
  static uint32_t lastChange = 0;
  
  uint32_t now = millis();
  if (now - lastChange < 5) return; // Debounce 5ms
  
  uint8_t newState = (digitalRead(ENC_A_PIN) << 1) | digitalRead(ENC_B_PIN);
  
  if (newState != oldState) {
    // Tranziții valide pentru encoder
    uint8_t transition = (oldState << 2) | newState;
    
    if (transition == 0b0001 || transition == 0b0111 || 
        transition == 0b1110 || transition == 0b1000) {
      encoderDelta++; // Rotire spre dreapta
    }
    else if (transition == 0b0010 || transition == 0b0100 || 
             transition == 0b1101 || transition == 0b1011) {
      encoderDelta--; // Rotire spre stânga
    }
    
    oldState = newState;
    lastChange = now;
  }
}

bool buttonPressed() {
  if (!digitalRead(ENC_SW_PIN)) {
    unsigned long now = millis();
    if (now - lastButtonTime > debounceTime) {
      lastButtonTime = now;
      while (!digitalRead(ENC_SW_PIN));
      delay(10);
      return true;
    }
  }
  return false;
}

// ================= APLICARE FRECVENȚE =================
void applyOneFrequency(uint8_t clk) {
  uint8_t clkNum = SI5351_CLK0 + clk;

  if (cfg.enabled[clk]) {
    uint32_t f = cfg.freq[clk];
    if (f < FREQ_MIN) f = FREQ_MIN;
    if (f > FREQ_MAX) f = FREQ_MAX;
    si5351.set_freq((uint64_t)f * 100ULL, clkNum);
    si5351.output_enable(clkNum, 1);
  } else {
    si5351.output_enable(clkNum, 0);
  }
}

void applyAllFrequencies() {
  for (uint8_t i = 0; i < 3; i++) applyOneFrequency(i);
}

// ================= MENIU PRINCIPAL =================
void showMainMenu() {
  lcdPrintLine(0, "Main: CLK/Calib");
  encoderDelta = 0;

  while (1) {
    char line[17];
    if (mainPos <= 2) snprintf(line, 17, "CLK%u", mainPos);
    else snprintf(line, 17, "Calib");

    lcdPrintLine(1, line);

    readEncoder();
    if (encoderDelta != 0) {
      mainPos += encoderDelta;
      encoderDelta = 0;
      if (mainPos < 0) mainPos = 3;
      if (mainPos > 3) mainPos = 0;
    }

    if (buttonPressed()) {
      if (mainPos <= 2) showCLKMenu(mainPos);
      else showCalibrationMenu();
      return;
    }
  }
}

// ================= MENIU CLK =================
void showCLKMenu(uint8_t clk) {
  const char* const items[] = {"Freq", "OnOff", "Step", "bak"};
  int8_t index = 0;
  encoderDelta = 0;

  while (1) {
    char title[17];
    snprintf(title, 17, "CLK%u %s", clk, items[index]);
    lcdPrintLine(0, title);

    char line[17];

    if (index == 0) {
      formatFreq(line, cfg.freq[clk]);
    }
    else if (index == 1) {
      snprintf(line, 17, "State:%s", cfg.enabled[clk] ? "ON" : "OFF");
    }
    else if (index == 2) {
      char st[17];
      formatStep(st, stepList[cfg.stepIndex]);
      snprintf(line, 17, "Step:%s", st);
    }
    else {
      snprintf(line, 17, "<-bak");
    }

    lcdPrintLine(1, line);

    while (1) {
      readEncoder();
      if (encoderDelta != 0) {
        index += encoderDelta;
        encoderDelta = 0;
        if (index < 0) index = 3;
        if (index > 3) index = 0;
        break;
      }

      if (buttonPressed()) {
        if (index == 0) showCLKFreqEdit(clk);
        else if (index == 1) showCLKOnOffMenu(clk);
        else if (index == 2) showCLKStepMenu();
        else return;
        break;
      }
    }
  }
}

// ================= EDIT FRECVENȚĂ =================
void showCLKFreqEdit(uint8_t clk) {
  char title[17];
  snprintf(title, 17, "Edit CLK%u", clk);
  lcdPrintLine(0, title);

  encoderDelta = 0;

  while (1) {
    char line[17];
    formatFreq(line, cfg.freq[clk]);
    lcdPrintLine(1, line);

    readEncoder();
    if (encoderDelta != 0) {
      int32_t nf = (int32_t)cfg.freq[clk] +
                   (int32_t)encoderDelta * (int32_t)stepList[cfg.stepIndex];
      encoderDelta = 0;

      if (nf < (int32_t)FREQ_MIN) nf = FREQ_MIN;
      if (nf > (int32_t)FREQ_MAX) nf = FREQ_MAX;

      cfg.freq[clk] = (uint32_t)nf;
      applyOneFrequency(clk);
      saveEEPROM();
    }

    if (buttonPressed()) return;
  }
}

// ================= STEP MENU =================
void showCLKStepMenu() {
  lcdPrintLine(0, "Step size");
  encoderDelta = 0;

  while (1) {
    char st[17], line[17];
    formatStep(st, stepList[cfg.stepIndex]);
    snprintf(line, 17, "Step:%s", st);
    lcdPrintLine(1, line);

    readEncoder();
    if (encoderDelta != 0) {
      int8_t idx = cfg.stepIndex + encoderDelta;
      encoderDelta = 0;
      if (idx < 0) idx = 0;
      if (idx >= STEP_COUNT) idx = STEP_COUNT - 1;
      cfg.stepIndex = idx;
      saveEEPROM();
    }

    if (buttonPressed()) return;
  }
}

// ================= ON/OFF MENU =================
void showCLKOnOffMenu(uint8_t clk) {
  char title[17];
  snprintf(title, 17, "CLK%u ON/OFF", clk);
  lcdPrintLine(0, title);
  encoderDelta = 0;

  while (1) {
    char line[17];
    snprintf(line, 17, "State:%s", cfg.enabled[clk] ? "ON" : "OFF");
    lcdPrintLine(1, line);

    readEncoder();
    if (encoderDelta != 0) {
      encoderDelta = 0;
      cfg.enabled[clk] = !cfg.enabled[clk];
      applyOneFrequency(clk);
      saveEEPROM();
    }

    if (buttonPressed()) return;
  }
}

// ================= CALIBRARE =================
void showCalibrationMenu() {
  lcdPrintLine(0, "Calib Si5351");
  encoderDelta = 0;

  while (1) {
    char line[17];
    snprintf(line, 17, "Corr:%ld", cfg.calibration);
    lcdPrintLine(1, line);

    readEncoder();
    if (encoderDelta != 0) {
      cfg.calibration += encoderDelta;
      encoderDelta = 0;

      si5351.init(SI5351_CRYSTAL_LOAD_8PF, 25000000UL, cfg.calibration);
      applyAllFrequencies();
      saveEEPROM();
    }

    if (buttonPressed()) return;
  }
}
