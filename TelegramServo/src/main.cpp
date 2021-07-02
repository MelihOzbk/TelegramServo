#include <Arduino.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <deneyap.h>
//

#define BOTtoken ""  // Bot anahtarı (Botfather üzerinden alınabilir)
#define wad "" //wifi ad
#define wsifre "" //wifi sifre
//ESP8266 Kullanıcıları için Önemli!
#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT)
#endif
//

//Her 1(1000ms) saniyede yeni mesaj kontrol edicek.
int botRequestDelay = 1000; 
unsigned long lastTimeBotRan;


// Servo Bölümü
#define datapin D0 // Kart üzerinde D0 Üzerinden bilgi aktarımı alıcağımız port seçiliyor.
Servo smotor; // Servo Motorumuz isimlendiriliyor
int degr= 0; //Servonun başlangıç derecesi
int yukseksinir= 180;
int altsinir= 0;


WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    // Buton basıldığında monitöre bu yazıyı gir
    if (bot.messages[i].type ==  F("callback_query")) {
      String text = bot.messages[i].text;

      Serial.print("Buton basılışı:");
      Serial.println(text);

      if (text == F("Arttir")) {
        if (degr == yukseksinir)
        {
          bot.sendMessage(chat_id, "Hata! Dereceyi azaltman lazım motor sınırında kod '"+ String(yukseksinir)+"'");
        } else {
          degr= degr+5;
          smotor.write(degr);
          bot.sendMessage(chat_id,"Başarıyla derece yükseltildi " + String(degr));
        }
        
      } else if (text == F("Azalt")) {
        if (degr == altsinir)
        {
          bot.sendMessage(chat_id, "Hata! Dereceyi yükseltmen lazım motor sınırında kod '"+String(altsinir)+"'");
        } else {
          degr = degr-5;
          smotor.write(degr);
          bot.sendMessage(chat_id,"Başarıyla derece düşürüldü "+ String(degr));
        }
        
      }
    } else {
      String text = bot.messages[i].text;

      if (text == F("/menu")) {

        //callback_data geri dönüş olarak hangi komut girdisi olucağını belirliyor.
        
        String keyboardJson = F("[[{ \"text\" : \"Azalt\", \"callback_data\" : \"Azalt\" },{ \"text\" : \"Arttir\", \"callback_data\" : \"Arttir\" }]]");
        bot.sendMessageWithInlineKeyboard(chat_id, "Servo Kontrol", "", keyboardJson);
      }

    }
  }
}

void setup() {
  Serial.begin(115200);
  smotor.attach(datapin);
  #ifdef ESP8266
    configTime(0, 0, "pool.ntp.org");      
    client.setTrustAnchors(&cert);
  #endif
  // Wifi Bağlantısı
  WiFi.mode(WIFI_STA);
  WiFi.begin(wad, wsifre);
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); 
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP());
}

void loop() {
    if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("Komut alındı.");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}