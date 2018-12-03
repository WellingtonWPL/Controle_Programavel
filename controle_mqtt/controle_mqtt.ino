//Bibliotecas Mqtt e wifi
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//variaveis wifi
const char* ssid = "NOME_DA_REDE";
const char* senha = "********";

//variaveis mqtt
const char* mqtt_server = "broker.mqtt-dashboard.com";
const char* topico = "Sistemas.Embarcados.Topico.SANSUNG";


WiFiClient clienteWIFI;
PubSubClient clienteMQTT(clienteWIFI);
String strMacAddress;
char macAddress[6];

#define BOTAO_CH_UP D7
#define BOTAO_CH_DOWN D6
#define BOTAO_VOL_UP  D5
#define BOTAO_VOL_DOWN  D2

void conectaMQTT() {
  // Loop ate conexão
  while (!clienteMQTT.connected()) {
    Serial.print("Aguardando conexao MQTT...");
    if (clienteMQTT.connect(macAddress)) {
      Serial.println("MQTT conectado");
      //faz subscribe automatico no topico
      clienteMQTT.subscribe(topico);
    } else {
      Serial.print("Falha, rc=");
      Serial.print(clienteMQTT.state());
      Serial.println(" tentando reconectar em 5 segundos.");
      delay(5000);
    }
  }
}

void conectaWifi() {

  // Conexao to Wi-Fi
  Serial.print("Conectando ");
  Serial.println(ssid);
  WiFi.begin(ssid, senha);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Mostra IP do servidor
  Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.RSSI());
  Serial.println("Use este endereço para conectar ao ESP8266");
  Serial.println();
  strMacAddress = WiFi.macAddress();
  strMacAddress.toCharArray(macAddress,6);
  // Conexao com broker no servidor MQTT
  clienteMQTT.setServer(mqtt_server, 1883);
  // Definicao do procedimento de recebimento de mensagens

}

void setup() {
  // put your setup code here, to run once:

  //Define os botões como pullup
  pinMode(BOTAO_CH_UP, INPUT_PULLUP);
  pinMode(BOTAO_CH_DOWN, INPUT_PULLUP);
  pinMode(BOTAO_VOL_UP, INPUT_PULLUP);
  pinMode(BOTAO_VOL_DOWN, INPUT_PULLUP);

  Serial.begin(115200);
  conectaWifi();

}

void loop() {
  if (!clienteMQTT.connected()) {
    conectaMQTT();
  }
  
  clienteMQTT.loop();

  
  // "Vol_UP", "Vol_DOWN", "Canal_UP" e "Canal_DOWN"
  const char* payload = "vazio";

  if(digitalRead(BOTAO_CH_UP)==LOW){
    payload = "Canal_UP"; 
    
  }

  if(digitalRead(BOTAO_CH_DOWN)==LOW){
    payload = "Canal_DOWN";
   
  }

  if(digitalRead(BOTAO_VOL_UP)==LOW){
    payload = "Vol_UP";
    
  }

  if(digitalRead(BOTAO_VOL_DOWN)==LOW){
    payload = "Vol_DOWN";
    
  }


  if(payload != "vazio"){
    Serial.println(payload);
    clienteMQTT.publish(topico, payload);
    delay(250);
  }
 
  

}
