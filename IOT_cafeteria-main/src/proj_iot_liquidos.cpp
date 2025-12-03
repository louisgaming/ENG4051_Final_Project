#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "certificados.h"
#include <MQTT.h>
#include <OneButton.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <MFRC522.h>
#include "tela.h"
// #include <Ultrasonic.h>  // Comentado - não usado no momento

// Pinos dos botões (restaurados do teste1.cpp)
#define PINO_BOTAO_LEFT 25   // Botão 1 do protótipo
#define PINO_BOTAO_RIGHT 32  // Botão 2 do protótipo

// Pinos dos relés para válvulas solenoides de líquidos (restaurados do teste1.cpp)
#define PINO_RELE_LIQUIDO1 33  // Relé 1 do protótipo - Café
#define PINO_RELE_LIQUIDO2 26  // Relé 2 do protótipo - Água

// Sensores ultrassônicos (comentados - não usado no momento)
// #define PINO_TRIGGER1 22  // Trigger sensor 1 (Café)
// #define PINO_ECHO1 21     // Echo sensor 1 (Café)
// #define PINO_TRIGGER2 19  // Trigger sensor 2 (Água)
// #define PINO_ECHO2 18     // Echo sensor 2 (Água)

// Tempos de dispensação (em milissegundos)
#define TEMPO_PEQUENO 3000  // 3 segundos para tamanho pequeno
#define TEMPO_MEDIO 5000    // 5 segundos para tamanho médio

// Variáveis de controle de dispensação
bool releAtivo = false;
unsigned long tempoDesligarRele = 0;
int pinoReleAtual = 0;

// Variáveis de controle de fluxo
enum Estado {
  AGUARDANDO_RFID,
  SELECIONANDO_PRODUTO,
  SELECIONANDO_TAMANHO,
  DISPENSANDO
};

Estado estadoAtual = AGUARDANDO_RFID;
int tamanhoSelecionado = 0; // 1 = pequeno, 2 = médio

WiFiClientSecure conexaoSegura;
MQTTClient mqtt(1000);

MFRC522 rfid(13, 17);  // CS = GPIO 13, RST = GPIO 17 (GPIO 13 não é ADC2 - funciona com WiFi)
MFRC522::MIFARE_Key chaveA = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};

// Sensores ultrassônicos comentados (não usado no momento)
// Ultrasonic ultrasonicLiquido1(PINO_TRIGGER1, PINO_ECHO1);
// Ultrasonic ultrasonicLiquido2(PINO_TRIGGER2, PINO_ECHO2);

OneButton left(PINO_BOTAO_LEFT, true, true);   // Botão para café (pin, activeLow, pullupActive)
OneButton right(PINO_BOTAO_RIGHT, true, true); // Botão para água (pin, activeLow, pullupActive)

JsonDocument produto;
JsonDocument user;
JsonDocument produtos;

bool usuarioValido = false;
bool fim = false;

unsigned long instanteAnterior = 0;

void verificaUser(String rfid){
  JsonDocument msg;
  msg["rfid"] = rfid;
  String textoJson;
  serializeJson(msg, textoJson);
  mqtt.publish("verifica_usuario_cafe", textoJson);
}

String lerRFID() {
  String id = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (i > 0) {
      id += " ";
    }
    if (rfid.uid.uidByte[i] < 0x10) {
      id += "0";
    }
    id += String(rfid.uid.uidByte[i], HEX);
  }
  id.toUpperCase();
  return id;
}

String lerTextoDoBloco(byte bloco) {
  byte tamanhoDados = 18;
  char dados[tamanhoDados];
  MFRC522::StatusCode status = rfid.PCD_Authenticate(
    MFRC522::PICC_CMD_MF_AUTH_KEY_A,
    bloco, &chaveA, &(rfid.uid)
  );
  if (status != MFRC522::STATUS_OK) { return ""; }
  status = rfid.MIFARE_Read(bloco,
              (byte*)dados, &tamanhoDados);
  if (status != MFRC522::STATUS_OK) { return ""; }
  dados[tamanhoDados - 2] = '\0';
  return String(dados);
}

void reconectarWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin("Projeto", "2022-11-07");
    Serial.print("Conectando ao WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(1000);
    }
    Serial.print("conectado!\nEndereço IP: ");
    Serial.println(WiFi.localIP());
  }
}

