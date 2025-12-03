# 🔧 Guia Rápido de Montagem - Sistema IoT Cafeteria

## 📍 Configuração Final de Pinos - ESP32-WROOM-32

### 🔵 Interface do Usuário (Teste1.cpp - PCB)
```
GPIO 25 → Botão LEFT  (Seleciona Café / Tamanho Pequeno)
GPIO 32 → Botão RIGHT (Seleciona Água / Tamanho Médio)
```
**Conexão:** Botão entre GPIO e GND (INPUT_PULLUP ativado no código)

---

### 🔴 Controle de Válvulas (Teste1.cpp - PCB)
```
GPIO 33 → Relé 1 (Válvula Café)
GPIO 26 → Relé 2 (Válvula Água)
```
**Conexão:**
- IN1 do módulo relé → GPIO 33
- IN2 do módulo relé → GPIO 26
- VCC relé → 5V (fonte externa)
- GND relé → GND comum

---

### 🟢 Sistema de Travamento da Porta
```
GPIO 27 → Solenoide PIN1 (Fase 1)
GPIO 12 → Solenoide PIN2 (Fase 2) ⚠️ Requer pull-down 10kΩ
GPIO 13 → Sensor de Contato (Reed switch / Microswitch)
```
**Conexão Solenoide:**
- Driver H-Bridge:
  - IN1 → GPIO 27
  - IN2 → GPIO 12
  - VCC → 12V (fonte externa)
  - GND → GND comum
  - OUT1/OUT2 → Solenoide 12V

**⚠️ IMPORTANTE:** GPIO 12 é boot strapping pin!
```
GPIO 12 ──[10kΩ]── GND
```

---

### 🟡 Sensores de Estoque (Ultrassônicos HC-SR04)

**Sensor 1 - Café:**
```
GPIO 22 → TRIGGER1
GPIO 21 → ECHO1 ⚠️ Requer divisor de tensão!
```

**Sensor 2 - Água:**
```
GPIO 19 → TRIGGER2 (Compartilhado com SPI MISO)
GPIO 18 → ECHO2    (Compartilhado com SPI SCK)
```

**⚠️ DIVISOR DE TENSÃO OBRIGATÓRIO (ECHO):**
```
HC-SR04 ECHO (5V) ──[1kΩ]──┬──[2kΩ]── GND
                            │
                            └──→ ESP32 GPIO (3.3V)
```

**Conexão Completa:**
```
HC-SR04:
  VCC  → 5V
  TRIG → GPIO (22 ou 19)
  ECHO → [Divisor] → GPIO (21 ou 18)
  GND  → GND
```

---

### 🟣 RFID RC522 (SPI)
```
GPIO 46 → CS/SDA
GPIO 17 → RST
GPIO 18 → SCK  (SPI compartilhado)
GPIO 19 → MISO (SPI compartilhado)
GPIO 23 → MOSI (SPI compartilhado)
```

**Conexão:**
```
RC522:
  SDA  → GPIO 46
  SCK  → GPIO 18
  MOSI → GPIO 23
  MISO → GPIO 19
  IRQ  → (não conectar)
  GND  → GND
  RST  → GPIO 17
  3.3V → 3.3V ⚠️ NÃO use 5V!
```

---

### 🟠 E-Paper Display 2.9" Waveshare (SPI)
```
GPIO 5  → CS   ✅ CORRIGIDO (era GPIO 10)
GPIO 14 → DC
GPIO 15 → RST
GPIO 16 → BUSY (INPUT)
GPIO 18 → CLK  (SPI compartilhado)
GPIO 23 → DIN  (SPI compartilhado)
```

**Conexão:**
```
E-Paper HAT:
  BUSY → GPIO 16
  RST  → GPIO 15
  DC   → GPIO 14
  CS   → GPIO 5  ✅
  CLK  → GPIO 18
  DIN  → GPIO 23
  GND  → GND
  VCC  → 3.3V ⚠️ NÃO use 5V!
```

---

## ⚠️ Pinos Compartilhados (OK - Uso Temporal)

| GPIO | Função 1 | Função 2 | Conflito? |
|------|----------|----------|-----------|
| **18** | SPI SCK | ECHO2 | ❌ Não - SPI só na inicialização |
| **19** | SPI MISO | TRIGGER2 | ❌ Não - SPI só na inicialização |
| **23** | SPI MOSI | - | ❌ Não |

**Ordem de uso:**
1. `setup()`: SPI inicializa RFID e e-Paper
2. `loop()`: SPI não é mais usado, GPIO 18/19 viram ultrassônico

---

## 🔌 Requisitos de Alimentação

| Tensão | Consumo Estimado | Componentes |
|--------|------------------|-------------|
| **3.3V** | ~500mA | ESP32, RFID, E-Paper |
| **5V** | ~300mA | Módulo Relé, HC-SR04 |
| **12V** | ~2A (picos) | Válvulas Solenoides, Solenoide Travamento |

