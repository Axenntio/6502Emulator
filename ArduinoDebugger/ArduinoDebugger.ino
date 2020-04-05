const uint8_t addressPins[16] = {22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52};
const uint8_t dataPins[8] = {23, 25, 27, 29, 31, 33, 35, 37};
bool isStepped = true;
bool shouldStep = false;
uint16_t stepSpeed = 25;

typedef enum {none, accumulator, absolute, absolute_x, absolute_y, immediate, implied, indirect, indirect_x, indirect_y, relative, zeropage, zeropage_x, zeropage_y} address_mode_e;

char *address_mode_text[] = {
  "---",
  "A",
  "$LLHH",
  "$LLHH, X",
  "$LLHH, Y",
  "#$BB",
  "",
  "($LLHH)",
  "($LL, X)",
  "($LL), Y",
  "$BB",
  "$LL",
  "$LL, X",
  "$LL, Y"
};

typedef struct {
  char name[4];
  address_mode_e mode;
} instruction_t;

instruction_t instructions[] =
{
  {"BRK",  implied}, // 0x00
  {"ORA", indirect_x},
  {"---", none},
  {"---", none},
  {"---", none},
  {"ORA", zeropage},
  {"ASL", zeropage},
  {"---", none},
  {"PHP", implied},
  {"ORA", immediate},
  {"ASL", accumulator},
  {"---", none},
  {"---", none},
  {"ORA", absolute},
  {"ASL", absolute},
  {"---", none},
  {"BLP", relative}, // 0x10
  {"ORA", indirect_y},
  {"---", none},
  {"---", none},
  {"---", none},
  {"ORA", zeropage_x},
  {"ASL", zeropage_x},
  {"---", none},
  {"CLC", implied},
  {"ORA", absolute_y},
  {"---", none},
  {"---", none},
  {"---", none},
  {"ORA", absolute_x},
  {"ASL", absolute_x},
  {"---", none},
  {"JSR", absolute}, // 0x20
  {"AND", indirect_x},
  {"---", none},
  {"---", none},
  {"BIT", zeropage},
  {"AND", zeropage},
  {"ROL", zeropage},
  {"---", none},
  {"PLP", implied},
  {"AND", immediate},
  {"ROL", accumulator},
  {"---", none},
  {"BIT", absolute},
  {"AND", absolute},
  {"ROL", absolute},
  {"---", none},
  {"BMI", relative}, // 0x30
  {"AND", indirect_y},
  {"---", none},
  {"---", none},
  {"---", none},
  {"AND", zeropage_x},
  {"ROL", zeropage_x},
  {"---", none},
  {"SEC", implied},
  {"AND", absolute_y},
  {"---", none},
  {"---", none},
  {"---", none},
  {"AND", absolute_x},
  {"ROL", absolute_x},
  {"---", none},
  {"RTI", implied}, // 0x40
  {"EOR", indirect_x},
  {"---", none},
  {"---", none},
  {"---", none},
  {"EOR", zeropage},
  {"LSR", zeropage},
  {"---", none},
  {"PHA", implied},
  {"EOR", immediate},
  {"LSR", accumulator},
  {"---", none},
  {"JMP",   absolute},
  {"EOR", absolute},
  {"LSR", absolute},
  {"---", none},
  {"BVC", relative}, // 0x50
  {"EOR", indirect_y},
  {"---", none},
  {"---", none},
  {"---", none},
  {"EOR", zeropage_x},
  {"LSR", zeropage_x},
  {"---", none},
  {"CLI", implied},
  {"EOR", absolute_y},
  {"---", none},
  {"---", none},
  {"---", none},
  {"EOR", absolute_x},
  {"LSR", absolute_x},
  {"---", none},
  {"RTS", implied}, // 0x60
  {"ADC", indirect_x},
  {"---", none},
  {"---", none},
  {"---", none},
  {"ADC", zeropage},
  {"ROR", zeropage},
  {"---", none},
  {"PLA", implied},
  {"ADC", immediate},
  {"ROR", accumulator},
  {"---", none},
  {"JMP", indirect},
  {"ADC", absolute},
  {"ROR", absolute},
  {"---", none},
  {"BVS", relative}, // 0x70
  {"ADC", indirect_y},
  {"---", none},
  {"---", none},
  {"---", none},
  {"ADC", zeropage_x},
  {"ROR", zeropage_x},
  {"---", none},
  {"SEI", implied},
  {"ADC", absolute_y},
  {"---", none},
  {"---", none},
  {"---", none},
  {"ADC", absolute_x},
  {"ROR", absolute_x},
  {"---", none},
  {"---", none}, // 0x80
  {"STA", indirect_x},
  {"---", none},
  {"---", none},
  {"STY", zeropage},
  {"STA", zeropage},
  {"STX", zeropage},
  {"---", none},
  {"DEY", implied},
  {"---", none},
  {"TXA", implied},
  {"---", none},
  {"STY", absolute},
  {"STA", absolute},
  {"STX", absolute},
  {"---", none},
  {"BCC", relative}, // 0x90
  {"STA", indirect_y},
  {"---", none},
  {"---", none},
  {"STY", zeropage_x},
  {"STA", zeropage_x},
  {"STX", zeropage_x},
  {"---", none},
  {"TYA", implied},
  {"STA", absolute_y},
  {"TXS", implied},
  {"---", none},
  {"---", none},
  {"STA", absolute_x},
  {"---", none},
  {"---", none},
  {"LDY", immediate}, // 0xa0
  {"LDA", indirect_x},
  {"LDX", immediate},
  {"---", none},
  {"LDY", zeropage},
  {"LDA", zeropage},
  {"LDX", zeropage},
  {"---", none},
  {"TAY", implied},
  {"LDA", immediate},
  {"TAX", implied},
  {"---", none},
  {"LDY", absolute},
  {"LDA", absolute},
  {"LDX", absolute},
  {"---", none},
  {"BCS", relative}, // 0xb0
  {"LDA", indirect_y},
  {"---", none},
  {"---", none},
  {"LDY", zeropage_x},
  {"LDA", zeropage_x},
  {"LDX", zeropage_y},
  {"---", none},
  {"CLV", implied},
  {"LDA", absolute_y},
  {"TSX", implied},
  {"---", none},
  {"LDY", absolute_x},
  {"LDA", absolute_x},
  {"LDX", absolute_y},
  {"---", none},
  {"CPY", immediate}, // 0xc0
  {"CMP", indirect_x},
  {"---", none},
  {"---", none},
  {"CPY", zeropage},
  {"CMP", zeropage},
  {"DEC", zeropage},
  {"---", none},
  {"INY", implied},
  {"CMP", immediate},
  {"DEX", implied},
  {"---", none},
  {"CPY", absolute},
  {"CMP", absolute},
  {"DEC", absolute},
  {"---", none},
  {"BNE", relative}, // 0xd0
  {"CMP", indirect_y},
  {"---", none},
  {"---", none},
  {"---", none},
  {"CMP", zeropage_x},
  {"DEC", zeropage_x},
  {"---", none},
  {"CLD", implied},
  {"CMP", absolute_y},
  {"---", none},
  {"---", none},
  {"---", none},
  {"CMP", absolute_x},
  {"DEC", absolute_x},
  {"---", none},
  {"CPX", immediate}, // 0xe0
  {"SBC", indirect_x},
  {"---", none},
  {"---", none},
  {"CPX", zeropage},
  {"SBC", zeropage},
  {"INC", zeropage},
  {"---", none},
  {"INX", implied},
  {"SBC", immediate},
  {"NOP", implied},
  {"---", none},
  {"CPX", absolute},
  {"SBC", absolute},
  {"INC", absolute},
  {"---", none},
  {"BEQ", relative}, // 0xf0
  {"SBC", indirect_y},
  {"---", none},
  {"---", none},
  {"---", none},
  {"SBC", zeropage_x},
  {"INC", zeropage_x},
  {"---", none},
  {"SED", implied},
  {"SBC", absolute_y},
  {"---", none},
  {"---", none},
  {"---", none},
  {"SBC", absolute_x},
  {"INC", absolute_x},
  {"---", none}
};

