#include <WiFi.h>
#include <Arduino.h>
#include <HTTPClient.h>
#include <freertos/task.h>
#include <freertos/FreeRTOS.h>

#include "time.h"

#define SENDING_INTERVAL 5000
#define GOOGLE_SCRIPT_MACRO "https://script.google.com/macros/s/AKfycbzzBZL_XmhT8hxePeLtZTuqGe-xzZU92rs-roG3CfvCb3yoEXzvbOz3jLKkMf-EzE53/exec"

#pragma region WiFi Configuration
#define WIFI_TIMEOUT_MS 20000
const char *ssid = "Astana-2.4G";
const char *pass = "1571@azz";
#pragma endregion

#pragma region Pin Configuration
const int SW1 = 25;
const int SW2 = 26;
const int SW3 = 27;
#pragma endregion

TaskHandle_t wifiTaskHandle = NULL;

void Blink(uint32_t delayMs)
{
    uint32_t virtualDelay = delayMs / 2;
    digitalWrite(LED_BUILTIN, HIGH);
    delay(virtualDelay);
    digitalWrite(LED_BUILTIN, LOW);
    delay(virtualDelay);
}

void ConnectWiFi()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);

    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        if (millis() - startTime > 10000)
        {
            Serial.println(F("Failed to connect to WiFi. Check your credentials and WiFi signal."));
            break;
        }
        Blink(500);
        Serial.println(F("Connecting to WiFi ..."));
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println(F("Connected to WiFi!"));
    }
}

/**
 * Sends the provided data to a Google Sheet.
 *
 * @param data The data to be sent to the Google Sheet.
 */
void SendDataToGoogleSheet(const String &data)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        String url = GOOGLE_SCRIPT_MACRO;

        http.begin(url);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");

        int httpCode = http.POST(data);

        if (httpCode > 0)
        {
            Serial.println("[HTTP] - POST CODE: " + String(httpCode));
        }
        else
        {
            Serial.println("[HTTP] - POST... failed, error: " + http.errorToString(httpCode));
        }

        http.end();
    }
}

void KeepWiFiAlive(void *parameters)
{
    for (;;)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println(F("[WiFi Task] - WiFi still connected!"));
            vTaskDelay(10000 / portTICK_PERIOD_MS);
            continue;
        }

        Serial.println(F("[WiFi Task] - WiFi is not connected!"));

        ConnectWiFi();

        unsigned long startAttemptTime = millis();

        while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS)
        {
            Serial.println(F("[WiFi Task] - Reconnecting to WiFi..."));
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        Serial.println(F("[WiFi Task] - WiFi connected!"));
    }
}

void setup()
{
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(SW1, INPUT_PULLDOWN);
    pinMode(SW2, INPUT_PULLDOWN);
    pinMode(SW3, INPUT_PULLDOWN);
    Serial.println(F("Booting..."));

    ConnectWiFi();

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println(F("WiFi connected!"));
    }
    else
    {
        Serial.println(F("WiFi not connected!"));
    }

    xTaskCreatePinnedToCore(
        KeepWiFiAlive,
        "KeepWiFiAlive",
        10000,
        NULL,
        1,
        &wifiTaskHandle,
        CONFIG_ARDUINO_RUNNING_CORE);
}

int prev_sw1 = 0;
int prev_sw2 = 0;
int prev_sw3 = 0;

int8_t count = 0;
unsigned long lastSendTime = 0;

int iterationCount = 0;
int currentStatus = 1;
bool isIncreasing = true;

void loop()
{
    int sw1 = !digitalRead(SW1);
    int sw2 = !digitalRead(SW2);
    int sw3 = !digitalRead(SW3);

    if (millis() - lastSendTime > SENDING_INTERVAL)
    {
        int swValue = currentStatus * 20;
        String payload = "status=" + String(currentStatus) + "&sw=" + String(swValue);
        Serial.println("Sending: " + payload);
        SendDataToGoogleSheet(payload);

        iterationCount++;

        if (iterationCount >= 5)
        {
            iterationCount = 0;

            if (isIncreasing)
            {
                currentStatus++;
                if (currentStatus >= 3)
                {
                    isIncreasing = false;
                }
            }
            else
            {
                currentStatus--;
                if (currentStatus <= 1)
                {
                    isIncreasing = true;
                }
            }
        }

        lastSendTime = millis();
    }
}