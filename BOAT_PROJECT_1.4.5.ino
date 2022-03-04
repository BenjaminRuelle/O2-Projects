#include <Servo.h> // Librarie servo motor
#include <SoftwareSerial.h> // Librarie moniteur serie

SoftwareSerial MyBlue(2, 3); // RX | TX // Création du serial bluetooht sur l'arduino. Avec les pins 2 et 3.

const int servoPin = 9; //Pin du servomoteur
Servo myservo; // Déclaration du servo moteur
int servopot = 90;
char flag = 0; // Variable qui contient la valeur du message bluetooht
String heading[] = {"North","South","East","West"}; //tableau des caps

int motorPin4 = 10;  //Pin du Pont en H L298N
int motorPin3 = 6;
int enablePin = 5;

int motorspeed = 0; //variable de la vitesse du moteur
int state = 0;

#define EXE_INTERVAL 5000 //Déclaration du delais de la montée des variables du bateau vers l'app
unsigned long lastExecutedMillis = 0; //Variable millis du temps écoulé

void setup() 
{   
  Serial.begin(9600); //Ouverture du serial sur l'ordnateur (utile uniqument pour debug)
  MyBlue.begin(9600);  //Ouverture du serial bluetooht
  myservo.attach(servoPin); //attache du servo motor à sa pin
  Serial.println("Go"); // Ecriture sur le serial PC (uniquement pour debug)
} 
void loop() 
 { 
  if (MyBlue.available()){ //test de l'état de la communication bluetooth
    flag = MyBlue.read();  //lecture du serial bluethoot
    Serial.println(flag);  //Recriture du serial bluethoot sur le serial PC

  unsigned long currentMillis = millis();

  if (currentMillis - lastExecutedMillis >= EXE_INTERVAL) { // Envoi de données toute les 5sec
    lastExecutedMillis = currentMillis;

    //Création du message qui sera recu par l'app. On utilise une suite de variable découper par des "/". L'application utilise un spliter (Voir spliter App) pour découper cette suite de caratère et reconstruire les variables.
    //Pour le moment ce sont des variables aléatoires. A terme le module GPS va fournir ces différentes valeurs: Position GPS (Lat et Lon), vitesse du bateau (A voir si on utilise la vitesse du GPS ou celle issue d'un calcule avec la vitesse de l'hélice) 
    //et la date+heure
    
    MyBlue.print(motorspeed); //Speed 
    MyBlue.print("/");   
    MyBlue.print(heading[random(4)]);  //Heading
    MyBlue.print("/");
    MyBlue.print("-1.504");
    MyBlue.print(random(99)); // Lon
    MyBlue.print("/");
    MyBlue.print("47.276");
    MyBlue.print(random(99)); //Lat
    MyBlue.print("/");
    MyBlue.print(random(12)); // pH
    
    
    Serial.println("Send"); // Message de debugging
  }
    //Partie direction
    
    if(flag == 'G'){              //Lecture et comparaison de la variable bluetooth, G ou D pour Gauche et Droite. La position du moteur est à 90 de base, tourne à +- 45
      if(servopot <= 140){
        servopot = servopot + 10;
        } 
      myservo.write(45);
      Serial.println("Gauche");
    }
    else if(flag == 'D'){
      if(servopot >= 40){
        servopot = servopot + 10;
        } 
      myservo.write(135);
      Serial.println("Droite");
      }
      
     else if(flag == 'A'){  // accelération
       if(motorspeed < 100){
        motorspeed = motorspeed + 10;
        }
        
       Serial.println("Accelération");
       Serial.println(motorspeed);
      }
      else if(flag == 'R'){  //ralentissement
        if(motorspeed > 0){
         motorspeed = motorspeed - 10;
        }
        Serial.println("Freinage");
        Serial.println(motorspeed);
      } 
      else if(flag == 'S'){   
             
      Switchdirection();
      
      }
      
      }  

    myservo.write(servopot);
    analogWrite(enablePin, map(motorspeed, 0, 100, 0, 255));
    
  }

    void Switchdirection(){
    
        motorspeed = 0;
        state = digitalRead(motorPin4);
        if (state == HIGH ){
 
          digitalWrite(motorPin4, LOW);
          digitalWrite(motorPin3, HIGH);
          Serial.println("Sens horraire");          
         } 
        else {

          digitalWrite(motorPin4, HIGH);
          digitalWrite(motorPin3, LOW);
          Serial.println("Sens antihorraire");
         }
        return;
  }
    
