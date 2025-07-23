#  Entrega 1 – Primeiras Etapas do Projeto

--------

##  Fluxograma

<p align="center">
  <img src="https://github.com/giovannaliiz/PI3-2025-1/blob/main/imagens/Fluxograma%201%20PI.png?raw=true" width="900"/>
</p>

---------

##  Estudo sobre Aspectos de Inteligência

- **Sensoriamento ambiental**: uso de sensores de chuva para detectar mudanças nas condições climáticas em tempo real.
- **Tomada de decisão automatizada**: a lógica embarcada no microcontrolador avalia as condições do ambiente e da previsão do tempo para acionar mecanismos de proteção automaticamente, como o fechamento da tenda e o acionamento do aquecedor.
- **Previsão de eventos**: ao integrar uma verificação automática da previsão do tempo, o sistema antecipa situações de risco (como chuva iminente), aumentando a eficiência do controle.
- **Conectividade e interação remota**: o uso do módulo Wi-Fi permite controle via aplicativo, monitoramento em tempo real e possível integração com assistentes virtuais ou outros dispositivos IoT no futuro.
- **Eficiência energética**: o sistema só aciona dispositivos como o aquecedor quando necessário, otimizando o uso de energia e aumentando a vida útil dos componentes.

-------

##  Diagrama de Blocos

<p align="center">
  <img src="https://github.com/giovannaliiz/PI3-2025-1/blob/main/imagens/DiagramaBlocos01.jpeg?raw=true" width="700"/>
</p>


-------

##  Sensores

### Sensor de chuva YL-83
Detecta presença de água. Principal sensor: com base nele, o toldo vai abrir ou fechar.

<p align="center">
  <img src="https://github.com/giovannaliiz/PI3-2025-1/blob/main/imagens/modulo_sensor_de_chuva.jpg?raw=true" width="250"/>
</p>


### Acelerômetro MPU-6050
Detecta quando há quedas ou inclinações. Quando o varal se move, inclina ou cai, as medidas dos eixos mudam rapidamente e o sistema é avisado de que algo ocorreu.

<p align="center">
  <img src="https://github.com/giovannaliiz/PI3-2025-1/blob/main/imagens/acelerometro.jpg?raw=true" width="250"/>
</p>


---

##  Atuadores

### Motor 5.9V
Será utilizado para abrir e fechar o toldo retrátil.

<p align="center">
  <img src="https://github.com/giovannaliiz/PI3-2025-1/blob/main/imagens/motor.jpg?raw=true" width="250"/>
</p>


### Módulo relé 4 canais
Permite o controle dos motores e do aquecedor pelo ESP32.

<p align="center">
  <img src="https://github.com/giovannaliiz/PI3-2025-1/blob/main/imagens/modulo_rele.jpeg?raw=true" width="250"/>
</p>


### Aquecedor PTC 12V
Ativado após o fechamento do toldo para continuar secando as roupas. Gera calor logo após ser alimentado, podendo chegar até 200 °C de forma gradual.

<p align="center">
  <img src="https://github.com/giovannaliiz/PI3-2025-1/blob/main/imagens/aquecedor_ptc.jpg?raw=true" width="250"/>
</p>


---

##  Estrutura Mecânica

A estrutura consiste em um toldo retrátil, acionado por motor de passo com braços articulados ou trilhos. O varal é fixado de forma estável, e os sensores são posicionados estrategicamente para proteção contra água e detecção de anomalias.

---

##  Definição do Processador

O ESP32 foi escolhido por ter Wi-Fi integrado, múltiplas interfaces GPIO, suporte a ADC e I2C, boa capacidade de processamento e baixo consumo energético, atendendo a todos os pré-requisitos do projeto.

<p align="center">
  <img src="https://github.com/giovannaliiz/PI3-2025-1/blob/main/imagens/esp32.jpg?raw=true" width="250"/>
</p>

