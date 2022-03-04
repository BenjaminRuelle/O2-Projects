#include <SoftwareSerial.h> // Librairie communication serie
#include <HCSR04.h> //Librairie ultrason

UltraSonicDistanceSensor distanceSensor(13, 12);  // Initialize sensor that uses digital pins 13 and 12.
long distance_cm = 100;       //Variable de la distance devant la voiture

SoftwareSerial MyBlue(2, 3); // RX | TX // Création du serial bluetooht sur l'arduino. Avec les pins 2 et 3.
char flag = 0; // Variable qui contient la valeur du message bluetooht
boolean automatic = false;
const int frontLed = 11;
const int backLed = 7;

int backLedstate = LOW;
int ledState = LOW;
int directionState = HIGH;

const int motorPin1 = 9;
const int motorPin2 = 8;
const int enablePin1 = 10; 

const int motorPin4 = 4;  //Pin du Pont en H L298N
const int motorPin3 = 6;
const int enablePin2 = 5;
int motorspeed = 0; //variable de la vitesse du moteur
int leftSpeed = 0;
int rightSpeed = 0;
int currentspeed = 0; //variable utile dans la manoeuvre

unsigned long lastExecutedMillis = 0; //Variable millis du temps écoulé

void setup() 
{  

  pinMode(frontLed, OUTPUT);
  pinMode(backLed, OUTPUT);  
  digitalWrite(frontLed, LOW);
  digitalWrite(backLed, LOW);
  
  Serial.begin(9600); //Ouverture du serial sur l'ordnateur (utile uniqument pour debug)
  MyBlue.begin(9600);  //Ouverture du serial bluetooht

  Serial.println("Go"); // Ecriture sur le serial PC (uniquement pour debug)
} 
void loop() 
 { 
  // Partie manoeuvre
  if(automatic == true){
    unsigned long currentMillis = millis();    
      
      if (currentMillis - lastExecutedMillis >=  100) { // On recule pendant 2sec
          lastExecutedMillis = currentMillis;
          distance_cm = distanceSensor.measureDistanceCm();  // Valeur du capteur 
         }     
     
    if(distance_cm < 16 && distance_cm > 0){
    Serial.print("Mur détecté à ");
    Serial.print(distance_cm);
    Serial.println(" cm");
    Maneuvering();
     }
  }
   // Partie communication
   
   if (MyBlue.available()){ //test de l'état de la communication bluetooth
    flag = MyBlue.read();  //lecture du serial bluethoot    

    //Partie direction et  vitesse moteur
    
    if(flag == 'G'){      
     Serial.println("U-turn gauche");
      motorspeed = 60;
      leftSpeed = motorspeed;
      rightSpeed = motorspeed;
     TurnLeft();
    }
    else if(flag == 'D'){
     Serial.println("U-turn droite");
      motorspeed = 60;
      leftSpeed = motorspeed;
      rightSpeed = motorspeed;
     TurnRight();
    }      
    else if(flag == 'A'){  // accelération
       if(motorspeed < 100){
         motorspeed = motorspeed + 10;
         leftSpeed = motorspeed;
         rightSpeed = motorspeed;
        }                  
       
       Serial.print("Accelération, la vitesse est de ");
       Serial.print(motorspeed);
       Serial.println("%");       
    }          
    else if(flag == 'R'){  //ralentissement
        if(motorspeed > 0){
         motorspeed = motorspeed - 10;
         leftSpeed = motorspeed;
         rightSpeed = motorspeed;
        }                   
          
        Serial.print("Freinage, la vitesse est de ");
        Serial.print(motorspeed);
        Serial.println('%');
       
    } 
    else if(flag == 'B'){              
        Backward();        
        Serial.println("Marche arrière");
        motorspeed = 60;
        rightSpeed = motorspeed;
        leftSpeed = motorspeed;
    }
    else if(flag == 'F'){              
        Forward();
        leftSpeed = motorspeed;
        rightSpeed = motorspeed;        
        Serial.println("Marche avant");
    } 
    else if(flag == 'U'){
        digitalWrite(frontLed, HIGH);        
        Serial.println("Led ON");
        
    }
   else if(flag == 'a'){
        automatic = true;
        motorspeed = 50;
        SetSpeedMotor(motorspeed, motorspeed);       
        Serial.println("AutoMode");
        
    }
   else if(flag == 'm'){
        automatic = false;
        motorspeed = 0;
        leftSpeed= 0;
        rightSpeed= 0;
        SetSpeedMotor(motorspeed, motorspeed);     
        Serial.println("ManualMode");
        
    }   
   else if(flag == 'I'){
        digitalWrite(frontLed, LOW);      
        Serial.println("Led OFF");
        
    }
   else if(flag == 'S'){
        motorspeed = 0;
        leftSpeed= 0;
        rightSpeed= 0;
        SetSpeedMotor(motorspeed, motorspeed);
        digitalWrite(frontLed, LOW);              
        Serial.println("Stop");
        
    }
    else if(flag == 'g'){
      if(leftSpeed < 100){
         leftSpeed = leftSpeed + 30;
         if(rightSpeed > 0){
         rightSpeed = rightSpeed - 30;
         }
       }  
       SetSpeedMotor(leftSpeed, rightSpeed);       
       delay(500); 
       rightSpeed = motorspeed;
       leftSpeed = motorspeed;             
      Serial.println("Gauche");
        
    }
    else if(flag == 'd'){
      if(rightSpeed < 100){
         rightSpeed = rightSpeed + 30;
         if(leftSpeed > 0){
         leftSpeed = leftSpeed - 30;
         } 
       }
       SetSpeedMotor(leftSpeed, rightSpeed);       
       delay(500); 
       rightSpeed = motorspeed;
       leftSpeed = motorspeed;             
      Serial.println("Droite");
        
    } 
   MyBlue.print("-"); 
   SetSpeedMotor(leftSpeed, rightSpeed);
   
  }
 }
 
 void Maneuvering(){ 
       
       
    MyBlue.print("Maneuvering");    
    Serial.println("Manoeuvre arrière");        
    Backward();     
    SetSpeedMotor(50,50);       

    delay(500);
    TurnLeft();
    SetSpeedMotor(40,40); 
    Serial.println("Manoeuvre à gauche");
        
    delay(500);
    Forward();   
    SetSpeedMotor(motorspeed,motorspeed); 
    Serial.println("Marche avant");
    MyBlue.print("Done");       
    Serial.println("Fin de Manoeuvre d'évitement");
    
    return;

  }
  
  void Switchdirection(){    

        if (directionState == HIGH){
          digitalWrite(backLed, HIGH);
          Backward();          
          Serial.println("Marche arrière");          
         } 
         
        else {
          digitalWrite(backLed, LOW);
          Forward();             
          Serial.println("Marche avant");
         }
        return;
  }

  void ForwardLeftWheels(){
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    }

  void ForwardRightWheels(){
     digitalWrite(motorPin3, HIGH);
     digitalWrite(motorPin4, LOW);     
  }

  void BackwardLeftWheels(){
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
  }
    
  void BackwardRightWheels(){
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, HIGH);
  }

  void TurnLeft(){
   digitalWrite(backLed, LOW);
    ForwardLeftWheels();
    BackwardRightWheels();
    }
  void TurnRight(){
    digitalWrite(backLed, LOW);
    ForwardRightWheels();
    BackwardLeftWheels();  
  }
  void Forward(){
   digitalWrite(backLed, LOW);
    ForwardLeftWheels();
    ForwardRightWheels();
  }
  void Backward(){
    digitalWrite(backLed, HIGH);
    BackwardLeftWheels();
    BackwardRightWheels();
  }


 void SetSpeedMotor(int powerLeft, int powerRight){
    
    Serial.print("Power Left: ");
    Serial.print(powerLeft);
    Serial.print(" %");
    Serial.print("  Power Right: ");
    Serial.print(powerRight);
    Serial.println(" %");
    
    analogWrite(enablePin1, map(powerLeft, 0, 100, 0, 255));
    analogWrite(enablePin2, map(powerRight, 0, 100, 0, 255));
  }

   


  
    
