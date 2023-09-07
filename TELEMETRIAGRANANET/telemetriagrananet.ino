#include <WiFi.h>
//#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "Arduino.h"
#include <HTTPClient.h>
#include "time.h"
#include "sntp.h"
#include <ArduinoJson.h>

#define _DISABLE_TLS_
WiFiClientSecure client;
// Cria um novo objeto WebServer
WebServer server(80);

bool wifiConnected = false;
String ssid;
String password;
String nomedobot;
void saveWifiCredentials(const String &ssid, const String &password, const String &nomedobot);
void loadWifiCredentials(String &ssid, String &password, String &nomedobot);

//##################### Configura IP

IPAddress localIP;
//IPAddress localIP(192, 168, 1, 200); // hardcoded

// Set your Gateway IP address
IPAddress localGateway;
//IPAddress localGateway(192, 168, 1, 1); //hardcoded
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8); //nao nao eh opcional nao, eh obrigatorio demorei mt tempo pra perceber q o relogio e o telegram n tava funcionando por causa do dns




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
String url6 = "https://santocyber.helioho.st/pix-gateway/verificaacao.php";

String State = "menu";
String StateUpdate = "desativado";
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


//###########################################CONFIGURA O LOOP DE UPDATE NO SERVDIOR SQL
unsigned long previousMillis = 0;
const long interval = 30000; // Intervalo de 30 segundos


String obterHoraAtual() {

    time_t agora = time(nullptr); // Obtemos a hora atual
    struct tm infoTempo;
    localtime_r(&agora, &infoTempo); // Converte o tempo em uma estrutura tm

    char buffer[20]; // Buffer para armazenar a hora formatada (HH:MM:SS\0)
    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d_%04d-%02d-%02d", infoTempo.tm_hour, infoTempo.tm_min, infoTempo.tm_sec, infoTempo.tm_year + 1900, infoTempo.tm_mday, infoTempo.tm_mon + 1);
             
    return String(buffer);
}

void setClock() {

  sntp_servermode_dhcp(1);    // (optional)
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
}

void handleMain() {
  String html = "<html><head>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; margin: 20px; }";
  html += "h1 { color: #333; }";
  html += "h2 { color: #666; }";
  html += "p { margin: 10px 0; }";
  html += "</style>";
  html += "</head><body>";
  html += "<h1>GRANANET WEBPAGE</h1>";
  html += "<h2>NOMEBOT:</h2>";
  html += "<p class='data'>" + nomedobot + "</p>";
  html += "<h2>SSID:</h2>";
  html += "<p class='data'>" + ssid + "</p>";
  html += "<h2>ENTRADA:</h2>";
  html += "<p class='data'>" + String(contagrana) + "</p>";
  html += "<h2>SAIDA:</h2>";
  html += "<p class='data'>" + String(contaobj) + "</p>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void handleRoot() {
  loadWifiCredentials(ssid, password, nomedobot);

  String html = "<html><head>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; margin: 20px; }";
  html += "h1 { color: #333; }";
  html += "h2 { color: #666; }";
  html += "p { margin: 10px 0; }";
  html += ".data { font-weight: bold; }";
  html += ".button { background-color: #007bff; color: #fff; padding: 10px 20px; border: none; cursor: pointer; }";
  html += "</style>";
  html += "</head><body>";
  html += "<h1>CONFIG GRANANET</h1>";
  html += "<h2>NOMEBOT:</h2>";
  html += "<p class='data'>" + nomedobot + "</p>";
  html += "<h2>SSID:</h2>";
  html += "<p class='data'>" + ssid + "</p>";
  html += "<h2>password:</h2>";
  html += "<p class='data'>" + password + "</p>";
  html += "<h2>ENTRADA:</h2>";
  html += "<p class='data'>" + String(contagrana) + "</p>";
  html += "<h2>SAIDA:</h2>";
  html += "<p class='data'>" + String(contaobj) + "</p>";
  html += "<form method='get' action='/scan'>";
  html += "<input type='submit' class='button' value='Escanear Redes'>";
  html += "</form>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void handleScan() {
  Serial.println("SCAN");
  // Send an HTTP response
  String html = "<html><head><style>";
  html += "body { font-family: Arial, Helvetica, sans-serif; background-color: #f4f4f4; text-align: center; }";
  html += "h1 { color: #333; }";
  html += "ul { list-style-type: none; padding: 0; }";
  html += "li { margin: 10px 0; }";
  html += "label { font-weight: bold; }";
  html += "input[type='text'], input[type='password'] { width: 100%; padding: 10px; margin: 5px 0; }";
  html += "input[type='submit'], input[type='button'] { background-color: #4CAF50; color: white; padding: 10px 20px; border: none; cursor: pointer; }";
  html += "a { text-decoration: none; }";
  html += "</style></head><body>";
  html += "<h1>Selecione Wi-Fi Network:</h1>";
  html += "<form method='post' action='/connect'>";
  html += "<ul>";
  WiFi.disconnect();

  // Scan for Wi-Fi networks
   int n = WiFi.scanNetworks();
    Serial.println("Scan done");
    if (n == 0) {
        html += "<h1>No Wi-Fi Network:</h1>";
        Serial.println("no networks found");
    } else {
  for (int i = 0; i < n; i++) {
    // Add each network to the HTML list
    String ssid = WiFi.SSID(i);
    int32_t rssi = WiFi.RSSI(i);
    html += "<li><input type='radio' name='ssid' value='" + ssid + "'>" + ssid + " (RSSI: " + String(rssi) + ")</li>";
  }}

  html += "</ul>";
  html += "<label for='ssid'>NOMEDOBOT:</label><input type='text' name='nomedobot'><br>";
  html += "<label for='password'>Password:</label><input type='password' name='password'><br>";
  html += "<input type='submit' value='CONECTAR'>";
  html += "</form>";
  html += "<p><br><br><br><a href='/'>Back</a></p>";
  html += "<input type='button' value='PESQUISAR NOVAMENTE' onclick='location.reload()'>";
  html += "</body></html>";

  // Send the HTML response
  server.send(200, "text/html", html);
}

