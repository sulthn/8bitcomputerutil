/*
  +-------------------------+
  |8-bit Computer Programmer|
  |Sulthann                 |
  +-------------------------+

            ARDUINO NANO
           +------------+
  PRGM EXE |D13      D12| READ
        NC |3V3      D11| DATA7
        NC |REF      D10| DATA6
     ADDR7 |A0        D9| DATA5
     ADDR6 |A1        D8| DATA4
     ADDR5 |A2        D7| DATA3
     ADDR4 |A3        D6| DATA2
     ADDR3 |A4        D5| DATA1
     ADDR2 |A5        D4| DATA0
      PROG |A6        D3| ADDR0
        NC |A7        D2| ADDR1
        NC |5V       GND| GND
        NC |RST      RST| NC
       GND |GND       RX| NC
        NC |VIN       TX| NC
           +------------+
*/

#define ADDR0 3
#define ADDR1 2
#define ADDR2 A5
#define ADDR3 A4
#define ADDR4 A3
#define ADDR5 A2
#define ADDR6 A1
#define ADDR7 A0

#define DATA0 4
#define DATA1 5
#define DATA2 6
#define DATA3 7
#define DATA4 8
#define DATA5 9
#define DATA6 10
#define DATA7 11

#define PROG A6
#define READ 12
#define EXE  13

#define LDA_imm 0x05
#define ADD_imm 0x09
#define STA     0x04
#define OUT_AR  0x1e
#define HLT 0x0f
#define JMP 0x06
#define JC  0x07
#define LDA 0x01
#define ADD 0x02
#define ROL 0b00101001
#define CLC 0b00010011

const int addr[8] = {ADDR0, ADDR1, ADDR2, ADDR3, ADDR4, ADDR5, ADDR6, ADDR7};
const int data[8] = {DATA0, DATA1, DATA2, DATA3, DATA4, DATA5, DATA6, DATA7};

#define store_1 0xf1
#define store_2 0xf0

unsigned char program[256] = {LDA_imm, 1, STA, store_1, LDA_imm, 0, STA, store_2, OUT_AR, LDA, store_1, ADD, store_2, STA, store_1, OUT_AR, LDA, store_2, ADD, store_1, JC, 0, JMP, 0x06};
int programAddress = 0;
int programLength = 23;

//const char program[] = {LDA_imm, 1, ROL, JMP, 2};

void resetPins() {
  for (int i = 0; i < 8; i++) {
    pinMode(addr[i], INPUT);
  }

  for (int i = 0; i < 8; i++) {
    pinMode(data[i], INPUT);
  }
}

void setup() {
  resetPins();

  pinMode(EXE, OUTPUT);
  digitalWrite(EXE, LOW);

  pinMode(READ, OUTPUT);
  digitalWrite(READ, LOW);

  Serial.begin(19200);
}

unsigned char readAddr(int Address) {
  for (int i = 0; i < 8; i++) {
    pinMode(addr[i], OUTPUT);
    digitalWrite(addr[i], LOW);
  }
  for (int i = 0; i < 8; i++) {
    pinMode(data[i], INPUT);
  }

  for (int z = 0; z < 8; z++) {
    digitalWrite(addr[z], (Address >> z) & 0x1);
  }

  unsigned char dd = 0;
  digitalWrite(READ, HIGH);
  delay(15);

  for (int j = 7; j >= 0; j--) {
    (digitalRead(data[j]) == 1) ? (dd = (dd << 1) + 1) : (dd = (dd << 1) + 0);
  }

  digitalWrite(READ, LOW);

  resetPins();
  return dd;
}

void substring(const char* input, char* output, int start, int stop) {
  const char* startptr = input + start;
  int length = stop - start;

  strncpy(output, startptr, length);
  output[length] = '\0';
}

char serialRead[1024] = {'\0'};
void rx() {
  int rxindex = 0;
  char rxbyte;
  bool incoming = true;

  while (!Serial.available());
  while (Serial.available() > 0 || incoming) {
    while (!Serial.available());
    rxbyte = Serial.read();
    Serial.print(rxbyte);

    if (rxbyte != '\n') {
      serialRead[rxindex] = rxbyte;
      rxindex++;
    }
    else {
      serialRead[rxindex] = '\n';
      incoming = false;
      break;
    }
  }
}

