const uint8_t addressPins[16] = {22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52};
const uint8_t dataPins[8] = {23, 25, 27, 29, 31, 33, 35, 37};
bool isStepped = true;
bool shouldStep = false;
uint16_t stepSpeed = 25;

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
  char output[9];
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
  if (!isStepped || (isStepped && shouldStep)) {
    digitalWrite(53, HIGH);
    if (!isStepped)
      delay(stepSpeed);
    for (uint8_t i = 0; i < 16; i++)
      address = (address << 1) + digitalRead(addressPins[i]);
    for (uint8_t i = 0; i < 8; i++)
      data = (data << 1) + digitalRead(dataPins[i]);
    sprintf(output, "%04x %02x %c", address, data, (digitalRead(51)) ? 'R' : 'W');
    Serial.println(output);
    digitalWrite(53, LOW);
    if (!isStepped)
      delay(stepSpeed);
    shouldStep = false;
  }
}
