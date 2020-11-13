/*
モータドライバ
1*4メンブレンスイッチ
ボリュームスイッチ（速度調整。A1に接続）
ボタン（リセットとGNDに接続。緊急停止スイッチの代わり）

ボタン1、2,3：1分2分3分の稼働
ボタン4：逆転10秒

*/

#include <Keypad.h>
const byte ROWS = 1;
const byte COLS = 4;

//1*4用のセッティング
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', '4'}
};
byte rowPins[ROWS] = {6}; //接続するピン番号
byte colPins[COLS] = {4, 5, 2, 3};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
char customKey;

//モータドライバのピンアサイン
int CS = 14; //A0
int EN = 12;
int INA = 11;
int INB = 10;
int pwm = 9;

int I;//CSから値を取得 今のところ使用していない。モータにかかる負荷からemptyが検出できるはず

//ボリューム用の設定
int sensorPin = 15;//A1
int value, mSpeed, i;

//タイマー関係
unsigned long startMillis ;
unsigned long setTimer,sec=1000; //時間はU-longなので注意

void setup() {

  pinMode(EN, OUTPUT);
  pinMode(INA, OUTPUT);
  pinMode(INB, OUTPUT);
  pinMode(pwm, OUTPUT);
  pinMode(CS, INPUT);
  pinMode(sensorPin, INPUT);

  digitalWrite(13, LOW);

  Serial.begin(9600);//test用 PCと9600で接続
  Serial.println(0);
}

void loop() {
  customKey = customKeypad.getKey();
  switch (customKey)//ボタンを押されるまで待機
  {
    case '1'://1分
      Serial.println(1);
      Serial.println(analogRead(sensorPin));
      digitalWrite(EN, HIGH);
      digitalWrite(INA, HIGH);
      digitalWrite(INB, LOW);
      setTimer = 60000;
      timeControl();
      Serial.println("F");
      break;

    case '2'://2分
      Serial.println(2);
      digitalWrite(EN, HIGH);
      digitalWrite(INA, HIGH);
      digitalWrite(INB, LOW);
      setTimer = 120000;
      Serial.println(setTimer);      
      timeControl();
      Serial.println("F2");
      break;

    case '3'://3分
      Serial.println(3);
      digitalWrite(EN, HIGH);
      digitalWrite(INA, HIGH);
      digitalWrite(INB, LOW);
      setTimer = 180000;
      timeControl();
      Serial.println("F3");
      break;

    case '4'://Reverse 10秒
      Serial.println(4);
      digitalWrite(EN, HIGH);
      digitalWrite(INA, LOW);
      digitalWrite(INB, HIGH);
      setTimer = 10 * 1000;
      timeControl();
      Serial.println("F4");
      break;

      //default:
      // どのcaseにも一致しなかったとき実行される
      // (defaultは省略可能)
  }
}

void timeControl() {
  startMillis = millis();//スタート時間を記録
  digitalWrite(LED_BUILTIN, HIGH);
  Acceleration();

  while (millis() - startMillis < setTimer) {//時間経過まで繰り返す
    value = analogRead(sensorPin);
    mSpeed = map(value, 0, 1023, 0, 255);// 0～1023の入力を0～255の範囲にスケーリング
    analogWrite(pwm, mSpeed);
    
    I=analogRead(14);
    Serial.println(I);
  }

  //時間経過後
  Deceleration();
  digitalWrite(EN, LOW);
  digitalWrite(LED_BUILTIN, LOW);
}

void Acceleration() {//スタート時の加速
  //可変抵抗の値からモータの値が決まる
  value = analogRead(sensorPin);
  mSpeed = map(value, 0, 1023, 0, 255);// 0～1023の入力を0～255の範囲にスケーリング
  for (i = 0; i < mSpeed; i++) {
    analogWrite(pwm, i);
    delay(8);
  }
}

void   Deceleration() {//終了時のブレーキ
  for (i = mSpeed; i > 0; i--) {
    analogWrite(pwm, i);
    delay(8);
  }
  analogWrite(pwm, 0);
}
