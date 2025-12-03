# ENG4051_Final_Project
Assets supporting the development of the final project for ENG4051 – Projeto Internet das Coisas (IoT Project), instructed by Professor Jan Krueger Siqueira, Department of Electrical Engineering, PUC-Rio.

Solids Dispenser's 3D models can be found here: https://cad.onshape.com/documents/5a55839f175b594ffbd2dcaa/w/03925e9c78100ab4ed96ae5d/e/1c131eb948d5f204225ffffa?renderMode=0&uiState=693086d84b71e3c8a5ab772c  

# The Idea
This project implements a solution for snacks and beverages dispensers, where an RFID Reader (RC522 module) indentifies a previously registered user for automatic balance deduction. The user can then view the quantity — measured by a scale — and the corresponding price of the order on an e-paper display. Two single product dispensers where implemented but the architecture supports further expansion. All system dynamics are managed by an ESP32-3S, with MQTT handling message transmission, Beekeeper Studio providing data storage, and Node-RED serving as the intermediary platform.  

# MQTT

In this project, MQTT is used as the communication layer between the ESP32 and the backend.  
The ESP32 connects to the broker `mqtt.janks.dev.br` over TLS and exchanges JSON messages on a few fixed topics:

- `verifica_usuario_cafe`: sent by the ESP32 when an RFID card is read, with the payload `{ "rfid": "<card_id>" }`, asking the backend to validate the user.  
- `retorna_usuario_cafe`: response from the backend with the user object (id, name, balance) or `{ "id": 0 }` if the RFID is not registered.  
- `pega_produtos_cafe`: request sent at startup so the backend can return the current product catalog.  
- `retorna_produtos_cafe`: list of products (id, name, price per 100 g) used by the ESP32 to show the menu and calculate the price during weighing.  
- `cafeteria_iot`: used by the ESP32 to send the finished purchase (`id_user`, `id_produto`, `peso`, `total`, `saldoAtual`) to the backend.  
- `estoque_baixo_cafe`: sent when the ultrasonic sensors detect low stock, with `{ "produto": 1 }` for peanuts or `{ "produto": 2 }` for M&M’s.  
- `novo_rfid`: used in admin mode to send a new RFID card to be registered on the web interface.

All system events (RFID read, product selection, purchase confirmation and stock alerts) are therefore propagated to the backend through these MQTT channels.


# Node-RED

Node-RED acts as the integration layer between MQTT, the PostgreSQL database and external services.  
Each MQTT topic is attached to a flow:

- Messages on `verifica_usuario_cafe` are passed as SQL parameters to a query:  
  `SELECT * FROM users_cafeteria WHERE rfid = $rfid;`  
  The result is returned to the device through `retorna_usuario_cafe`.

- Messages on `pega_produtos_cafe` trigger a query:  
  `SELECT * FROM produtos_cafeteria;`  
  The resulting product list is published on `retorna_produtos_cafe`.

- Purchases received on `cafeteria_iot` are stored in the `transacoes_cafeteria` table and the user’s balance is updated in `users_cafeteria`.

- Low-stock alerts on `estoque_baixo_cafe` are converted into text notifications and forwarded through a Telegram bot.

- New RFIDs sent on `novo_rfid` are forwarded via WebSocket to a web page, where they can be associated with a user registration flow.

With this setup, Node-RED centralizes the business logic, while the ESP32 focuses on hardware control and message transmission.

# Data Base

# Grafana

Grafana bisualizations of important data can be found here:

- link to shared grafana (breaks because grafana doesn't support variables): https://gustavogm001.grafana.net/public-dashboards/852cb6763ca445c5bca0822fc30f2653

- link to grafana with fixed variables: https://gustavogm001.grafana.net/dashboard/snapshot/2dvfy0RoR87vJqPKb1MvSyjfQxw9A4QK

# PCB
PCB schematics can be found here: https://oshwlab.com/siqueiraluizlb/iot-projeto-final

![PCB_schematics](https://raw.githubusercontent.com/louisgaming/ENG4051_Final_Project/refs/heads/main/README_images/PCB_schematics.svg)

![PCB_front](https://raw.githubusercontent.com/louisgaming/ENG4051_Final_Project/refs/heads/main/README_images/PCB_front.svg)

![PCB_back](https://raw.githubusercontent.com/louisgaming/ENG4051_Final_Project/refs/heads/main/README_images/PCB_back.svg)

EPAPER:          WeAct 2.9" Epaper Module
B1 and B2:       Push Button modules
BALANCA:         HX711 load cell amplifier 
RFID:            RC522 RFID module
Ctt+ and Ctt-:   Door closure detection through a contact-based circuit 
US1 and US2:     HC-SR04 Ultrasonic sensors 
RL:              2-Channel Relay Module
SR1 and SR2:     SG90 Servomotors 

Originally, the Ctt+, Ctt−, RL, SR2, B2, and US2 pins were intended to enable two product options per dispenser and an automatic locking system. However, due to time constraints, the system had to be limited to a single-product dispenser. 

# Project Video
