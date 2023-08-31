

#include <WiFi.h>
#include "Arduino.h"
#include <WiFiMulti.h>
#include <HTTPClient.h>
//#include <http_client.h>

#include <Arduino.h>

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
String url4 = "http://santocyber.helioho.st/pix-gateway/test.php";
int pagamento = 0;

const int httpPort = 80;


const int ledPin = 48;     // Pino do LED

// Variáveis para rastrear o estado do botão e os cliques
int buttonState = 0;       // Variável para armazenar o estado do botão

int Button1 = 0;  // Pino do botão

int contacendled = 0;
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

  //timer_configure();

  generate_qrcode = false;

  tickNumber = 0;

  last_id = read_nvs_data();
  

  pinMode(Button1, INPUT); // Define o pino do botão como entrada
  pinMode(ledPin, OUTPUT);   // Define o pino do LED como saída

  
  Serial.begin(115200);
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


    buttonState = digitalRead(Button1); // Lê o estado do botão

 if (buttonState == LOW) { // Se o botão estiver pressionado
 //if ( digitalRead(Button1)==1) {
      Serial.println("botao pressionado");
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
       Serial.println("botao 1");
       pix();
       webhook();
 
  }
  if(contacendled == 2){
  //##########################################################################################

      neopixelWrite(RGB_BUILTIN,0,RGB_BRIGHTNESS,0); // Green
      Serial.println("botao 2");
      orders();
  }
  if(contacendled == 3){
  //##########################################################################################

  neopixelWrite(RGB_BUILTIN,0,0,RGB_BRIGHTNESS); // Blue
  Serial.println("botao 3"); 
    webhook();      
  }
   if(contacendled == 4){
  neopixelWrite(RGB_BUILTIN,0,60,78); 
  Serial.println("botao 4");       
  }



}   

     // Serial.println(contacendled);

}
  

  void pix(){
WiFiClient client;
         current_status = STATUS_REQUEST_QRCODE;
          last_id++;
          save_nvs_data(last_id);
          sprintf(info, "Ultima Compra: %d", last_id);
          tft.setCursor(10, 460);
          tft.println(info);
     

  //http_get_qrcode(buffer, last_id);
  // http_get_qrcode_test(buffer);
  Serial.print("QR CODE: ");
  Serial.println(buffer);
  Serial.println("Gerando QR Code...");
 // bool ok = qrcodegen_encodeText(buffer, tempBuffer, qrcode,
    //qrcodegen_Ecc_HIGH, qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX, qrcodegen_Mask_AUTO, true);


  if (client.connect(host, 80)) 
  {
  Serial.println(F("connected"));


  
  String CHAVEPHP = "?id=";
         CHAVEPHP += last_id;
         CHAVEPHP += "";



  client.print(F("GET ")); client.print(url); 
  client.print(CHAVEPHP.c_str());  
  Serial.print(CHAVEPHP.c_str()); 
  //client.println(F(" HTTP/1.1")); 
 // client.println(F("Host: SomeHost"));
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
  client.println(F(" HTTP/1.0")); 
  client.println(F("Host: SomeHost"));
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
WiFiClient client;


  if (client.connect(host, 80)) 
  {
  Serial.println(F("connected"));
    
    String CHAVE2PHP =  "?external_id=";
           CHAVE2PHP +=  last_id;
           CHAVE2PHP += "&order_status=";
           CHAVE2PHP +=  pagamento;


  client.print(F("GET ")); 
  client.print(url4); 
  client.print(CHAVE2PHP.c_str());  
  Serial.println(CHAVE2PHP.c_str());  
//  client.println(F(" HTTP/1.0")); 
//  client.println(F("Host: SomeHost"));
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



  

uint32_t read_nvs_data()
{
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

void save_nvs_data(uint32_t data)
{
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