void reconectarMQTT() {
  if (!mqtt.connected()) {
    Serial.print("Conectando MQTT...");
    while(!mqtt.connected()) {
      mqtt.connect("cafeteria_iot", "aula", "zowmad-tavQez");
      Serial.print(".");
      delay(1000);
    }
    Serial.println(" conectado!");

    mqtt.subscribe("cafeteria_iot");
    mqtt.subscribe("verifica_usuario_cafe");
    mqtt.subscribe("retorna_usuario_cafe");
    mqtt.subscribe("retorna_produtos_cafe");
  }
}

void pegaProdutos(){
  mqtt.publish("pega_produtos_cafe", "");

  // Produtos líquidos
  int id = 1;
  produtos[0]["id"] = id;
  String nome = "Café";
  produtos[0]["nome"] = nome;
  float preco_pequeno = 2.00;
  produtos[0]["preco_pequeno"] = preco_pequeno;
  float preco_medio = 2.50;
  produtos[0]["preco_medio"] = preco_medio;

  id = 2;
  produtos[1]["id"] = id;
  nome = "Agua";
  produtos[1]["nome"] = nome;
  preco_pequeno = 1.00;
  produtos[1]["preco_pequeno"] = preco_pequeno;
  preco_medio = 2.00;
  produtos[1]["preco_medio"] = preco_medio;
}

// Funções de verificação de estoque comentadas (sensores ultrassônicos não usados no momento)
/*
float lerDistanciaMedia(Ultrasonic &sensor, int nAmostras = 5) {
  long soma = 0;

  for (int i = 0; i < nAmostras; i++) {
    unsigned int d = sensor.read();
    soma += d;
    delay(30);
  }

  float media = soma / (float)nAmostras;
  Serial.print("Distância média estoque: ");
  Serial.print(media);
  Serial.println(" cm");

  return media;
}

bool estoqueBaixoSensor(Ultrasonic &sensor, float limiarCm, const char *nome) {
  float media = lerDistanciaMedia(sensor);

  if (media > limiarCm) {
    Serial.print("Estoque baixo de ");
    Serial.println(nome);
    return true;
  }

  Serial.print("Estoque OK de ");
  Serial.println(nome);
  return false;
}

// produto = 1 -> Café
// produto = 2 -> Agua
// produto = 0 -> verificar os dois
bool verificaEstoque(int produtoId) {
  bool baixo = false;
  bool baixoCafe = false;
  bool baixoChocolate = false;

  float limite = 15.0; // Distância em cm para considerar estoque baixo

  if (produtoId == 1) {
    baixo = estoqueBaixoSensor(ultrasonicLiquido1, limite, "Café");
  }
  else if (produtoId == 2) {
    baixo = estoqueBaixoSensor(ultrasonicLiquido2, limite, "Chocolate Quente");
  }
  else {
    // verifica os dois
    baixoCafe = estoqueBaixoSensor(ultrasonicLiquido1, limite, "Café");
    baixoChocolate = estoqueBaixoSensor(ultrasonicLiquido2, limite, "Chocolate Quente");
  }

  if (baixo && produtoId != 0) {
    telaSemEstoque();
    String produtoStr = "{\"produto\":" + String(produtoId) + "}";
    mqtt.publish("estoque_baixo_cafe", produtoStr);
    fim = true;
    instanteAnterior = millis();
  }

  if (produtoId == 0) {
    if (baixoCafe) {
      mqtt.publish("estoque_baixo_cafe", "{\"produto\":1}");
      fim = true;
      instanteAnterior = millis();
    }
    if (baixoChocolate) {
      mqtt.publish("estoque_baixo_cafe", "{\"produto\":2}");
      fim = true;
      instanteAnterior = millis();
    }
  }
  return baixo;
}
*/

