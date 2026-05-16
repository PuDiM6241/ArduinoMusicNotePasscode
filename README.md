O projeto consiste em um passcode musical desenvolvido em C++ para Arduino Uno utilizando um display LCD 16x2 com caracteres personalizados para representar notas musicais em uma pauta.

O projeto foi desenvolvido e testado virtualmente utilizando o Tinkercad
arquivo com detalhes desse projeto usa a extensao .BRD
<img width="1252" height="799" alt="image" src="https://github.com/user-attachments/assets/98abbec1-2684-4fa8-ae28-87cf1227b155" />


O sistema possui:

Seleção de notas através de potenciômetro analógico
Reprodução sonora usando buzzer
Exibição gráfica das notas no LCD
Sistema de senha musical
Botões com interrupção externa
Cache dinâmico de caracteres personalizados do LCD
Conversão automática de notas antigas para representação simplificada para evitar corrupção visual da memória CGRAM do LCD



Funcionamento

O potenciômetro define a nota musical atual, variando entre:

DO
RE
MI
FA
SOL
LA
SI
DO1
RE1
MI1
FA1

A nota selecionada é exibida graficamente no LCD em forma de pauta musical.

Ao pressionar o botão de salvar:

a nota atual é armazenada;
o buzzer reproduz sua frequência correspondente;
a nota é adicionada à sequência digitada.

Quando a quantidade de notas digitadas atinge o tamanho da senha:

o sistema verifica se a sequência está correta;
caso esteja correta, uma música é reproduzida;
caso contrário, a tentativa é resetada.

O segundo botão serve para limpar a tentativa atual.

Componentes utilizados
Arduino Uno
Display LCD 16x2 compatível com biblioteca LiquidCrystal
Buzzer 5V
2 potenciômetros
2 botões
Resistores para os botões (opcional usando INPUT_PULLUP)
Protoboard
Jumpers



O projeto utiliza os 8 slots de caracteres customizados do display LCD:

pauta musical;
clave de sol;
símbolo genérico de nota;
cache dinâmico de notas musicais.




Interrupções externas

Os dois botões utilizam interrupções:

salvar nota;
resetar tentativa.

Foi implementado debounce por software utilizando millis().
