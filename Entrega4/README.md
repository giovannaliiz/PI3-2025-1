# Entrega 4

## Estrutura mecânica completa

O varal foi desenvolvido a partir do reaproveitamento de estruturas já existentes, resultando em uma base estável e funcional. A haste principal, de formato cilíndrico, foi montada com canos, e na parte superior utilizamos um arco que serve como trilho para o movimento do toldo. O sistema é acionado por quatro motores, sendo dois responsáveis pela abertura e dois pelo fechamento. No centro, foram posicionados estrategicamente o aquecedor PTC e a ventoinha, alinhados acima da haste destinada a pendurar as roupas.

A abertura e o fechamento do varal são controlados pelo ESP32, seguindo a lógica previamente programada. Para o mecanismo de movimentação, utilizamos fios de nylon e suportes de cortina, garantindo leveza e eficiência no deslocamento do toldo.

<p align="center">
  <img src="https://github.com/giovannaliiz/PI3-2025-1/blob/main/imagens/varal_lateral.jpeg?raw=true" width="500"/>
</p>

<p align="center">
  <img src="https://github.com/giovannaliiz/PI3-2025-1/blob/main/imagens/varal.jpeg?raw=true" width="500"/>
</p>

<p align="center">
  <img src="https://github.com/giovannaliiz/PI3-2025-1/blob/main/imagens/varal_toalha.jpeg?raw=true" width="500"/>
</p>

Vídeo do funcionamento da estrutura:

https://github.com/user-attachments/assets/558d8165-9627-459b-863c-1a679fcd9187


## Integração da parte eletrônica
Este projeto utiliza um microcontrolador ESP32 programado com o framework ESP-IDF no Visual Studio Code. A arquitetura foi desenvolvida de forma modular, permitindo a integração progressiva de periféricos adicionais, como:

Sensor de chuva (GPIO)

Acelerômetro MPU6050 (I2C) para detecção de tombamento

Sensor de tensão (ADC)

A funcionalidade principal consiste no controle automatizado de uma cortina, que se fecha automaticamente ao detectar chuva e se reabre após um período sem precipitação. Durante o fechamento, um ventilador e um motor PTC são acionados com um atraso de dois segundos. Ambos são desligados quando a cortina é reaberta.

O projeto também conta com conectividade Wi-Fi e integração com a plataforma Blynk, permitindo o monitoramento remoto do estado dos sensores e atuadores. A fase de depuração foi realizada por meio da análise dos logs do terminal, com foco na otimização da lógica de controle e no ajuste do funcionamento dos relés e demais dispositivos conectados ao hardware.


> Para acessar o código-fonte principal, [clique aqui](https://github.com/giovannaliiz/PI3-2025-1/blob/main/main.c).



## Integração da parte eletrônica + mecânica

A integração entre a parte eletrônica e a estrutura mecânica foi realizada com sucesso, assim como em bancada, mantendo a funcionalidade e a organização dos componentes, além de garantir proteção e facilidade na manutenção do sistema.

<p align="center">
  <img src="https://github.com/giovannaliiz/PI3-2025-1/blob/main/imagens/case_final_2.jpeg" width="400"/>
</p>



## Geraçãos dos cases das placas

Foram utilizadas duas cases no projeto, ambas fixadas na estrutura do protótipo para facilitar a manutenção. Elas são fundamental para a montagem final, garantindo proteção, organização e portabilidade aos componentes eletrônicos do sistema.


<p align="center">
  <img src="https://github.com/giovannaliiz/PI3-2025-1/blob/main/imagens/case_final.jpeg" width="500"/>
</p>

<p align="center">
  <img src="https://github.com/giovannaliiz/PI3-2025-1/blob/main/imagens/case_final_3.jpeg" width="500"/>
</p>

<p align="center">
  <img src="https://github.com/giovannaliiz/PI3-2025-1/blob/main/imagens/case_frontal.jpeg?raw=true" width="500"/>
</p>

<p align="center">
  <img src="https://github.com/giovannaliiz/PI3-2025-1/blob/main/imagens/case_superior.jpeg?raw=true" width="500"/>
</p>

Para a realização da placa de circuito impresso, foram utilizados o KiCad para o desenvolvimento e o almoxarifado do IFSC para a produção. O esquemático foi cuidadosamente projetado para integrar um microcontrolador ESP32 com sensores de chuva e acelerômetro, com capacidade de atuação via relé. Ele utiliza fontes separadas de tensões diferentes, sendo o módulo MB102 incorporado para converter a tensão de entrada, fornecendo 3.3V para o ESP e os sensores, e 5V para os módulos dos relés. Adicionalmente, o esquemático incluiu uma entrada dedicada para a bateria que alimenta o motor do aquecedor. O layout da PCB foi cuidadosamente planejado para garantir uma montagem prática e funcional

<p align="center">
  <img src="https://github.com/giovannaliiz/PI3-2025-1/blob/main/imagens/esquematico.png" width="500"/>
</p>


<p align="center">
  <img src="https://github.com/giovannaliiz/PI3-2025-1/blob/main/imagens/pcb.png" width="500"/>
</p>

