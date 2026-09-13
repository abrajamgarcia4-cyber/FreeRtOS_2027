#define led 19
#define led2 20
#define led3 21

#define t_1 2000
#define t_2 500
#define t_3 2000

void setup() {
  // put your setup code here, to run once:
  pinMode(led,OUTPUT);
  pinMode(led2,OUTPUT);
  pinMode(led3,OUTPUT);
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(led,1);
  delay(t_1);
  digitalWrite(led,0);

  for(int x=0; x<=2; x++){
    digitalWrite(led2,1);
    delay(t_2);
    digitalWrite(led2,0);
    delay(t_2);
    x++;
  }

  digitalWrite(led3,1);
  delay(t_2);
  digitalWrite(led3,0);

}

