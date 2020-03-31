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
  uint8_t cycle;
} instruction_t;

instruction_t instructions[] =
{
  {"BRK",  implied,    7}, // 0x00
  {"ORA", indirect_x,   0},
  {"---", none,     0},
  {"---", none,     0},
  {"---", none,     0},
  {"ORA", zeropage,   0},
  {"ASL", zeropage,   0},
  {"---", none,     0},
  {"PHP", implied,    0},
  {"ORA", immediate,    0},
  {"ASL", accumulator,  0},
  {"---", none,     0},
  {"---", none,     0},
  {"ORA", absolute,   0},
  {"ASL", absolute,   0},
  {"---", none,     0},
  {"BLP", relative,   0}, // 0x10
  {"ORA", indirect_y,   0},
  {"---", none,     0},
  {"---", none,     0},
  {"---", none,     0},
  {"ORA", zeropage_x,   0},
  {"ASL", zeropage_x,   0},
  {"---", none,     0},
  {"CLC", implied,    0},
  {"ORA", absolute_y,   0},
  {"---", none,     0},
  {"---", none,     0},
  {"---", none,     0},
  {"ORA", absolute_x,   0},
  {"ASL", absolute_x,   0},
  {"---", none,     0},
  {"JSR", absolute,   0}, // 0x20
  {"AND", indirect_x,   0},
  {"---", none,     0},
  {"---", none,     0},
  {"BIT", zeropage,   0},
  {"AND", zeropage,   0},
  {"ROL", zeropage,   0},
  {"---", none,     0},
  {"PLP", implied,    0},
  {"AND", immediate,    0},
  {"ROL", accumulator,  0},
  {"---", none,     0},
  {"BIT", absolute,   0},
  {"AND", absolute,   0},
  {"ROL", absolute,   0},
  {"---", none,     0},
  {"BMI", relative,   0}, // 0x30
  {"AND", indirect_y,   0},
  {"---", none,     0},
  {"---", none,     0},
  {"---", none,     0},
  {"AND", zeropage_x,   0},
  {"ROL", zeropage_x,   0},
  {"---", none,     0},
  {"SEC", implied,    0},
  {"AND", absolute_y,   0},
  {"---", none,     0},
  {"---", none,     0},
  {"---", none,     0},
  {"AND", absolute_x,   0},
  {"ROL", absolute_x,   0},
  {"---", none,     0},
  {"RTI", implied,    0}, // 0x40
  {"EOR", indirect_x,   0},
  {"---", none,     0},
  {"---", none,     0},
  {"---", none,     0},
  {"EOR", zeropage,   0},
  {"LSR", zeropage,   0},
  {"---", none,     0},
  {"PHA", implied,    0},
  {"EOR", immediate,    0},
  {"LSR", accumulator,  0},
  {"---", none,     0},
  {"JMP",   absolute,   0},
  {"EOR", absolute,   0},
  {"LSR", absolute,   0},
  {"---", none,     0},
  {"BVC", relative,   0}, // 0x50
  {"EOR", indirect_y,   0},
  {"---", none,     0},
  {"---", none,     0},
  {"---", none,     0},
  {"EOR", zeropage_x,   0},
  {"LSR", zeropage_x,   0},
  {"---", none,     0},
  {"CLI", implied,    0},
  {"EOR", absolute_y,   0},
  {"---", none,     0},
  {"---", none,     0},
  {"---", none,     0},
  {"EOR", absolute_x,   0},
  {"LSR", absolute_x,   0},
  {"---", none,     0},
  {"RTS", implied,    0}, // 0x60
  {"ADC", indirect_x,   0},
  {"---", none,     0},
  {"---", none,     0},
  {"---", none,     0},
  {"ADC", zeropage,   0},
  {"ROR", zeropage,   0},
  {"---", none,     0},
  {"PLA", implied,    0},
  {"ADC", immediate,    0},
  {"ROR", accumulator,  0},
  {"---", none,     0},
  {"JMP", indirect,   0},
  {"ADC", absolute,   0},
  {"ROR", absolute,   0},
  {"---", none,     0},
  {"BVS", relative,   0}, // 0x70
  {"ADC", indirect_y,   0},
  {"---", none,     0},
  {"---", none,     0},
  {"---", none,     0},
  {"ADC", zeropage_x,   0},
  {"ROR", zeropage_x,   0},
  {"---", none,     0},
  {"SEI", implied,    0},
  {"ADC", absolute_y,   0},
  {"---", none,     0},
  {"---", none,     0},
  {"---", none,     0},
  {"ADC", absolute_x,   0},
  {"ROR", absolute_x,   0},
  {"---", none,     0},
  {"---", none,     0}, // 0x80
  {"STA", indirect_x,   0},
  {"---", none,     0},
  {"---", none,     0},
  {"STY", zeropage,   0},
  {"STA", zeropage,   0},
  {"STX", zeropage,   0},
  {"---", none,     0},
  {"DEY", implied,    0},
  {"---", none,     0},
  {"TXA", implied,    0},
  {"---", none,     0},
  {"STY", absolute,   0},
  {"STA", absolute,   0},
  {"STX", absolute,   0},
  {"---", none,     0},
  {"BCC", relative,   0}, // 0x90
  {"STA", indirect_y,   0},
  {"---", none,     0},
  {"---", none,     0},
  {"STY", zeropage_x,   0},
  {"STA", zeropage_x,   0},
  {"STX", zeropage_x,   0},
  {"---", none,     0},
  {"TYA", implied,    0},
  {"STA", absolute_y,   0},
  {"TXS", implied,    0},
  {"---", none,     0},
  {"---", none,     0},
  {"STA", absolute_x,   0},
  {"---", none,     0},
  {"---", none,     0},
  {"LDY", immediate,    0}, // 0xa0
  {"LDA", indirect_x,   0},
  {"LDX", immediate,    0},
  {"---", none,     0},
  {"LDY", zeropage,   0},
  {"LDA", zeropage,   0},
  {"LDX", zeropage,   0},
  {"---", none,     0},
  {"TAY", implied,    0},
  {"LDA", immediate,    0},
  {"TAX", implied,    0},
  {"---", none,     0},
  {"LDY", absolute,   0},
  {"LDA", absolute,   0},
  {"LDX", absolute,   0},
  {"---", none,     0},
  {"BCS", relative,   0}, // 0xb0
  {"LDA", indirect_y,   0},
  {"---", none,     0},
  {"---", none,     0},
  {"LDY", zeropage_x,   0},
  {"LDA", zeropage_x,   0},
  {"LDX", zeropage_y,   0},
  {"---", none,     0},
  {"CLV", implied,    0},
  {"LDA", absolute_y,   0},
  {"TSX", implied,    0},
  {"---", none,     0},
  {"LDY", absolute_x,   0},
  {"LDA", absolute_x,   0},
  {"LDX", absolute_y,   0},
  {"---", none,     0},
  {"CPY", immediate,    0}, // 0xc0
  {"CMP", indirect_x,   0},
  {"---", none,     0},
  {"---", none,     0},
  {"CPY", zeropage,   0},
  {"CMP", zeropage,   0},
  {"DEC", zeropage,   0},
  {"---", none,     0},
  {"INY", implied,    0},
  {"CMP", immediate,    0},
  {"DEX", implied,    0},
  {"---", none,     0},
  {"CPY", absolute,   0},
  {"CMP", absolute,   0},
  {"DEC", absolute,   0},
  {"---", none,     0},
  {"BNE", relative,   0}, // 0xd0
  {"CMP", indirect_y,   0},
  {"---", none,     0},
  {"---", none,     0},
  {"---", none,     0},
  {"CMP", zeropage_x,   0},
  {"DEC", zeropage_x,   0},
  {"---", none,     0},
  {"CLD", implied,    0},
  {"CMP", absolute_y,   0},
  {"---", none,     0},
  {"---", none,     0},
  {"---", none,     0},
  {"CMP", absolute_x,   0},
  {"DEC", absolute_x,   0},
  {"---", none,     0},
  {"CPX", immediate,    0}, // 0xe0
  {"SBC", indirect_x,   0},
  {"---", none,     0},
  {"---", none,     0},
  {"CPX", zeropage,   0},
  {"SBC", zeropage,   0},
  {"INC", zeropage,   0},
  {"---", none,     0},
  {"INX", implied,    0},
  {"SBC", immediate,    0},
  {"NOP", implied,    0},
  {"---", none,     0},
  {"CPX", absolute,   0},
  {"SBC", absolute,   0},
  {"INC", absolute,   0},
  {"---", none,     0},
  {"BEQ", relative,   0}, // 0xf0
  {"SBC", indirect_y,   0},
  {"---", none,     0},
  {"---", none,     0},
  {"---", none,     0},
  {"SBC", zeropage_x,   0},
  {"INC", zeropage_x,   0},
  {"---", none,     0},
  {"SED", implied,    0},
  {"SBC", absolute_y,   0},
  {"---", none,     0},
  {"---", none,     0},
  {"---", none,     0},
  {"SBC", absolute_x,   0},
  {"INC", absolute_x,   0},
  {"---", none,     0}
};

