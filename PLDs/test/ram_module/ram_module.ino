const int addr[8] = {9, 8, 7, 6, 5, 4, 3, 2};
#define MIS 12
#define MMO 11
#define PRG 10

void setup() {
  // put your setup code here, to run once:
  for (int i = 0; i < 8; i++) {
    pinMode(addr[i], OUTPUT);
    digitalWrite(addr[i], LOW);
  }

  pinMode(MIS, OUTPUT);
  digitalWrite(MIS, LOW);
  pinMode(MMO, OUTPUT);
  digitalWrite(MMO, LOW);
  pinMode(PRG, OUTPUT);
  digitalWrite(PRG, LOW);
  Serial.begin(9600);
}

String serialRead = "";q

void loop() {
  serialRead = "";
  if (Serial.available()) {
    serialRead = Serial.readString();
    Serial.print(serialRead);
  }

  if (serialRead == "prg\n") {
    Serial.println("Toggled PRG");
    digitalWrite(PRG, !digitalRead(PRG));
  }
  
  if (serialRead == "mis\n") {
    Serial.println("Toggled MIS");
    digitalWrite(MIS, !digitalRead(MIS));
  }

  if (serialRead == "mmo\n") {
    Serial.println("Toggled MMO");
    digitalWrite(MMO, !digitalRead(MMO));
  }

}
