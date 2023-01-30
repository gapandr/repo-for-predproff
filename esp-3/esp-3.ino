#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

/* Название сети и пароль */
const char* ssid = "Теплица";       // SSID
const char* password = "12345678";  // пароль

/* Объявляем переменные */
float temp1 = 0.0;
float hum1 = 0.0;
float temp2 = 0.0;
float hum2 = 0.0;

/* Настройки IP адреса */
IPAddress local_ip(10,100,1,1);
IPAddress gateway(10,100,1,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80); //Cоздаём сервер

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

    server.begin(); //Запускаем сервер
}

void loop() {
  server.handleClient();  //Отслеживаем действия клиента
}
