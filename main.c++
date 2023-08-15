#include <ESP32Servo.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "wifi 이름";
const char* password = "wifi 비밀번호";

String serverName = "IP 주소";
int serverPort = 8000;
WiFiClient client;

String response="";

int score = 0; // 점수 합계
int s = 0;    //res: status

//서보 각도
int up = 90;
int down = 20;

int scores[5] = {10, 10, 10, 20, 20};

int lightPin[5] = {9, 10, 11, 12, 13};
int lightValue[5] = {0, 0, 0, 0, 0};

int servoPin[5] = {4, 5, 6, 7, 17};

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;

int minUs = 1000;
int maxUs = 2000;

int flag = 0;

void setup() {
  Serial.begin(9600);

  for(int i = 0; i < 5; i++) 
    pinMode(lightPin[i], INPUT);

  	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
  	ESP32PWM::allocateTimer(4);

	servo1.setPeriodHertz(50);      // Standard 50hz servo
	servo2.setPeriodHertz(50);      // Standard 50hz servo
	servo3.setPeriodHertz(330);      // Standard 50hz servo
	servo4.setPeriodHertz(200);      // Standard 50hz servo
	servo5.setPeriodHertz(330);      // Standard 50hz servo
  
  // WiFi 연결
  WiFi.begin(ssid, password);
  Serial.println("WiFi Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  servo1.write(90);
  servo2.write(90);
  servo3.write(90);
  servo4.write(90);
  servo5.write(90);

  servo1.attach(servoPin[0], 500, 2400);
  servo2.attach(servoPin[1], 500, 2400);
  servo3.attach(servoPin[2], 500, 2400);
  servo4.attach(servoPin[3], 500, 2400);
  servo5.attach(servoPin[4], 500, 2400);
  
}

void loop() {

  // readLight();

  postToServer();
  setServo();
  workingLightServo();

}

void postToServer(){
  if (WiFi.status() == WL_CONNECTED) {
    if (client.connect(serverName.c_str(), serverPort)) {
      Serial.println("Connected To Server");
    }
    else{
      Serial.println("Connection Error");
      return;
    }
      
    String jsonData = String("{\"target\":1,\"score\":")+ score +"}";
        
    client.println("POST /arduino/status HTTP/1.1");
    client.println("Host: localhost");
    client.println("Content-Type: application/json");
    client.println("Content-Length: " + String(jsonData.length()));
    client.println();
    client.println(jsonData);
    client.println();

    Serial.print("요청 : ");
    Serial.println(jsonData);

    // 서버로부터 응답 받기
    while (client.connected()) {

      if (client.available()) {
        response = client.readString();
        // Serial.println(response);
        // Serial.println();
        s = response.charAt(response.indexOf("status") + 8) - '0';
        Serial.print("res: ");
        Serial.println(s);
        break;
      }
    }
    client.flush();
    client.stop();
   
  }
  else {
    Serial.println("WiFi Disconnected");
  }
    
}

void readLight(){

//  for(int i = 0; i < 1; i++) {
    int value = analogRead(lightPin[0]);
    Serial.print("센서값 ");
    Serial.print(": ");
    Serial.println(value);
    // delay(300);
//  }
   
}

void setServo() {

  if(s == 0) {
    servo1.write(down);
    servo2.write(down);
    servo3.write(down);
    servo4.write(down);
    servo5.write(down);
    
    flag = 0;
    score = 0;

    Serial.print("flag: ");
    Serial.println(flag);
  }
  if(s == 1 && flag==0) {

    servo1.write(up);
    servo2.write(up);
    servo3.write(up);
    servo4.write(up);
    servo5.write(up);

    flag = 1;

    Serial.print("flag: ");
    Serial.println(flag);
  }
}

void workingLightServo() {

    for(int i = 0; i < 5; i++) { 
      lightValue[i] = analogRead(lightPin[i]);
      if(lightValue[i] > 1500 && flag) 
        servoWrite(i);
    }
}

void servoWrite(int i) {
  switch(i + 1) {
    case 1:
      servo1.write(down); // 타겟 내리기
      score += scores[i];
      servo1.write(up);// 타겟 올리기
      break;

    case 2:
      servo2.write(down); // 타겟 내리기
      score += scores[i];
      servo2.write(up);// 타겟 올리기
      break;
    case 3:
      servo3.write(down); // 타겟 내리기
      score += scores[i];
      servo3.write(up);// 타겟 올리기
      break;
    case 4:
      servo4.write(down); // 타겟 내리기
      score += scores[i];
      servo4.write(up);// 타겟 올리기
      break;
    case 5:
      servo5.write(down); // 타겟 내리기
      score += scores[i];
      servo5.write(up);// 타겟 올리기
      break;
  }
}
