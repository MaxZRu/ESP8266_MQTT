// подключаем библиотеку «ESP8266WiFi»:
#include <ESP8266WiFi.h>
//#include <OneWire.h>
#include <PubSubClient.h>
//#include <DallasTemperature.h>

// Update these with values suitable for your network.

const char* ssid = "xxx";
const char* password = "xxx";
const char* mqtt_server = "192.168.111.9";

#define RELE_1 4                       // Реле №1
#define RELE_2 5                       // Реле №2
#define RELE_3 13                       // Реле №3
#define ONE_WIRE_BUS 2                  // Температура
#define BUTTON_1 12                       // Кнопочный выключатель №1
#define BUTTON_2 13                        // Кнопочный выключатель №2
#define relays_topic1 "homebridge/from/POEZD1/RELE_1"          // Топик реле №1
#define relays_topic2 "homebridge/from/POEZD1/RELE_2"          // Топик реле №2
#define relays_topic3 "homebridge/from/POEZD1/RELE_3"          // Топик реле №3
#define ONE_WIRE_BUS1 "homebridge/from/POEZD1/ONE_WIRE"         // Топик ONE_WIRE


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
long last_mls = millis();
char msg[50];
int value = 0;
int hum = 0;
int diff = 1;
boolean rState1 = false;
boolean rState2 = false;
boolean rState3 = false;
boolean btnPress1 = false;
boolean btnPress2 = false;
boolean btnPress3 = false;
boolean lastbtnStat1 = false;
boolean lastbtnStat2 = false;
boolean lastbtnStat3 = false;

// создаем экземпляр класса oneWire; с его помощью
// можно коммуницировать с любыми девайсами, работающими
// через интерфейс 1-Wire, а не только с температурными датчиками
// от компании Maxim/Dallas:
//OneWire oneWire(ONE_WIRE_BUS);

// передаем объект oneWire объекту DS18B20:
//DallasTemperature DS18B20(&oneWire);
char temperatureCString[6];
char temperatureFString[6];

void setup() {
// инициализируем последовательный порт (для отладочных целей):
  Serial.begin(115200);
  delay(10);
  Serial.println("INIT");
  
//  DS18B20.begin(); // по умолчанию разрешение датчика – 9-битное;
                   // если у вас какие-то проблемы, его имеет смысл
                   // поднять до 12 бит; если увеличить задержку,
                   // это даст датчику больше времени на обработку
                   // температурных данных
  pinMode(RELE_1, OUTPUT);
  pinMode(RELE_2, OUTPUT);
  pinMode(RELE_3, OUTPUT);
//  pinMode(BUTTON_1, INPUT);
//  pinMode(BUTTON_2, INPUT);
//  pinMode(ONE_WIRE_BUS, INPUT);
  analogWrite(RELE_1, 0);
  analogWrite(RELE_2, 0);
  Serial.begin(115200); 
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  delay(100);
  WiFi.begin(ssid, password);
  Serial.println("WIFI Coonnect");

    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WIFI OK");

  delay(1000);
  Serial.println("MQTT Connect");

  client.connect("ESP8266Client");
  client.subscribe(relays_topic1);
  client.subscribe(relays_topic2);
  client.subscribe(relays_topic3);
  lastbtnStat1 = digitalRead(BUTTON_1);
  lastbtnStat2 = digitalRead(BUTTON_2);
//  lastbtnStat3 = digitalRead(BUTTON_3);

  client.publish(relays_topic1, String(rState1).c_str(), true);
  client.publish(relays_topic2, String(rState1).c_str(), true);
  client.publish(relays_topic3, String(rState1).c_str(), true);
  
}



