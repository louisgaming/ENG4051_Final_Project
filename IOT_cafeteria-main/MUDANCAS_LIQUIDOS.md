# 🔧 Alterações do Projeto - Dispensador de Líquidos

## 📁 Arquivos Criados

1. **[src/proj_iot_liquidos.cpp](src/proj_iot_liquidos.cpp)** - Código principal do sistema de líquidos
2. **[funcoes_tela_liquidos.txt](funcoes_tela_liquidos.txt)** - Documentação das funções de tela necessárias

---

## 🔌 Alterações de Pinos (Restaurado do teste1.cpp)

| Componente | Protótipo (teste1.cpp) | **Projeto Final (Líquidos)** | Status |
|------------|------------------------|------------------------------|--------|
| **Botão LEFT** | GPIO 25 | **GPIO 25** | ✅ Mantido |
| **Botão RIGHT** | GPIO 32 | **GPIO 32** | ✅ Mantido |
| **Relé 1 (Café)** | GPIO 33 | **GPIO 33** | ✅ Mantido |
| **Relé 2 (Água)** | GPIO 26 | **GPIO 26** | ✅ Mantido |

### Pinos Novos (não existiam no teste1.cpp):
- **SOL_PIN1**: GPIO 27 (Solenoide de travamento - fase 1)
- **SOL_PIN2**: GPIO 4 (Solenoide de travamento - fase 2)
- **CONTATO**: GPIO 13 (Sensor de porta)
- **Ultrassônicos**:
  - Trigger 1: GPIO 22, Echo 1: GPIO 21 (Reservatório Café)
  - Trigger 2: GPIO 19, Echo 2: GPIO 18 (Reservatório Água)
- **RFID**: CS=GPIO 46, RST=GPIO 17 (fixo)
- **E-Paper**: CS=GPIO 10, DC=GPIO 14, RST=GPIO 15, BUSY=GPIO 16 (fixo)

---

## 🔄 Fluxo de Operação

```
1. AGUARDANDO_RFID
   ↓ (Usuário aproxima cartão)

2. SELECIONANDO_PRODUTO
   ↓ (Pressiona botão LEFT=Café ou RIGHT=Chocolate)

3. SELECIONANDO_TAMANHO  ← NOVA ETAPA
   ↓ (Pressiona botão LEFT=Pequeno ou RIGHT=Médio)

4. DISPENSANDO
   ↓ (Aguarda tempo de dispensação)

5. Finaliza compra e volta ao estado 1
```

---

## ⏱️ Tempos de Dispensação

| Tamanho | Tempo | Aplicação |
|---------|-------|-----------|
| **Pequeno** | 3 segundos | Ambos os produtos |
| **Médio** | 5 segundos | Ambos os produtos |

---

## 💰 Tabela de Preços

| Produto | Pequeno (3s) | Médio (5s) |
|---------|--------------|------------|
| **Café** | R$ 2,50 | R$ 3,50 |
| **Chocolate Quente** | R$ 3,00 | R$ 4,50 |

---

## 🎮 Controle dos Botões

### Estado: SELECIONANDO_PRODUTO
- **Botão LEFT (5)**: Seleciona Café
- **Botão RIGHT (4)**: Seleciona Chocolate Quente

### Estado: SELECIONANDO_TAMANHO
- **Botão LEFT (5)**: Seleciona Pequeno (3s)
- **Botão RIGHT (4)**: Seleciona Médio (5s)

---

## 📊 Variáveis de Estado

### Enum Estado
```cpp
enum Estado {
  AGUARDANDO_RFID,      // Aguardando leitura do cartão
  SELECIONANDO_PRODUTO, // Escolhendo Café ou Chocolate
  SELECIONANDO_TAMANHO, // Escolhendo Pequeno ou Médio
  DISPENSANDO           // Liberando líquido
};
```

### Variáveis de Controle
- `estadoAtual`: Estado atual da máquina de estados
- `tamanhoSelecionado`: 1=Pequeno, 2=Médio
- `releAtivo`: Indica se há dispensação em andamento
- `pinoReleAtual`: Qual relé está ativo (21 ou 15)
- `tempoDesligarRele`: Momento em que deve desligar

