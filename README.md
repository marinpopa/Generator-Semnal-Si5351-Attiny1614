# Si5351 + ATtiny1614 Frequency Generator - Programmable Signal Source

![Project Banner](https://img.shields.io/badge/Platform-Arduino-blue)
![License](https://img.shields.io/badge/License-MIT-green)
![Frequency Range](https://img.shields.io/badge/Frequency-8kHz--150MHz-orange)

A professional-grade programmable frequency generator built with **ATtiny1614 microcontroller** and **Si5351 clock generator module**. This open-source instrument generates three independent frequency outputs with high precision and flexible control.

## ✨ Features

### 🎛️ **Core Specifications**
- **Microcontroller**: ATtiny1614/414 (modern AVR architecture)
- **Clock Generator**: Si5351 programmable oscillator
- **Output Channels**: 3 independent outputs (CLK0, CLK1, CLK2)
- **Frequency Range**: 8 kHz to 150 MHz (extendable to 160 MHz)
- **Resolution**: Down to 1 Hz steps
- **Interface**: I2C 16×2 LCD + Rotary encoder with push button
- **Storage**: EEPROM for saving settings

### 📊 **Technical Highlights**
- **Six-decimal precision** in MHz mode (X.YYYYYY MHz)
- **Three-decimal precision** in kHz mode (XXX.XXX kHz)
- **Real-time calibration** with Si5351 crystal correction
- **Automatic save** of all settings to non-volatile memory
- **Modular menu system** with intuitive navigation

## 🖥️ User Interface

### Hardware Components
- **Rotary Encoder**: Navigation and value adjustment
- **Encoder Button**: Selection and menu confirmation
- **16×2 I2C LCD**: Clear display of menus and values
- **Status LED**: Visual feedback (optional)

### Navigation System
- **Rotate**: Browse options or adjust values
- **Press**: Select/enter menu or confirm action
- **"bak" option**: Return to previous menu level

## 📱 Menu Structure

### Main Menu (Startup)..
- **At startup, the following options are available: CLK0, CLK1, CLK2, and Calib. Rotate the encoder to select, press to enter.
##CLK Menu
- **Each output provides: Freq (frequency adjustment), OnOff (enable/disable), Step (step size selection), bak (return).


### Frequency Adjustment
- **Range**: 8 kHz minimum, 160 MHz maximum
- **Format**: Auto-switching between MHz/kHz display
- **Storage**: Automatically saved to EEPROM
- **Limits**: Protected against out-of-range values

### Step Size Selection
Choose from 7 precision levels:
- **Fine**: 1 Hz, 10 Hz, 100 Hz
- **Medium**: 1 kHz, 10 kHz
- **Coarse**: 100 kHz, 1 MHz

### Output Control
- **Enable/Disable**: Individual channel control
- **State memory**: ON/OFF status preserved
- **Instant apply**: Changes take effect immediately

### Calibration Menu
- **Real-time adjustment**: Immediate frequency correction
- **Crystal compensation**: ±30 ppm typical range
- **Auto-save**: Calibration values stored permanently

## 💾 EEPROM Storage

The following parameters are automatically preserved:
- **Frequency settings** for all three channels
- **ON/OFF states** for each output
- **Step size** selection per channel
- **Calibration value** for Si5351 crystal

## 🚀 Getting Started

### Prerequisites
- **Arduino IDE** with [megaTinyCore](https://github.com/SpenceKonde/megaTinyCore)
- **Si5351 Library** by NT7S or Etherkit
- **LiquidCrystal_I2C** library
- **RotaryEncoder** library

### Wiring Diagram

ATtiny1614 ── I2C ── 16×2 LCD
│
├── SDA/SCL ── Si5351
│
└── Pins ── Rotary Encoder

## 🔧 How to Build & Flash
1. Install **megaTinyCore** in Arduino IDE  
2. Choose the ATtiny1614/1604/814/804/441/404/241/204 
3. Select board: **ATtiny1614**  
4. Programmer: **UPDI (jtag2updi / serialUPDI)**  
5. Flash the firmware from `/firmware`  
6. Calibrate using reference frequency source

## ⚠️ Important Notes

### Operating Limits
- **Minimum frequency**: 8 kHz (Si5351 limitation)
- **Maximum frequency**: 160 MHz (absolute maximum)
- **Recommended max**: 150 MHz for stable operation

### Calibration Recommendations
- Perform initial calibration with known reference
- Recalibrate if frequency drift is observed
- Use stable power supply for best accuracy
- Allow warm-up time for temperature stability

## 🔧 Troubleshooting

| Issue | Solution |
|-------|----------|
| No display | Check I2C address and connections |
| Unstable frequency | Verify power supply quality |
| Cannot set frequency | Check Si5351 library initialization |
| Settings not saved | Verify EEPROM write cycles |

## 📈 Performance

- **Frequency stability**: ±(calibration accuracy + crystal drift)
- **Phase noise**: Dependent on Si5351 performance
- **Output levels**: Si5351 typical (3.3V CMOS)
- **Current consumption**: <50mA typical

## 🤝 Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Submit a pull request with detailed description

## 📄 License

MIT License - See LICENSE file for details

## 🙏 Acknowledgments

- **Si5351 Library Maintainers**: NT7S, Etherkit
- **megaTinyCore**: Spence Konde
- **AVR Community**: For continuous support and development

---

**Keywords**: frequency generator, Si5351, ATtiny1614, signal generator, clock generator, programmable oscillator, RF source, lab equipment, DIY instrumentation, Arduino project, electronics hobbyist, microcontroller project

**Categories**: Electronics, Test Equipment, Arduino, Microcontrollers, RF Design




