# ✅ Resumo Final - Mapeamento de Pinos Sistema de Líquidos

## 🔌 Pinos Restaurados do teste1.cpp

| Componente | Pino ESP32 | Descrição |
|------------|------------|-----------|
| **Botão LEFT** | **GPIO 25** | Seleciona Café / Tamanho Pequeno |
| **Botão RIGHT** | **GPIO 32** | Seleciona Água / Tamanho Médio |
| **Relé 1 (Café)** | **GPIO 33** | Válvula solenoide do café |
| **Relé 2 (Água)** | **GPIO 26** | Válvula solenoide da água |

---

## 🆕 Pinos Novos Escolhidos

| Componente | Pino ESP32 | Descrição |
|------------|------------|-----------|
| **SOL_PIN1** | **GPIO 27** | Solenoide travamento (fase 1) |
| **SOL_PIN2** | **GPIO 4** | Solenoide travamento (fase 2) |
| **CONTATO** | **GPIO 13** | Sensor porta aberta/fechada |
| **TRIGGER1** | **GPIO 22** | Ultrassônico Café (trigger) |
| **ECHO1** | **GPIO 21** | Ultrassônico Café (echo) |
| **TRIGGER2** | **GPIO 19** | Ultrassônico Água (trigger) |
| **ECHO2** | **GPIO 18** | Ultrassônico Água (echo) |

---

## 📡 Pinos Fixos dos Periféricos

### E-Paper Waveshare 2.9"
- CS: GPIO 10
- DC: GPIO 14
- RST: GPIO 15
- BUSY: GPIO 16

### RFID RC522
- CS: GPIO 46
- RST: GPIO 17

### SPI (Compartilhado)
- SCK: GPIO 18
- MISO: GPIO 19
- MOSI: GPIO 23

---

## 🎯 Diagrama de Conexão Rápido

```
ESP32-WROOM-32
┌─────────────────────────┐
│                         │
│  GPIO 25 ←─── Botão 1   │ (LEFT - Café/Pequeno)
│  GPIO 32 ←─── Botão 2   │ (RIGHT - Água/Médio)
│                         │
│  GPIO 33 ───→ Relé 1    │ (Café)
│  GPIO 26 ───→ Relé 2    │ (Água)
│                         │
│  GPIO 27 ───→ SOL_PIN1  │ (Trava)
│  GPIO 4  ───→ SOL_PIN2  │ (Trava)
│  GPIO 13 ←─── CONTATO   │ (Sensor porta)
│                         │
│  GPIO 22 ───→ TRIG1     │ (Ultra Café)
│  GPIO 21 ←─── ECHO1     │ (Ultra Café)
│  GPIO 19 ───→ TRIG2     │ (Ultra Água)
│  GPIO 18 ←─── ECHO2     │ (Ultra Água)
│                         │
│  GPIO 46 ───→ RFID CS   │
│  GPIO 17 ───→ RFID RST  │
│                         │
│  GPIO 10 ───→ EPD CS    │
│  GPIO 14 ───→ EPD DC    │
│  GPIO 15 ───→ EPD RST   │
│  GPIO 16 ←─── EPD BUSY  │
│                         │
│  GPIO 18 ───→ SPI SCK   │ (Compartilhado)
│  GPIO 19 ←─── SPI MISO  │ (Compartilhado)
│  GPIO 23 ───→ SPI MOSI  │ (Compartilhado)
│                         │
└─────────────────────────┘
```

---

## 📋 Lista de Compras - Componentes

- [x] ESP32-WROOM-32
- [x] E-Paper Waveshare 2.9"
- [x] RFID RC522
- [ ] 2× Relés 5V
- [ ] 2× Válvulas solenoides para líquidos
- [ ] 2× Botões tácteis (normalmente abertos)
- [ ] 1× Solenoide de travamento
- [ ] 1× Sensor de contato (reed switch ou microswitch)
- [ ] 2× Sensores ultrassônicos HC-SR04
- [ ] Resistores para divisor de tensão (1kΩ + 2kΩ para cada HC-SR04)
- [ ] Fonte de alimentação 5V/12V externa (para relés/solenoide)

---

## ⚡ Checklist de Montagem

### Etapa 1: Periféricos SPI (Teste Básico)
- [ ] Conectar e-paper (pinos 10, 14, 15, 16 + SPI)
- [ ] Conectar RFID (pinos 46, 17 + SPI)
- [ ] Testar comunicação SPI
- [ ] Testar display mostrando algo na tela
- [ ] Testar leitura de cartão RFID

