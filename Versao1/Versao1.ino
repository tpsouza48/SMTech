#include <LiquidCrystal.h>
#include <Adafruit_Fingerprint.h>
#include <Wire.h> // Biblioteca utilizada para fazer a comunicação com o I2C
#include <LiquidCrystal_I2C.h> // Biblioteca utilizada para fazer a comunicação com o display 20x4 

#define RELE 8
#define RELE2 9

#define CONTRASTE 6
#define col 16 // Serve para definir o numero de colunas do display utilizado
#define lin  2 // Serve para definir o numero de linhas do display utilizado
#define ende  0x27 // Serve para definir o endereço do display.

LiquidCrystal_I2C lcd(ende,col,lin); // Chamada da funcação LiquidCrystal para ser usada com o I2C

volatile int finger_status = -1;

SoftwareSerial mySerial(2, 3); // TX/RX on fingerprint sensor

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup()  
{
  Serial.begin(9600);

  pinMode(RELE, OUTPUT);
  pinMode(RELE2, OUTPUT);

  digitalWrite(RELE, LOW);
  digitalWrite(RELE2, LOW);


  lcd.begin(16, 2);
  lcd.init(); // Serve para iniciar a comunicação com o display já conectado
  lcd.backlight(); // Serve para ligar a luz do display

  analogWrite(CONTRASTE, 100);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Iniciando...");

  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  
  while(true) {
    lcd.clear();
    lcd.print("Detectando leitor...");

    if (finger.verifyPassword()) {
      Serial.println("Found fingerprint sensor!");
      break;
    } else {
      Serial.println("Did not find fingerprint sensor :(");
      lcd.clear();
      lcd.print("Erro.");
      lcd.setCursor(0, 1);
      lcd.print("- Leitor de digital -");
      delay(1000);
    }
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  Serial.println("Waiting for valid finger...");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Lendo...");
}

void loop()                     // run over and over again
{
  digitalWrite(RELE, LOW);
  digitalWrite(RELE2, LOW);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Lendo...");

  finger_status = getFingerprintIDez();
  if (finger_status !=-1 and finger_status != -2) {
    Serial.print("Match");
    reconhecerPessoa(finger_status);
    digitalWrite(RELE, HIGH);
    digitalWrite(RELE2, HIGH);
    delay(4000);
  } else {
    if (finger_status == -2){
      Serial.print("Not Match");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Nao aceito.");
      delay(2000);
    }
  }
  delay(100);            //don't ned to run this at full speed.
}

void reconhecerPessoa(int id) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CONCEDIDO!");
  lcd.setCursor(0, 1);
  
  if(id == 1) {
    Serial.print("Bem vindo Thiago!");
  } else if (id == 2) {
    Serial.print("Bem vindo Lucas!");
  } else if (id == 3) {
    Serial.print("Bem vindo Matheus!");
  }


}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p!=2){
    Serial.println(p);
  }
  if (p != FINGERPRINT_OK)  return -1;
  
  p = finger.image2Tz();
  if (p!=2){
    Serial.println(p);
  }
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -2;
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID; 
}
