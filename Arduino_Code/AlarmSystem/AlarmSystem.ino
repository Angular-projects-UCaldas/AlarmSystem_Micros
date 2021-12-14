#include <Arduino.h>
#include <WiFi.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"
#include <HTTPClient.h>
#include "SD_MMC.h"

#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22
#define CAMERA_MODEL_AI_THINKER

const char *ssid = "Room";
const char *password = "-M89rc7l9s-";
char serverNameC[] = "alarm.ga";
String serverName = "alarm.ga";
String serverPath = "/api/saveImage";
String phoneNumber = "3136367416";
const int serverPort = 80;
int value = 0;
int timer = 0;

WiFiClient client;

void setup()
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  pinMode(12, OUTPUT);
  pinMode(13, INPUT);
  pinMode(4, OUTPUT);

  Serial.begin(115200);
  Serial.println("Connecting");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound())
  {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  }
  else
  {
    config.frame_size = FRAMESIZE_CIF;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
  }

  delay(20000);
}

void loop()
{
  value = digitalRead(13);
  if (value == HIGH)
  {
    buzzing = 1;
    sendPhoto();
    delay(5000);
    sendAlarmMessage();
  }

  timer = timer + 1;

  if (timer == 4500000)
  {
    getStatus();
  }
}

void sendAlarmMessage()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    http.begin(client, "http://alarm.ga/api/sendmessage");

    http.addHeader("Content-Type", "application/json");

    String httpRequestData = "{\"message\":\"The alarm is active send your password to deactivate\", \"phoneNumber\":" + phoneNumber + "}";
    int httpResponseCode = http.POST(httpRequestData);

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    http.end();
  }
}

void getStatus()
{
  timer = 0;
  if (WiFi.status() == WL_CONNECTED)
  {

    HTTPClient http;

    http.begin(client, "http://alarm.ga/api/getstatus");

    int httpResponseCode = http.GET();

    if (httpResponseCode == 403)
    {
      digitalWrite(12, LOW);
    }
    else
    {
      buzz();
    }

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    http.end();
  }
}

String sendPhoto()
{
  String getAll;
  String getBody;

  camera_fb_t *fb = NULL;
  digitalWrite(4, HIGH);
  delay(500);
  fb = esp_camera_fb_get();
  delay(500);
  digitalWrite(4, LOW);
  if (!fb)
  {
    Serial.println("Camera capture failed");
    delay(1000);
    ESP.restart();
  }

  Serial.println("Connecting to server: " + serverName);

  if (client.connect(serverNameC, serverPort))
  {
    Serial.println("Connection successful!");
    String head = "--MicroProcesadores\r\nContent-Disposition: form-data; name=\"imageFile\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--MicroProcesadores--\r\n";

    uint32_t imageLen = fb->len;
    uint32_t extraLen = head.length() + tail.length();
    uint32_t totalLen = imageLen + extraLen;

    client.println("POST " + serverPath + " HTTP/1.1");
    client.println("Host: " + serverName);

    client.println("Content-Length: " + String(totalLen));
    client.println("Content-Type: multipart/form-data; boundary=MicroProcesadores");

    client.println();
    client.print(head);

    uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;
    for (size_t n = 0; n < fbLen; n = n + 1024)
    {
      if (n + 1024 < fbLen)
      {
        client.write(fbBuf, 1024);
        fbBuf += 1024;
      }
      else if (fbLen % 1024 > 0)
      {
        size_t remainder = fbLen % 1024;
        client.write(fbBuf, remainder);
      }
    }
    client.print(tail);

    esp_camera_fb_return(fb);

    int timoutTimer = 10000;
    long startTimer = millis();
    boolean state = false;

    while ((startTimer + timoutTimer) > millis())
    {
      Serial.print(".");
      delay(100);
      while (client.available())
      {
        char c = client.read();
        if (c == '\n')
        {
          if (getAll.length() == 0)
          {
            state = true;
          }
          getAll = "";
        }
        else if (c != '\r')
        {
          getAll += String(c);
        }
        if (state == true)
        {
          getBody += String(c);
        }
        startTimer = millis();
      }
      if (getBody.length() > 0)
      {
        break;
      }
    }
    Serial.println();
    client.stop();
    Serial.println(getBody);
  }
  else
  {
    getBody = "Connection to " + serverName + " failed.";
    Serial.println(getBody);
  }
  return getBody;
}

void buzz()
{
  digitalWrite(12, LOW);
  delay(500);
  digitalWrite(12, HIGH);
}
