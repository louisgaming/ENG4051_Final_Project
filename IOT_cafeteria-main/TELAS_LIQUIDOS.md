# 🖥️ Interface de Tela - Sistema de Líquidos

## Layout das Novas Telas Implementadas

---

### 1️⃣ Tela: Seleção de Tamanho
**Função:** `telaSelecionaTamanho(String nomeProduto)`
**Quando aparece:** Após o usuário selecionar Café ou Chocolate

```
┌─────────────────────────────────────┐
│                                     │
│          Café                       │  ← Nome do produto (fonte grande)
│                                     │
│     Escolha o tamanho:              │  ← Subtítulo
│                                     │
│                                     │
│   PEQUENO            MEDIO          │  ← Opções em negrito
│   3 seg              5 seg          │  ← Tempo de dispensação
│                                     │
└─────────────────────────────────────┘
   [BTN LEFT]          [BTN RIGHT]        ← Botões físicos
```

**Layout Visual:**
- **Título**: Fonte Helvetica Bold 18pt, centralizado
- **Subtítulo**: Fonte Helvetica Bold 12pt
- **Opções**:
  - PEQUENO (esquerda, posição 20px)
  - MÉDIO (direita, posição 200px)
- **Tempos**: Fonte Helvetica Regular 10pt

**Interação:**
- Botão LEFT → Seleciona PEQUENO (3s)
- Botão RIGHT → Seleciona MÉDIO (5s)

---

### 2️⃣ Tela: Dispensando Líquido
**Função:** `telaDispensando(String produtoTamanho)`
**Quando aparece:** Durante a dispensação do líquido (3s ou 5s)

```
┌─────────────────────────────────────┐
│                                     │
│        AGUARDE                      │  ← Mensagem em destaque (24pt)
│                                     │
│                                     │
│      Café Pequeno                   │  ← Produto + Tamanho (14pt)
│                                     │
│                                     │
│      Dispensando...                 │  ← Status (12pt)
│                                     │
└─────────────────────────────────────┘
```

**Layout Visual:**
- **AGUARDE**: Fonte Helvetica Bold 24pt, posição central-superior
- **Produto+Tamanho**: Fonte Helvetica Bold 14pt, centralizado
- **Status**: Fonte Helvetica Regular 12pt, parte inferior

**Duração:**
- Pequeno: Exibida por 3 segundos
- Médio: Exibida por 5 segundos
- Após término → Transição automática para tela de saldo final

---

## Fluxo Completo de Telas

```
┌──────────────┐
│ Tela Inicial │ ← Aguardando RFID
└──────┬───────┘
       │ (Aproxima cartão)
       ↓
┌──────────────────┐
│ Tela Produtos    │ ← Mostra Café e Chocolate
└──────┬───────────┘
       │ (Seleciona produto com botão)
       ↓
┌─────────────────────────┐
│ Tela Seleção Tamanho    │ ← NOVA TELA (Pequeno/Médio)
└──────┬──────────────────┘
       │ (Seleciona tamanho com botão)
       ↓
┌─────────────────────────┐
│ Tela Dispensando        │ ← NOVA TELA (3s ou 5s)
└──────┬──────────────────┘
       │ (Aguarda término da dispensação)
       ↓
┌──────────────────┐
│ Tela Saldo Final │ ← Mostra valor cobrado
└──────┬───────────┘
       │ (Após 5 segundos)
       ↓
┌──────────────┐
│ Tela Inicial │ ← Volta ao início
└──────────────┘
```

---

## Exemplos de Uso

### Exemplo 1: Café Pequeno
```
1. Usuário aproxima RFID
   → Tela mostra: "Luizinho: R$29.99"

2. Pressiona botão LEFT (Café)
   → Tela mostra: "Café - Escolha o tamanho"

3. Pressiona botão LEFT (Pequeno)
   → Tela mostra: "AGUARDE - Café Pequeno - Dispensando..."
   → Dispensa por 3 segundos

4. Após 3 segundos
   → Tela mostra: "Saldo atual: R$27.49" (cobrou R$2.50)
```

### Exemplo 2: Chocolate Médio
```
1. Usuário aproxima RFID
   → Tela mostra: "Luizinho: R$29.99"

2. Pressiona botão RIGHT (Chocolate)
   → Tela mostra: "Chocolate Quente - Escolha o tamanho"

3. Pressiona botão RIGHT (Médio)
   → Tela mostra: "AGUARDE - Chocolate Quente Médio - Dispensando..."
   → Dispensa por 5 segundos

4. Após 5 segundos
   → Tela mostra: "Saldo atual: R$25.49" (cobrou R$4.50)
```

---

## Especificações Técnicas

### Display Usado
- **Modelo**: GxEPD2_290_T94_V2 (e-paper 2.9")
- **Resolução**: 296x128 pixels
- **Rotação**: 3 (landscape)
- **Cores**: Preto e Branco

### Fontes Utilizadas
- `u8g2_font_helvB24_te` - Helvetica Bold 24pt (títulos principais)
- `u8g2_font_helvB18_te` - Helvetica Bold 18pt (nome do produto)
- `u8g2_font_helvB14_te` - Helvetica Bold 14pt (opções, subtítulos)
- `u8g2_font_helvB12_te` - Helvetica Bold 12pt (labels)
- `u8g2_font_helvR12_te` - Helvetica Regular 12pt (status)
- `u8g2_font_helvR10_te` - Helvetica Regular 10pt (detalhes)

### Posicionamento
- **Esquerda (LEFT)**: x = 20-30px
- **Centro**: x = 60-120px
- **Direita (RIGHT)**: x = 200-210px
- **Altura do título**: y = 30-40px
- **Altura do conteúdo**: y = 55-85px
- **Altura do rodapé**: y = 105-110px

---

## Arquivos Modificados

✅ **[src/tela_iot.cpp](src/tela_iot.cpp#L176-L238)** - Implementação das funções
✅ **[src/tela.h](src/tela.h#L14-L15)** - Declaração das funções

---

## Teste Visual Sugerido

Para verificar se as telas estão corretas:

1. Compile e faça upload do código
2. Aproxime um cartão RFID válido
3. Verifique se a tela de produtos aparece
4. Pressione botão LEFT ou RIGHT
5. **VERIFIQUE**: Aparece a tela com "Escolha o tamanho"?
6. Pressione botão LEFT (Pequeno) ou RIGHT (Médio)
7. **VERIFIQUE**: Aparece "AGUARDE - Dispensando..."?
8. **VERIFIQUE**: Relé liga por 3s (pequeno) ou 5s (médio)?
9. **VERIFIQUE**: Após dispensação, aparece o saldo atualizado?

---

## Notas de Design

- ✅ Layout limpo e legível
- ✅ Fontes grandes para boa visualização
- ✅ Hierarquia visual clara (título → opções → detalhes)
- ✅ Mensagens objetivas e diretas
- ✅ Feedback visual durante toda a operação
- ✅ Compatível com display e-paper (apenas preto/branco)
