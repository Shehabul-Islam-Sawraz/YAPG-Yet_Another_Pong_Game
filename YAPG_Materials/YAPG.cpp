/*
 * lcd_test_adc.c
 *
 * Created: 19-Feb-19 19:46:12
 *  Author: David Prentice
 */
 
#define F_CPU 1000000
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "i2cmaster.h"
#include "lcd_i2cmaster.c"
#include "twimaster.c"
extern void lcd_backlight(char on);   //not in lcd.h
 
#include <stdlib.h>       //for itoa()
#define NOT_INITIATED 0
#define INITIATED 1
#define PLAYING 2
#define POINT 3
#define GAME_OVER 4
#define GAME_POINT 3
#define INP_COUNT 6
 
 
char points[] = "P1: 0, P2: 0";
 
static volatile int pulse1 = 0;
static volatile int trig1 = 0;
 
static volatile int pulse2 = 0;
static volatile int trig2 = 0;
 
int16_t count_a = 0;
int16_t count_b = 0;
 
int pad1X = 0;
int pad1Y = 0;
int pad2X = 0;
int pad2Y = 0;
 
int prevPad1X = 0;
int prevPad1Y = 0;
int prevPad2X = 0;
int prevPad2Y = 0;
 
int ballX = 1;
int ballY = 1;
 
int prevBallX = 1;
int prevBallY = 1;
 
int dx = 1;
int dy = 1;
 
int pointPlayer1 = 0;
int pointPlayer2 = 0;
 
int gameStartCountDown;
 
int cycCount = 0;
int inp1 = INP_COUNT / 2 - 1;
int inp2 = INP_COUNT - 1;
 
int state = 0;  // state = 0 -> game just started
// state = 1 -> game initiated (players can move pad horizontally)
// state = 2 -> in game
// state = 3 -> player got point :D
// state = 4 -> game over. someone won :D
void printMatrix();
void printInLCD(char a[]);
int GameOverCycle = 10;
 
int matrix[16][8]= {
	{1,0,0,0,0,0,0,1}, //pad1
	{1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,1} //pad2
};
 
void callFromPlayer(int p){
	ballX = 4;
	if(p == 1) {
		ballY = 1;
		dy = -1;
		}else {
		ballY = 14;
		dy = 1;
	}
	state = INITIATED;
}
 
void init1(int playerToStart=1){
 
	for(int i=0; i<16; i++){
		for(int j=1; j<7; j++){
			matrix[i][j] = 0;
		}
	}
 
	pad1X = 0;
	pad1Y = 0;
	pad2X = 0;
	pad2Y = 0;
 
	prevPad1X = 0;
	prevPad1Y = 0;
	prevPad2X = 0;
	prevPad2Y = 0;
 
	callFromPlayer(playerToStart);
	gameStartCountDown = 5;
}
 
void init2(){
	pointPlayer1 = 0;
	pointPlayer2 = 0;
	init1(1);
	points[4] = '0';
	points[11] = '0';
	printInLCD(points);
}
 
