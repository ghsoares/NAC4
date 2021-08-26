//////Json
#include <ArduinoJson.h>
const int TAMANHO = 200;

// Pino do fotoressistor
const int pinPhotoR = A0;
// Pino do termistor
const int pinThermR = A1;
// Resistência do fotoressistor
const int luxR1 = 10000;
// Resistência do termistor
const int thermR1 = 10000;
// Valor quando o fotoressistor detectar nenhuma luz
const float darkValue = 127410;
// Gamma do fotoressistor
const float gamma = 0.8582;

// Tempo em segundos de delay para cada loop do programa
float waitDelay = .5;

// Variável para calcular velocidade do vento
unsigned int windCount;

void setup() {
	//inicializa comunicação serial
	Serial.begin(115200);
	
	attachInterrupt(digitalPinToInterrupt(2),windInterruption,RISING);
}

void loop() {
	// Valor bruto do fotoressistor
	float rawLux = analogRead(pinPhotoR);
	// Valor bruto do termistor
	float rawTemp = analogRead(pinThermR);
	
	// Calcula temperatura e luminosidade
	float temp = calcTemp(rawTemp);
	float lux = calcLux(rawLux);
	
	// Calcula velocidade do vento, leva em consideração a
	// variação de tempo entre o loop anterior e esse
	// (usado o delay no caso)
	float windSpeed = calcWindSpeed();
	
	StaticJsonDocument<TAMANHO> json; //cria o objeto Json dos valores dos sensores
	json["topic"] = "sensores";
	json["temp"] = temp;
	json["lux"] = lux;
	json["windSpeed"] = windSpeed;

	serializeJson(json, Serial);
  	Serial.println();
	
	// Reseta a contagem
	windCount = 0;
	
	// Espera o delay
	delay(waitDelay * 1000.0);
}

// Função para calcular a luminosidade a partir de um valor bruto
float calcLux(float raw) {
	// Converte de 0-1023 para 0v-5v
	float voltage = raw * (5.0 / 1023.0);
	float rLDR = luxR1 / ((5.0 / voltage) - 1);
	return pow(abs(darkValue / rLDR), 1.0/gamma);
}

//função que faz leitura da temperatura e retorna o valor em graus celcius
float calcTemp(float raw){
	//valores constante para calculo da temperatura
	float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07; 

	// Lê a entrada
  float tempOut = raw;
//calculo R2, demonstração no arquivo pdf da aula
  tempOut = thermR1 * (1023.0 / tempOut - 1.0);
  tempOut = log(tempOut);
// Equação de Steinhart–Hart 
  tempOut = (1.0 / (c1 + c2*tempOut + c3*tempOut*tempOut*tempOut));// Equação de Steinhart–Hart 
  tempOut = tempOut - 273.15; //temperatura em Graus celcius
  //Tf = (Tc * 9.0)/ 5.0 + 32.0; // temp em fahrenheit
  return tempOut;
}

// função que calcula a velocidade do vento
// de hertz (ciclos/segundo) para km/h
float calcWindSpeed() {
	return (windCount / waitDelay) * 6.12;
}

// interrupção, chamado pelo gerador de onda quadrada
void windInterruption() {
	windCount++;
}