---

## 🖥️ Funções de Tela

### ✅ Já Existentes (do projeto original)
- `telaSetup()`
- `telaInicial()`
- `telaProdutos(nome, saldo)`
- `telaSaldoNegativo(saldo)`
- `telaSemEstoque()`
- `telaSaldoFinal(total, saldo)`
- `usuarioInvalido()`

### ✅ NOVAS - Implementadas
1. **`telaSelecionaTamanho(String nomeProduto)`** ✓
   - Exibe opções Pequeno/Médio após escolher o produto
   - Mostra tempo de dispensação (3s ou 5s)
   - Layout: PEQUENO (esquerda) | MÉDIO (direita)

2. **`telaDispensando(String produtoTamanho)`** ✓
   - Mostra "AGUARDE" enquanto dispensa
   - Exibe o produto e tamanho sendo preparado
   - Mensagem "Dispensando..."

> 📝 Implementadas em [tela_iot.cpp:176-238](src/tela_iot.cpp#L176-L238)

---

## 📡 Mensagens MQTT

### Publicadas
- `verifica_usuario_cafe`: Valida RFID do usuário
- `pega_produtos_cafe`: Solicita lista de produtos
- `estoque_baixo_cafe`: Alerta de estoque baixo
- `cafeteria_iot`: Registra venda finalizada

### JSON da Venda (publicado em `cafeteria_iot`)
```json
{
  "id_user": 1,
  "id_produto": 1,
  "tamanho": "pequeno",  // ou "medio"
  "total": 2.50,
  "saldoAtual": 27.49
}
```

### Subscritas
- `cafeteria_iot`: Comandos remotos
- `verifica_usuario_cafe`: (debug)
- `retorna_usuario_cafe`: Resposta com dados do usuário
- `retorna_produtos_cafe`: Lista de produtos

---

## 🔧 Hardware Necessário

### Componentes Principais
- ESP32
- 2× Relés (para válvulas solenoides)
- 2× Válvulas solenoides (líquidos)
- RFID MFRC522
- 2× Sensores ultrassônicos (nível dos reservatórios)
- Display (conforme implementação em tela.h)
- 2× Botões
- Solenoide de travamento + sensor de contato

### Alimentação
- Relés: Conforme especificação das válvulas
- Válvulas: Verificar tensão nominal (geralmente 12V)

---

## ⚡ Principais Mudanças do Código

### Do Protótipo (teste1.cpp)
- ✅ Controle por tempo fixo mantido
- ✅ Lógica de relés com desligamento automático
- ➕ Adicionado sistema de estados
- ➕ Adicionado seleção de tamanho
- ➕ Integrado com RFID/MQTT/Display

### Do Projeto Original (proj_iot.cpp)
- ❌ Removido servo motor
- ❌ Removido balança HX711
- ❌ Removido controle por pressão de botão
- ➕ Adicionado relés para válvulas
- ➕ Adicionado dois tamanhos de dispensação
- ➕ Mudado de preço por peso para preço fixo

---

## 🚀 Próximos Passos

1. ✅ Código principal criado
2. ✅ Funções de tela implementadas (`telaSelecionaTamanho` e `telaDispensando`)
3. ⚠️ Testar calibração dos tempos de dispensação (3s e 5s)
4. ⚠️ Ajustar limites dos sensores ultrassônicos para líquidos
5. ⚠️ Validar comunicação MQTT com backend
6. ⚠️ Testar fluxo completo: RFID → Produto → Tamanho → Dispensação → Cobrança

---

## 📝 Notas Importantes

- Os tempos de 3s e 5s são **estimativas** - calibre conforme a vazão real das válvulas
- Os preços podem ser ajustados no backend via MQTT (função `pegaProdutos()`)
- O código mantém compatibilidade com a infraestrutura existente (WiFi, MQTT, RFID)
- Os sensores ultrassônicos agora monitoram **nível de líquido** ao invés de sólidos
