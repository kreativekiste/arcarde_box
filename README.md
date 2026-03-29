# 🕹️ Arcade Game Box V1 - Arduino Uno R4 Minima von kreativekiste.de

Die **Arcade Game Box** ist ein multifunktionales Retro-Gaming-Handheld auf Basis des **Arduino Uno R4 Minima**. Neben 10 vorinstallierten Spielen bietet die Box eine integrierte Wetterstation, eine Analoguhr und nützliche Alltags-Tools wie einen Taschenrechner und eine Stoppuhr.

## ✨ Features
- **10 Games:** Von Breakout über Vier Gewinnt bis hin zu Simon Sagt.
- **Smart Screen Saver:** Automatische Analoguhr mit Live-Daten (Temperatur & Feuchtigkeit via DHT22).
- **Hardware-Test-Modus:** Integriertes Diagnose-Tool für LEDs, Buzzer und RTC.
- **Easter Eggs:** Schneller Zugriff auf Stoppuhr und Smiley-Boost über Hotkeys.
- **Modularer Code:** Sauber strukturierter Master-Code mit Registerkarten (Tabs).

## 🛠️ Hardware-Komponenten
- **Controller:** Arduino Uno R4 Minima
- **Display:** 3.5" TFT LCD (ILI9486 SPI)
- **Sensorik:** DHT22 (Temperatur & Luftfeuchtigkeit), Interne RTC
- **Interaktion:** Rotary Encoder + 4 Arcade-Buttons
- **Effekte:** 16er NeoPixel Ring & Piezo-Buzzer
- **Strom:** 3V CR2032 Batterie am VRTC-Pin (für die Uhrzeit)

## 📂 Software-Struktur (Tabs)
Das Projekt nutzt eine modulare Struktur, um den Code übersichtlich zu halten:
- `master.ino`: Die Schaltzentrale (Setup, Loop, LED-Handler).
- `analogclock.ino`: Die Logik des Bildschirmschoners und der Sensoren.
- `calculator.ino` / `stopwatch.ino`: Die Utility-Tools.
- `games_... .ino`: Jedes Spiel hat seinen eigenen Tab.
- `setup_menu.ino`: Das Einstellungsmenü für Farben und Effekte.

## 🚀 Installation
1. Lade dir alle `.ino` Dateien in einen Ordner namens `master`.
2. Installiere folgende Libraries in deiner Arduino IDE:
   - `Adafruit_NeoPixel`
   - `DHT sensor library` (Adafruit)
   - `ILI9486_SPI`
   - `RTC` (Arduino Renesas Library)
3. Verbinde deinen Arduino Uno R4 Minima und lade den Sketch hoch.

## 📜 Lizenz
Dieses Projekt ist unter der MIT-Lizenz lizenziert.

Credits & Entwicklung
Dieses Projekt wurde von kreativekiste.de konzipiert und gebaut. Die komplexe Software-Architektur und das Debugging der Spiele-Logik entstanden im interaktiven Dialog mit Gemini AI. Ein Beweis dafür, wie Mensch und KI gemeinsam beeindruckende Hardware-Projekte realisieren können.
