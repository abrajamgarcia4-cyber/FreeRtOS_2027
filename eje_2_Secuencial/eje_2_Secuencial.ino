#define led 2
#define led2 22
#define boton1 18

#define t_1 500
#define t_2 500

void setup() {
  // put your setup code here, to run once:
  pinMode(led,OUTPUT);
  pinMode(led2,OUTPUT);
  pinMode(boton1,INPUT_PULLUP);
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(led,1);
  delay(t_1);
  digitalWrite(led,0);
  delay(t_1);

  digitalWrite(led2,1);
  delay(t_2);
  digitalWrite(led2,0);
  delay(t_2);

  bool estado_boton = digitalRead(boton1);
  if(estado_boton == 0){
    Serial.println("boton presionado");
  }
  //delay(60);
}

