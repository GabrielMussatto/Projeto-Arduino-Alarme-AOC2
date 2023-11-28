#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
byte som[] = {0x01,0x03,0x0F,0x0F,0x0F,0x03,0x01,0x00};
byte somOn[] = {0x08,0x04,0x0A,0x15,0x15,0x0A,0x04,0x08};
byte somOff[] = {0x00,0x11,0x0A,0x04,0x0A,0x11,0x00,0x00};

const int pir = 7;
const int piezo = 10;
const int led = 8;

const int botaoA = A1;
const int botaoB = A2;

bool ligarDia = false; // Se for falso, ligará de noite
bool alarmeLigado = false;

void setup() {
  
  lcd.begin(16, 2);
  lcd.createChar(1, som);
  lcd.createChar(2, somOn);
  lcd.createChar(3, somOff);

  pinMode(pir, INPUT); // pir
  pinMode(piezo, OUTPUT); // piezo
  pinMode(led, OUTPUT); // led

  pinMode(botaoA, INPUT);
  pinMode(botaoB, INPUT);

  Serial.begin(9600);
}

void loop() {
  sensorAutomatico();

  while(alarmeLigado && estaDentroPeriodo()){
    if(verificaMovimento()){
      while(alarmeLigado){
        alertaLigado();
        detectaControle();
      }
    }
    detectaControle();
  }

  detectaControle();
  delay(100);

}

void sensorAutomatico(){
  if (digitalRead(pir) == HIGH) {
    digitalWrite(led, HIGH);
    mostrarDisplay(3);
  }
  
  else if(digitalRead(pir) == LOW){ 
  	digitalWrite(led, LOW);
  }
}

bool verificaMovimento(){
  int leitura = digitalRead(pir);

  return leitura == 1;
}

bool estaDentroPeriodo(){
  int leitura = analogRead(A0);
  Serial.println(leitura);
  if (!ligarDia && leitura <= 70){ // Se estiver de noite
    return true;
  }
  else if (ligarDia && leitura > 70){ // Se estiver de dia
  	return true;
  }
  return false;
}

void alertaLigado(){
  lcd.setCursor(0, 0);
  lcd.print("     Intruso");
  lcd.setCursor(0,1);
  lcd.print("    Detectado");

  digitalWrite(led, HIGH);

  tone(piezo, 200, 700);
  delay(1000);
  noTone(piezo);
  
  digitalWrite(led, LOW);

  lcd.clear();
}

void detectaControle(){
  int limite = 400;
  int cont = 0;
  while (cont <= limite){
    delay(limite/100);

    if(analogRead(botaoA) > 0) {
      alarmeLigado = alarmeLigado ? false : true;
      mostrarDisplay(1);
      return;
    }
    
    else if(analogRead(botaoB) > 0){
      ligarDia = ligarDia ? false : true;
      mostrarDisplay(2);
      return;
    }

    cont += limite/100;
  }

  return;
}

void mostrarDisplay(int opc){
  lcd.setCursor(0, 0);

  if(opc == 1){
    lcd.print("     Alarme");
    lcd.setCursor(0, 1);
    lcd.print(alarmeLigado ? "     Ligado   " : "    Desligado ");
    lcd.write(1);
    lcd.write(alarmeLigado ? 2 : 3);
    somControle();
  }
  else if(opc == 2){
    lcd.print("      Ligar");
    lcd.setCursor(0, 1);
    lcd.print(ligarDia ? "     De Dia" : "    De Noite");
    somControle();
  }
  else if(opc == 3){
    lcd.print("   Movimento");
    lcd.setCursor(0,1);
    lcd.print("   Detectado");
  }

  delay(550);
  lcd.clear();
}

void somControle(){
    tone(piezo, 500, 300);
    delay(200);
    tone(piezo, 500, 300);
    noTone(piezo);
}