void handleConnect() {
  Serial.println("CONECT");

  String ssid = server.arg("ssid");
  String password = server.arg("password");
  String nomedobot = server.arg("nomedobot");
  saveWifiCredentials(ssid.c_str(), password.c_str(), nomedobot.c_str());

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
      String configg = "Prontinho. GRANANET reiniciando, conecte no seu WIFI e clique no endereco abaixo IP: " + WiFi.localIP().toString();
      configg += "<br><br>";
      configg += "CLIQUE -> VVVVVVVV";
      configg += "<br><br>";
      configg += "<a href='http://";
      configg += WiFi.localIP().toString();
      configg += "'>";
      configg += nomedobot.c_str();
      configg += "</a>";
      server.send(200, "text/html", configg);
      delay(5000);
      ESP.restart();
    } else {
      server.send(200, "text/plain", "n conectado!  ");
    }
  }
}


bool initWiFi() {
  loadWifiCredentials(ssid, password, nomedobot);
  ssid.trim();
  password.trim();
  nomedobot.trim();
  
  WiFi.mode(WIFI_STA);
  
  if(ssid==""){
    Serial.println("SSID indefindo");
    return false;
  }

 // Verifique se o SSID e a senha não estão vazios
  if (ssid.length() > 0 && password.length() > 0) {

    WiFi.begin(ssid, password);
    Serial.println("Connecting to WiFi...");
    Serial.println(ssid);
    Serial.println(password);
    Serial.println(nomedobot);
    }
   delay(5000);
  if (WiFi.status() == WL_CONNECTED) {
  Serial.println("conectado");
  WiFi.setHostname(nomedobot.c_str());
  localIP;
  localGateway;
  Serial.println(WiFi.localIP());
  StateUpdate = "ativo";



 if (!MDNS.begin(nomedobot.c_str())) {
    Serial.println("Error setting up MDNS responder!");
    return false;
  } else {
    MDNS.addService("http", "tcp", 80);
    Serial.printf("mDNS responder started '%s'\n", nomedobot.c_str());
    Serial.print("You can now connect to http://");
    Serial.print(nomedobot.c_str());
    Serial.println(".local");   

 }

    return true;

  
  if (!WiFi.config(localIP, localGateway, subnet,primaryDNS)){
    Serial.println("STA Failed to configure");
    return false;
  }
    } else {
    Serial.println("nao conectado");
   return false;

    }
  }


  

