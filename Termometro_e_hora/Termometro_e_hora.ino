
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <SSD1306Wire.h>
#include <AM2320.h>

#define botao 13 //d7
#define SDA_PIN       4
#define SCL_PIN       5
#define ENDERECO_OLED 0x3C
#define TAMANHO       GEOMETRY_128_32
#define TIME_ZONE    -3

// WiFi network info.
const char* ssid = "wpl";
const char* senha = "32398383";

unsigned long contador;         // Armazena o valor dos milisegundos até o próximo intervalo
unsigned long intervalo = 1000; // Tempo em ms do intervalo a ser executado
float temperatura;
float umidade;

WiFiUDP ntpUDP; // Cliente UDP para o NTP
NTPClient horaCliente(ntpUDP, "pool.ntp.org", TIME_ZONE * 3600, 60000); // Configuracao do Cliente NTP
SSD1306Wire display(ENDERECO_OLED, SDA_PIN, SCL_PIN, TAMANHO); // SDA, SCL -> Configuracao do display SSD1306
AM2320 sensor; // Cria uma instancia do sensor AM2320

void setup() {
  pinMode(botao, INPUT);
  // Conexao to Wi-Fi
  // Mostrar informacao no Display OLED
  display.init();
  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display.setFont(ArialMT_Plain_24);
  display.drawString(display.getWidth() / 2, display.getHeight() / 2, "Conectando");
  display.display();
  // Mostrar informacao na porta Serial
  Serial.begin(115200);
  Serial.print("Conectando ");
  Serial.println(ssid);
  WiFi.begin(ssid, senha);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Mostra IP da conexao na porta Serial
  Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
  Serial.println("Use este endereço para conectar ao ESP8266");
  Serial.println();
  // Mostra o IP da conexao no display OLED
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.drawString(display.getWidth() / 2, 18, "IP: " + WiFi.localIP().toString());
  display.display();

  horaCliente.begin();
  sensor.begin(SDA_PIN, SCL_PIN); // Conecta o sensor AM2320 ao barramento I2C - SDA, SCL
  contador = millis(); // Inicializa o contador para o intervalo
}

void loop() {
  if(digitalRead(botao) == HIGH){
    String infoDisplay; // Variavel que armazena a informacao a ser mostrada no display ou porta serial
    char strDisplay[30]; // Variavel auxiliar para armazenar infoDisplay formatada
    if (millis() - contador > intervalo) {
      if (sensor.measure()) { // Verifica se o sensor esta operacional para leitura dos valores 
        temperatura = sensor.getTemperature(); // Obtem o valor de temperatura
        umidade = sensor.getHumidity(); // Obtem o valor da umidade relativa
        sprintf(strDisplay, "%.1fºC  -  %.0f%%", temperatura, umidade); // Formata a saida para ser mostrada no display
      } else {
        sprintf(strDisplay, "Erro leitura");
      }
      infoDisplay = strDisplay; // Atualiza o conteudo da informacao para String infoDisplay
      Serial.println(infoDisplay); // Imprime informacao formatada na serial
  }
  else{
     String infoDisplay;
    if (millis() - contador > intervalo) {
      horaCliente.update(); // Atualiza a hora no sistema utilizando o servidor NTP
      infoDisplay = horaCliente.getFormattedTime();
      // Mostra hora na porta Serial
      Serial.println(infoDisplay); // Imprime informacao formatada na serial
      // Mostra hora no display OLED
      display.clear();
      display.drawRect(0, 0, display.getWidth() - 1, display.getHeight() - 1);
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.setFont(ArialMT_Plain_16);
      display.drawString(display.getWidth() / 2,  0, infoDisplay);
      display.setFont(ArialMT_Plain_10);
      display.drawString(display.getWidth() / 2, 16, "IP: " + WiFi.localIP().toString());
      display.display();
      contador = millis();
    }
  }      
 }
}