void recebeuMensagem(String topico, String conteudo) {
  Serial.println(topico + ": " + conteudo);
  if (topico == "retorna_usuario_cafe"){
    deserializeJson(user, conteudo);
    int id = user["id"];
    usuarioValido = id != 0;
    fim = !usuarioValido;

    if (usuarioValido){
      float balance = user["balance"].as<float>();
      if(balance < 0){
        telaSaldoNegativo(balance);
        usuarioValido = false;
        fim = true;
        instanteAnterior = millis();
        estadoAtual = AGUARDANDO_RFID;
        return;
      }
      telaProdutos(user["name"], user["balance"]);
      estadoAtual = SELECIONANDO_PRODUTO;
    }
    else {
      usuarioInvalido();
      estadoAtual = AGUARDANDO_RFID;
    }
  }
  else if (topico == "cafeteria_iot"){
    usuarioValido = false;
    estadoAtual = AGUARDANDO_RFID;
  }
  else if(topico == "retorna_produtos_cafe"){
    deserializeJson(produtos, conteudo);
    Serial.println("Produtos recebidos:");
    serializeJson(produtos, Serial);
  }
}

void dispensarLiquido() {
  if (!usuarioValido || releAtivo) return;

  int id_produto = produto["id"];
  int pino = (id_produto == 1) ? PINO_RELE_LIQUIDO1 : PINO_RELE_LIQUIDO2;
  int tempo = (tamanhoSelecionado == 1) ? TEMPO_PEQUENO : TEMPO_MEDIO;
  String tamanho = (tamanhoSelecionado == 1) ? "Pequeno" : "Médio";

  Serial.println("Dispensando " + produto["nome"].as<String>() + " " + tamanho + "...");

  digitalWrite(pino, HIGH);
  releAtivo = true;
  pinoReleAtual = pino;
  tempoDesligarRele = millis() + tempo;
  estadoAtual = DISPENSANDO;

  telaDispensando(produto["nome"].as<String>() + " " + tamanho);
}

void selecionaProdutoLeft() {
  if (estadoAtual != SELECIONANDO_PRODUTO) return;

  bool isLeft = true;
  int id_produto_int = isLeft ? 1 : 2;
  String id_produto = String(id_produto_int);

  // Verificação de estoque comentada (sensores não usados no momento)
  /*
  if (verificaEstoque(id_produto_int)) {
    Serial.println("Tentou selecionar produto sem estoque.");
    return;
  }
  */

  JsonArray arr = produtos.as<JsonArray>();
  JsonObject escolhido;

  for (JsonObject p : arr) {
    if (p["id"].as<String>() == id_produto) {
      escolhido = p;
      break;
    }
  }

  produto.clear();
  if (!escolhido.isNull()) {
    produto.set(escolhido);
  } else {
    Serial.println("Produto não encontrado!");
    return;
  }

  String nome_produto = produto["nome"];
  Serial.println("Produto selecionado: " + nome_produto);

  // Avança para seleção de tamanho
  estadoAtual = SELECIONANDO_TAMANHO;
  telaSelecionaTamanho(nome_produto);
}

void selecionaProdutoRight() {
  if (estadoAtual != SELECIONANDO_PRODUTO) return;

  bool isLeft = false;
  int id_produto_int = isLeft ? 1 : 2;
  String id_produto = String(id_produto_int);

  // Verificação de estoque comentada (sensores não usados no momento)
  /*
  if (verificaEstoque(id_produto_int)) {
    Serial.println("Tentou selecionar produto sem estoque.");
    return;
  }
  */

  JsonArray arr = produtos.as<JsonArray>();
  JsonObject escolhido;

  for (JsonObject p : arr) {
    if (p["id"].as<String>() == id_produto) {
      escolhido = p;
      break;
    }
  }

  produto.clear();
  if (!escolhido.isNull()) {
    produto.set(escolhido);
  } else {
    Serial.println("Produto não encontrado!");
    return;
  }

  String nome_produto = produto["nome"];
  Serial.println("Produto selecionado: " + nome_produto);

  // Avança para seleção de tamanho
  estadoAtual = SELECIONANDO_TAMANHO;
  telaSelecionaTamanho(nome_produto);
}

void selecionaTamanhoLeft() {
  if (estadoAtual != SELECIONANDO_TAMANHO) return;

  bool isLeft = true;
  tamanhoSelecionado = isLeft ? 1 : 2; // 1 = pequeno, 2 = médio
  String tamanho = isLeft ? "Pequeno" : "Médio";

  Serial.println("Tamanho selecionado: " + tamanho);

  // Dispensa o líquido
  dispensarLiquido();
}