void setup() {
  Serial.begin(19200);
  pinMode(53, OUTPUT);
  pinMode(51, INPUT);

  for (uint8_t i = 0; i < 16; i++)
    pinMode(addressPins[i], INPUT);
  for (uint8_t i = 0; i < 8; i++)
    pinMode(dataPins[i], INPUT);
}

void loop() {
  char output[64];
  uint8_t data = 0;
  uint16_t address = 0;
  uint8_t wait_cycle = 0;

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
  if (!isStepped || (isStepped && shouldStep)) {
    digitalWrite(53, HIGH);
    if (!isStepped)
      delay(stepSpeed);
    for (uint8_t i = 0; i < 16; i++)
      address = (address << 1) + digitalRead(addressPins[i]);
    for (uint8_t i = 0; i < 8; i++)
      data = (data << 1) + digitalRead(dataPins[i]);
    if (!wait_cycle) {
      sprintf(output, "%04x %02x %c\t%s %s", address, data, (digitalRead(51)) ? 'R' : 'W', instructions[data].name, address_mode_text[instructions[data].mode]);
      wait_cycle = instructions[data].cycle -1;
    }
    else {
      sprintf(output, "%04x %02x %c", address, data, (digitalRead(51)) ? 'R' : 'W');
      wait_cycle--;
    }
    Serial.println(output);
    digitalWrite(53, LOW);
    if (!isStepped)
      delay(stepSpeed);
    shouldStep = false;
  }
}
