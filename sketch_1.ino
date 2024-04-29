#include <Servo.h>
#include <EncButton.h>

#define LED_PIN 3           //Пин светодиода
#define SERVO_PIN 5         //Пин сервы

uint8_t ANGL_OPEN = 90;     //Угол открытия
uint8_t ANGL_CLOSE = 40;    //Угол закрытия
bool mode = false;          //Статус работы: true - таймер включен, false - таймер выключен
bool state;                 //Статус сервы: true - открыта, false - закрыта+
bool indikator = false;
uint8_t time_set = 10;      //Время сброса по умолчанию, сек
uint32_t tmr1;              //Переменная таймера
uint32_t i;                 //Переменная для цикла
uint8_t clicks = 0;

Button btn(4);              //Пин кнопки
Servo servo_drop;           //Имя сервы servo_drop

void setup() { 
    //Serial.begin(9600);
    pinMode(LED_PIN, OUTPUT);
    pinMode(SERVO_PIN, OUTPUT);
    pinMode(4, INPUT);
    servo_drop.attach(SERVO_PIN);
    btn.setHoldTimeout(1000);             //
    digitalWrite(LED_PIN, LOW);
    servo_drop.write(ANGL_CLOSE);
}

void loop() {
  if (mode == true && millis() - tmr1 >= time_set*1000)       //Основной цикл остановки таймера (открытие сброса по истечению времени) если режим - включен и время превысило заданное
  {
    mode = false;                                             //Сбрасываем режим на выключен
    servo_drop.write(ANGL_OPEN);                              //Открываем
    digitalWrite(LED_PIN, LOW);
    state = true;                                             //Устанавливаем статус сервы как открытая
    //Serial.println("timer OFF");
  }
  if (mode == true) {                                         //Цикл для индикации, если режим работа - просто горит
    digitalWrite(LED_PIN, HIGH);
  }
  else if (mode == false && i < time_set / 10) {              //Если НЕ работа и проверка счетчика морганий. Моргает столько раз, сколько выбрано десятков секунд
    digitalWrite(LED_PIN, HIGH);
    delay(50);
    digitalWrite(LED_PIN, LOW);
    delay(200);
    i++;
    //Serial.print("|");
  }
  else if (mode == false && i >= time_set / 10) {             //Сброс счетчика морганий
    //Serial.println(i);
    i = 0;
    delay(1000);
    indikator = false;
  }
}

void yield() {                                                  //Цикл прерываний для считывания кнопки
  btn.tick();
    if (btn.hold()) {                                             //Если зажата
    //Serial.print("hold");
    //Serial.print(time_set);
    //Serial.println("sec");
    if (mode == false && state == false)                        //Если таймер ещё не запущен и статус сервы закрыта - запускает таймер
    {
      mode = true;                                              //Флаг работа - включен
      tmr1 = millis();                                          //Такущее время для засечки таймера
      //Serial.println("timer ON");
    }
  }

  if (btn.hasClicks()) {                                        //Обработка одиночных нажатий
    clicks = btn.getClicks();
    if (clicks > 1)                                    // Если больше одного клика, устанавливает время сброса в десятках секунд в соответствии с количеством кликов
    {
      time_set = 10*clicks;
      indikator = true;
      clicks = 0;
      //Serial.println(time_set);
    }
    if (clicks == 1 && state == false)            //Если один клик и серва закрыта - открывает серву и останавливает таймер, если запущен
    {
      clicks = 0;
      mode = false;
      state = true;
      servo_drop.write(ANGL_OPEN);
      
      //Serial.println("OPEN");
    }
    if (clicks == 1 && state == true)             //Если один клик и серва открыта - закрывает серву
    {
      clicks = 0;
      mode = false;
      state = false;
      servo_drop.write(ANGL_CLOSE);
      
      //Serial.println("CLOSE");
    }
  }
    delay(100);
}
