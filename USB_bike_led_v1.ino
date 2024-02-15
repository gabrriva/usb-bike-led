/*
 * 
 USB BIKE LED V1 By Gabriele Riva
 Canale Youtube: https://www.youtube.com/@gabrieleriva_elettronica 
 
*/

//#include <DigiKeyboard.h> // usato per stampa di debug su notepad
#include <PinButton.h> // libreria per gastione pulsante
#include <EEPROM.h>

// assegnazione PIN
int in0 = 0; // input 0 (pulsante)
int out1 = 1; // output CE  (abilitazione driver CN5711)
int out2 = 2; // output HC  (alta corrente Led)
PinButton myButton(0, INPUT_PULLUP);

// assegnazione variabili e costanti ------------------------------------
word interval_ON; // tempo ON
word interval_OFF; // tempo OFF
int ledState = 0;
int count; // conteggio impulsi pulsante
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
const unsigned int address = 0; // Indirizzo EEprom
bool fronte1; // bit per fronte 1
bool prev_fronte1; // bit per stato precedente fronte 1
bool longClickON = 0;
int n = 0; // conteggio num. flash

void setup() {
  pinMode(out1, OUTPUT);
  pinMode(out2, OUTPUT);
  digitalWrite(out1, LOW);
  digitalWrite(out2, LOW);
  //DigiKeyboard.delay(3000); // usato per stampa di debug su notepad

  count = EEPROM.read(address); // legge EEprom
  //DigiKeyboard.println(count);
}


void flash() {
  currentMillis = millis();

  // Modo 0 (1 impulso alta corrente) ====================================================
  //out1 __|ˉ|__________
  //out2 __|ˉ|__________
  if (count == 0) {
    if (n == 1) {
      interval_ON = 110;
      interval_OFF = 890;
    }
    else if (n > 1) n = 0;
  }

  // Modo 1 (1 impulso bassa corrente) ===================================================
  //out1 __|ˉ|__________
  //out2 _______________
  if (count == 1) {
    if (n == 1) {
      interval_ON = 200;
      interval_OFF = 600;
    }
    else if (n > 1) n = 0;
  }

  // Modo 2 (3 impulsi misto alta/bassa corrente)  ========================================
  //out1 __|ˉ|__|ˉ|__|ˉˉ|________
  //out2 ____________|ˉˉ|________
  else if (count == 2) {
    if (n >= 1 && n <= 2) {
      interval_ON = 20;
      interval_OFF = 200;
    }
    else if (n == 3) {
      interval_ON = 200;
      interval_OFF = 600;
    }
    else if (n == 4) n = 0;
  }

  // Modo 3 (2 impulsi misto alta/bassa corrente)  ========================================
  //out1 __|ˉ|__|ˉ|__________
  //out2 __|ˉ|_______________
  else if (count == 3) {
    if (n == 1) {
      interval_ON = 100;
      interval_OFF = 600;
    }
    else if (n == 2) {
      interval_ON = 80;
      interval_OFF = 300;
      n = 0;
    }
  }
  // =====================================================================================

  if (ledState == 0) {
    if (currentMillis - previousMillis >= interval_OFF) {
      previousMillis = currentMillis;
      ledState = 1;
    }
  }
  else {
    if (currentMillis - previousMillis >= interval_ON) {
      previousMillis = currentMillis;
      ledState = 0;
    }
  }

  // gestione uscite =====================================================================
  if (count == 0) {
    digitalWrite(out1, ledState);
    digitalWrite(out2, ledState);
  }
  else if (count == 1) {
    digitalWrite(out2, LOW);
    digitalWrite(out1, ledState);
  }
  else if (count == 2) {
    digitalWrite(out1, ledState);
    if (n == 3) digitalWrite(out2, ledState);
    else if (n == 0) digitalWrite(out2, LOW);
  }
  else if (count == 3) {
    digitalWrite(out1, ledState);
    if (n == 1) digitalWrite(out2, ledState);
    else if (n == 0) digitalWrite(out2, LOW);
  }
  // =====================================================================================



  fronte1 = digitalRead(out1); // fronte di out1 per contare gli impulsi
  if (prev_fronte1 != fronte1) {
    prev_fronte1 = fronte1;
    if (fronte1 == 1) { // fronte salita
      n++;
      //DigiKeyboard.println(n);
    }
  }

} // fine void flash()




void loop() {
  myButton.update();

  if (myButton.isLongClick()) {
    EEPROM.update(address, count);
    longClickON = 1;
    //DigiKeyboard.println("Click lungo");
  }
  if (longClickON == 1) {
    digitalWrite(out1, HIGH);
  }

  if (myButton.isReleased()) {
    if (longClickON == 0) count++;
    longClickON = 0;
    //DigiKeyboard.println(count);
  }

  if (count >= 5) count = 0; // 4 = OFF
  if (count >= 0 && count <= 3 && longClickON == 0) flash();
  if (count == 4) {
    digitalWrite(out1, LOW); // spegne out 1
    digitalWrite(out2, LOW); // spegne out 2
    n = 0;
    currentMillis = 0;
  }


}
