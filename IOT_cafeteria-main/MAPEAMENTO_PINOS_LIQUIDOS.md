# 🔌 Mapeamento de Pinos - Sistema de Líquidos ESP32-WROOM

## ✅ Pinos Restaurados do Protótipo (teste1.cpp)

| Componente | Pino | Observação |
|------------|------|------------|
| **Botão LEFT** (Pequeno) | **GPIO 25** | ✅ Restaurado do teste1.cpp |
| **Botão RIGHT** (Médio) | **GPIO 32** | ✅ Restaurado do teste1.cpp |
| **Relé 1** (Café) | **GPIO 33** | ✅ Restaurado do teste1.cpp |
| **Relé 2** (Água) | **GPIO 26** | ✅ Restaurado do teste1.cpp |

---

## 🆕 Pinos Novos (não estavam no teste1.cpp)

### Solenoide de Travamento da Porta
| Componente | Pino | Observação |
|------------|------|------------|
| **SOL_PIN1** | **GPIO 27** | Controle do solenoide (fase 1) |
| **SOL_PIN2** | **GPIO 14** | Controle do solenoide (fase 2) |

### Sensor de Contato da Porta
| Componente | Pino | Observação |
|------------|------|------------|
| **CONTATO** | **GPIO 13** | Detecta se a porta está aberta/fechada |

### Sensores Ultrassônicos (Nível dos Reservatórios)
| Componente | Pino | Observação |
|------------|------|------------|
| **TRIGGER1** (Café) | **GPIO 22** | Trigger do sensor ultrassônico 1 |
| **ECHO1** (Café) | **GPIO 21** | Echo do sensor ultrassônico 1 |
| **TRIGGER2** (Água) | **GPIO 19** | Trigger do sensor ultrassônico 2 |
| **ECHO2** (Água) | **GPIO 18** | Echo do sensor ultrassônico 2 |

---

## 📡 Pinos Fixos (Periféricos SPI)

### E-Paper Display (Waveshare 2.9")
| Função | Pino | Descrição |
|--------|------|-----------|
| **CS** | **GPIO 10** | Chip Select |
| **DC** | **GPIO 14** | Data/Command |
| **RST** | **GPIO 15** | Reset |
| **BUSY** | **GPIO 16** | Busy Signal |
| **SCK** | **GPIO 18** | SPI Clock (compartilhado) |
| **MOSI** | **GPIO 23** | SPI Data Out (compartilhado) |

### RFID RC522
| Função | Pino | Descrição |
|--------|------|-----------|
| **CS/SDA** | **GPIO 46** | Chip Select |
| **RST** | **GPIO 17** | Reset |
| **SCK** | **GPIO 18** | SPI Clock (compartilhado) |
| **MISO** | **GPIO 19** | SPI Data In (compartilhado) |
| **MOSI** | **GPIO 23** | SPI Data Out (compartilhado) |

---

## ⚠️ Pinos a Evitar no ESP32-WROOM

| Pino | Motivo |
|------|--------|
| **GPIO 0** | Boot mode - pode impedir boot |
| **GPIO 2** | Boot mode - conectado ao LED onboard |
| **GPIO 5** | Boot strapping - pode causar problemas |
| **GPIO 6-11** | Conectados à flash interna - **NUNCA USE** |
| **GPIO 12** | Boot strapping - cuidado com pull-up/down |
| **GPIO 15** | Boot strapping - já usado pelo e-paper |
| **GPIO 34-39** | Input only - sem pull-up/down interno |

---

## 📊 Resumo Completo de Uso de Pinos

| GPIO | Função | Tipo | Periférico |
|------|--------|------|------------|
| 10 | E-Paper CS | Output | Display |
| 13 | Sensor Contato | Input | Porta |
| 14 | SOL_PIN2 | Output | Solenoide |
| 15 | E-Paper RST | Output | Display |
| 16 | E-Paper BUSY | Input | Display |
| 17 | RFID RST | Output | RFID |
| 18 | ECHO2 / SPI SCK | Shared | Ultrassônico / SPI |
| 19 | TRIGGER2 / SPI MISO | Shared | Ultrassônico / SPI |
| 21 | ECHO1 | Input | Ultrassônico |
| 22 | TRIGGER1 | Output | Ultrassônico |
| 23 | SPI MOSI | Output | SPI |
| **25** | **Botão LEFT** | **Input** | **Botão** ✅ |
| **26** | **Relé 2 (Água)** | **Output** | **Relé** ✅ |
| 27 | SOL_PIN1 | Output | Solenoide |
| **32** | **Botão RIGHT** | **Input** | **Botão** ✅ |
| **33** | **Relé 1 (Café)** | **Output** | **Relé** ✅ |
| 46 | RFID CS | Output | RFID |

