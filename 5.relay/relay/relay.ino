//
//#include <Button.h>
//#define pinButton 0
//#define pinRelay 12
//long lastDebounceTime = 0;
//long debounceDelay = 250;
//int statusButton = LOW;
//void setup() {
//  pinMode(pinButton, INPUT);
//  pinMode(pinRelay, OUTPUT);
//  Serial.begin(9600);
//}
//
//void loop() {
//  statusButton = digitalRead(pinButton);
//  if ((millis() - lastDebounceTime) > debounceDelay)
//  {
//    if (statusButton == LOW) {
//    digitalWrite(pinRelay, HIGH);
//      Serial.print("HIGH");
//      Serial.println();
//      lastDebounceTime = millis();
//    }
//  
//    if (statusButton == HIGH)
//    {
//      digitalWrite(pinRelay, LOW);
//      Serial.print("LOW");
//      Serial.println();
//      lastDebounceTime = millis();
//
//    }
//  }
//}

//int dem=0;
//boolean buttonStatus=1;
// 
//void setup(){
//    pinMode(0,INPUT_PULLUP);
//    pinMode(3,OUTPUT); 
//    Serial.begin(9600); 
//}
// 
//void loop(){
//    if(digitalRead(0)!=buttonStatus){
//        dem += 1;
//        buttonStatus = digitalRead(0);
//        Serial.print(buttonStatus);
//        Serial.println();
//    }
//    if(fmod(dem,4)==0){
//        digitalWrite(3,0);
//    }else{
//        digitalWrite(3,1);
//    }
//}

#define pinButton 3
void setup(){
//    pinMode(0,INPUT_PULLUP);
    pinMode(pinButton,OUTPUT); 
    Serial.begin(9600); 
}
 
void loop(){
digitalWrite(pinButton, HIGH);
Serial.print("HIGH");
Serial.println();
delay(2000);
digitalWrite(pinButton, LOW);
Serial.print("LOW");
Serial.println();
delay(2000);
}


