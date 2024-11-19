const int BUTTON_PIN = 2;    // Pino do Arduino conectado ao botão
const int BUZZER_PIN = 4;    // Pino do Arduino conectado ao buzzer
const int LED_RED_PIN = 3;   // Pino do Arduino conectado ao LED vermelho
const int LED_GREEN_PIN = 5; // Pino do Arduino conectado ao LED verde

unsigned long previousMillis = 0;       // Armazena o último tempo em que o buzzer foi ligado
unsigned long alarmStartMillis = 0;     // Tempo em que o alarme começou
const long interval = 1800000;           // Intervalo de 30 minutos (1800000 milissegundos)
bool isBuzzerOn = false;                // Estado do buzzer e do LED vermelho
bool systemActive = false;              // Estado do sistema (se está ativado ou não)
int buttonState = HIGH;                 // Estado atual do botão
int lastButtonState = HIGH;             // Estado anterior do botão
unsigned long buttonPressTime = 0;      // Armazena o tempo em que o botão foi pressionado
unsigned long currentMillis;            // Tempo atual do sistema
bool alarmRunning = false;              // Indica se o alarme está em execução

void setup() {
  Serial.begin(9600);                 // Inicializa a comunicação serial
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Configura o pino do botão como entrada com pull-up
  pinMode(BUZZER_PIN, OUTPUT);        // Configura o pino do buzzer como saída
  pinMode(LED_RED_PIN, OUTPUT);       // Configura o pino do LED vermelho como saída
  pinMode(LED_GREEN_PIN, OUTPUT);     // Configura o pino do LED verde como saída

  // Inicializa todos os LEDs e o buzzer como desligados
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_RED_PIN, LOW);
  digitalWrite(LED_GREEN_PIN, LOW);
}

void loop() {
  currentMillis = millis();  // Atualiza o tempo atual do sistema
  buttonState = digitalRead(BUTTON_PIN);  // Lê o estado atual do botão

  // Verifica se o botão foi pressionado (transição de HIGH para LOW)
  if (lastButtonState == HIGH && buttonState == LOW) {
    buttonPressTime = currentMillis;  // Armazena o tempo em que o botão foi pressionado
  }

  // Verifica se o botão foi solto (transição de LOW para HIGH)
  if (lastButtonState == LOW && buttonState == HIGH) {
    unsigned long pressDuration = currentMillis - buttonPressTime;

    if (pressDuration >= 3000) {  // Se o botão foi pressionado por 3 segundos ou mais
      systemActive = !systemActive;  // Alterna o estado do sistema
      if (systemActive) {
        digitalWrite(LED_GREEN_PIN, HIGH); // Liga o LED verde
        Serial.println("System activated");
      } else {
        digitalWrite(LED_GREEN_PIN, LOW);  // Desliga o LED verde
        digitalWrite(LED_RED_PIN, LOW);    // Desliga o LED vermelho
        digitalWrite(BUZZER_PIN, LOW);     // Desliga o buzzer
        isBuzzerOn = false;
        alarmRunning = false;
        Serial.println("System deactivated");
      }
    } else if (systemActive && isBuzzerOn) {  // Se o sistema estiver ativo e o botão foi pressionado rapidamente
      isBuzzerOn = false;                     // Desliga o alarme
      digitalWrite(BUZZER_PIN, LOW);
      digitalWrite(LED_RED_PIN, LOW);
      alarmRunning = false;
      Serial.println("Alarm turned off");
    }
  }

  // Se o sistema estiver ativo, verifica o intervalo de 30 segundos
  if (systemActive) {
    if (currentMillis - previousMillis >= interval && !isBuzzerOn) {
      previousMillis = currentMillis;  // Atualiza o tempo anterior
      isBuzzerOn = true;               // Liga o alarme
      alarmStartMillis = currentMillis; // Marca o tempo de início do alarme
      alarmRunning = true;
      Serial.println("Alarm activated");
    }
  }

  // Gerencia a dinâmica do alarme
  if (alarmRunning) {
    unsigned long elapsedTime = currentMillis - alarmStartMillis;

    if (elapsedTime < 10000) {  // Nos primeiros 10 segundos
      if ((elapsedTime / 1000) % 2 == 0) {  // Liga o buzzer por 1 segundo, pausa por 1 segundo
        digitalWrite(BUZZER_PIN, HIGH);
        digitalWrite(LED_RED_PIN, HIGH);
      } else {
        digitalWrite(BUZZER_PIN, LOW);
        digitalWrite(LED_RED_PIN, LOW);
      }
    } else if (elapsedTime < 20000) {  // Dos 10 aos 20 segundos
      if ((elapsedTime / 500) % 2 == 0) {  // Liga o buzzer por 0.5 segundos, pausa por 0.5 segundos
        digitalWrite(BUZZER_PIN, HIGH);
        digitalWrite(LED_RED_PIN, HIGH);
      } else {
        digitalWrite(BUZZER_PIN, LOW);
        digitalWrite(LED_RED_PIN, LOW);
      }
    } else {  // Após 20 segundos, som constante
      digitalWrite(BUZZER_PIN, HIGH);
      digitalWrite(LED_RED_PIN, HIGH);
    }
  }

  lastButtonState = buttonState;  // Atualiza o estado anterior do botão
}
