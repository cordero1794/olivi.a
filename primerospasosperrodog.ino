#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Llamado de esta manera, utiliza la dirección predeterminada 0x40
Adafruit_PWMServoDriver board1 = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver board2 = Adafruit_PWMServoDriver(0x41);

// Dependiendo de tu modelo de servomotor, el ancho de pulso mínimo y máximo puede variar
#define SERVOMIN  125 // este es el 'mínimo' conteo de longitud de pulso (de 4096)
#define SERVOMAX  575 // este es el 'máximo' conteo de longitud de pulso (de 4096)

int angle[16];
int servoref[16] = {84, 90, 90, 0, 90, 90, 85, 0, 96, 94, 94, 0, 88, 88, 0, 96};
int pitch;
float c[4], d[4], x[4], z[4];

void setup() {
  Serial.begin(9600);
  Serial.println("Robot cuadrúpedo con PCA9685");

  board1.begin();
  board2.begin();
  board1.setPWMFreq(60);  // Los servos analógicos funcionan a ~60 Hz
  board2.setPWMFreq(60);

  delay(10);
  c[0] = c[1] = c[2] = c[3] = 150;
  x[0] = x[1] = x[2] = x[3] = 0;
  pitch = 0;
  Serial.println("Listo para comando");
}

void loop() {
  if (Serial.available() == 0) {
    IK();
    holdposition();
  } else {
    char command = Serial.read();
    switch (command) {
      case 'w':
        moveLegsForward();
        break;
      case 's':
        moveLegsBackward();
        break;
      case 'n':
        resetLegPositions();
        break;
      case 'p':
        performPattern();
        break;
      case 'b':
        moveLegsLeft();
        break;
      case 'f':
        moveLegsRight();
        break;
      case 'a':
        performAlternatePattern();
        break;
      default:
        break;
    }
  }
}

void moveLegsForward() {
  c[1] = 150;
  c[0] = 170;
  delay(500);
  c[0] = 150;
  c[1] = 170;
  delay(500);
  c[2] = 150;
  c[3] = 170;
  delay(500);
  c[2] = 170;
  c[3] = 150;
  delay(500);
}

void moveLegsBackward() {
  c[1] = 150;
  c[0] = 120;
  delay(500);
  c[0] = 150;
  c[1] = 120;
  delay(500);
  c[2] = 150;
  c[3] = 120;
  delay(500);
  c[2] = 120;
  c[3] = 150;
  delay(500);
}

void resetLegPositions() {
  for (int i = 0; i < 4; i++) {
    c[i] = 150;
    x[i] = 0;
  }
  z[0] = z[1] = z[2] = z[3] = 0;
  pitch = 0;
  delay(500);
}

void performPattern() {
  pitch = 10;
  delay(500);
  pitch = 0;
  delay(500);
  pitch = -10;
  delay(500);
  pitch = 0;
  delay(500);
}

void moveLegsLeft() {
  z[0] = 20;
  z[1] = 20;
  delay(500);
  z[0] = 0;
  z[1] = 0;
  delay(500);
  z[2] = 20;
  z[3] = 20;
  delay(500);
  z[2] = 0;
  z[3] = 0;
  delay(500);
}

void moveLegsRight() {
  z[0] = -20;
  z[1] = -20;
  delay(500);
  z[0] = 0;
  z[1] = 0;
  delay(500);
  z[2] = -20;
  z[3] = -20;
  delay(500);
  z[2] = 0;
  z[3] = 0;
  delay(500);
}

void performAlternatePattern() {
  x[0] = x[1] = x[2] = x[3] = 0;
  z[0] = z[1] = z[2] = z[3] = 0;
  pitch = 0;
  delay(500);
  x[0] = x[1] = x[2] = x[3] = 10;
  delay(500);
  x[0] = x[1] = x[2] = x[3] = 0;
  delay(500);
}

void holdposition() {
  for (int i = 0; i < 16; i++) {
    board1.setPWM(i, 0, angleToPulse(angle[i]));
    board2.setPWM(i, 0, angleToPulse(angle[i]));
  }
}

void IK() {
  // Implementar cinemática inversa
  for (int i = 0; i < 4; i++) {
    angle[i * 4] = c[i];
    angle[i * 4 + 1] = c[i] + x[i];
    angle[i * 4 + 2] = c[i] + z[i];
    angle[i * 4 + 3] = servoref[i * 4 + 3] + pitch;
  }
}

int angleToPulse(int ang) {
  int pulse = map(ang, 0, 180, SERVOMIN, SERVOMAX); // mapear ángulo de 0 a 180 al pulso mínimo y máximo
  Serial.print("Ángulo: "); Serial.print(ang);
  Serial.print(" pulso: "); Serial.println(pulse);
  return pulse;
}