void removePrevBallAndPads(){
	matrix[prevPad1Y][prevPad1X+1] = matrix[prevPad1Y][prevPad1X+2] = matrix[prevPad1Y][prevPad1X+3] = 0;
	matrix[15-prevPad2Y][prevPad2X+1] = matrix[15-prevPad2Y][prevPad2X+2] = matrix[15-prevPad2Y][prevPad2X+3] = 0;
	matrix[prevBallY][prevBallX] = 0;
}
void storePrevStates(){
	prevPad1X = pad1X;
	prevPad1Y = pad1Y;
	prevPad2X = pad2X;
	prevPad2Y = pad2Y;
 
	prevBallX = ballX;
	prevBallY = ballY;
}
void printPad(){
	if(pad1X < 0) pad1X = 0;
	else if(pad1X > 3) pad1X = 3;
	if(pad2X < 0) pad2X = 0;
	else if(pad2X > 3) pad2X = 3;
 
	if(pad1Y < 0) pad1Y = 0;
	else if(pad1Y > 3) pad1Y = 3;
 
	if(pad2Y < 0) pad2Y = 0;
	else if(pad2Y > 3) pad2Y = 3;
 
	matrix[pad1Y][pad1X+1] = matrix[pad1Y][pad1X+2] = matrix[pad1Y][pad1X+3] = 1;
	matrix[15-pad2Y][pad2X+1] = matrix[15-pad2Y][pad2X+2] = matrix[15-pad2Y][pad2X+3] = 1;
}
void pointWon(int player){
	if(player == 1) {
		pointPlayer1++;
		if(pointPlayer1 == GAME_POINT){
			//ekhane onek kichu hobe :D
			for(int i = 0; i < 10; i++){
				PORTC |= 1 << PINC2;
				_delay_ms(100);
				PORTC &= ~(1 << PINC2);
				_delay_ms(100);
			}
			state = GAME_OVER;
			printInLCD("PLAYER 1 WON");
			_delay_ms(2000);
			init2();
			}else{
			init1(1);
			state = PLAYING;
		}
		dy = 1;
		points[4] = pointPlayer1 + '0';
		PORTC |= 1 << PINC2;
	}
	else if(player == 2) {
		pointPlayer2++;
		if(pointPlayer2 == GAME_POINT){
 
			for(int i = 0; i < 10; i++){
				PORTC |= 1 << PINC3;
				_delay_ms(100);
				PORTC &= ~(1 << PINC3);
				_delay_ms(100);
			}
			//ekhane onek kichu hobe :D
			state = GAME_OVER;
			printInLCD("PLAYER 2 WON");
			_delay_ms(2000);
			init2();
			}else{
 
 
			init1(2);
			state = PLAYING;
		}
		dy = -1;
		points[11] = pointPlayer2 + '0';
		PORTC |= 1 << PINC3;
	}
	printInLCD(points);
	removePrevBallAndPads();
	_delay_ms(250);
	PORTC &= ~(1 << PINC2);
	PORTC &= ~(1 << PINC3);
}
 
void checkCollision(){
	if(ballY == 0){
		//cout<<"point for 2 check korbe!"<<endl;
		//cout<<"Player 1 missed!"<<endl;
		pointWon(1);
	}
	else if(ballY == 15){
		//cout<<"point for 2 check korbe!"<<endl;
		//cout<<"Player 2 missed!"<<endl;
		pointWon(2);
	}
	else if(ballY == pad1Y+1){
		//cout<<"pad1 check korbe!"<<endl;
		//cout<<"ballX: "<<ballX<<endl;
		//cout<<"pad1X: "<<pad1X<<endl;
		int px = (ballX - pad1X-2);
		//cout<<"PX: "<<px<<endl;
		if(px > 1 || px < -1) {
			//cout<<"ki ar korar return kori!"<<endl;
			return;
		}
		dx = px;
		if(ballX == 1) dx = 1;
		if(ballX == 6) dx = -1;
		dy = 1;
		}else if(ballY == 15-(pad2Y+1)){
		//cout<<"pad2 check korbe!"<<endl;
		int px = (ballX-2-pad2X);
		//cout<<"ballX: "<<ballX<<endl;
		//cout<<"pad2X: "<<pad2X<<endl;
		//cout<<"PX: "<<px<<endl;
		if(px > 1 || px < -1){
			//cout<<"ki ar korar, return kori!"<<endl;
			return;
		}
		dx = px;
		if(ballX == 1) dx = 1;
		if(ballX == 6) dx = -1;
		dy = -1;
	}
}
void moveBall(int check = true){
	int maxPoint = pointPlayer2 > pointPlayer1 ? pointPlayer2 : pointPlayer1;
	int CYC = 50;
	if(maxPoint == 0) {
		CYC = 50;
	}
	else if(maxPoint == 1){
		CYC = 40;
	}
	else {
		CYC = 30;
	}
	if(cycCount != CYC){
		return;
	}
	ballX = prevBallX + dx;
	ballY = prevBallY + dy;
	if(ballX == 6) dx *= -1;
	if(ballX == 1) dx *= -1;
	cycCount = 0;
}
void printBall(){
	matrix[ballY][ballX] = 1;
}
 