void reconnect_server() {
  
  if (WiFi.status() != WL_CONNECTED){
    WiFi.begin(ssid, password);
    Serial.println("");
    Serial.println("WiFi connect...");
  } else {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
    
  if(!client.connected() && WiFi.status() == WL_CONNECTED){  
    if (client.connect("ESP8266Client")) {    
      Serial.println("Mosquitto connect...");
  client.subscribe(relays_topic1);
  client.subscribe(relays_topic2);
  client.subscribe(relays_topic3);
    } else {    
      Serial.print("failed connect Mosquitto, rc=");
      Serial.print(client.state());
      Serial.println("");
    }
  }
}


// Блок [обратная связь] ****************************************************
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    String strTopic = String(topic);
    String strPayload = String((char*)payload);
    strPayload=strPayload.substring(0,length);
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
// Блок [Управление Реле 1 из MajorDomo] ************************************
if (strTopic == "homebridge/from/POEZD1/RELE_1") {
    if ((char)payload[0] == '1') {
        rState1 = true;
        //digitalWrite(RELE_1, rState1);
        analogWrite(RELE_1, 500);
    }
    else {
        rState1 = false;
        analogWrite(RELE_1, 0);
    }
}
// END Блок [Управление Реле 1 из MajorDomo] ********************************
// Блок [Управление Реле 2 из MajorDomo] ************************************
if (strTopic == "homebridge/from/POEZD1/RELE_2") {
    if ((char)payload[0] == '1') {
        rState2 = true;
       // digitalWrite(RELE_2, rState2);
        analogWrite(RELE_2, 500);
    }
    else {
        rState2 = false;
        analogWrite(RELE_2, 0);
    }
}
// Блок [Управление Реле 2 из MajorDomo] ************************************
// Блок [Управление Реле 3 из MajorDomo] ************************************
if (strTopic == "homebridge/from/POEZD1/RELE_3") {
 int  speedMotor = strPayload.toInt();
 Serial.println(speedMotor);
    if (speedMotor>0)
    {
         analogWrite(RELE_1, 0);
         analogWrite(RELE_2, speedMotor);
    }
    else {
         analogWrite(RELE_2, 0);
         analogWrite(RELE_1, -speedMotor);
    }
}
// Блок [Управление Реле 3 из MajorDomo] ************************************
}
// END Блок [обратная связь] ************************************************



bool checkBound(float newValue, float prevValue, float maxDiff) {
  return newValue < prevValue - maxDiff || newValue > prevValue + maxDiff;
}



// Выключатель №1
void buttonWF(){
  btnPress1 = digitalRead(BUTTON_1);
  if (btnPress1 && !lastbtnStat1){
    delay(400); // защита от дребезга (600 мсек.)
    btnPress1 = digitalRead(BUTTON_1);
  
    if(btnPress1){
      rState1 = !rState1;
      digitalWrite(RELE_1, rState1);
      // публикуем изменение состояния реле на брокер    
      client.publish(relays_topic1, String(rState1).c_str(), true);
    }
  }
  lastbtnStat1 = btnPress1;
}
// END Выключатель №1

// Выключатель №2

void buttonWD(){
  btnPress2 = digitalRead(BUTTON_2);
  if (btnPress2 && !lastbtnStat2){
    delay(400); // защита от дребезга (600 мсек.)
    btnPress2 = digitalRead(BUTTON_2);
  
    if(btnPress2){
      rState2 = !rState2;
      digitalWrite(RELE_2, rState2);
      // публикуем изменение состояния реле на брокер    
      client.publish(relays_topic2, String(rState2).c_str(), true);
    }
  }
  lastbtnStat2 = btnPress2;
  
}
// END Выключатель №2

// Выключатель №3


void getTemperature() {
//float celsius = 0;
//Serial.println(celsius);
//char temperaturenow [15];
//dtostrf(celsius,7, 3, temperaturenow); //// convert float to char
//client.publish(ONE_WIRE_BUS1, String(temperaturenow).c_str(), true);
}

void loop() {

  client.loop();
  // 120 секунд периодичность проверки переподключения
  if (millis() - last_mls > 10000) {
    last_mls = millis();
    reconnect_server();
  }
//  buttonWF(); // Функция кнопки №1 без фиксации и публикация статуса реле на MQTT
//  buttonWD(); // Функция кнопки №1 без фиксации и публикация статуса реле на MQTT
//  getTemperature(); // Функция кнопки №1 без фиксации и публикация статуса реле на MQTT
  delay(20);
}