void setup() {
  Serial.begin(19200);
  pinMode(53, OUTPUT);
  pinMode(51, INPUT); // R/W
  pinMode(49, INPUT); // SYNC

  for (uint8_t i = 0; i < 16; i++)
    pinMode(addressPins[i], INPUT);
  for (uint8_t i = 0; i < 8; i++)
    pinMode(dataPins[i], INPUT);
}

void loop() {
  char output[64];
  uint8_t data = 0;
  uint16_t address = 0;

  while (Serial.available()) {
    switch(Serial.read()) {
      case 's':
        isStepped = true;
        shouldStep = true;
        break;
      case 'r':
        isStepped = false;
        break;
      case '=':
      case '+':
        if (stepSpeed)
          stepSpeed--;
        break;
      case '-':
        stepSpeed++;
        break;
    }
  }
  if (!isStepped || (isStepped && (shouldStep || !digitalRead(49)))) {
    digitalWrite(53, HIGH);
    if (!isStepped)
      delay(stepSpeed);
    for (uint8_t i = 0; i < 16; i++)
      address = (address << 1) + digitalRead(addressPins[i]);
    for (uint8_t i = 0; i < 8; i++)
      data = (data << 1) + digitalRead(dataPins[i]);
    if (digitalRead(49)) {
      sprintf(output, "%04x %02x %c\t%s %s", address, data, (digitalRead(51)) ? 'R' : 'W', instructions[data].name, address_mode_text[instructions[data].mode]);
    }
    else {
      sprintf(output, "%04x %02x %c", address, data, (digitalRead(51)) ? 'R' : 'W');
    }
    Serial.println(output);
    digitalWrite(53, LOW);
    if (!isStepped)
      delay(stepSpeed);
    shouldStep = false;
  }
}
