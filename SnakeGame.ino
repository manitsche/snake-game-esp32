#include <Adafruit_NeoPixel.h>

// Configuração inicial
#define PIN 6
#define LED_COUNT 64
#define BRIGHTNESS 5

Adafruit_NeoPixel strip(LED_COUNT, PIN, NEO_GRB + NEO_KHZ800);

int snake1[64], snake2[64];
int snakeLength1 = 0, snakeLength2 = 3;
int food = -1;
int direction1, direction2;  // Direções aleatórias para as cobras

void setup() {
    Serial.begin(9600);  // Inicia comunicação serial
    strip.begin();
    strip.show();
    strip.setBrightness(BRIGHTNESS);
    randomSeed(analogRead(0));
    
    resetGame();
}

void loop() {
    updateDirection(snake1, direction1, food);
    updateDirection(snake2, direction2, food);
    
  //moveSnake(snake1, snakeLength1, direction1);
    moveSnake(snake2, snakeLength2, direction2);

    if (checkCollision()) {
        Serial.println("Fim do Jogo!");  // Exibe mensagem no monitor serial
        blinkLEDs();  // Pisca os LEDs uma vez
        resetGame();
        return;
    }

    drawSnakes();
    delay(750);  // Velocidade mais lenta
}

// Funções auxiliares
void spawnFood() {
    do {
        food = random(0, LED_COUNT);
    } while (isOccupied(food, snake1, snakeLength1) || isOccupied(food, snake2, snakeLength2));
    strip.setPixelColor(food, strip.Color(255, 0, 0));
    strip.show();
}

bool isOccupied(int position, int snake[], int length) {
    for (int i = 0; i < length; i++) {
        if (snake[i] == position) return true;
    }
    return false;
}

void updateDirection(int snake[], int &direction, int food) {
    int snakeX = snake[0] % 8;  // Coordenada X da cabeça da cobra
    int snakeY = snake[0] / 8;  // Coordenada Y da cabeça da cobra
    int foodX = food % 8;       // Coordenada X da comida
    int foodY = food / 8;       // Coordenada Y da comida
    
    // Priorizar o movimento na direção do alimento
    if (snakeX < foodX && direction != 0) direction = 2;  // Move para a direita
    else if (snakeX > foodX && direction != 2) direction = 0;  // Move para a esquerda
    else if (snakeY < foodY && direction != 1) direction = 3;  // Move para baixo
    else if (snakeY > foodY && direction != 3) direction = 1;  // Move para cima
}

void moveSnake(int snake[], int &length, int direction) {
    int next = nextPosition(snake[0], direction);
    if (next == food) {
        length++;
        spawnFood();
    } else {
        for (int i = length - 1; i > 0; i--) {
            snake[i] = snake[i - 1];
        }
    }
    snake[0] = next;
}

int nextPosition(int current, int direction) {
    switch (direction) {
        case 0: return (current % 8 == 0) ? current + 7 : current - 1;
        case 1: return (current < 8) ? current + 56 : current - 8;
        case 2: return (current % 8 == 7) ? current - 7 : current + 1;
        case 3: return (current >= 56) ? current - 56 : current + 8;
    }
    return current;
}

bool checkCollision() {
    // Verifica colisão com os próprios corpos
    for (int i = 1; i < snakeLength1; i++) {
        if (snake1[0] == snake1[i]) return true;
    }
    for (int i = 1; i < snakeLength2; i++) {
        if (snake2[0] == snake2[i]) return true;
    }

    // Verifica colisão entre as cobras
    for (int i = 0; i < snakeLength1; i++) {
        if (snake1[i] == snake2[0]) return true;
    }
    for (int i = 0; i < snakeLength2; i++) {
        if (snake2[i] == snake1[0]) return true;
    }
    
    return false;
}

void resetGame() {
    // Inicializa a cobra 1 em posição aleatória
    snakeLength1 = 3;
    snake1[0] = random(0, LED_COUNT);
    snake1[1] = (snake1[0] + 1) % LED_COUNT;
    snake1[2] = (snake1[0] + 2) % LED_COUNT;
    direction1 = random(0, 4);  // Direção aleatória entre 0 e 3
    
    // Inicializa a cobra 2 em posição aleatória
    snakeLength2 = 3;
    snake2[0] = random(0, LED_COUNT);
    snake2[1] = (snake2[0] + 1) % LED_COUNT;
    snake2[2] = (snake2[0] + 2) % LED_COUNT;
    direction2 = random(0, 4);  // Direção aleatória entre 0 e 3

    spawnFood();
}

void drawSnakes() {
    strip.clear();
    for (int i = 0; i < snakeLength1; i++) {
        strip.setPixelColor(snake1[i], strip.Color(0, 255, 0));  // Cobra 1 em verde
    }
    for (int i = 0; i < snakeLength2; i++) {
        strip.setPixelColor(snake2[i], strip.Color(0, 0, 255));  // Cobra 2 em azul
    }
    strip.setPixelColor(food, strip.Color(255, 0, 0));  // Comida em vermelho
    strip.show();
}

void blinkLEDs() {
    strip.clear();
    strip.show();
    delay(700);
    strip.fill(strip.Color(255, 255, 255));  // Acende todos os LEDs
    strip.show();
    delay(700);  // Pisca apenas uma vez
}