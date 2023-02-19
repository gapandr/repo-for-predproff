#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <espnow.h>

/* Название сети и пароль */
const char* ssid = "T104-1-2";       // SSID
const char* password = "Desyatbukv10";  // пароль

/* Объявляем переменные */
float temp1 = 0.0;
float hum1 = 0.0;
float temp2 = 0.0;
float hum2 = 0.0;
float soil1 = 0.0;
float soil2 = 0.0;
int angle = 0;
int min_soil = 30;
int min_hum = 25;
int humid = 0;
int counter = 0;
bool is_watering = false;

// Объявление контаков датчиков 
#define sensorPower 14
#define sensorPin A0
#define PIN_RELAY D6

// Значение для хранения уровня воды
int val = 0;

/* Mac адреса других плат */
uint8_t Address1[] = {0xC4,0x5B,0xBE,0x63,0x70,0xDC}; //esp1
uint8_t Address2[] = {0x48,0x3F,0xDA,0x72,0x4F,0x8A}; //esp2

ESP8266WebServer server(80); //Cоздаём сервер

/* Передаваемые данные */
typedef struct message {
    float temp;
    float hum;
    float soil;
} message;
message Data;

void DataRecived(uint8_t * mac, uint8_t *incomingData, uint8_t len) //Функция для обработки принимаемой информации
{
  memcpy(&Data, incomingData, sizeof(Data)); //Копируем полученную информацию в переменную Data (чтобы корректно её обработать)
  if (mac[0] == 196) //Если информация получена с esp1, записываем информацию в переменные temp1, hum1
  {
    temp1 = Data.temp;
    hum1 = Data.hum;
    soil1 = Data.soil;
  }
  else //Иначе (информация получена с esp2), записываем информацию в переменные temp2, hum2
  {
    temp2 = Data.temp;
    hum2 = Data.hum;
    soil2 = Data.soil;
  }
}

void DataSent(uint8_t *mac_addr, uint8_t sendStatus) { //Функция при отправке данных (отладочная информация)
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.print("Delivery success to ");
  }
  else{
    Serial.print("Delivery fail to ");
  }
	Serial.println(mac_addr[0]);
}

//Это функция, используемая для получения показаний
int readSensor() {
	digitalWrite(sensorPower, HIGH);	// Включение датчика
	delay(10);							// ждём 10 миллисекунд
	val = analogRead(sensorPin);		// Чтение значений с датчика
  	val = map(val, 12, 106, 0, 100);
	digitalWrite(sensorPower, LOW);		// Выключение датчика
	return val;							// Отправка текущего значения
}

void handleRoot() //Клиент делает запрос на сервер
{
    File html = LittleFS.open("/index.html", "r"); //Получаем html файл из файловой системы
    server.streamFile(html, "text/html");  //Отправляем html файл клиенту
}

void handleJS()   //Клиент запрашивает script.js
{
    File js = LittleFS.open("/script.js", "r");  //Получаем js файл из файловой системы
    server.streamFile(js, "text/js");  //Отправляем js файл клиенту
}

void handleCss()  //Клиент запрашивает style.css
{
    File css = LittleFS.open("/style.css", "r");  //Получаем css файл из файловой системы
    server.streamFile(css, "text/css");  //Отправляем css файл клиенту
}

void setup() {
    Serial.begin(115200);
  // Начало подключения Wi-Fi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());

    // Объявляем пин реле как выход
    pinMode(PIN_RELAY, OUTPUT); 
  
    // Выключаем реле - посылаем высокий сигнал (для реле)
    digitalWrite(PIN_RELAY, HIGH);  

	  // Установите D5 в качестве выходного
	  pinMode(sensorPower, OUTPUT);
	
	  // Установите на низкий уровень, чтобы через датчик не протекала мощность (для уровня воды)
	  digitalWrite(sensorPower, LOW);

    /* Подключаем файловую систему */
    LittleFS.begin();

    /* Настраиваем ответы сервера на запросы */
    server.on("/", handleRoot);
    server.on("/style.css", handleCss);
    server.on("/script.js", handleJS);

    /* Клиент запрашивает показания датчиков */ 
    server.on("/temp1", []() {
        server.send(200, "text/plain", String(temp1));
      });
    server.on("/hum1", []() {
        server.send(200, "text/plain", String(hum1));
      });
    server.on("/temp2", []() {
        server.send(200, "text/plain", String(temp2));
      });
    server.on("/hum2", []() {
        server.send(200, "text/plain", String(hum2));
      });
    server.on("/soil1", []() {
        server.send(200, "text/plain", String(soil1));
      });
    server.on("/soil2", []() {
        server.send(200, "text/plain", String(soil2));
      });
    //Управление форточкой
    server.on("/angle0", []() {
        angle = 0;
      });
    server.on("/angle30", []() {
        angle = 30;
      });
    server.on("/angle45", []() {
        angle = 45;
      });
    server.on("/angle90", []() {
        angle = 90;
      });
    server.on("/angle110", []() {
        angle = 110;
      });
    server.on("/humid", []() {
        humid = 1;
      });
    
    server.begin(); //Запускаем сервер

    esp_now_init(); //Инициилизируем протокол esp now
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO); //Устанавливаем роль combo, чтобы мы могли и отправлять и принимать данные
    esp_now_register_recv_cb(DataRecived); //Устанавливаем функцию DataRecived для обработки принимаемой информации
    esp_now_add_peer(Address1, ESP_NOW_ROLE_COMBO, 1, NULL, 0); //Добавляем нового пира (esp1), чтобы мы могли отправлять команды
    esp_now_add_peer(Address2, ESP_NOW_ROLE_COMBO, 1, NULL, 0); //Добавляем нового пира (esp2), чтобы мы могли отправлять команды
    esp_now_register_send_cb(DataSent); //Устанавливаем функцию DataSent при отправке данных
}

void loop() {
server.handleClient();  //Отслеживаем действия клиента
  if (counter == 39) {
    if ((soil1+soil2)/2 < min_soil){ // Реле с поливом, зависящая от температуры
      digitalWrite(PIN_RELAY, LOW);
      delay(100);
    } else {
      digitalWrite(PIN_RELAY, HIGH);
      delay(100);
    }
    readSensor();

    esp_now_send(Address1, (uint8_t *) &angle, sizeof(angle));
    Serial.print("angle= ");
    Serial.println((uint8_t) angle);
    esp_now_send(Address2, (uint8_t *) &humid, sizeof(humid));
    Serial.print("humid= ");
    Serial.println((uint8_t) humid);
    if ((hum1+hum2)/2 < min_hum && is_watering == false) {
      humid = 1;
      is_watering = true;
    }
    else {
      humid = 0;
      is_watering = false;
    }
    counter = 0;
  }
  else {
    counter++;  
  }
  delay(250);
}
