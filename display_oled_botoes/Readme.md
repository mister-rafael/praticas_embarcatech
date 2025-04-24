# Sistema de Controle de Acesso com Raspberry Pi Pico

### 1. Escopo do Projeto
### 1.1. Título do Projeto
Sistema de Controle de Acesso com Display OLED e LED RGB
### 1.2. Objetivos do Projeto
Este projeto tem como objetivo desenvolver um sistema de controle de acesso utilizando a BitDogLab, que integra um Raspberry Pi Pico, um display OLED SSD1306, LEDs RGB e botões. O sistema solicita ao usuário que digite uma senha de quatro dígitos e forneçe feedback visual indicando se a senha está correta ou não.
### 1.3. Descrição do Funcionamento
- O sistema exibe uma mensagem no display OLED, solicitando a senha.
- O usuário insere os dígitos utilizando os botões A (para selecionar) e o B (para confirmar).
- Ao completar a digitação, a senha é validada.
- Se a senha estiver correta, um LED verde acende e uma mensagem de "Acesso Permitido" é exibida no display OLED.
- Se a senha estiver incorreta, um LED vermelho acende e a mensagem "Acesso Negado" é exibida no display OLED.
- O sistema reinicia para uma nova tentativa após alguns segundos.
### 1.4. Justificativa
O projeto justifica-se pela necessidade de um sistema simples, específico e eficiente de controle de acesso, podendo ser utilizado para aprendizado em eletrônica embarcada, segurança residencial ou como parte de um sistema mais complexo.

### 2. Especificação do Hardware
### 2.1. Descrição dos Componentes Eletrônicos
- **Raspberry Pi Pico**: Microcontrolador responsável pelo processamento.
- **Display OLED SSD1306**: Utilizado para exibir mensagens e interface com o usuário.
- **Botões (2 unidades)**: Usados para inserir a senha.
- **LED RGB**: Fornece feedback visual sobre o status da senha.
- **Resistores (internos) de pull-up**: Utilizados para garantir estabilidade nas leituras dos botões.
### 2.2. Descrição da Pinagem Usada

### Componentes          |   Pino GPIO Utilizado 

Display OLED SDA     | 14
Display OLED SCL     | 15
Botão A              | 5
Botão B              | 6
LED Azul             | 12
LED Vermelho         | 13
LED Verde            | 11

### 3. Especificação do Firmware
### 3.1. Fluxograma
Inicio
  │
  ▼
Inicializa Sistema
  │
  ├──► Inicializa Display OLED
  │
  ├──► Configura Botões e LEDs
  │
  ├──► Exibe Mensagem no Display
  │
  ▼
Loop Principal
  │
  ├──► Usuário Digita Senha?
  │         │
  │         ├──► Sim → Atualiza Display com Dígitos
  │         │
  │         ├──► Não → Continua esperando entrada
  │
  ▼
Senha Completa?
  │
  ├──► Sim → Verifica Senha
  │         │
  │         ├──► Correta? 
  │         │         │
  │         │         ├──► Sim → Exibe "Acesso Permitido" 
  │         │         │         │
  │         │         │         ├──► Acende LED Verde
  │         │         │
  │         │         ├──► Não → Exibe "Acesso Negado"
  │         │                   │
  │         │                   ├──► Acende LED Vermelho
  │
  ▼
Aguarda 2 Segundos
  │
  ├──► Reinicia Variáveis
  │
  ├──► Retorna para Loop Principal
  │
  ▼
Fim

### 4. Execução do Projeto
### 4.1. Metodologia
- **Pesquisa e Planejamento**: Escolha dos componentes eletrônicos e estrutura do código.
- **Desenvolvimento do Hardware**: Identificação dos componentes conforme a tabela de pinagem.
- **Desenvolvimento do Firmware**:
    - Configuração do OLED e interface I2C.
    - Implementação da interação via botões.
    - Lógica para validar a senha e controlar os LEDs.
- **Testes**: Testes funcionais de leitura dos botões, exibição no OLED e resposta dos LEDs.
### 4.2. Discussão dos Resultados
O sistema funcionou conforme o esperado, garantindo uma resposta rápida à entrada do usuário. O uso do display OLED facilitou a interação e a indicação visual com LEDs proporcionou feedback intuitivo. Possíveis melhorias incluem a implementação de um sistema de bloqueio após tentativas mal sucedidas e sinal sonoro usando os buzzers.
### 5. Referências
Documentação oficial do Raspberry Pi Pico.
Datasheet do display OLED SSD1306.
Tutoriais e guias sobre GPIO e I2C no Raspberry Pi Pico, no canal do professor Jvagio. Disponível em: https://www.youtube.com/@profjivago9719