**Fontes Recomendadas:**
- 1x Fonte 5V/2A (ESP32 + periféricos 5V)
- 1x Fonte 12V/2A (válvulas + travamento)
- **GND comum entre todas as fontes!**

---

## 📋 Checklist de Montagem

### Antes de Ligar:
- [ ] Todos os GNDs estão conectados (comum)
- [ ] ESP32 recebe 3.3V (via regulador ou USB)
- [ ] RFID e E-Paper em 3.3V (NÃO 5V!)
- [ ] HC-SR04 em 5V com divisores de tensão nos ECHO
- [ ] Relés alimentados por 5V externo (não GPIO)
- [ ] Solenoide com driver H-Bridge (não direto no GPIO)
- [ ] GPIO 12 tem pull-down de 10kΩ
- [ ] Botões configurados para GND (pull-up interno)

### Primeira Inicialização:
- [ ] Monitor Serial aberto (115200 baud)
- [ ] Observar boot do ESP32 (não deve travar)
- [ ] Verificar conexão WiFi
- [ ] Testar cada botão individualmente
- [ ] Testar cada relé individualmente (sem líquido)
- [ ] Testar RFID com cartão
- [ ] Verificar display e-Paper atualiza

### Testes de Sistema:
- [ ] Fluxo completo: RFID → Selecionar Produto → Selecionar Tamanho
- [ ] Dispensar Café (Pequeno 3s / Médio 5s)
- [ ] Dispensar Água (Pequeno 3s / Médio 5s)
- [ ] Sensores ultrassônicos lendo nível
- [ ] Sistema de travamento funcionando
- [ ] MQTT enviando dados corretamente

---

## 🚨 Avisos Críticos

### ❌ NÃO FAÇA:
1. **NÃO conecte 5V no RFID ou E-Paper** → Queima!
2. **NÃO conecte HC-SR04 ECHO direto** → Queima ESP32!
3. **NÃO alimente relés/válvulas pelo ESP32** → Sobrecarga!
4. **NÃO use GPIOs 6-11** → Flash interna!
5. **NÃO esqueça pull-down no GPIO 12** → Boot failure!

### ✅ SEMPRE:
1. Divisores de tensão nos ECHO (1kΩ + 2kΩ)
2. GND comum entre todas as fontes
3. Fontes externas para cargas pesadas (relés, válvulas)
4. Testar cada módulo individualmente antes de integrar
5. Usar fonte adequada (não só USB)

---

## 🔧 Ferramentas Necessárias

- Multímetro (verificar tensões)
- Soldador (caso use PCB)
- Alicate de corte/decapador
- Protoboard (para testes)
- Jumpers macho-fêmea
- Resistores: 4x 1kΩ, 4x 2kΩ, 1x 10kΩ

---

## 📊 Resumo Visual

```
ESP32-WROOM-32 (3.3V)
    │
    ├─ GPIO 25, 32 ──→ Botões (GND)
    ├─ GPIO 33, 26 ──→ Relés (5V ext)
    ├─ GPIO 27, 12 ──→ H-Bridge (12V ext)
    ├─ GPIO 13 ─────→ Sensor Porta
    │
    ├─ SPI (18,19,23) ─┬─→ RFID (GPIO 46 CS, 17 RST)
    │                   └─→ E-Paper (GPIO 5 CS, 14 DC, 15 RST, 16 BUSY)
    │
    └─ GPIO 22,21,19,18 ─→ Ultrassônicos (com divisor!)
```

---

## 📄 Arquivos Relacionados

- [DIAGRAMA_COMPLETO_CONEXOES.txt](DIAGRAMA_COMPLETO_CONEXOES.txt) - Diagrama completo ASCII
- [CORRECAO_GPIO10_EPAPER.md](CORRECAO_GPIO10_EPAPER.md) - Por que GPIO 5 no e-Paper
- [CORRECAO_GPIO4_ADC2.md](CORRECAO_GPIO4_ADC2.md) - Conflito ADC2 com WiFi
- [VALIDACAO_PINOS_BOOT.md](VALIDACAO_PINOS_BOOT.md) - Validação de boot strapping
- [MAPEAMENTO_PINOS_LIQUIDOS.md](MAPEAMENTO_PINOS_LIQUIDOS.md) - Mapeamento detalhado

---

## 🎯 Próximos Passos

1. Montar circuito em protoboard
2. Fazer upload do código [src/proj_iot_liquidos.cpp](src/proj_iot_liquidos.cpp)
3. Testar módulo por módulo
4. Calibrar tempos de dispensação
5. Ajustar limiares dos ultrassônicos
6. Testar comunicação MQTT
7. Passar para PCB final

---

**Versão:** 1.0
**Data:** 2025
**Status:** ✅ Pronto para montagem
**Arquivos validados:** Todos os conflitos de pinos resolvidos