void setup() {
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
   Serial.begin(115200);
   //Serial1.begin(115200);
   Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
 
  
  // Configurar o pino do relé como saída
  pinMode(Button1, INPUT); // Define o pino do botão como entrada
  pinMode(ledPin, OUTPUT);   // Define o pino do LED como saída
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Inicialmente, o relé está desligado

if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");

  Serial.println("VARIAVEIS ARMAZENADA");
  Serial.println(contagrana);
  Serial.println(contaobj);
  Serial.println(last_id);


  if(initWiFi()) {
  server.on("/", handleMain);
 // server.on("/desligar", handleDesligar);
 // server.on("/ligar", handleLigar);
  server.begin();
  Serial.println("Servidor web iniciado");
  }

else {
// Iniciar o modo AP
  WiFi.mode(WIFI_AP);
  WiFi.softAP("GRANANET-Config"); // Nome da rede AP
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("Endereço IP do AP: ");
  Serial.println(myIP);
  // Inicializa a biblioteca WiFi
  WiFi.begin();

    // Inicializa o servidor web
  server.on("/", handleRoot);
  server.on("/scan", HTTP_GET, handleScan);
  server.on("/connect", handleConnect);
 // server.on("/desligar", handleDesligar);
 // server.on("/ligar", handleLigar);
  server.begin();
  Serial.println("Servidor web AP iniciado");

}

  current_status = STATUS_WAIT_USER_INPUT;
  generate_qrcode = false;
  tickNumber = 0;
  last_id = read_nvs_data();
  contagrana = read_contagrana();
  contaobj = read_contaobj();
  
  // Serial.setDebugOutput(true);
//#####################################################wifi scan
  setClock();  
}



void loop() {

  // Executa o servidor web
  server.handleClient();

//##################################################LOOP SE CONECTADO
 if (StateUpdate == "ativo") {

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
    verifyActionAndExecute();
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
      contaobj_data(contaobj);

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


//########################################TELEMETRIA
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
           CHAVE2PHP += "&nomedobot=";
           CHAVE2PHP += nomedobot;



  

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




//##########################################VERIFICA FUNCOES PENDENTES NO SQL

void verifyActionAndExecute() {
  // Get the MAC address from the ESP32
  //uint8_t mac[6];
  //WiFi.macAddress(mac);

  // Convert the MAC address to a string
  //String macStr = String(mac[0], HEX) + ":" + String(mac[1], HEX) + ":" + String(mac[2], HEX) + ":" + String(mac[3], HEX) + ":" + String(mac[4], HEX) + ":" + String(mac[5], HEX);

  // Make a HTTP request to the database to get the action information
  HTTPClient httpClient;
  httpClient.begin(url6 + "?mac=" + WiFi.macAddress());
  int httpCode = httpClient.GET();

  // Check the HTTP status code
  if (httpCode == 200) {
    // The action was found
    String payload = httpClient.getString();
    Serial.println(payload);

    // Parse the JSON payload to get the action information
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);

    String action = doc["acoes"];

    // Execute the action
    if (action == "ligado") {
      Serial.println("LIGADO PELO SQL");
      digitalWrite(relayPin, HIGH);
      neopixelWrite(RGB_BUILTIN, 0, 255, 127); // Pink
    } else if (action == "desligado") {
      Serial.println("DESLIGADO PELO SQL");
      digitalWrite(relayPin, LOW);
    } else if (action == "credito") {
      Serial.println("ACAO CREDITO ATIVADO PELO SQL");
       contagrana++;
       contagrana_data(contagrana);
      digitalWrite(relayPin, HIGH);
      neopixelWrite(RGB_BUILTIN, 0, 255, 127); // Pink
      delay(500);
      neopixelWrite(RGB_BUILTIN, 0, 0, 0); // Pink
      digitalWrite(relayPin, LOW);

    } else {
      Serial.println("Unknown action: " + action);
      Serial.println(WiFi.macAddress());

    }
  } else {
    // The action was not found
    Serial.println("Action not found: " + WiFi.macAddress());
  }

  httpClient.end();
}



//################################ Função para salvar o SSID e a senha no SPIFFS
void saveWifiCredentials(const String &ssid, const String &password, const String &nomedobot) {
  File file = SPIFFS.open("/wifi_credentials.txt", "w");
  if (!file) {
    Serial.println("Erro ao abrir o arquivo para salvar as credenciais.");
    return;
  }

  file.println(ssid);
  file.println(password);
  file.println(nomedobot);

  file.close();
}

// Função para carregar o SSID e a senha do SPIFFS
void  loadWifiCredentials(String &ssid, String &password, String &nomedobot) {
  File file = SPIFFS.open("/wifi_credentials.txt", "r");
  if (!file) {
    Serial.println("Arquivo de credenciais não encontrado.");
    return;
  }

  ssid = file.readStringUntil('\n');
  password = file.readStringUntil('\n');
  nomedobot = file.readStringUntil('\n');

  file.close();
}

  void deletewififile() {
String ssid0 ="";
String password0 ="";
String nomedobot0 ="";
        
          saveWifiCredentials(ssid0,password0,nomedobot0);
          ESP.restart();

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
