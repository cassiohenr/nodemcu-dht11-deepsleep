#include <DHT.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
 
#define DHTPIN 12            // pino que estamos conectado
#define DHTTYPE DHT11         // DHT 11
DHT dht(DHTPIN, DHTTYPE);

uint32_t sleep_time_s = 25 * 1000000;                  //25 Equivale a 25segundos
 
ADC_MODE(ADC_VCC);                                           //vcc read
 
const char* ssid = "WIFI";                             //Definir o SSID da rede WiFi
const char* password = "SENHA";                     //Definir a senha da rede WiFi
 
WiFiUDP ntpUDP;
 
int16_t utc = -3;                                                         //UTC -3:00 Brazil
NTPClient timeClient(ntpUDP, "a.st1.ntp.br", utc*3600, 60000);
 
String apiKey = "CHAVE";                                      //Colocar a API Key para escrita neste campo
const char* server = "api.thingspeak.com";                            //Ela é fornecida no canal que foi criado na aba API Keys
 
WiFiClient client;
 
void setup()
{
  Serial.begin(115200);                       //Configuração da UART
  delay(10);
  dht.begin();
  
  Serial.print("Conectando na rede ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);                       //utilizar somento na primeira gravação.     
  WiFi.begin(ssid, password);                  //Inicia o WiFi
  
 while (WiFi.status() != WL_CONNECTED)        //Loop até conectar no WiFi
  {
    delay(500);
    Serial.print(".");
  }
 
  timeClient.begin();
  timeClient.update();
 
  Serial.println("");                              //Logs na porta serial
  Serial.println("WiFi conectado!");
  Serial.print("Conectado na rede ");
  Serial.println(ssid);
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}
 
void envia_dados(void)
{ 
  if (client.connect(server,80))                                //Inicia um client TCP para o envio dos dados
  {
    float vdd = ESP.getVcc() / 1024.0;
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    String postStr = apiKey;
           postStr +="&field1=1";
           //postStr += String(h);
           postStr +="&field2=2";
           //postStr += String(t);
          postStr +="&field3=";
          postStr += String(vdd+0.27);
           postStr += "\r\n\r\n";
 
     client.print("POST /update HTTP/1.1\n");
     client.print("Host: api.thingspeak.com\n");
     client.print("Connection: close\n");
     client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
     client.print("Content-Type: application/x-www-form-urlencoded\n");
     client.print("Content-Length: ");
     client.print(postStr.length());
     client.print("\n\n");
     client.print(postStr);
     Serial.println(postStr);
 
  }
  client.stop();
}
 
void loop() 
{
  envia_dados();    
  ESP.deepSleep(sleep_time_s);
}
