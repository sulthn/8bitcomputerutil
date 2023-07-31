#define P0  52
#define P1  50
#define P2  48
#define P3  46
#define P4  44
#define P5  42
#define P6  40
#define P7  38
#define P8  36
#define P9  34
#define P10 32


#define CLK 53
#define CLR 51
#define LD  49
#define CLRACTV HIGH
#define LDACTV LOW

const int pins[12] = {CLK, P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10};

void setup() {
  // put your setup code here, to run once:
  for (int i = 0; i < 12; i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
  }

  pinMode(CLR, OUTPUT);
  digitalWrite(CLR, CLRACTV ^ 1);

  pinMode(LD, OUTPUT);
  digitalWrite(LD, LDACTV ^ 1);



  Serial.begin(9600);
}

String serialRead = "";

void loop() {
  serialRead = "";
  if (Serial.available()) {
    serialRead = Serial.readString();
    Serial.print(serialRead);
  }

  if (serialRead == "p0\n") {
    Serial.println("Pin 0 Toggle");
    digitalWrite(P0, !digitalRead(P0));
    Serial.println(digitalRead(P0));
  }
  if (serialRead == "p1\n") {
    Serial.println("Pin 1 Toggle");
    digitalWrite(P1, !digitalRead(P1));
    Serial.println(digitalRead(P1));
  }
  if (serialRead == "p2\n") {
    Serial.println("Pin 2 Toggle");
    digitalWrite(P2, !digitalRead(P2));
    Serial.println(digitalRead(P2));
  }
  if (serialRead == "p3\n") {
    Serial.println("Pin 3 Toggle");
    digitalWrite(P3, !digitalRead(P3));
    Serial.println(digitalRead(P3));
  }
  if (serialRead == "p4\n") {
    Serial.println("Pin 4 Toggle");
    digitalWrite(P4, !digitalRead(P4));
    Serial.println(digitalRead(P4));
  }
  if (serialRead == "p5\n") {
    Serial.println("Pin 5 Toggle");
    digitalWrite(P5, !digitalRead(P5));
    Serial.println(digitalRead(P5));
  }
  if (serialRead == "p6\n") {
    Serial.println("Pin 6 Toggle");
    digitalWrite(P6, !digitalRead(P6));
    Serial.println(digitalRead(P6));
  }
  if (serialRead == "p7\n") {
    Serial.println("Pin 7 Toggle");
    digitalWrite(P7, !digitalRead(P7));
    Serial.println(digitalRead(P7));
  }
  if (serialRead == "p8\n") {
    Serial.println("Pin 8 Toggle");
    digitalWrite(P8, !digitalRead(P8));
    Serial.println(digitalRead(P8));
  }
  if (serialRead == "p9\n") {
    Serial.println("Pin 9 Toggle");
    digitalWrite(P9, !digitalRead(P9));
    Serial.println(digitalRead(P9));
  }
  if (serialRead == "p10\n") {
    Serial.println("Pin 10 Toggle");
    digitalWrite(P10, !digitalRead(P10));
    Serial.println(digitalRead(P10));
  }

  if (serialRead == "l\n") {
    Serial.println("Loaded");
    digitalWrite(LD, !digitalRead(LD));
    Serial.println(digitalRead(LD));
  }
  if (serialRead == "p\n") {
    Serial.println("Clock pulse");
    digitalWrite(CLK, HIGH);
    digitalWrite(CLK, LOW);
  }
  if (serialRead == "clr\n") {
    Serial.println("Clear");
    digitalWrite(CLR, CLRACTV);
    digitalWrite(CLR, CLRACTV ^ 1);
  }
}