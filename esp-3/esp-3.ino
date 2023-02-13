#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <espnow.h>

/* Название сети и пароль */
const char* ssid = "Теплица";       // SSID
const char* password = "12345678";  // пароль

/* Объявляем переменные */
float temp1 = 0.0;
float hum1 = 0.0;
float temp2 = 0.0;
float hum2 = 0.0;
float soil1 = 0.0;
float soil2 = 0.0;

/* Настройки IP адреса */
IPAddress local_ip(10,100,1,1);
IPAddress gateway(10,100,1,1);
IPAddress subnet(255,255,255,0);

/* Mac адреса других плат */
uint8_t Address1[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //esp1
uint8_t Address2[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //esp2

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
  if (mac == Address1) //Если информация получена с esp1, записываем информацию в переменные temp1, hum1
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
    /* Создаём точку доступа */
    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    delay(100);

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

    server.begin(); //Запускаем сервер

    esp_now_init(); //Инициилизируем протокол esp now
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO); //Устанавливаем роль combo, чтобы мы могли и отправлять и принимать данные
    esp_now_register_recv_cb(DataRecived); //Устанавливаем функцию DataRecived для обработки принимаемой информации
}

void loop() {
  server.handleClient();  //Отслеживаем действия клиента
}
