#include <LiquidCrystal.h>

LiquidCrystal lcd_1(12, 11, 7, 6, 5, 4);

const int pot = A0;
const int buzzer = 8;
const int botaoSalvar = 2;
const int botaoReset = 3;

// =========================
// FLAGS DE INTERRUPT
// =========================

volatile bool salvarRequest = false;
volatile bool resetRequest = false;
volatile unsigned long lastInterrupt = 0;


// Senha: Mi mi m1 mi mi re1
// Do = 0, Mi=2, mi1=9
int senha[] = {2, 2, 9, 2, 2, 8};

const int tamanhoSenha = 6;

int tentativa[6];
int indice = 0;

bool ultimoSalvar = HIGH;
bool ultimoReset = HIGH;

// guarda nota
int ultimaNotaMostrada = -1;

//=========================
// ShowNotesSupData
//=========================


//REQUIRES:
// 3 = BlankLinesUp
// 4 = BlankLinesDown
// 5 = solkeyUp
// 5 = solkeyDown
// 7 = BasicNoteSymbol

// slots 0-2
int NextToSubstitute = 0;



// ========================
// LCD notes
// ========================

//===========BASE==========
byte solKeyUp[8] = {
  B00000,
  B00000,
  B00010,
  B00101,
  B00101,
  B00110,
  B00110,
  B01100
};

byte solKeyDown[8] = {
  B01110,
  B11101,
  B10101,
  B10101,
  B01110,
  B00100,
  B10100,
  B01000
};

byte BlankLinesUp[8] = {
  B00000,
  B11111,
  B00000,
  B00000,
  B11111,
  B00000,
  B00000,
  B11111
};

byte BlankLinesDown[8] = {
  B00000,
  B00000,
  B11111,
  B00000,
  B00000,
  B11111,
  B00000,
  B00000
};

byte BasicNoteSymbol[8] = {
  B00000,
  B00010,
  B00011,
  B00010,
  B01110,
  B01110,
  B01110,
  B00000
};

//===========Notes==========
byte Notes[11][8] = {
  // Do [0]
  {
  B00000,
  B00000,
  B11111,
  B00000,
  B00000,
  B11111,
  B00000,
  B01110
  },
  // Re [1]
  {
  B00000,
  B00000,
  B11111,
  B00000,
  B00000,
  B11111,
  B01110,
  B01110
  },
  // Mi [2]
  {
  B00000,
  B00000,
  B11111,
  B00000,
  B01110,
  B11111,
  B00000,
  B00000
  },
  // Fa [3]
  {
  B00000,
  B00000,
  B11111,
  B01110,
  B01110,
  B11111,
  B00000,
  B00000
  },
  // Sol [4]
  {
  B00000,
  B01110,
  B11111,
  B00000,
  B00000,
  B11111,
  B00000,
  B00000
  },
  // La [5]
  {
  B01110,
  B01110,
  B11111,
  B00000,
  B00000,
  B11111,
  B00000,
  B00000
  },
  // Si [6]
  {
  B00000,
  B11111,
  B00000,
  B00000,
  B11111,
  B00000,
  B01110,
  B11111
  },
  // Do1 [7]
  {
  B00000,
  B11111,
  B00000,
  B00000,
  B11111,
  B01110,
  B01110,
  B11111
  },
  // Re1 [8]
  {
  B00000,
  B11111,
  B00000,
  B01110,
  B11111,
  B00000,
  B00000,
  B11111
  },
  // Mi1 [9]
  {
  B00000,
  B11111,
  B01110,
  B01110,
  B11111,
  B00000,
  B00000,
  B11111
  },
  // Fa1 [10]
  {
  B00000,
  B11111,
  B00000,
  B00000,
  B11111,
  B00000,
  B00000,
  B11111
  }
};


//========================
//       FUNCTIONS
//========================
void tocarNota(int nota, int d = 300);
void tocarMusica();
void verificarSenha();
void resetarTentativa();
void ShowNote(int position, int note);

int lerNota();
char nomeNota(int nota);

void salvarInterrupt();
void resetInterrupt();


