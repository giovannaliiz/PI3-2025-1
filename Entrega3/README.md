# Entrega 3

## Testes preliminares dos sensores e atuadores
Foram realizados testes isoladamente de cada motor, do aquecedor sozinho e depois o mesmo com a ventoinha. 

Teste motor 1: Teste realizados apenas com esp32, relé e o motor.

https://github.com/user-attachments/assets/6216d324-cf8a-46f8-abe0-dfbe666b14d1

Teste motor 2: Teste realizados apenas com esp32, relé e o motor.

https://github.com/user-attachments/assets/f46bb684-a913-4610-92dc-a691eb5c6a05

Teste aquecedor: Teste realizados apenas com esp32, relé e o aquecedor.

https://github.com/user-attachments/assets/ad0795f3-a723-4de0-9a04-a8e7355d2980

Teste aquecedor e ventoinha: Teste realizados apenas com esp32, relé, aquecedor e ventoinha fazendo ligar ambos ao mesmo tempo.

https://github.com/user-attachments/assets/a80e9e50-3602-43b4-a0d8-00919ef7b718

## Integração os componentes em um protótipo em bancada

Teste dos motores e sensores em conjunto: Neste teste ligamos os motores no módulo relé e no esp32, nele podemos observar se a lógica de ativação está correta. Foi realizado da seguinte maneira: Quando detectado chuva o motor 1 liga por 10s, após os 10s ambos os motores ficam desligados e o motor 2 somente liga quando detectar que parou a chuva e este também fica ligado por 10s.

https://github.com/user-attachments/assets/bebdd496-b92b-49b1-9776-2e549a879597


https://github.com/user-attachments/assets/07dfc5e6-c991-4457-a178-5d9e7fed472f



Teste da bancada completa: Neste teste todos os sensores e atuadores serão ligados, o sensor de tensão e o acelerômetro ficam constantemente ativos e mandando mensagem para o esp32, o sensor de chuva também fica monitorando o tempo todo porém, quando este detecta chuva o motor 1 é ligado por 10s, somente quando o motor 1 termina seu ciclo é que são ligadas a ventoinha e o aquecedor que permanecem ligados. Assim que é detectado que parou de chover o motor 2 é acionado também por 10s e quando completado este ciclo a ventoinha e o aquecedor juntamente a ele irão desligar.

https://github.com/user-attachments/assets/e433fd9b-2932-4dfc-9582-c2d3f96f4e4c


https://github.com/user-attachments/assets/5bff236a-8ab8-48cc-bad9-5d469a3b472d


## Implementação preliminar da interface do usuário

Após a criação dos layouts no aplicativo Blynk, iniciamos a comunicação do microcontrolador com o app. O ESP32 conecta-se à internet via Wi-Fi e utiliza um token de autenticação exclusivo (BLYNK_AUTH_TOKEN) para se comunicar com o projeto no Blynk. Em operação contínua, ele lê os sensores de chuva e o acelerômetro, enviando os dados em tempo real para a nuvem, que atualiza a interface do usuário de forma imediata e intuitiva.

https://github.com/user-attachments/assets/773221c3-16fb-4270-a398-a2c1a77053a4


## Design 3D Case
<p align="center">
  <img src="imagens/casefrente.jpeg" width="400"/>
</p>

<p align="center">
  <img src="imagens/caseinferior.jpeg" width="400"/>
</p>

<p align="center">
  <img src="imagens/casesolida.jpeg" width="400"/>
</p>

A case do varal foi modelada no Tinkercad. Ela tem três furos verticais em cada lateral, totalizando seis furos para a passagem dos fios e cabos necessários. Também foram incluídos os encaixes para os parafusos que fixam a tampa. Por fim, foi adicionada uma imagem da case real que será usada no nosso projeto.

## Design 3D da estrutura mecânica

<p align="center">
  <img src="imagens/varal_lateral.jpeg" width="400"/>
</p>


<p align="center">
  <img src="imagens/varal_traseira.jpeg" width="400"/>
</p>

O design 3D do varal foi desenvolvido no Tinkercad. Ele conta com uma base fixa que sustenta uma haste vertical, onde o varal está montado. Na imagem, a cortina aparece na posição fechada.
Na parte traseira da haste estão fixados o motor 1 e o motor 2, responsáveis por abrir e fechar a cortina lateralmente quando acionados. A case do sistema está parafusada na parte de trás da haste, e sobre ela foi adicionado um triângulo de apoio, levemente inclinado, que serve como suporte para o sensor de chuva.

<p align="center">
  <img src="imagens/varal_prototipo1.jpeg" width="400"/>
</p>

Por fim, temos um protótipo da estrutura mecânica que está na fase inicial de montagem.


