/*
    zapojte nasledovne:
    
    ultrasonik TRIG == d2, ECHO == 3
    motory D1, D2 ak by to slo naopak tak prepojte motory
    IR senzor == D11
    IR senzor == D12 (nemusi byt)
    senzor na sledovanie ciary 1 == A1, 2 == A2, 3 == A3
    sirenka == D7
*/




#include <IRremote.h>
#include <Servo.h>
//#include <MsTimer2.h> (bije sa to s IR ovladacom)

#define VLAVO 0xFF22DD
#define VPRAVO 0xFFC23D
#define STOP 0xFF02FD
#define VPRED 0xFF629D
#define VZAD 0xFFA857
#define CIARA 0xFF6897
#define BLUDIC 0xFF9867
#define C 0xFFB04F
#define TRIG 2
#define ECHO 3

Servo motor;
Servo motor1;

int vychilkaserva1 = 0; //sem si nastavte pripadnu odchylku motorov
int vychilkaserva = 0;
int sensor1Value = analogRead(1);
int sensor2Value = analogRead(2);
int sensor3Value = analogRead(3);
int ciaratrue;
int bludictrue;
int ton;
IRrecv ovladac(11);
IRrecv ovladac2(12);

decode_results vysledky;

void setup()
{
    ovladac.enableIRIn();
    ovladac2.enableIRIn();
    Serial.begin(115200);
    motor.attach(9);
    motor1.attach(10);
    pinMode(7, OUTPUT); 
    ciaratrue = 0;
    bludictrue = 0;
    ton = 0;
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
    attachInterrupt(digitalPinToInterrupt(ECHO), zmena, CHANGE);
    //MsTimer2::set(20, vysli_pulz);
    //MsTimer2::start();
}
int cakame_na_zaciatok;
volatile int vzdialenost;
long zaciatocny_cas, konecny_cas;

void zmena()
{
    if (cakame_na_zaciatok == 1) 
    {
         zaciatocny_cas = micros();
         cakame_na_zaciatok = 0;
    }
    else 
    {
        konecny_cas = micros();
        long dlzka_pulzu = konecny_cas - zaciatocny_cas;
        vzdialenost = dlzka_pulzu / 58;
    }
}

void vysli_pulz()
{
    digitalWrite(TRIG, 1);
    delayMicroseconds(10);
    digitalWrite(TRIG, 0);
    cakame_na_zaciatok = 1;
}

void bludic () 
{
  if (vzdialenost < 10)
    {
     
          motor.write(60 + vychilkaserva);
          motor1.write(60 + vychilkaserva);
          delay(500);
     
    }
   
    motor.write(60 + vychilkaserva);
    motor1.write(120 + vychilkaserva1);
    Serial.println(vzdialenost);
}

void ciara ()
{
  sensor1Value = analogRead(1);
  sensor2Value = analogRead(2);
  sensor3Value = analogRead(3);          
  if (sensor1Value > 600)
  {
    motor.write(60 + vychilkaserva);
    motor1.write(90 + vychilkaserva1);
  }
  if (sensor2Value > 600)
  {
    motor.write(60 + vychilkaserva);
    motor1.write(120 + vychilkaserva1);
  }
  if (sensor3Value > 600)
  {
    motor.write(90 + vychilkaserva);
    motor1.write(120 + vychilkaserva1);
  }
            
    Serial.print(sensor1Value);
    Serial.print(" ");
    Serial.print(sensor2Value);
    Serial.print(" ");
    Serial.println(sensor3Value);
}

void zahraj(long frekvencia, long dlzka)
{
    long polperioda = 1000 * 500L / frekvencia;
   long pocet = dlzka * 500L / polperioda;
        
    while (pocet > 0)
    {
      pocet--;
      digitalWrite(7, 1);
      delayMicroseconds(polperioda);
      digitalWrite(7, 0);
      delayMicroseconds(polperioda);
    }    
}

void akcia()
{
          Serial.println(vysledky.value, HEX);
        
        
        if (vysledky.value == VZAD) 
        {
            motor1.write(60 + vychilkaserva1);
            motor.write(120 + vychilkaserva);
            ciaratrue = 0;
            bludictrue = 0;

        }
        if (vysledky.value == CIARA) 
        {
          ciaratrue = 1;
          bludictrue = 0;
        }
        if (vysledky.value == BLUDIC) 
        {
          ciaratrue = 0;
          bludictrue = 1;
        }
        if (vysledky.value == VPRED) 
        {
            motor.write(60 + vychilkaserva);
            motor1.write(120 + vychilkaserva1);
            ciaratrue = 0;
            bludictrue = 0;

        }
        if (vysledky.value == VPRAVO) 
        {
            motor.write(120 + vychilkaserva);
            motor1.write(120 + vychilkaserva1);
            ciaratrue = 0;
            bludictrue = 0;
        }
        
        if (vysledky.value == C) 
        {
            ton = 1;
            ciaratrue = 0;
            bludictrue = 0;
        }
        
         if (vysledky.value == VLAVO) 
        {
            motor.write(60 + vychilkaserva);
            motor1.write(60 + vychilkaserva1);
            ciaratrue = 0;
            bludictrue = 0;
        }
       if (vysledky.value == STOP) 
        {
            motor.write(90 + vychilkaserva);
            motor1.write(90 + vychilkaserva1);
            ciaratrue = 0;
            bludictrue = 0;
        }

}

void loop()
{
    vysli_pulz();
    delay(20);
    
    if (ovladac2.decode(&vysledky))  
    {    
      akcia();
      ovladac2.resume();
    }

    if (ovladac.decode(&vysledky)) 
    {
      akcia();    
      ovladac.resume();
    }
    
    if (ciaratrue == 1)
    {
      ciara();
    }
    if (bludictrue == 1)
    {

      bludic();
    }
    if (ton == 1)
    {
      zahraj(440,1000);
      ton = 0;
    }
    delay(10);
}
