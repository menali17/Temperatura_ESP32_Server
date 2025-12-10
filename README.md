
# Monitoramento de Temperatura com ESP32 + CEP (Controle Estatístico de Processo)

Um sistema IoT para monitoramento de temperatura utilizando **ESP32 + sensor NTC** com dashboard em tempo real, aplicação de **CEP**, cartas de controle X/MR, cálculo de **Cp/Cpk** e **Regras Western Electric**, com acesso remoto via **Ngrok**.

---

##  Funcionalidades

- Leitura automática da temperatura via NTC 10k
- Dashboard HTML responsivo hospedado no ESP32 via LittleFS
- Gráficos em tempo real (Carta X e MR)
- Cálculo de Média, Mediana, Amplitude, σ, MR
- Cp e Cpk para capabilidade do processo
- Detecção de instabilidade via Western Electric Rules
- Acesso remoto via tunelamento Ngrok

---

##  Estrutura do Projeto

📁 ESP32-CEP  
 ├ `dashboard.html` — interface web + gráficos  
 ├ `chart.umd.min.js` — Chart.js local  
 ├ `main.ino/.cpp` — código do ESP32  

---

## Configuração

Edite no código:

```cpp
const char* WIFI_SSID = "SUA_REDE";
const char* WIFI_PASS = "SENHA";
```

Envie para o ESP32 e abra o serial para ver o IP.

Acesse no navegador:

```
http://IP_DO_ESP32/
```

---

##  Acesso Externo (Nuvem)

```bash
ngrok http IP_DO_ESP32:80
```

Você receberá um link público para acessar de qualquer lugar.

---

## Estatística Aplicada

**Limites de Controle:**  
UCL = X̄ + 3σ  
LCL = X̄ - 3σ  

✔ Cp e Cpk para capabilidade  
✔ Carta X Individual  
✔ Carta MR  

**Western Electric implementadas:**  
1️⃣ Ponto acima/abaixo de UCL/LCL  
2️⃣ Tendência (6 crescentes/decrescentes)  
3️⃣ 8 pontos no mesmo lado da média  

---

