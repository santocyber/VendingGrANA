
#include <WiFi.h>
#include "Arduino.h"
#include <WiFiMulti.h>
#include <HTTPClient.h>
WiFiClientSecure client;


// Configuração do certificado CA (Certificado de Autoridade) para confiar no servidor remoto
const char* caCert = \
 "-----BEGIN CERTIFICATE-----\n"
  "MIICtjCCAZ4CAQAwFTETMBEGA1UEAwwKdGVzLm9ibS5wdDCCASIwDQYJKoZIhvcN\n"
  "AQEBBQADggEPADCCAQoCggEBANvuYarL3NB+2S4uGc8IftHLctFT8qKAzAcJt3yJ\n"
  "iFa4dixMgxNMnNZ8t1iAMEIlm0exzQ9Jbhi0i27RGwXnUDU5sS9RESqc02ca3HSd\n"
  "DBWCc3RYxetHJTJWAIam7D7qlsHTdLBAKUKKhytHD+uFYcC0D44IHfUmXmoQva9c\n"
  "QNSWeFZJQOywcclQyaThjSpg9xbg09Nx1/JLRV4ocKMnSm1oR9EkGu1oI8ppgeQv\n"
  "wrEyPYniRlNY0uT7K+NvCKZ46J1naPrnWwleqxp/JGfykH63u6s4VLZEQ7ibmAI3\n"
  "WX3hXTcGYxcz3LvwZNjpRslAX3iFLr9docf0HjSbF70zcv0CAwEAAaBcMFoGCSqG\n"
  "SIb3DQEJDjFNMEswCQYDVR0TBAIwADALBgNVHQ8EBAMCBeAwMQYDVR0RBCowKIIK\n"
  "dGVzLm9ibS5wdIIMKi50ZXMub2JtLnB0ggwqLnRlcy5vYm0ucHQwDQYJKoZIhvcN\n"
  "AQELBQADggEBALYRG8PoLow3R6Z7X9ZntRvHYksRhaUsgmOqgqB/H2SjEFZ0kTvY\n"
  "0BIG98FFfHRG8omo/jlsPBBD/el8WzrBNRWvennpvWRteBheM5eSruHjZO5tOwHd\n"
  "b+A6i8GmMpQVAzx31oM+fNXUzUDUGtltIxxIQMsclkqiGWuT56QZ2oZEtVTOmQP9\n"
  "NPsmirCe0CfN47LgHz73X1wjp6nd09yloR/pFO4H+gruyh+NwnKp9wP4Pq/3i6g7\n"
  "hl0mVBvlvMs/7Jt3guvcdSHPc2yyEiarRUPUxCzrjG8RRr7HpY7xkKYRI06ROPsj\n"
  "LOYRaRnaPslLfcSXH1MsIjXRgq077tzeHdo=\n"
  "-----END CERTIFICATE-----\n";


#include <SPIFFS.h>
#include <TFT_eSPI.h>
#include "qrcodegen.h"
//#include <qrcodegen.h>

#define BUTTON_INPUT 0
#define ATUADOR 15
#define INTERVAL 400
#define STATUS_WAIT_USER_INPUT 0
#define STATUS_REQUEST_QRCODE 1
#define STATUS_CHECK_PAYMENT 2
#define STATUS_ACTUATE_ON_GPIO 3
#define LAST_ID_KEY "last_id"

#define RXD2 44
#define TXD2 43


TFT_eSPI tft = TFT_eSPI();
           
uint8_t current_status;
uint32_t tickNumber = 0;
uint8_t segundos = 0;
uint8_t minutos = 0;
uint8_t horas = 0;
uint8_t order_status;

bool generate_qrcode;
bool get_order_status;
bool primeira_vez = true;

char info[30];

uint32_t last_id;

void timer_tick_func();

//void timer_configure();

uint32_t read_nvs_data();

void save_nvs_data(uint32_t data);
char buffer[300];
uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];




const char* ssid = "InternetSA";
const char* password = "cadebabaca";
const char* host = "santocyber.helioho.st";
String url = "http://santocyber.helioho.st/pix-gateway/v1/api_qrcode.php";
String url2 = "http://santocyber.helioho.st/pix-gateway/v1/api_orders.php";
String url3 = "http://santocyber.helioho.st/pix-gateway/v1/api_webhook.php";
String url5 = "http://santocyber.helioho.st/pix-gateway/v1/api_teste.php";
String url4 = "http://santocyber.helioho.st/pix-gateway/test.php";
int pagamento = 0;
int ordervalue = 0;

