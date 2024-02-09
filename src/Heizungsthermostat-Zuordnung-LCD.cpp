#include <Wire.h>
#include <Arduino.h>

#define busSlaveAddress B0111001    //I2C Adresse des Displaytreibers
#define modeSet B11001000           //Bits zum setzen des Betriebsmodus des Displaytreibers
#define deviceSelect B11100001      //Subadresse des Displaytreibers

//Deklaration aller Funktionen
void displayByte(uint8_t pointer, uint8_t data);
void allSegmentsOn(bool on);
void oneBit();

void setup() {
  Serial.begin(115200);   //Initialisiere serielle Kommunikation
  Wire.begin();           //Initialisiere I2C-Bus
}

void loop() {
  if (Serial.available() > 0) {           //Falls auf der seriellen Schnittstelle Daten ankommen
    int incomingChar = Serial.read();     //Schreibe das ankommende Zeichen in Variable
    if (incomingChar == '1') {            //Falls ankommendes Zeichen eine "1" ist
      oneBit();                           //Rufe Funktion oneBit auf
    }
  }
}

void oneBit() {                           //Funktion die ein Segmengt auf dem Display darstellt
  static uint8_t pointer = 0;             //Definition des Pointer-Byte
  static uint8_t data = B10000000;        //Definition des Daten-Byte (enthält schon eine 1 für das erste Bit)
  static int i = 1;                       //Zählervariable

  allSegmentsOn(false);                   //Alle Segmente des Displays ausschalten
  displayByte(pointer, data);             //oben definiertes Byte an Stelle pointer = 0 darstellen

  //Ausgabe der Zähleravariable i auf der seriellen Schnittstelle
  Serial.print("Nummer: ");
  Serial.println(i);

  //Ausgabe des aktuellen Pointers (nur zur Kontrolle)
  Serial.print("Pointer: ");
  Serial.println(pointer);

  //Ausgabe des aktuellen Daten-Bytes (nur zur Kontrolle)
  Serial.print("Data: ");
  Serial.println(data, BIN);

  i++;                  //Zählervariable inkrementieren
  if (data > 1) {       //Falls data > 1 (wird null wenn Bit das siebte mal geshiftet wurde)
    data >>= 1;         //In diesem Falle (weniger als sieben mal geschiftet) Bit um eins shiften
  } else {              
    data = B10000000;   //Ansonsten Bit wieder auf erste Stelle setzen
    pointer += 2;       //Pointer um 2! inkrementieren (siehe Artikel, ein Byte füllt immer zwei Spalten)
  }
}


//Funktion, die ein Byte an den Displaytreiber überträgt (im Artikel im Detail beschrieben)
void displayByte(uint8_t pointer, uint8_t data) {
  Wire.beginTransmission(busSlaveAddress);
  Wire.write(modeSet);
  Wire.write(deviceSelect);
  Wire.write(pointer);
  Wire.write(data);
  Wire.endTransmission();
}

//Funktion die alle Display-Segmente an und ausschalten kann (Funktionstest o.Ä.)
void allSegmentsOn(bool on) {
  uint8_t data = B10000000;
  uint8_t dataPointer = B00000000;

  Wire.beginTransmission(busSlaveAddress);
  Wire.write(modeSet);
  Wire.write(deviceSelect);
  Wire.write(dataPointer);

  if (on)
    data = B11111111;
  else
    data = B00000000;

  for (uint8_t i = 0; i < 20; i++) {
    Wire.write(data);
  }
  Wire.endTransmission();
}