### Etapa 2: Controle de Líquidos (Teste Individual)
- [ ] Conectar Relé 1 no GPIO 33
- [ ] Conectar Relé 2 no GPIO 26
- [ ] Testar acionamento de cada relé separadamente
- [ ] Conectar válvulas solenoides aos relés
- [ ] Testar dispensação manual (código simples)

### Etapa 3: Interface de Usuário
- [ ] Conectar Botão LEFT no GPIO 25 (com pull-up)
- [ ] Conectar Botão RIGHT no GPIO 32 (com pull-up)
- [ ] Testar detecção de pressionar botões
- [ ] Integrar botões com seleção de produtos

### Etapa 4: Sensores de Estoque
- [ ] Montar divisores de tensão para HC-SR04 (5V→3.3V)
- [ ] Conectar HC-SR04 #1 (GPIO 22/21) - Café
- [ ] Conectar HC-SR04 #2 (GPIO 19/18) - Água
- [ ] Testar leitura de distância
- [ ] Calibrar limites de estoque baixo

### Etapa 5: Sistema de Travamento
- [ ] Conectar Solenoide (GPIO 27/4)
- [ ] Conectar Sensor de contato (GPIO 13)
- [ ] Testar travar/destravar
- [ ] Testar detecção de porta aberta/fechada
- [ ] Integrar com fluxo RFID

### Etapa 6: Integração Final
- [ ] Upload do código completo
- [ ] Testar fluxo: RFID → Produto → Tamanho → Dispensação
- [ ] Verificar atualização de saldo
- [ ] Testar alertas de estoque baixo
- [ ] Validar travamento após compra

---

## 🔧 Código de Teste Rápido - Relés

```cpp
// Teste rápido dos relés
#define RELE1 33
#define RELE2 26

void setup() {
  pinMode(RELE1, OUTPUT);
  pinMode(RELE2, OUTPUT);
  digitalWrite(RELE1, LOW);
  digitalWrite(RELE2, LOW);
}

void loop() {
  // Liga relé 1 por 3s
  digitalWrite(RELE1, HIGH);
  delay(3000);
  digitalWrite(RELE1, LOW);
  delay(1000);

  // Liga relé 2 por 5s
  digitalWrite(RELE2, HIGH);
  delay(5000);
  digitalWrite(RELE2, LOW);
  delay(1000);
}
```

---

## 🔧 Código de Teste Rápido - Botões

```cpp
// Teste rápido dos botões
#define BTN_LEFT 25
#define BTN_RIGHT 32

void setup() {
  Serial.begin(115200);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(BTN_LEFT) == LOW) {
    Serial.println("Botão LEFT pressionado!");
    delay(200);
  }

  if (digitalRead(BTN_RIGHT) == LOW) {
    Serial.println("Botão RIGHT pressionado!");
    delay(200);
  }
}
```

---

## 📝 Observações Importantes

1. **Divisor de Tensão Obrigatório**: HC-SR04 retorna 5V no ECHO, ESP32 aceita apenas 3.3V
   ```
   5V ──[R1: 1kΩ]──┬──[R2: 2kΩ]── GND
                   │
                   └─→ ESP32 (ECHO)
   ```

2. **Pull-ups Internos**: Botões e sensor de contato usam `INPUT_PULLUP`

3. **Alimentação Externa**: Relés e solenoide precisam de fonte externa (não usar 3.3V do ESP32)

4. **GND Comum**: Todos os grounds (ESP32, fonte externa, periféricos) devem estar conectados

5. **GPIO 18/19 Compartilhados**: Funciona porque SPI é usado apenas na inicialização

---

## ✅ Arquivo Atualizado

**[src/proj_iot_liquidos.cpp](src/proj_iot_liquidos.cpp#L13-L32)**

```cpp
// Pinos dos botões (restaurados do teste1.cpp)
#define PINO_BOTAO_LEFT 25
#define PINO_BOTAO_RIGHT 32

// Pinos dos relés (restaurados do teste1.cpp)
#define PINO_RELE_LIQUIDO1 33
#define PINO_RELE_LIQUIDO2 26

// Novos pinos
#define SOL_PIN1 27
#define SOL_PIN2 4
#define CONTATO 13
#define PINO_TRIGGER1 22
#define PINO_ECHO1 21
#define PINO_TRIGGER2 19
#define PINO_ECHO2 18
```
