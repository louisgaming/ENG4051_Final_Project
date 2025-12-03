# ⚠️ CORREÇÃO CRÍTICA: GPIO 10 não acessível no ESP32-WROOM-32

## 🚨 Problema Identificado

**GPIO 10 NÃO ESTÁ DISPONÍVEL nos pinos externos do ESP32-WROOM-32!**

### Por quê?

Os GPIOs 6-11 estão **internamente conectados à flash SPI** do módulo ESP32-WROOM-32 e não são expostos nos pinos físicos:

| GPIO | Função Flash | Acessível? |
|------|--------------|------------|
| GPIO 6 | SD_CLK | ❌ NÃO |
| GPIO 7 | SD_DATA0 | ❌ NÃO |
| GPIO 8 | SD_DATA1 | ❌ NÃO |
| GPIO 9 | SD_DATA2 | ❌ NÃO |
| **GPIO 10** | **SD_DATA3** | ❌ **NÃO** |
| GPIO 11 | SD_CMD | ❌ NÃO |

**O código estava usando GPIO 10 como CS do e-Paper, mas esse pino não existe fisicamente no módulo!**

---

## ✅ Solução Aplicada

### Mudança Realizada:

**Antes (INCORRETO):**
```cpp
GxEPD2_290_T94_V2 modeloTela(10, 14, 15, 16);
                              ↑
                           GPIO 10 ❌ Não existe!
```

**Depois (CORRETO):**
```cpp
GxEPD2_290_T94_V2 modeloTela(5, 14, 15, 16);  // CS, DC, RST, BUSY
                             ↑
                          GPIO 5 ✅ Acessível e livre!
```

---

## 📊 Nova Configuração do e-Paper

### Pinos Atualizados:

| Pino e-Paper | GPIO Antigo | **GPIO Novo** | Status |
|--------------|-------------|---------------|--------|
| CS | GPIO 10 ❌ | **GPIO 5** ✅ | CORRIGIDO |
| DC | GPIO 14 | GPIO 14 | Mantido |
| RST | GPIO 15 | GPIO 15 | Mantido |
| BUSY | GPIO 16 | GPIO 16 | Mantido |
| CLK | GPIO 18 | GPIO 18 | Mantido |
| DIN | GPIO 23 | GPIO 23 | Mantido |

---

## 🔍 Por que GPIO 5?

### Análise do GPIO 5:

| Característica | Status |
|----------------|--------|
| Acessível externamente? | ✅ SIM |
| Conflita com WiFi (ADC2)? | ❌ NÃO |
| Boot strapping pin? | ⚠️ SIM (VSPI CS) |
| Funciona como CS? | ✅ SIM |
| Usado por outro periférico? | ✅ **SIM - SOL_PIN2** |

### ⚠️ CONFLITO DETECTADO!

**GPIO 5 estava sendo usado como SOL_PIN2 (solenoide)!**

Precisamos mover o SOL_PIN2 para outro pino livre.

---

## 🔄 Ajustes em Cascata

Como GPIO 5 agora é do e-Paper CS, precisamos mover SOL_PIN2:

### Opções de pinos livres:

| GPIO | Disponível? | Observação |
|------|-------------|------------|
| GPIO 2 | ⚠️ | Boot strapping, evitar |
| GPIO 4 | ❌ | ADC2 - não funciona com WiFi |
| GPIO 12 | ⚠️ | Boot voltage |
| GPIO 35 | ✅ | Input only (não serve para OUTPUT) |
| GPIO 36-39 | ✅ | Input only (não serve para OUTPUT) |

### Melhor opção: **GPIO 12**

Apesar de ser boot strapping, funciona bem após o boot para saída digital.

---

## ✅ Configuração Final Corrigida

### Pinos do e-Paper:

```cpp
// tela_iot.cpp
GxEPD2_290_T94_V2 modeloTela(5, 14, 15, 16);
                             │  │   │   │
                             │  │   │   └─ BUSY  (GPIO 16)
                             │  │   └───── RST   (GPIO 15)
                             │  └───────── DC    (GPIO 14)
                             └──────────── CS    (GPIO 5) ✅ CORRIGIDO
```

### Pinos do Solenoide:

```cpp
// proj_iot_liquidos.cpp
#define SOL_PIN1 27  // ✅ Mantido
#define SOL_PIN2 12  // ✅ Movido de GPIO 5 para GPIO 12
```

---

## 📝 Atualização Necessária

Preciso atualizar o arquivo `proj_iot_liquidos.cpp`:

```cpp
// ANTES
#define SOL_PIN2 5   // ❌ Conflito com e-Paper CS

// DEPOIS
#define SOL_PIN2 12  // ✅ Pino livre (boot strapping, mas funciona)
```

---

## ⚠️ Atenção com GPIO 12

**GPIO 12 é boot strapping pin** (MTDI):
- Deve estar **LOW durante o boot**
- Se estiver HIGH, o ESP32 tenta bootar em modo de 1.8V flash
- **Solução:** Adicionar pull-down de 10kΩ no GPIO 12

---

## 🔌 Conexões Finais do e-Paper

```
e-Paper HAT         ESP32-WROOM-32
┌──────────┐        ┌──────────┐
│ BUSY     │───────>│ GPIO 16  │
│ RST      │<───────│ GPIO 15  │
│ DC       │<───────│ GPIO 14  │
│ CS       │<───────│ GPIO 5   │ ✅ NOVO!
│ CLK      │<───────│ GPIO 18  │ (SPI compartilhado)
│ DIN      │<───────│ GPIO 23  │ (SPI compartilhado)
│ GND      │────────│ GND      │
│ VCC      │<───────│ 3.3V     │
└──────────┘        └──────────┘
```

---

## 📋 Checklist de Atualização

- [x] Atualizar `tela_iot.cpp`: GPIO 10 → GPIO 5
- [ ] Atualizar `proj_iot_liquidos.cpp`: SOL_PIN2 de GPIO 5 → GPIO 12
- [ ] Atualizar documentação de pinos
- [ ] Adicionar pull-down 10kΩ no GPIO 12 (hardware)
- [ ] Testar e-Paper com novo CS (GPIO 5)
- [ ] Testar solenoide com novo pino (GPIO 12)

---

## 🎯 Resumo da Correção

| Componente | Pino Antigo | **Pino Novo** | Motivo |
|------------|-------------|---------------|--------|
| **e-Paper CS** | GPIO 10 ❌ | **GPIO 5** ✅ | GPIO 10 não acessível |
| **SOL_PIN2** | GPIO 5 | **GPIO 12** ✅ | Conflito com e-Paper CS |

---

## 📚 Referências

- [ESP32-WROOM-32 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_en.pdf) - Página 8: Pinout
- GPIOs 6-11 são internos e não estão no pinout externo

---

Data da correção: 2025
Status: ✅ GPIO 10 → GPIO 5 (e-Paper CS)
Pendente: ⚠️ Atualizar SOL_PIN2: GPIO 5 → GPIO 12