void printMatrixTorBap(){
	for(int i=0; i<=15; i++){
		PORTB = i;
		PORTA = 0;
		for(int j=0; j<8; j++){
			if(matrix[i][j] == 1) PORTA = PORTA | 1<<j;
		}
		_delay_us(100);
	}
}
 
void printMatrix(){
	//printStatus();
	removePrevBallAndPads();
	printPad();
	printBall();
	// printStatus();
	/*
	for(int i=0; i<16; i++){
		for(int j=0; j<8; j++){
			if(matrix[i][j] == 1) //printf("#");
			else if(matrix[i][j] == 2) //printf("-");
			else if(matrix[i][j] == 3) //printf("*");
			else //printf(" ");
		}
		//printf("\n");
	}*/
	printMatrixTorBap();
	storePrevStates();
	checkCollision();
	//printStatus();
}
 
 
void startPlay(){
	if(ballY == 1){
		dy = 1;
		dx = 2 - pad1X;
	}
	else if(ballY == 14){
		dy = -1;
		dx = 2-pad2X;
	}
	if(dx > 1 || dy < -1) dx = 0;
	moveBall(false);
}
 
void printInLCD(char a[]){
	lcd_clrscr();
	lcd_gotoxy(0, 0);
	lcd_puts(a);
}
void startTotalGame(){
	init2();
	state = PLAYING;
	startPlay();
}
 
int main(void)
{
	MCUCSR = (1<<JTD);
	MCUCSR = (1<<JTD);
 
	DDRA = 0b11111111;
	DDRB = 0b11111111;
	DDRC = 0b11111111;
	DDRD = 0b11110011;
 
    lcd_init(LCD_ON_DISPLAY);
    lcd_backlight(0);
    _delay_ms(200);
    lcd_backlight(1);
    _delay_ms(200);
	printInLCD(points);
 
	GICR |= 1<<INT1;
	GICR |= 1<<INT0;
	MCUCR &= ~(1<<ISC11);
	MCUCR |= 1<<ISC10;
	MCUCR &= ~(1<<ISC01);
	MCUCR |= 1<<ISC00;
 
	sei();
	state = GAME_OVER;
    pad2X = pad1X = 3;
	while (1)
    {
		if(cycCount == inp1){
			PORTD |= 1<<PIND0;
			_delay_us(15);
			PORTD &= ~(1<<PIND0);
			count_a = pulse1/100;
			if(count_a > 9){
				count_a = 9;
			}
			pad1X = count_a / 3;
		}
		if(cycCount == inp2){
			PORTD |= 1<<PIND1;
			_delay_us(15);
			PORTD &= ~(1<<PIND1);
			count_b = pulse2/100;
			if(count_b > 9){
				count_b = 9;
			}
			pad2X = count_b / 3;
		}
		cycCount++;
		printMatrix();
		if(state != GAME_OVER){
			moveBall();
		}else if(state == GAME_OVER) {
			if(pad2X == 0 && pad1X == 0){
				startTotalGame();
			}
		}
 
    }
}
 
ISR(INT0_vect)
{
	if(trig1 == 0)
	{
		TCCR1B |= 1<<CS10;
		trig1 = 1;
	}
	else
	{
		TCCR1B = 0;
		pulse1 = TCNT1;
		TCNT1 = 0;
		trig1 = 0;
	}
}
 
ISR(INT1_vect)
{
	if(trig2 == 0)
	{
		TCCR1B |= 1<<CS10;
		trig2 = 1;
	}
	else
	{
		TCCR1B = 0;
		pulse2 = TCNT1;
		TCNT1 = 0;
		trig2 = 0;
	}
}