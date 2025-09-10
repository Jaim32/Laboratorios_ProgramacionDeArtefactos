

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define I2C_ADDRESS   0x3C        
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int PIN_LM35 = A0;                 
const unsigned long INTERVALO_MS = 1000; 
unsigned long tAnterior = 0;

const float VREF = 5.0; 
void setup() {
  Serial.begin(9600);
  delay(100);

  // ---- Inicializar OLED ----
  display.begin(I2C_ADDRESS, true);
  display.setContrast(255);   
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);

  // Mensaje de arranque
  display.setCursor(0, 0);
  display.println(F("LM35 + OLED SH1106"));
  display.println(F("Inicializando..."));
  display.display();
  delay(800);

  // Cabecera en Serial
  Serial.println(F("LM35 - Lectura simple (Temp en C)"));
}

void loop() {
  unsigned long ahora = millis();
  if (ahora - tAnterior >= INTERVALO_MS) {
    tAnterior = ahora;

    int lectura = analogRead(PIN_LM35);
    float voltaje = lectura * (VREF / 1023.0);     
    float tempC   = voltaje * 100.0;              

    Serial.print(F("Temperatura: "));
    Serial.print(tempC, 2);
    Serial.println(F(" °C"));

   

    // ---- Mostrar en OLED ----
    display.clearDisplay();       
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println(F("LM35 - Lectura"));

    display.setTextSize(2);        
    display.setCursor(0, 42);
    display.print(tempC, 1);
    display.print((char)247);      
    display.print(F("C"));

    display.display();            
  }

}