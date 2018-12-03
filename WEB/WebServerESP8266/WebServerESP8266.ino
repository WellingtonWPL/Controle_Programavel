#include <ESP8266WiFi.h>

#define PIN_FRENTE    12
#define PIN_TRAZ      13
#define PIN_ESQUERDA  14
#define PIN_DIREITA   15

const char* ssid = "VIRUS";
const char* senha = "predador2018";

int frenteLigado = LOW;
int trazLigado = LOW;
int esquerdaLigado = LOW;
int direitaLigado = LOW;

// Porta web server = 80
WiFiServer server(80);

void setup() {
  pinMode(PIN_FRENTE, OUTPUT);
  pinMode(PIN_TRAZ, OUTPUT);
  pinMode(PIN_ESQUERDA, OUTPUT);
  pinMode(PIN_DIREITA, OUTPUT);

  Serial.begin(115200);
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
  Serial.println("Use este endereço para conectar ao ESP8266");
  Serial.println();

  server.begin();
}

void loop() {
  WiFiClient client = server.available();   // Aguarda requisicoes de clientes
  if (client) {                             // Se cliente conectado,
    String cabec = ""; // Armazenamento HTTP request
    boolean linhaAtualVazia = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        cabec += c; // armazena conteudo do cabecalho para processamento
        // se é o final da linha e a linha está em branco,
        // significa que a solicitação http terminou,
        // desta forma e possível enviar uma resposta http
        if (c == '\n' && linhaAtualVazia) {
          // processa o comando GET enviado do cliente
          processaGET(cabec);
          // envia um cabecalho de resposta http padrao
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // a conexao será fechada após a conclusão da resposta
          client.println();
          // Pagina HTML
          client.println("<!DOCTYPE html>");
          client.println("<html>");
          client.println("<head>");
          client.println("  <title>Controle Agendável</title>");
          client.println("</head>");
          client.println("<body>");
          client.println("<style type=\"text/css\">");
          client.println(" "); 
          client.println("  label {");
          client.println("    display: block;");
          client.println("    font: 1rem 'Fira Sans', sans-serif;");
          client.println("  }");
          client.println("");
          client.println("  input,");
          client.println("  label {");
          client.println("      margin: .4rem 0;");
          client.println("  }");
          client.println("");
          client.println("  .note {");
          client.println("      font-size: .8em;");
          client.println("  }");
          client.println("");
          client.println("</style>");
          client.println("<script type=\"text/javascript\">");
          client.println("  function saveData(){");
          client.println("");
          client.println("  }");
          client.println("");
          client.println("");
          client.println("</script>");
          client.println("");
          client.println("");
          client.println("<meta charset=\"utf-8\"/>");
          client.println("  <div id=\"grupo\" class=\"grupo\">");
          client.println("    <fieldset>");
          client.println("      <legend><h2>Canais</h2></legend>");
          client.println("      <form method=\"post\" action="" id=\"formMedias\">");
          client.println("        <fieldset id=\"nome\" style=\"width: 500px; margin-right: 50%; margin-left: 25%; margin-top: -30px;\">");
          client.println("        <legend><h3>Seu canal:</h3></legend>");
          client.println("            <label for=\"appt\">A TV estará ligada antes do evento?</label>");
          client.println("            <tr id=\"escolha\"> ");
          client.println("                        <td><input type=\"radio\" name=\"choice\" id=\"choice1\" value=\"Sim\" style=\"margin-left: 10px; margin-top: 10px;\"> Sim</td>");
          client.println("                        <td><input type=\"radio\" name=\"choice\" id=\"choice2\" value=\"Não\" style=\"margin-left: 10px; \"> Não</td>");
          client.println("                    </tr>");             
          client.println("          <br><br>");
          client.println("          <td>");
          client.println("            <label for=\"ch\">Escolha o número do seu canal:</label>");
          client.println("            <input type=\"number\" id=\"ch\" name=\"ch\"");
          client.println("                   min=\"1\" max=\"300\" value=\"0\" required>");
          client.println("            <span class=\"note\">Canal.</span>");
          client.println("          </td>");
          client.println("          <br><br>");
          client.println("          <td>");
          client.println("            <label for=\"appt\">Escolha o horário de seu programa:</label>");
          client.println("            <input type=\"time\" id=\"appt\" name=\"appt\">");
          client.println("");
          client.println("          </td>");           
          client.println("        </fieldset>");
          client.println("      </form>");
          client.println("    </fieldset>");
          client.println("  </div>");
          client.println("  <input type=\"submit\" id=\"save\" onclick=\"saveData();\" value=\"Save\" />");
          client.println("</body>");
          client.println("</html>");
          // Envia a HTTP response
          client.println();
          break;
        
        }
        if (c == '\n') {  // inicio de uma nova linha
          linhaAtualVazia = true;
        } else if (c != '\r') {  // existe pelo menos um caracter na linha atual
          linhaAtualVazia = false;
        }
      }
    }
    // um atraso para o web browser processar a resposta
    delay(10);
    // close the connection:
    // Fecha conexao
    client.stop();
  }
}

void processaGET(String c) {
  if (c.indexOf("GET /") >= 0) {
    String linha = c.substring(c.indexOf("GET /") + 5);
    int ind = 0;
    String info = "";
    while (linha[ind] != '\n') {
      info += linha[ind];
      ind++;
    }
    String acao = info.substring(0, info.indexOf(" HTTP/1.1"));
    if (acao == "btnCimaEsq") {
      frenteLigado = HIGH;
      esquerdaLigado = HIGH;
    }
    else if (acao == "btnCima") {
      frenteLigado = HIGH;
    }
    else if (acao == "btnCimaDir") {
      frenteLigado = HIGH;
      direitaLigado = HIGH;
    }
    else if (acao == "btnEsquerda") {
      esquerdaLigado = HIGH;
    }
    else if (acao == "btnHome") {
      frenteLigado = LOW;
      trazLigado = LOW;
      esquerdaLigado = LOW;
      direitaLigado = LOW;
    }
    else if (acao == "btnDireita") {
      direitaLigado = HIGH;
    }
    if (acao == "btnBaixoEsq") {
      trazLigado = HIGH;
      esquerdaLigado = HIGH;
    }
    else if (acao == "btnBaixo") {
      trazLigado = HIGH;
    }
    else if (acao == "btnBaixoDir") {
      trazLigado = HIGH;
      direitaLigado = HIGH;
    }

  }

  //Atualiza as saidas
  digitalWrite(PIN_FRENTE, frenteLigado);
  digitalWrite(PIN_TRAZ, trazLigado);
  digitalWrite(PIN_ESQUERDA, esquerdaLigado);
  digitalWrite(PIN_DIREITA, direitaLigado);
}

