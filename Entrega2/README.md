# Entrega 2

## Fluxograma do comportamento do sistema

<p align="center">
  <img src="https://github.com/giovannaliiz/PI3-2025-1/blob/main/imagens/Fluxograma_2.jpg?raw=true" width="900"/>
</p>


## Testes iniciais dos sensores

Foram realizados testes individuais de cada sensor para verificar seu funcionamento. O módulo de tensão, o acelerômetro e o sensor de chuva foram avaliados separadamente, sendo que o módulo de tensão e o acelerômetro apresentaram seus resultados por meio do monitor serial.

Teste inicial do Módulo de Tensão:

https://github.com/user-attachments/assets/a1b5b0d0-355b-4a44-a402-4f0e2d8aa150

Teste inicial do Acelerômetro:

https://github.com/user-attachments/assets/a39f9e20-5d1d-4470-aab8-4f512680a43d

Teste inicial do Sensor de Chuva:

https://github.com/user-attachments/assets/b8497144-41e5-45c0-88ff-a969a148b5c5

## Programação inicial e verificação com integração com os sensores

Após a validação individual de cada sensor, iniciou-se a fase de programação integrada do projeto. Nesta etapa, os sensores de chuva e o acelerômetro foram conectados simultaneamente ao ESP32, com a montagem realizada em uma protoboard, conforme mostrado na imagem abaixo. O objetivo foi desenvolver a lógica inicial de detecção de eventos relevantes: chuva e possível queda ou tombamento do varal.

<p align="center">
  <img src="https://github.com/giovannaliiz/PI3-2025-1/blob/main/imagens/vista_de_frente.jpeg?raw=true" width="700"/>
</p>


<p align="center">
  <img src="https://github.com/giovannaliiz/PI3-2025-1/blob/main/imagens/vista_de_cima.jpeg?raw=true" width="700"/>
</p>


 A partir das leituras dos sensores, mensagens de alerta são exibidas no monitor serial indicando a presença de chuva ou alterações nos ângulos de inclinação que sugerem instabilidade do varal. 

Com os sensores de chuva e de inclinação já integrados e funcionando conforme o esperado. Esse módulo opera com um divisor resistivo interno, permitindo a leitura de tensões mais altas por meio da entrada analógica do ESP32. A adição deste sensor tem como finalidade monitorar a alimentação do sistema, identificando quedas de tensão ou falhas de fornecimento que possam comprometer o funcionamento do varal. Como mostra a figura a seguir:


 
<p align="center">
  <img src="https://github.com/giovannaliiz/PI3-2025-1/blob/main/imagens/Imagem_serial.jpeg?raw=true" width="700"/>
</p>


<p align="center">
  <img src="https://github.com/giovannaliiz/PI3-2025-1/blob/main/imagens/integracao_foto.jpeg?raw=true" width="700"/>
</p>


<p align="center">
  <img src="https://github.com/giovannaliiz/PI3-2025-1/blob/main/imagens/integracao_teste.jpeg?raw=true" width="700"/>
</p>



## Definição da interface com o usuário

Foi desenvolvido um protótipo da interface de comunicação com o usuário utilizando a plataforma Blynk. O Blynk é uma ferramenta que permite a criação de dashboards personalizáveis para monitoramento e interação com dispositivos inteligentes. Para este protótipo, foram configurados painéis tanto para acesso móvel, com o aplicativo disponível na App Store e Play Store, quanto para acesso via navegador, através do Blynk Web Dashboard, facilitando a visualização das informações do sistema em diferentes dispositivos.

### Blynk Web Dashboard

<p align="center">
  <img src="https://github.com/giovannaliiz/PI3-2025-1/blob/main/imagens/BlynkWeb.png?raw=true" width="700"/>
</p>


### Blink Mobile Dashboard

<p align="center">
  <img src="https://github.com/giovannaliiz/PI3-2025-1/blob/main/imagens/BlynkMob.png?raw=true" width="400"/>
</p>


