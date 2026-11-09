// Файл LineRobot_v1.ino
// Лабораторная работа «Гонки робота по линии на двухколёсной платформе»
// Arduino UNO + Motor Shield L298P + Troyka Shield + 2 датчика TCRT5000

// директивы препроцессора, задаём символьные имена
// двигатели подключаются к клеммам M1+, M1-, M2+, M2-
// Motor Shield использует четыре контакта 6, 5, 7, 4 для управления двигателями
// контакты 6, 5 поддерживают ШИМ, являются контактами включения двигателей
// 4 и 7 — для направления, 5 и 6 — для скорости
#define SPEED_LEFT   6   // пин 6 определяет скорость движения L-двигателя
#define SPEED_RIGHT  5   // пин 5 определяет скорость движения R-двигателя

// контакты 7, 4 отвечают за направление вращения двигателей
#define DIR_LEFT     7   // L-двигатель
#define DIR_RIGHT    4   // R-двигатель

#define LEFT_SENSOR_PIN   8   // датчик линии Left
#define RIGHT_SENSOR_PIN  9   // датчик линии Right

// скорость, с которой робот движется вперёд (0-255)
#define SPEED            200
// скорость прохождения сложных участков
#define SLOW_SPEED       130
// скорость обратного хода для сложного участка
#define BACK_SLOW_SPEED   30
// скорость обратного хода
#define BACK_FAST_SPEED   50

// коэффициент, задающий во сколько раз нужно затормозить
// одно из колёс для поворота
#define BRAKE_K            3   // коэффициент торможения

// имена состояний робота
#define STATE_FORWARD  0
#define STATE_RIGHT    1
#define STATE_LEFT     2
#define STATE_STOP     3

#define SPEED_STEP     2       // приращение скорости
// пороговое значение времени завершения поворота
#define FAST_TIME_THRESHOLD 40

int state = STATE_FORWARD;     // начальное состояние
int currentSpeed = SPEED;      // текущая скорость
int fastTime = 0;              // начальное время

// функция «Следование линии»
void runForward()
{
  state = STATE_FORWARD;
  fastTime = fastTime + 1;
  if (fastTime < FAST_TIME_THRESHOLD) {
    currentSpeed = SLOW_SPEED;
  } else {
    currentSpeed = min(currentSpeed + SPEED_STEP, SPEED);
  }

  // генерация аналогового сигнала на M1 и M2
  // двигатели движутся с одинаковой скоростью currentSpeed
  analogWrite(SPEED_LEFT, currentSpeed);
  analogWrite(SPEED_RIGHT, currentSpeed);

  // если HIGH, двигатели будут перемещать соответствующие колёса вперёд,
  // если LOW - назад
  digitalWrite(DIR_LEFT, HIGH);
  digitalWrite(DIR_RIGHT, HIGH);
}

// функция «Поворот направо»
void steerRight()
{
  state = STATE_RIGHT;
  fastTime = 0;
  // замедляем правое колесо относительно левого, чтобы начать поворот
  analogWrite(SPEED_RIGHT, SPEED / BRAKE_K);
  analogWrite(SPEED_LEFT, SPEED);
  // двигатели будут перемещать соответствующие колёса вперёд
  digitalWrite(DIR_LEFT, HIGH);
  digitalWrite(DIR_RIGHT, HIGH);
}

// функция «Поворот налево»
void steerLeft()
{
  state = STATE_LEFT;
  fastTime = 0;
  // замедляем левое колесо относительно правого
  analogWrite(SPEED_LEFT, SPEED / BRAKE_K);
  analogWrite(SPEED_RIGHT, SPEED);
  digitalWrite(DIR_LEFT, HIGH);
  digitalWrite(DIR_RIGHT, HIGH);
}

// функция «Двигатели останавливаются»
void stop()
{
  state = STATE_STOP;
  fastTime = 0;
  // остановка, скорость равна нулю
  analogWrite(SPEED_RIGHT, 0);
  analogWrite(SPEED_LEFT, 0);
  digitalWrite(DIR_LEFT, LOW);
  digitalWrite(DIR_RIGHT, LOW);
  // ожидание нажатия Reset или отключения питания
  while (true);
}

// управление направлением движения и скоростью
// duration - длительность, state - состояние.
// Когда робот находится перед поворотом, ему необходимо двигаться
// не прямо назад, а немного поворачиваясь.
void stepBack(int duration, int state)
{
  if (!duration)
    return;

  // в зависимости от направления поворота при движении назад
  // делается небольшой разворот
  int leftSpeed;
  if (state == STATE_RIGHT) {
    leftSpeed = BACK_SLOW_SPEED;
  } else {
    leftSpeed = BACK_FAST_SPEED;
  }

  int rightSpeed;
  if (state == STATE_LEFT) {
    rightSpeed = BACK_SLOW_SPEED;
  } else {
    rightSpeed = BACK_FAST_SPEED;
  }

  analogWrite(SPEED_LEFT, leftSpeed);
  analogWrite(SPEED_RIGHT, rightSpeed);
  // реверс колёс
  digitalWrite(DIR_RIGHT, LOW);
  digitalWrite(DIR_LEFT, LOW);
  delay(duration);
}

// инициализация пинов платы расширения Motor Shield
// и начального состояния автомата
void setup()
{
  // настройка выводов 4, 5, 6, 7 платы на вывод сигналов
  for (int i = 4; i <= 7; i++)
    pinMode(i, OUTPUT);

  // выводы датчиков на вход
  pinMode(LEFT_SENSOR_PIN, INPUT);
  pinMode(RIGHT_SENSOR_PIN, INPUT);

  // перемещение вперёд
  runForward();
}

// бесконечный цикл
void loop()
{
  // робот едет по белому полю с чёрным треком.
  // В обратном случае не нужно инвертировать значения с датчиков
  boolean left = !digitalRead(LEFT_SENSOR_PIN);
  boolean right = !digitalRead(RIGHT_SENSOR_PIN);

  // в какое состояние нужно перейти?
  int targetState;   // последующее состояние

  if (left == 0 && right == 0) {
    // под сенсорами всё белое, едем вперёд
    targetState = STATE_FORWARD;
  } else if (left == 1 && right == 1) {
    // под сенсорами всё чёрное, робот останавливается
    targetState = STATE_STOP;
  } else if (left) {
    // левый сенсор упёрся в трек, поворот налево
    targetState = STATE_LEFT;
  } else {
    // правый сенсор упёрся в трек, поворот направо
    targetState = STATE_RIGHT;
  }

  if (state == targetState) {
    // маршрут не изменился, скорость и направление движения не меняем.
    // На прямой продолжаем вызывать runForward(), чтобы робот
    // постепенно набирал скорость
    if (state == STATE_FORWARD) {
      runForward();
    }
    delay(50);
    return;   // выполнение измерения заново
  }

  switch (targetState) {
    case STATE_FORWARD:
      runForward();
      break;
    case STATE_RIGHT:
      steerRight();
      break;
    case STATE_LEFT:
      steerLeft();
      break;
    case STATE_STOP:
      stop();
      break;
  }

  // не позволяем сильно вилять на прямой
  delay(50);
}
