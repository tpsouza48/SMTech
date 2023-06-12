#include <Adafruit_Fingerprint.h> // Leitor de Digital
#include <LiquidCrystal_I2C.h> // Backpack I2C (Display LCD)
#include <MFRC522.h> // Leitor RFID
#include <Wire.h> // Conexão I2C

/*     Display LCD     */
#define enderecoLCD 0x27 // Endereço do Display
#define colunasLCD 16 // Quantidade de colunas do Display
#define linhasLCD 2 // Quantidade de linhas do Display
LiquidCrystal_I2C lcd(enderecoLCD, colunasLCD, linhasLCD);

/*      Relés     */
#define pinoReleIgnicao 8
#define pinoReleEletrico 9
class Rele {
  public:
    int pinoR; // Pino do relé
    int estado; // 0 - Desligado, 1 - Ligado;

    Rele(int pino) {
      pinoR = pino;
      estado = 0;

      pinMode(pinoR, OUTPUT);
      digitalWrite(pinoR, LOW);
    }
    
    void ligar() {
      digitalWrite(pinoR, HIGH);
      estado = 1;  
    }

    void desligar() {
      digitalWrite(pinoR, LOW);
      estado = 0;
    }
};

Rele rIgnicao = Rele(pinoReleIgnicao);
Rele rEletrico = Rele(pinoReleEletrico);

/*      Leitor de Digital     */
volatile int finger_status = -1;
SoftwareSerial leitorSerial(2, 3); // TX/RX do Leitor de Digital
Adafruit_Fingerprint leitorDigital = Adafruit_Fingerprint(&leitorSerial); // Iniciando o objeto


void iniciarSerial() {
  Serial.begin(9600);
  Serial.println("Serial iniciado com sucesso.");
}

void iniciarDisplay() {
  lcd.begin(colunasLCD, linhasLCD); // Iniciando o LCD
  lcd.init(); // Iniciando a comunicação com o LCD
  lcd.backlight(); // Ligando a luz traseira do display
}

void printD(String txt1, String txt2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(txt1);

  lcd.setCursor(0, 1);
  lcd.print(txt2);
}

void iniciarLeitorDigital() {
  Serial.println("\nIniciando teste do leitor de digital...");

  // Taxa de dados do leitor
  leitorDigital.begin(57600);

  while(true) {
    if (leitorDigital.verifyPassword()) {
      Serial.println("Leitor de Digital encontrado!\n");
      break;
    } else {
      Serial.println("Falha ao iniciar o leitor de digital!");
      Serial.println("Tentando novamente em 3 segundos...");

      printD("Falha Leitor Dig.", "Tentando em 3seg...");
      delay(3000);
    }
  }

  leitorDigital.getTemplateCount();
  Serial.print("O leitor contém "); Serial.print(leitorDigital.templateCount); Serial.println(" templates de digitais.");
}

//  -1 = Falha | !-1 = Número do ID
int coletarIdDigital() {
  uint8_t p = leitorDigital.getImage();
  
  if (p!=2) {
    Serial.println(p);
  }

  if (p != FINGERPRINT_OK)  return -1;
  
  p = leitorDigital.image2Tz();
  if (p!=2) {
    Serial.println(p);
  }

  if (p != FINGERPRINT_OK)  return -1;

  p = leitorDigital.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -2;
  
  // Correspondência
  Serial.print("Encontrado ID N°"); Serial.print(leitorDigital.fingerID); 
  Serial.print(" com uma confiança de "); Serial.println(leitorDigital.confidence);
  return leitorDigital.fingerID;
}

void setup() {
  Serial.println("\n\n\n\n\n");
  iniciarSerial();
  
  Serial.println("Iniciando display...");
  iniciarDisplay();
  Serial.println("Display iniciado com sucesso.");
  
  printD("Display Iniciado.", "");
  delay(2000);

  printD("Iniciando", "Leitor de Dig.");
  delay(2000);

  Serial.println("Iniciando leitor de digital...");
  
  iniciarLeitorDigital();
  Serial.println("Leitor de digital iniciado com sucesso.");
  printD("Leitor Iniciado.", "");
  delay(2000);
}

void loop() {
  printD("Lendo...", "Coloque seu dedo.");
  Serial.print("Aguardando leitura...\n");

  finger_status = coletarIdDigital();
  if (finger_status !=-1 and finger_status != -2) {
    Serial.print("Correspondência!");
    printD("Leitura aceita.", "Bem-vindo!");
    rIgnicao.ligar();
    rEletrico.ligar();
    delay(4000);
    rIgnicao.desligar();
    rEletrico.desligar();
  } else {
    if (finger_status == -2){
      Serial.print("Nenhuma correspondência!");
      printD("Nao corresponde.", "Tente novamente...");
      delay(1000);
    }
  }
  delay(1000);            //don't ned to run this at full speed.
}