void loop() {
  memset(serialRead, '\0', 1024);
  Serial.print("\n> ");
  rx();


  // PROGRAM
  if (serialRead[0] == 'p') {
    if (analogRead(PROG) > 512)
    {
        Serial.println("Set computer to PROGRAM mode.");
        return;
    }

    Serial.println("Programming...");

    for (int i = 0; i < 8; i++) {
      pinMode(addr[i], OUTPUT);
      digitalWrite(addr[i], LOW);
    }

    for (int i = 0; i < 8; i++) {
      pinMode(data[i], OUTPUT);
      digitalWrite(data[i], LOW);
    }

    int programLen = programLength + 1;

    for (int i = programAddress; i < programLen + programAddress; i++) {
      for (int z = 0; z < 8; z++) {
        digitalWrite(addr[z], (i >> z) & 0x1);
      }

      for (int y = 0; y < 8; y++) {
        digitalWrite(data[y], (program[i - programAddress] >> y) & 0x1);
      }
      digitalWrite(EXE, HIGH);
      delay(10);
      digitalWrite(EXE, LOW);
    }
    resetPins();
    Serial.println("\nDone programming.");
  }


  // READ
  if (serialRead[0] == 'r') {
    if (analogRead(PROG) > 512)
    {
        Serial.println("Set computer to PROGRAM mode.");
        return;
    }

    if (strlen(serialRead) < 1 /*|| serialRead.indexOf(':') != 3*/) {
      Serial.println("Invalid use of READ.");
    }

    int sep;
    if (strchr(serialRead, ':') != NULL) {
      sep = (strchr(serialRead, ':') - serialRead + 1);
    }
    else {
      sep = strlen(serialRead) - 1;
    }
    
    char saddr[3] = {0};
    substring(serialRead, saddr, 1, sep);
    unsigned char start_addr = strtol(saddr, 0, 16);
    unsigned char end_addr = start_addr;
    
    if (strchr(serialRead, ':') != NULL) {
      substring(serialRead, saddr, sep, strlen(serialRead) - 1);
      end_addr = strtol(saddr, 0, 16);
    }

    if (start_addr > 255 | end_addr > 255)
    {
      Serial.println("Invalid memory space (>0xFF)");
      return;
    }

    int addressSpace = end_addr - start_addr;
    
    for (int h = 0; h <= addressSpace; h++) {
      if (!(h % 16)) {
        Serial.println("");
        Serial.print((start_addr + h) < 16 ? "0" : "");
        Serial.print(start_addr + h, HEX);
        Serial.print(": ");
      }
      Serial.print(readAddr(start_addr + h) < 16 ? "0" : "");
      Serial.print(readAddr(start_addr + h), HEX);
      Serial.print(" ");
    }
    Serial.println("\nDone reading.");
  }

  // WRITE
  if (serialRead[0] == 'w') {
    if (analogRead(PROG) > 512)
    {
        Serial.println("Set computer to PROGRAM mode.");
        return;
    }

    if (strlen(serialRead) < 1 || strchr(serialRead, ':') == NULL || (strchr(serialRead, ':') - serialRead) != 3) {
      Serial.println("Invalid use of WRITE.");
      return;
    }

    int sep = strchr(serialRead, ':') - serialRead + 1;

    for (int i = 0; i < 8; i++) {
      pinMode(addr[i], OUTPUT);
      digitalWrite(addr[i], LOW);
    }

    for (int i = 0; i < 8; i++) {
      pinMode(data[i], OUTPUT);
      digitalWrite(data[i], LOW);
    }

    char saddr[3] = { '\0' };
    substring(serialRead, saddr, 1, sep - 1);
    
    int programLen = ((strlen(serialRead) - sep + 1)/2) - 1;
    int start_addr = strtol(saddr, 0, 16);

    if (programLen > 256 || (programLen + start_addr) > 256) {
      Serial.println("Invalid program size >0xFF");
      return;
    }

    Serial.println("Writing...");
    
    memset(program, '\0', 256);
    programAddress = start_addr;
    programLength = programLen;

    char byteToProgram[3] = { '\0' };

    for (int i = start_addr; i < (programLen + start_addr); i++) {
      for (int z = 0; z < 8; z++) {
        digitalWrite(addr[z], (i >> z) & 0x1);
      }

      substring(serialRead, byteToProgram, sep + (i - start_addr) * 2, (sep + (i - start_addr) * 2) + 2);
      unsigned char byteSend = strtol(byteToProgram, 0, 16);

      program[i - start_addr] = byteSend;
      for (int y = 0; y < 8; y++) {
        digitalWrite(data[y], (byteSend >> y) & 0x1);
      }
      digitalWrite(EXE, HIGH);
      delay(10);
      digitalWrite(EXE, LOW);
      
    }
    resetPins();
    
    int addressSpace = programLen;
    
    for (int h = 0; h < addressSpace; h++) {
      if (!(h % 16)) {
        Serial.println("");
        Serial.print((start_addr + h) < 16 ? "0" : "");
        Serial.print(start_addr + h, HEX);
        Serial.print(": ");
      }
      Serial.print(readAddr(start_addr + h) < 16 ? "0" : "");
      Serial.print(readAddr(start_addr + h), HEX);
      Serial.print(" ");
    }
    Serial.println("\nDone writing.");
  }

  // PROGRAM BUFFER
  if (serialRead[0] == 'b') {
    Serial.println("Program buffer");

    int addressSpace = programLength;
    
    for (int h = 0; h < addressSpace; h++) {
      if (!(h % 16)) {
        Serial.println("");
        Serial.print((programAddress + h) < 16 ? "0" : "");
        Serial.print(programAddress + h, HEX);
        Serial.print(": ");
      }
      Serial.print(program[h] < 16 ? "0" : "");
      Serial.print(program[h], HEX);
      Serial.print(" ");
    }
    Serial.println("\nBuffer read.");
  }
}
