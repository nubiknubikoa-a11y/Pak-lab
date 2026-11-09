// Датчики линии
const int LEFT_SENSOR = 5;
const int RIGHT_SENSOR = 6;

// Мотор A
const int MOTOR_A_SPEED = 3;
const int MOTOR_A_DIR = 12;

// Мотор B
const int MOTOR_B_SPEED = 11;
const int MOTOR_B_DIR = 13;

// Скорость движения (0-255)
int speedForward = 130;
int speedTurn = 110;

// Если датчик видит черную линию,
// обычно его сигнал LOW.
// При необходимости поменяем на HIGH.
const int BLACK = LOW;

void setup() {
  pinMode(LEFT_SENSOR, INPUT);
  pinMode(RIGHT_SENSOR, INPUT);

  pinMode(MOTOR_A_SPEED, OUTPUT);
  pinMode(MOTOR_A_DIR, OUTPUT);

  pinMode(MOTOR_B_SPEED, OUTPUT);
  pinMode(MOTOR_B_DIR, OUTPUT);

  stopRobot();
}

void loop() {
  int left = digitalRead(LEFT_SENSOR);
  int right = digitalRead(RIGHT_SENSOR);

  // Оба датчика видят черную линию
  if (left == BLACK && right == BLACK) {
    forward();
  }

  // Левый датчик видит линию
  else if (left == BLACK && right != BLACK) {
    turnLeft();
  }

  // Правый датчик видит линию
  else if (left != BLACK && right == BLACK) {
    turnRight();
  }

  // Линия потеряна
  else {
    stopRobot();
  }
}

// Движение вперед
void forward() {
  digitalWrite(MOTOR_A_DIR, HIGH);
  digitalWrite(MOTOR_B_DIR, HIGH);

  analogWrite(MOTOR_A_SPEED, speedForward);
  analogWrite(MOTOR_B_SPEED, speedForward);
}

// Поворот налево
void turnLeft() {
  digitalWrite(MOTOR_A_DIR, LOW);
  digitalWrite(MOTOR_B_DIR, HIGH);

  analogWrite(MOTOR_A_SPEED, speedTurn);
  analogWrite(MOTOR_B_SPEED, speedTurn);
}

// Поворот направо
void turnRight() {
  digitalWrite(MOTOR_A_DIR, HIGH);
  digitalWrite(MOTOR_B_DIR, LOW);

  analogWrite(MOTOR_A_SPEED, speedTurn);
  analogWrite(MOTOR_B_SPEED, speedTurn);
}

// Остановка
void stopRobot() {
  analogWrite(MOTOR_A_SPEED, 0);
  analogWrite(MOTOR_B_SPEED, 0);
}
