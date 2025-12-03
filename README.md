# ENG4051_Final_Project
Assets supporting the development of the final project for ENG4051 – Projeto Internet das Coisas (IoT Project), instructed by Professor Jan Krueger Siqueira, Department of Electrical Engineering, PUC-Rio.

Solids Dispenser's 3D models can be found here: https://cad.onshape.com/documents/5a55839f175b594ffbd2dcaa/w/03925e9c78100ab4ed96ae5d/e/1c131eb948d5f204225ffffa?renderMode=0&uiState=693086d84b71e3c8a5ab772c  

# The Idea
This project implements a solution for snacks and beverages dispensers, where an RFID Reader (RC522 module) indentifies a previously registered user for automatic balance deduction. The user can then view the quantity — measured by a scale — and the corresponding price of the order on an e-paper display. Two single product dispensers where implemented but the architecture supports further expansion. All system dynamics are managed by an ESP32-3S, with MQTT handling message transmission, Beekeeper Studio providing data storage, and Node-RED serving as the intermediary platform.  

# MQTT

# Node-Red

# Data Base

# PCB
PCB schematics can be found here: https://oshwlab.com/siqueiraluizlb/iot-projeto-final

![PCB_schematics](https://raw.githubusercontent.com/louisgaming/ENG4051_Final_Project/refs/heads/main/README_images/PCB_schematics.svg)

![PCB_front](https://raw.githubusercontent.com/louisgaming/ENG4051_Final_Project/refs/heads/main/README_images/PCB_front.svg)

![PCB_back](https://raw.githubusercontent.com/louisgaming/ENG4051_Final_Project/refs/heads/main/README_images/PCB_back.svg)

Originally, the Ctt+, Ctt−, RL, SR2, B2, and US2 pins were intended to enable two product options per dispenser and an automatic locking system. However, due to time constraints, the system had to be limited to a single-product dispenser. 

# Project Video
