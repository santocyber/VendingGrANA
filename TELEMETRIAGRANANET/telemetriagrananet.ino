
#include <WiFi.h>
#include "Arduino.h"
#include <HTTPClient.h>
WiFiClientSecure client;
#include "time.h"
#include "sntp.h"

#include <WebServer.h>
void saveWifiCredentials(const String &ssid, const String &password);

// Cria um novo objeto WebServer
WebServer server(80);

bool wifiConnected = false;

const char* ssid = ""; // Deixe em branco
const char* password = ""; // Deixe em branco


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
#define CONTA_ID_KEY "contagrana"




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


void timer_tick_func();

//void timer_configure();


//##############CONFIG DOS CONTADORES
uint32_t read_nvs_data();
uint32_t contagrana_data();
uint32_t contaobj_data();

void save_nvs_data(uint32_t data);
void contagrana_data(uint32_t data);
void contaobj_data(uint32_t data);

uint32_t last_id;
uint32_t contagrana;
uint32_t contaobj;


char buffer[300];
uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];



const char* host = "santocyber.helioho.st";
String url = "https://santocyber.helioho.st/pix-gateway/gravatele.php";
String url2 = "http://santocyber.helioho.st/pix-gateway/v1/api_orders.php";
String url3 = "http://santocyber.helioho.st/pix-gateway/v1/api_webhook.php";
String url5 = "http://santocyber.helioho.st/pix-gateway/v1/api_qrcode.php";
String url4 = "http://santocyber.helioho.st/pix-gateway/test.php";
String State = "menu";
String payload = "";

int pagamento = 0;
int ordervalue = 1;


// Variáveis para rastrear o estado do botão e os cliques
int buttonState = 0;       // Variável para armazenar o estado do botão

int contacendled = 0;
int conta = 1;
unsigned long intervalo = 0;


//###########################PCONFIGURACAO DAS PINAGENS
      int Button1 = 0;  // Pino do botão
const int ledPin = 48;     // Pino do LED
const int relayPin = 2; // Pino GPIO para o relé


#define RXD2 44
#define TXD2 43



//########################## Setting clock just to be sure...

const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const long  gmtOffset_sec = -10800;
const int   daylightOffset_sec = 0;

const char* time_zone = "CET-3CEST,M3.5.0,M10.5.0/3";  // TimeZone rule for Europe/Rome including daylight adjustment rules (optional)


//###########################################CONFIGURA O LOOP DE UPDATE NO SERVDIOR
unsigned long previousMillis = 0;
const long interval = 30000; // Intervalo de 30 segundos


void setClock() {

  sntp_servermode_dhcp(1);    // (optional)
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
}


void handleRoot() {
  String html = "<html><body>";
  html += "<h1>CONFIG GRANANET:</h1>";
  
  // Adicionar um botão para escanear redes
  html += "<form method='get' action='/scan'>";
  html += "<input type='submit' value='Escanear Redes'>";
  html += "</form>";

  html += "<p>";

  html += "</p>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}
void handleScan() {
  String html = "<html><body>";
  html += "<h1>Selecione uma Rede Wi-Fi:</h1>";
  html += "<form method='post' action='/connect'>";
  html += "<ul>";

  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; i++) {
    String ssid = WiFi.SSID(i);
    html += "<li>";
    html += "<input type='radio' name='ssid' value='" + ssid + "'>" + ssid;
    html += "</li>";
  }



  // Se o formulário for enviado, salve as credenciais
  if (server.method() == HTTP_POST) {
    String selectedSSID = server.arg("ssid");
    String enteredPassword = server.arg("password");
    saveWifiCredentials(selectedSSID, enteredPassword);
  }
  
  html += "</ul>";
  html += "<label for='password'>Senha:</label><input type='password' name='password'><br>";
  html += "<input type='submit' value='Conectar'>";
  html += "</form>";
  html += "<p><br><br><br><a href='/'>Voltar</a></p>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void handleConnect() {
  String ssid = server.arg("ssid");
  String password = server.arg("password");

  // Verifique se o SSID e a senha não estão vazios
  if (ssid.length() > 0 && password.length() > 0) {
    WiFi.begin(ssid.c_str(), password.c_str());

    // Aguarde a conexão ser estabelecida
    int timeout = 10; // Tempo limite para a conexão em segundos
    while (WiFi.status() != WL_CONNECTED && timeout > 0) {
      delay(1000); // Aguarde 1 segundo
      timeout--;
    }

    if (WiFi.status() == WL_CONNECTED) {
      server.send(200, "text/plain", "Conectado com sucesso! IP: " + WiFi.localIP().toString());
      wifiConnected = true;
    } else {
      server.send(200, "text/plain", "n conectado!  ");


    }}}