const int ledPin = 48;     // Pino do LED

// Variáveis para rastrear o estado do botão e os cliques
int buttonState = 0;       // Variável para armazenar o estado do botão
String State = "menu";
String payload = "";

int Button1 = 0;  // Pino do botão

int contacendled = 0;
int conta = 0;
unsigned long intervalo = 0;







// Setting clock just to be sure...
void setClock() {
  configTime(0, 0, "pool.ntp.org");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}


WiFiMulti WiFiMulti;

void setup() {

  current_status = STATUS_WAIT_USER_INPUT;
  generate_qrcode = false;
  tickNumber = 0;
  last_id = read_nvs_data();
  

  pinMode(Button1, INPUT); // Define o pino do botão como entrada
  pinMode(ledPin, OUTPUT);   // Define o pino do LED como saída


    // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(115200);
  //Serial1.begin(115200);

   Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("InternetSA", "cadebabaca");

  // wait for WiFi connection
  Serial.print("Waiting for WiFi to connect...");
  while ((WiFiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
  }
  Serial.println(" connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  setClock();  
}


void loop() {


 if (State == "menu") {
  menu();
 }
  
 if(State == "valor"){
    geravalor();
}
 if(State == "pix"){      
    pix();
    webhook();
    ordervalue = 0;
    conta = 0;
    State = "menu";

}

}
  
void menu(){
     // Serial.println("MENU");


  
    buttonState = digitalRead(Button1); // Lê o estado do botão

 if (buttonState == LOW) { // Se o botão estiver pressionado
      Serial.println("botao pressionado MENU");
      Serial.println(contacendled);

      digitalWrite(RGB_BUILTIN, HIGH);
      contacendled++;
      delay(500); 
      digitalWrite(RGB_BUILTIN, LOW);      
      intervalo = millis();}

  if(millis() > intervalo+4000){
  contacendled = 0;
}    
   if(millis() > intervalo+2000){

  if(contacendled == 1){
    //##########################################################################################
       Serial.println("INICIA VALOR");

       State = "valor";
    
  }
  if(contacendled == 2){
  //##########################################################################################

      neopixelWrite(RGB_BUILTIN,0,RGB_BRIGHTNESS,0); // Green
      Serial.println("botao 2");
      orders();
  }
  if(contacendled == 3){

  }
   if(contacendled == 4){
  neopixelWrite(RGB_BUILTIN,0,60,78); 
  Serial.println("botao 4");       
  
  }}}

void geravalor(){
    buttonState = digitalRead(Button1); // Lê o estado do botão

 if (buttonState == LOW) { // Se o botão estiver pressionado
      Serial.println("botao pressionado GERAVALOR");
      Serial.println(conta);

      digitalWrite(RGB_BUILTIN, HIGH);
      conta++;
      delay(200); 
      digitalWrite(RGB_BUILTIN, LOW);      
      intervalo = millis();
     
      }

  if(millis() > intervalo+15000){
  conta = 0;
}    
  if(millis() > intervalo+5000){
  if (ordervalue > 3) {State = "pix";}
  else{State = "menu";}
}    
  if(millis() > intervalo+2000){
          Serial.println("VALOR");
          Serial.println(conta);
          ordervalue = conta;
}}

  void pix(){
          current_status = STATUS_REQUEST_QRCODE;
          last_id++;
          save_nvs_data(last_id);
          sprintf(info, "Ultima Compra: %d", last_id);
          tft.setCursor(10, 460);
          tft.println(info);




          client.setCACert(caCert);
          HTTPClient https;
    
    String CHAVEPHP =   url;
           CHAVEPHP +=  "?id=";
           CHAVEPHP +=  last_id;
           CHAVEPHP += "&value=";
           CHAVEPHP +=  ordervalue;


  neopixelWrite(RGB_BUILTIN,0,RGB_BRIGHTNESS,0); // Green

  if (https.begin(CHAVEPHP)) 
  {

 Serial.print("[HTTPS] GET...\n");
        // start connection and send HTTP header
        int httpCode = https.GET();
  
        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

            String payload = https.getString();
            Serial.println(payload);
            Serial.println(CHAVEPHP);

           
            }

             Serial.println("ESPERANDO PAGAMENTO...");
             delay(15000);
             if (pagamento == 0){
                           Serial.println("PAGAMENTO NAO EFETUADO");
                           neopixelWrite(RGB_BUILTIN,RGB_BRIGHTNESS,0,0); // Red
                           delay(2000);


              }
             if (pagamento == 1){
                           Serial.println("PAGAMENTO APROVADO");
                           Serial.println("PAGAMENTO de");
                           Serial.println(ordervalue);
}





            

  neopixelWrite(RGB_BUILTIN,0,0,RGB_BRIGHTNESS); // Blue

  delay(1000);
 
  Serial.println("");
  Serial.println(F("Desconectando."));
  https.end();

  digitalWrite(RGB_BUILTIN, LOW);
  
  } 
   else 
    {
    Serial.println(F("Falha na conexao")); 
    Serial.println();
    neopixelWrite(RGB_BUILTIN,RGB_BRIGHTNESS,0,0); // Red
    }
    






  //http_get_qrcode(buffer, last_id);
  // http_get_qrcode_test(buffer);
  Serial.println("QR CODE FUNCAO");
  Serial.print("VALOR:");
  Serial.println(ordervalue);
  Serial.println("QR CODE:");
  Serial.println(buffer);
  Serial.println("Gerando QR Code...");



  bool ok = qrcodegen_encodeText(payload.c_str(), tempBuffer, qrcode, qrcodegen_Ecc_HIGH, qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX, qrcodegen_Mask_AUTO, true);


  tickNumber++;
  neopixelWrite(RGB_BUILTIN,0,0,RGB_BRIGHTNESS); // Blue
  delay(1000);
 
  Serial.println("");
  Serial.println(F("Desconectando."));
  client.stop();
  digitalWrite(RGB_BUILTIN, LOW);
}

      void orders(){
WiFiClient client;


  if (client.connect(host, 80)) 
  {
  Serial.println(F("connected"));
  

  client.print(F("GET ")); client.print(url2);  
  client.println();
    neopixelWrite(RGB_BUILTIN,0,RGB_BRIGHTNESS,0); // Green


  } 
   else 
    {
    Serial.println(F("connection failed")); Serial.println();
      neopixelWrite(RGB_BUILTIN,RGB_BRIGHTNESS,0,0); // Red
    }

  while(client.connected() && !client.available()) delay(1); //waits for data
  while (client.connected() || client.available()) 
  { //connected or data available
    char c = client.read();
    Serial.print(c);
    
  }
  neopixelWrite(RGB_BUILTIN,0,0,RGB_BRIGHTNESS); // Blue
  delay(1000);
 
  Serial.println("");
  Serial.println(F("Desconectando."));
  client.stop();
  digitalWrite(RGB_BUILTIN, LOW);
}
    
    
     void webhook(){
client.setCACert(caCert);
HTTPClient https;
    
    String CHAVE2PHP =   url4;
           CHAVE2PHP +=  "?external_id=";
           CHAVE2PHP +=  last_id;
           CHAVE2PHP += "&order_status=";
           CHAVE2PHP +=  pagamento;
           CHAVE2PHP += "&order_value=";
           CHAVE2PHP +=  ordervalue;
           CHAVE2PHP += "&mac_value=";
           CHAVE2PHP +=  WiFi.macAddress();


  neopixelWrite(RGB_BUILTIN,0,RGB_BRIGHTNESS,0); // Green

  if (https.begin(CHAVE2PHP)) 
  {

 Serial.print("[HTTPS] GET...\n");
        // start connection and send HTTP header
        int httpCode = https.GET();
  
        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
            String payload = https.getString();
            Serial.println(payload);
            Serial.println(CHAVE2PHP);

            }
            

  neopixelWrite(RGB_BUILTIN,0,0,RGB_BRIGHTNESS); // Blue

  delay(1000);
 
  Serial.println("");
  Serial.println(F("Desconectando."));
  https.end();

  digitalWrite(RGB_BUILTIN, LOW);
  
  } 
   else 
    {
    Serial.println(F("connection failed")); 
    Serial.println();
    neopixelWrite(RGB_BUILTIN,RGB_BRIGHTNESS,0,0); // Red
    }


}



  

uint32_t read_nvs_data(){
uint32_t last_id = 0;
  if (SPIFFS.begin(true))
  {
    File nvsFile = SPIFFS.open("/nvs.txt", "r");
    if (nvsFile)
    {
      last_id = nvsFile.parseInt();
      nvsFile.close();
    }
  }
  return last_id;
}

void save_nvs_data(uint32_t data){
  if (SPIFFS.begin(true))
  {
    File nvsFile = SPIFFS.open("/nvs.txt", "w");
    if (nvsFile)
    {
      nvsFile.print(data);
      nvsFile.close();
    }
  }
}


