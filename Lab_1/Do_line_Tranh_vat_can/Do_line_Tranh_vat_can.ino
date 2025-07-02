#include <RobotControl.h>
#include <QTRSensors.h>
RobotControl motor;
QTRSensors qtr;

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];

#define LED 13

int Speed =  60;

float Kp = 0.06;
float Ki = 0.0;
float Kd = 0.24; 
int  P;
int  I;
int  D;
int lastError = 0;
const uint8_t maxspeeda = 255;
const uint8_t maxspeedb = 255;
const uint8_t basespeeda = Speed;
const uint8_t basespeedb = Speed;

#define trigPin 3
#define echoPin 2
#define SOUND_SPEED 0.034             // Tốc độ âm thanh
#define CM_TO_INCH 0.393701           // Chuyển đổi cm to inch

void setup() {
  motor.begin();
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){A7, A6, A5, A4, A3, A2, A1, A0}, SensorCount);
  pinMode(LED, OUTPUT);
  delay(2000);
  digitalWrite(LED, HIGH);
  Serial.println("Calib");
  for(int i = 0; i < 300; i++){
    qtr.calibrate();
  }

  for(byte i = 0; i < 10; i++){
    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);
    delay(200);
  }
  Serial.println("Calib Done!");
}

long distanceCm;
long duration;

void readHC(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Đặt trigPin ở trạng thái HIGH trong 10 micro giây
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Đọc echoPin, trả về thời gian sóng âm di chuyển trong micro giây
  duration = pulseIn(echoPin, HIGH);
  
  // Tính khoảng cách
  distanceCm = duration * SOUND_SPEED / 2;
  Serial.println(distanceCm);
  delay(10);
}

int delat_Xoay = 500;
int dl = 400;
int hd = 250;

int16_t position;

void loop() {
  if(distanceCm<20){
    readHC();
    while(distanceCm<20){
      readHC();
      int x = 0;
      motor.speed_run(0, 0);        // Dừng
      delay(300);

      motor.speed_run(-60, -60);    // lùi
      delay(500);

      motor.speed_run(0, 0);        // Dừng
      delay(300);

      motor.speed_run(100, 0);      // Xoay trái
      delay(400);

      motor.speed_run(0, 0);        // Dừng
      delay(300);

      motor.speed_run(60, 60);      // đi thằng
      delay(700);

      motor.speed_run(0, 0);        // dừng
      delay(300);

      motor.speed_run(0, 100);      // Xoay phải
      delay(250);

      motor.speed_run(0, 0);        // dừng
      delay(300);

      motor.speed_run(60, 60);      // đi thằng
      delay(800);

      motor.speed_run(0, 0);        // dừng
      delay(300);

      motor.speed_run(0, 100);      // Xoay phải
      delay(250);

      motor.speed_run(0, 0);        // dừng
      delay(300);

      x = 1;

      while(x == 1){
        position = qtr.readLineBlack(sensorValues);
        if(position <= 6999 && position >= 1){
          break;
        }
        motor.speed_run(60, 60);
      }
      break;
    }
  }


  position = qtr.readLineBlack(sensorValues);
  Serial.println(position);
  int error = 3500 - position;
  //motor.speed_run(20, 20);
  P = error;
  I = I + error;
  D = error - lastError;
  lastError = error;

  int motorspeed = P*Kp + I*Ki + D*Kd;

  int motorspeeda = basespeeda + motorspeed / 2;
  int motorspeedb = basespeedb - motorspeed / 2;
  
  if (motorspeeda > maxspeeda) {
    motorspeeda = maxspeeda;
  }
  if (motorspeedb > maxspeedb) {
    motorspeedb = maxspeedb;
  }
  if (motorspeeda < 0) {
    motorspeeda = 0;
  }
  if (motorspeedb < 0) {
    motorspeedb = 0;
  } 
  motor.speed_run(motorspeedb, motorspeeda);
  readHC();
}