void setup() {
  


  current_status = STATUS_WAIT_USER_INPUT;
  generate_qrcode = false;
  tickNumber = 0;
  last_id = read_nvs_data();
  contagrana = read_contagrana();
  contaobj = read_contaobj();



  pinMode(Button1, INPUT); // Define o pino do botão como entrada
  pinMode(ledPin, OUTPUT);   // Define o pino do LED como saída

  // Configurar o pino do relé como saída
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Inicialmente, o relé está desligado

  
    // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(115200);
  //Serial1.begin(115200);

   Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  
  // Serial.setDebugOutput(true);
//#####################################################wifi scan

 if (!SPIFFS.begin(true)) {
    Serial.println("Falha ao montar o sistema de arquivos SPIFFS.");
    return;
  }




if (wifiConnected != true) {

// Iniciar o modo AP
  WiFi.mode(WIFI_AP);
  WiFi.softAP("GRANANET-Config"); // Nome da rede AP
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Endereço IP do AP: ");
  Serial.println(myIP);
  // Inicializa a biblioteca WiFi
  WiFi.begin();

  }


  // Inicializa o servidor web
  server.on("/", handleRoot);
  server.on("/scan", HTTP_GET, handleScan);
  server.on("/connect", handleConnect);
 // server.on("/desligar", handleDesligar);
 // server.on("/ligar", handleLigar);
  server.begin();

  Serial.println("Servidor web iniciado");
  

  setClock();  
}


String obterHoraAtual() {

    time_t agora = time(nullptr); // Obtemos a hora atual
    struct tm infoTempo;
    localtime_r(&agora, &infoTempo); // Converte o tempo em uma estrutura tm

    char buffer[20]; // Buffer para armazenar a hora formatada (HH:MM:SS\0)
    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d_%04d-%02d-%02d", infoTempo.tm_hour, infoTempo.tm_min, infoTempo.tm_sec, infoTempo.tm_year + 1900, infoTempo.tm_mday, infoTempo.tm_mon + 1);
             
    return String(buffer);
}