void setup() {
  Serial.begin(9600);

  pinMode(botaoSalvar, INPUT_PULLUP);
  pinMode(botaoReset, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  pinMode(pot, INPUT);
  
  attachInterrupt(
    digitalPinToInterrupt(botaoSalvar),
    salvarInterrupt,
    FALLING
  );

  attachInterrupt(
    digitalPinToInterrupt(botaoReset),
    resetInterrupt,
    FALLING
  );

  lcd_1.begin(16, 2);

  // ========================
  // FIXED SLOTS
  // 3 = BlankLinesUp
  // 4 = BlankLinesDown
  // 5 = solkeyUp
  // 5 = solkeyDown
  // 7 = BasicNoteSymbol
  // ========================

  lcd_1.createChar(3, BlankLinesUp);
  lcd_1.createChar(4, BlankLinesDown);

  //BLANK LINES
  for (int i = 0; i < 16; i++) {
    lcd_1.setCursor(i, 0);
    lcd_1.write(byte(3));
    lcd_1.setCursor(i, 1);
    lcd_1.write(byte(4));
  }
  
  //BasicNote
  lcd_1.createChar(7, BasicNoteSymbol);
  
  //SOL KEY
  lcd_1.createChar(5, solKeyUp);
  lcd_1.createChar(6, solKeyDown);

  lcd_1.setCursor(0, 0);
  lcd_1.write(byte(5));

  lcd_1.setCursor(0, 1);
  lcd_1.write(byte(6));
}


void loop() {

  int notaAtual = lerNota();

  // print no serial
  if (notaAtual != ultimaNotaMostrada) {

    Serial.print("Nota atual: ");
    Serial.println(nomeNota(notaAtual));

    ultimaNotaMostrada = notaAtual;

    // mostra nota atual
    ShowNote(indice + 1, notaAtual);
  }

  // =========================
  //          SALVAR
  // =========================

  if (salvarRequest) {

    salvarRequest = false;

    tentativa[indice] = notaAtual;

  //SHOW NOTE
    ShowNote(indice + 1, notaAtual);
  NextToSubstitute++;
  if (NextToSubstitute > 2)
    NextToSubstitute = 0;
  
    indice++;
  
  if (indice >= 3) {// uma nota atraz do indice atual tem que vira nota em letra pra evita bug visual

    int oldPos = indice -2;

    // letra da nota
    lcd_1.setCursor(oldPos, 0);
    lcd_1.print(nomeNota(tentativa[indice - 1]));

    // simbolo generico
    lcd_1.setCursor(oldPos, 1);
    lcd_1.write(byte(7));
  }
  
    tocarNota(notaAtual);

    Serial.print("Salvou: ");
    Serial.println(nomeNota(notaAtual));

    // verifica senha
    if (indice == tamanhoSenha) {
      verificarSenha();
    }
  }

  // =========================
  //          RESET
  // =========================

  if (resetRequest) {

    resetRequest = false;

    resetarTentativa();
  }
}

void salvarInterrupt() {
  unsigned long now = millis();
  if (now - lastInterrupt > 150) {
    salvarRequest = true;
    lastInterrupt = now;
  }
}

void resetInterrupt() {
  unsigned long now = millis();
  tocarMusica();
    if (now - lastInterrupt > 150) {
    resetRequest = true;
    lastInterrupt = now;
  }
}

// =========================
//      LER NOTA DO POT
// =========================
int lerNota() {

  int valor = 0;

  //AVARAGE
  for (int i = 0; i < 5; i++) {
  valor += analogRead(pot);
  delay(2);
  }

  valor /= 5;
  valor = map(valor, 0, 1023, 0, 10);
  valor = constrain(valor, 0, 10);
  return valor;
}


// =========================
//        NOME DA NOTA
// =========================
char nomeNota(int nota) {
  switch (nota) {
  case 0: return 'C'; // DO 
  case 1: return 'D'; // RE 
  case 2: return 'E'; // MI 
  case 3: return 'F'; // FA 
  case 4: return 'G'; // SOL 
  case 5: return 'A'; // LA 
  case 6: return 'B'; // SI 
  case 7: return 'C'; // DO1
  case 8: return 'D'; // RE1 
  case 9: return 'E'; // MI1 
  case 10: return'F'; // FA1
  }
  return '?';
}


// =========================
//      TOCAR UMA NOTA
// =========================
void tocarNota(int nota, int d) {

  int frequencia = 0;

  switch (nota) {
      case 0: frequencia = 262; break; // DO 
      case 1: frequencia = 294; break; // RE 
      case 2: frequencia = 330; break; // MI 
      case 3: frequencia = 349; break; // FA 
      case 4: frequencia = 392; break; // SOL 
      case 5: frequencia = 440; break; // LA 
      case 6: frequencia = 494; break; // SI 
      case 7: frequencia = 523; break; // DO1
      case 8: frequencia = 587; break; // RE1 
      case 9: frequencia = 659; break; // MI1 
      case 10: frequencia = 698; break; // FA1 
  }

  tone(buzzer, frequencia);

  delay(d);

  noTone(buzzer);
}


// =========================
//    RESETAR TENTATIVA
// =========================
void resetarTentativa() {
  //tocarMusica();
  indice = 0;

  Serial.println("RESETADO");

  NextToSubstitute = 0;

  //Fixed Characters
  lcd_1.createChar(3, BlankLinesUp);
  lcd_1.createChar(4, BlankLinesDown);
  lcd_1.createChar(5, solKeyUp);
  lcd_1.createChar(6, solKeyDown);
  lcd_1.createChar(7, BasicNoteSymbol);
  
  //LCD
  for (int i = 1; i < 16; i++) {

    lcd_1.setCursor(i, 0);
    lcd_1.write(byte(3));

    lcd_1.setCursor(i, 1);
    lcd_1.write(byte(4));
  }


  //SolKey
  lcd_1.setCursor(0, 0);
  lcd_1.write(byte(5));
  lcd_1.setCursor(0, 1);
  lcd_1.write(byte(6));
}


// =========================
//     VERIFICAR SENHA
// =========================
void verificarSenha() {
  bool correta = true;

  for (int i = 0; i < tamanhoSenha; i++) {
  if (tentativa[i] != senha[i]) {
    correta = false;
    break;
  }
  }

  if (correta) {
  Serial.println("ACERTOU!");
  tocarMusica();
  } else {
  Serial.println("ERROU!");
  }

  resetarTentativa();
}


// =========================
//      TOCAR MUSICA
// =========================
void tocarMusica() {
  /*  case 0: 262;  // DO 
      case 1: 294; // RE 
      case 2:  330; // MI 
      case 3: 349; // FA 
      case 4:  392;  // SOL 
      case 5: 440; // LA 
      case 6: 494;  // SI 
      case 7:  523;  // DO1
      case 8:  587;  // RE1 
      case 9:  659;  // MI1 
      case 10:  698; // FA1 */
  
  
  int musica[] = {
    330, 330, 659, 330, 330, 587, 330, 330, 466, 330, 330, 494, 523,
    330, 330, 659, 330, 330, 587, 330, 330
  };
    int Time[]={ 0
    };

  int tamanho = sizeof(musica) / sizeof(musica[0]);

  for (int i = 0; i < tamanho; i++) {
    tone(buzzer, musica[i]);
    delay(125);
    noTone(buzzer);
    delay(50);
  }
  tone(buzzer, 466);
  delay(400);
  noTone(buzzer);
}
  

  
  
// position = coluna a imprimir do LCD
// note = 0 até 10
void ShowNote(int position, int note){

  if (position <= 0 || position > 15)
    return;

  //SUBSTITUTE
  lcd_1.createChar(NextToSubstitute, Notes[note]);
  
  //LINE Y
  int y;

  if (note <= 5)
    y = 1;
  else
    y = 0;

  //CLEAN AND INSERT NOTE
  // 3 = BlankLinesUp
  // 4 = BlankLinesDown

  lcd_1.setCursor(position, 1 - y);
  lcd_1.write(byte(4 - y));

  lcd_1.setCursor(position, y);
  lcd_1.write(byte(NextToSubstitute));
}