void selecionaTamanhoRight() {
  if (estadoAtual != SELECIONANDO_TAMANHO) return;

  bool isLeft = false;
  tamanhoSelecionado = isLeft ? 1 : 2; // 1 = pequeno, 2 = médio
  String tamanho = isLeft ? "Pequeno" : "Médio";

  Serial.println("Tamanho selecionado: " + tamanho);

  // Dispensa o líquido
  dispensarLiquido();
}

void finalizarCompra() {
  float preco = (tamanhoSelecionado == 1) ?
                produto["preco_pequeno"].as<float>() :
                produto["preco_medio"].as<float>();

  Serial.println("Finalizando compra...");
  Serial.println("Valor: R$" + String(preco));

  float total = preco;
  float saldoUsuario = user["balance"];

  JsonDocument msg;
  msg["id_user"] = user["id"];
  msg["id_produto"] = produto["id"];
  msg["tamanho"] = (tamanhoSelecionado == 1) ? "pequeno" : "medio";
  msg["total"] = total;
  msg["saldoAtual"] = saldoUsuario - total;

  telaSaldoFinal(total, saldoUsuario);
  String textoJson;
  serializeJson(msg, textoJson);
  mqtt.publish("cafeteria_iot", textoJson);

  produto.clear();
  usuarioValido = false;
  tamanhoSelecionado = 0;
  estadoAtual = AGUARDANDO_RFID;
}

void setup() {
  Serial.begin(115200); delay(500);
  Serial.println("Projeto Final – Dispensador de Líquidos");

  SPI.begin();
  rfid.PCD_Init();

  reconectarWiFi();

  conexaoSegura.setCACert(certificado1);

  mqtt.begin("mqtt.janks.dev.br", 8883, conexaoSegura);
  mqtt.onMessage(recebeuMensagem);
  mqtt.setKeepAlive(10);
  mqtt.setWill("tópico da desconexão", "conteúdo");
  reconectarMQTT();

  pegaProdutos();

  telaSetup();

  // Configuração dos relés para válvulas solenoides
  pinMode(PINO_RELE_LIQUIDO1, OUTPUT);
  digitalWrite(PINO_RELE_LIQUIDO1, LOW);

  pinMode(PINO_RELE_LIQUIDO2, OUTPUT);
  digitalWrite(PINO_RELE_LIQUIDO2, LOW);

  // Configuração dos botões com OneButton
  left.attachClick(selecionaProdutoLeft);
  right.attachClick(selecionaProdutoRight);

  estadoAtual = AGUARDANDO_RFID;

  telaInicial();
}

void loop() {
  reconectarWiFi();
  reconectarMQTT();
  mqtt.loop();

  if(fim){
    unsigned long instanteAtual = millis();
    if (instanteAtual > instanteAnterior + 5000) {
      instanteAnterior = instanteAtual;
      fim = false;
      usuarioValido = false;
      telaInicial();
    }
  }

  // Leitura do cartão RFID
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()){
    String id = lerRFID();
    Serial.println("UID: " + id);
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();

    // Debug - remover em produção
    JsonDocument envio;
    int idd = 1;
    String name = "Luizinho";
    float balance = 29.99;
    envio["id"] = idd;
    envio["name"] = name;
    envio["balance"] = balance;
    String txt;
    serializeJson(envio, txt);
    mqtt.publish("retorna_usuario_cafe", txt);
  }

  else if (usuarioValido){
    // Atualiza handlers dos botões conforme o estado
    static Estado estadoAnterior = AGUARDANDO_RFID;

    if (estadoAtual != estadoAnterior) {
      if (estadoAtual == SELECIONANDO_PRODUTO) {
        left.attachClick(selecionaProdutoLeft);
        right.attachClick(selecionaProdutoRight);
      }
      else if (estadoAtual == SELECIONANDO_TAMANHO) {
        left.attachClick(selecionaTamanhoLeft);
        right.attachClick(selecionaTamanhoRight);
      }
      estadoAnterior = estadoAtual;
    }

    // Processa botões (OneButton tick)
    left.tick();
    right.tick();
  }

  // Controle de desligamento automático do relé
  unsigned long agora = millis();

  if (releAtivo && agora >= tempoDesligarRele) {
    digitalWrite(pinoReleAtual, LOW);
    releAtivo = false;
    Serial.println("Relé DESLIGADO - Líquido dispensado");

    // Finaliza a compra
    finalizarCompra();
    fim = true;
    instanteAnterior = millis();
  }
}