✅ = Restaurado do teste1.cpp

---

## 🔄 Comparação: Antes vs Depois

### Pinos que MUDARAM

| Componente | Projeto Original | Teste1.cpp | **Atual (Líquidos)** |
|------------|------------------|------------|----------------------|
| Botão LEFT | GPIO 5 | GPIO 25 | **GPIO 25** ✅ |
| Botão RIGHT | GPIO 4 | GPIO 32 | **GPIO 32** ✅ |
| Relé 1 | GPIO 21 (servo) | GPIO 33 | **GPIO 33** ✅ |
| Relé 2 | GPIO 15 | GPIO 26 | **GPIO 26** ✅ |
| SOL_PIN1 | GPIO 39 | - | **GPIO 27** 🆕 |
| SOL_PIN2 | GPIO 40 | - | **GPIO 14** 🆕 |
| CONTATO | GPIO 8 | - | **GPIO 13** 🆕 |
| TRIGGER1 | GPIO 42 | - | **GPIO 22** 🆕 |
| ECHO1 | GPIO 41 | - | **GPIO 21** 🆕 |
| TRIGGER2 | GPIO 1 | - | **GPIO 19** 🆕 |
| ECHO2 | GPIO 2 | - | **GPIO 18** 🆕 |

---

## 🛠️ Conexões Físicas Recomendadas

### Módulo de Relés
```
Relé 1 (Café):
  - IN1 → GPIO 33
  - VCC → 5V
  - GND → GND

Relé 2 (Água):
  - IN2 → GPIO 26
  - VCC → 5V
  - GND → GND
```

### Botões
```
Botão LEFT (Pequeno):
  - Um lado → GPIO 25
  - Outro lado → GND
  - Pull-up interno ativado no código

Botão RIGHT (Médio):
  - Um lado → GPIO 32
  - Outro lado → GND
  - Pull-up interno ativado no código
```

### Solenoide de Travamento
```
Driver do Solenoide:
  - IN1 → GPIO 27 (SOL_PIN1)
  - IN2 → GPIO 14 (SOL_PIN2)
  - VCC → 12V (ou tensão do solenoide)
  - GND → GND comum
```

### Sensores Ultrassônicos
```
Sensor 1 (Café - HC-SR04):
  - VCC → 5V
  - TRIG → GPIO 22
  - ECHO → GPIO 21 (usar divisor de tensão 5V→3.3V)
  - GND → GND

Sensor 2 (Água - HC-SR04):
  - VCC → 5V
  - TRIG → GPIO 19
  - ECHO → GPIO 18 (usar divisor de tensão 5V→3.3V)
  - GND → GND
```

⚠️ **IMPORTANTE:** HC-SR04 opera em 5V no ECHO. Use divisor de tensão resistivo (ex: 1kΩ + 2kΩ) para reduzir para 3.3V antes de conectar ao ESP32!

### Sensor de Contato (Reed Switch ou Microswitch)
```
  - Um lado → GPIO 13
  - Outro lado → GND
  - Pull-up interno ativado no código
```

---

## 🔍 Verificação de Conflitos

### ✅ Sem Conflitos
- Todos os pinos escolhidos são seguros para uso no ESP32-WROOM
- Não há sobreposição com pinos de boot strapping críticos
- SPI compartilhado corretamente entre RFID e E-Paper

### ⚠️ Observações
1. **GPIO 18 e 19**: Compartilhados entre SPI e ultrassônicos
   - Funciona porque SPI é usado apenas na inicialização
   - Ultrassônicos são lidos no loop principal

2. **GPIO 14**: Usado tanto pelo e-paper (DC) quanto pelo SOL_PIN2
   - ⚠️ **POTENCIAL CONFLITO!** Verifique se isso causa problemas
   - Considere mover SOL_PIN2 para outro pino se houver interferência

---

## 🔧 Sugestão de Melhoria (Evitar Conflito GPIO 14)

Se houver conflito no GPIO 14, altere para:

```cpp
#define SOL_PIN2 4  // Pino livre (era do proj original)
```

Ou:

```cpp
#define SOL_PIN2 5  // Pino livre (era do proj original)
```

---

## 📝 Notas Importantes

1. **Alimentação**: Use fonte externa para relés e solenoide (5V/12V)
2. **Pull-ups**: Botões e sensor de contato usam pull-up interno
3. **Divisor de Tensão**: Obrigatório para ECHO dos HC-SR04
4. **GND Comum**: Todos os grounds devem estar conectados
5. **Testagem**: Teste cada periférico individualmente antes de integrar

---

## 📄 Arquivo Atualizado

✅ **[src/proj_iot_liquidos.cpp](src/proj_iot_liquidos.cpp#L13-L32)** - Definições de pinos atualizadas