void loop() {

  // Executa o servidor web
  server.handleClient();


if (wifiConnected) {
unsigned long currentMillis = millis();

  // Verificar se passaram 30 segundos
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Obter e imprimir a hora atual
    time_t now = time(nullptr);
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    Serial.print("Hora atual: ");
    Serial.printf("%02d:%02d:%02d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    telemetria();
  }
}

 if (State == "menu") {
  menu();
 }
  
 if(State == "valor"){
    geravalor();
}
 if(State == "pix"){      
    pix();
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
  neopixelWrite(RGB_BUILTIN,0,60,78); 
  Serial.println("botao 3 granaentrada");  
       contagrana++;
       contagrana_data(contagrana);
  
          sprintf(info, "Total Grana: %d", contagrana);
   Serial.println(contagrana);  


     delay(2000);
     State = "menu";


  }
   if(contacendled == 4){
  neopixelWrite(RGB_BUILTIN,0,60,78); 
  Serial.println("botao 4  objsaida");  


    contaobj++;
      contaobj_data(contagrana);

  Serial.println(contaobj);  

  delay(2000);
     State = "menu";     
  
  }


 if(contacendled == 10){
  neopixelWrite(RGB_BUILTIN,0,60,78); 
  Serial.println("botao 10  apaga granaentrada obdjsiada");  
  delete_contagrana_data_file();
  delete_contaobj_data_file();
  Serial.println("apagando grana entrada");  

  delay(2000);
     State = "menu";     
  
  }
   if(contacendled == 11){
  neopixelWrite(RGB_BUILTIN,0,60,78); 
  Serial.println("botao 11  apaga wifi");  
deletewififile();
  Serial.println("apagando wifi");  

  delay(2000);
     State = "menu";     
  
  }

  
  
  
  
  }}

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
          Serial.println(ordervalue);
          ordervalue = conta;
}}
      

  void pix(){
          current_status = STATUS_REQUEST_QRCODE;
          last_id++;
          save_nvs_data(last_id);
          sprintf(info, "Ultima Compra: %d", last_id);


          contagrana += ordervalue;
          contagrana_data(contagrana);
          
          tft.setCursor(10, 460);
          tft.println(info);




          client.setCACert(caCert);
          HTTPClient https;
    
    String CHAVEPHP =   url5;
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


    
     void telemetria(){
client.setCACert(caCert);
HTTPClient https;

  String horaAtual = obterHoraAtual();
    Serial.print("Hora atual:");
    Serial.println(horaAtual);

    
    String CHAVE2PHP =   url;
           CHAVE2PHP += "?&granaentrada=";
           CHAVE2PHP +=  contagrana;
           CHAVE2PHP += "&objsaida=";
           CHAVE2PHP +=  contaobj;
           CHAVE2PHP += "&mac_value=";
           CHAVE2PHP +=  WiFi.macAddress();
           CHAVE2PHP += "&iplocal=";
           CHAVE2PHP +=  WiFi.localIP().toString();
           CHAVE2PHP += "&wifilocal=";
           CHAVE2PHP +=  WiFi.SSID();       
           CHAVE2PHP += "&timelocal=";
           CHAVE2PHP +=  horaAtual;


  

  neopixelWrite(RGB_BUILTIN,0,RGB_BRIGHTNESS,0); // Green

  if (https.begin(CHAVE2PHP)) 
  {

 Serial.print("[HTTPS] GET...\n");
        // start connection and send HTTP header
        int httpCode = https.GET();
        Serial.println(CHAVE2PHP);

  
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



//################################ Função para salvar o SSID e a senha no SPIFFS
void saveWifiCredentials(const String &ssid, const String &password) {
  File file = SPIFFS.open("/wifi_credentials.txt", "w");
  if (!file) {
    Serial.println("Erro ao abrir o arquivo para salvar as credenciais.");
    return;
  }

  file.println(ssid);
  file.println(password);
  file.close();
}

// Função para carregar o SSID e a senha do SPIFFS
void  loadWifiCredentials(String &ssid, String &password) {
  File file = SPIFFS.open("/wifi_credentials.txt", "r");
  if (!file) {
    Serial.println("Arquivo de credenciais não encontrado.");
    return;
  }

  ssid = file.readStringUntil('\n');
  password = file.readStringUntil('\n');
  file.close();
}

  void deletewififile() {
String ssid0 ="0";
String password0 ="0";
        
          saveWifiCredentials(ssid0,password0);
}
//#########################SALVANDO e apagando CONTADORES EM ARQUIVOS

void delete_contagrana_data_file() {

          contagrana = 0;
          contagrana_data(contagrana);
}
void delete_contaobj_data_file() {
          contaobj = 0;
          contaobj_data(contaobj);
}
uint32_t read_contagrana(){
uint32_t contagrana = 0;
  if (SPIFFS.begin(true))
  {
    File contagranaFile = SPIFFS.open("/contagrana.txt", "r");
    if (contagranaFile)
    {
      contagrana = contagranaFile.parseInt();
      contagranaFile.close();
    }
  }
  return contagrana;
}

void contagrana_data(uint32_t data){
  if (SPIFFS.begin(true))
  {
    File contagranaFile = SPIFFS.open("/contagrana.txt", "w");
    if (contagranaFile)
    {
      contagranaFile.print(data);
      contagranaFile.close();
    }
  }
}


uint32_t read_contaobj(){
uint32_t contaobj = 0;
  if (SPIFFS.begin(true))
  {
    File contaobjFile = SPIFFS.open("/contaobj.txt", "r");
    if (contaobjFile)
    {
      contaobj = contaobjFile.parseInt();
      contaobjFile.close();
    }
  }
  return contaobj;
}

void contaobj_data(uint32_t data){
  if (SPIFFS.begin(true))
  {
    File contaobjFile = SPIFFS.open("/contaobj.txt", "w");
    if (contaobjFile)
    {
      contaobjFile.print(data);
      contaobjFile.close();
    }
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
