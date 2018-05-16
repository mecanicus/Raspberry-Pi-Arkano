
#include "arkanoPi_1.h"
#include "fsm.h"


static /*volatile*/ tipo_juego juego;

//------------------------------------------------------
// FUNCIONES DE ACCION
//------------------------------------------------------
enum fsm_state {           //Se definen los 7 estados posibles
	WAIT_START=0,
	WAIT_PUSH=1,
	WAIT_END=2,
	WAIT_PAUSE=3,
	WAIT_PAUSE_VELOCIDAD=4,
	WAIT_PAUSE_VIDAS=5,
	WAIT_END_DERROTA=6,
	WAIT_CONTRASENA=7,
};
int columna=0;
int debounceTime=DEBOUNCE_TIME;
static tmr_t* tmrt;		// Temporizador para pintar la matriz
static tmr_t* tmrp;  	// Temporizador de la pelota
static tmr_t* tmrm;  	// Temporizador de la variable menosDe100ms para la doble pulsaci髇
static tmr_t* tmrs;		// Temporizador de sonido
static tmr_t* tmra;		// Temporizador del acelerometro
static tmr_t* tmrz;		// Temporizador del teclado matricial
int velocidadPelota = 1000;  // Velocidad de la pelota con la que empieza el juego
int numeroVidas=1;   // Numero de vidas con los que empieza el juego
int menosDe100ms=0;  // Variable que se pone a 1 si han pasado menos de 100 ms
int temporizador=0;
// Se escriben los posibles mensajes a mostrar por pantalla
static tipo_pantalla mensaje_suma = { {
					{0,0,1,1,0,0,0},
					{0,0,0,1,1,0,0},
					{0,0,0,0,1,1,0},
					{0,0,0,0,1,1,0},
					{0,0,0,1,1,0,0},
					{0,0,1,1,0,0,0},
					{0,0,0,0,0,0,0},
					{0,0,0,1,0,0,0},
					{0,0,1,1,1,0,0},
					{0,0,0,1,0,0,0},
			} };
static tipo_pantalla mensaje_resta = { {
					{0,0,1,1,0,0,0},
					{0,0,0,1,1,0,0},
					{0,0,0,0,1,1,0},
					{0,0,0,0,1,1,0},
					{0,0,0,1,1,0,0},
					{0,0,1,1,0,0,0},
					{0,0,0,0,0,0,0},
					{0,0,0,1,0,0,0},
					{0,0,0,1,0,0,0},
					{0,0,0,1,0,0,0},

			} };
static tipo_pantalla mensaje_velocidadPelota = { {
					{0,0,0,0,0,0,0},
					{0,0,1,1,0,0,0},
					{0,0,0,1,1,0,0},
					{0,0,0,0,1,1,0},
					{0,0,0,0,0,1,1},
					{0,0,0,0,0,1,1},
					{0,0,0,0,1,1,0},
					{0,0,0,1,1,0,0},
					{0,0,1,1,0,0,0},
					{0,0,0,0,0,0,0},
			} };
static tipo_pantalla mensaje_vidas = { {
					{0,0,0,0,0,0,0},
					{0,0,0,0,0,0,0},
					{0,1,1,1,1,0,0},
					{1,1,0,0,0,1,0},
					{0,0,1,0,0,0,1},
					{0,0,1,0,0,0,1},
					{1,1,0,0,0,1,0},
					{0,1,1,1,1,0,0},
					{0,0,0,0,0,0,0},
					{0,0,0,0,0,0,0},
			} };
static tipo_pantalla mensaje_suma_vida = { {
					{0,0,1,1,0,0,0},
					{0,1,1,0,1,1,0},
					{0,0,0,1,0,1,1},
					{0,0,0,1,0,1,1},
					{0,1,1,0,1,1,0},
					{0,0,1,1,0,0,0},
					{0,0,0,1,0,0,0},
					{0,0,1,1,1,0,0},
					{0,0,0,1,0,0,0},
			} };
static tipo_pantalla mensaje_resta_vida = { {
					{0,0,1,1,0,0,0},
					{0,1,1,0,1,1,0},
					{0,0,0,1,0,1,1},
					{0,0,0,1,0,1,1},
					{0,1,1,0,1,1,0},
					{0,0,1,1,0,0,0},
					{0,0,0,0,0,0,0},
					{0,0,0,1,0,0,0},
					{0,0,0,1,0,0,0},
			} };
static tipo_pantalla mensaje_final_victoria = { {
					{0,0,0,0,0,0,0},
					{0,0,0,0,0,0,0},
					{0,0,1,0,0,1,0},
					{0,0,0,1,1,0,0},
					{0,1,1,1,1,1,0},
					{0,0,0,0,0,0,0},
					{0,1,1,1,1,1,0},
					{0,1,0,0,0,1,0},
					{0,1,1,1,1,1,0},
			} };

static tipo_pantalla mensaje_final_derrota = { {
					{0,0,0,0,0,0,0},
					{0,0,0,0,0,1,0},
					{1,1,0,0,1,1,0},
					{1,1,0,0,1,0,0},
					{0,0,0,1,1,0,0},
					{0,0,0,1,1,0,0},
					{1,1,0,0,1,0,0},
					{1,1,0,0,1,1,0},
					{0,0,0,0,0,1,0},
					{0,0,0,0,0,0,0},
			} };
static int matrizTeclado[4][4] = {
			{1,2,3,0},
			{4,5,6,0},
			{7,8,9,0},
			{0,0,0,0},
	};


int col[7] = {0,1,2,3,4,7,23};
int contrasena[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int posicion=0;
int columnaT=0;
int filaTeclado [4]= {5,6,12,13};
volatile int flags = 0;   //Variable donde se pondr醤 los bits a unos en funci髇 de la flag concreta activada
void delay_until (unsigned int next) {
	unsigned int now = millis();

	if (next > now) {
		delay (next - now);
	}
}
int CompruebaTeclaPausa (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);  //Bloqueamos flags para evitar problemas de concurrencia
	result = (flags & FLAG_PAUSA);  // Se comprueba el flag
	piUnlock (FLAGS_KEY);  //Desbloqueamos flags al dejar de acceder a ella
	return result;
}
int CompruebaContrasena(fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);  //Bloqueamos flags para evitar problemas de concurrencia
	result = (flags & FLAG_CONTRASENA);  // Se comprueba el flag
	piUnlock (FLAGS_KEY);  //Desbloqueamos flags al dejar de acceder a ella
	return result;
}
int CompruebaTeclaPulsada (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);  //Bloqueamos flags para evitar problemas de concurrencia
	result = (flags & FLAG_TECLA);  // Se comprueba el flag
	piUnlock (FLAGS_KEY);  //Desbloqueamos flags al dejar de acceder a ella
	return result;
}

int CompruebaTeclaPelota (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);  //Bloqueamos flags para evitar problemas de concurrencia
	result = (flags & FLAG_PELOTA);  // Se comprueba el flag
	piUnlock (FLAGS_KEY);//Desbloqueamos flags al dejar de acceder a ella

	return result;
}


int CompruebaTeclaRaquetaIzquierda (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);  //Bloqueamos flags para evitar problemas de concurrencia
	result = (flags & FLAG_RAQUETA_IZQUIERDA);  // Se comprueba el flag
	piUnlock (FLAGS_KEY);  //Desbloqueamos flags al dejar de acceder a ella


	return result;
}

int CompruebaTeclaRaquetaDerecha (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);  //Bloqueamos flags para evitar problemas de concurrencia
	result = (flags & FLAG_RAQUETA_DERECHA);  // Se comprueba el flag
	piUnlock (FLAGS_KEY);  //Desbloqueamos flags al dejar de acceder a ella


	return result;
}
int CompruebaFinalJuego (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);  //Bloqueamos flags para evitar problemas de concurrencia
	result = (flags & FLAG_FINAL_JUEGO); // Se comprueba el flag
	piUnlock (FLAGS_KEY);  //Desbloqueamos flags al dejar de acceder a ella

	return result;
}
int CompruebaFinalJuegoDerrota (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);   //Bloqueamos flags para evitar problemas de concurrencia
	result = (flags & FLAG_FINAL_JUEGO_DERROTA);  //Se comprueba el flag
	piUnlock (FLAGS_KEY);  //Desbloqueamos flags al dejar de acceder a ella

	return result;
}
int CompruebaDoblePulsacion (fsm_t* this) {
	int result;

	piLock (FLAGS_KEY);   //Bloqueamos flags para evitar problemas de concurrencia
	result = (flags & FLAG_DOBLE_PULSACION);  //Se comprueba el flag
	piUnlock (FLAGS_KEY);  //Desbloqueamos flags al dejar de acceder a ella

	return result;
}


// void InicializaJuego (void): funcion encargada de llevar a cabo
// la oportuna inicializaci贸n de toda variable o estructura de datos
// que resulte necesaria para el desarrollo del juego.
void InicializaJuego (fsm_t *fsm) {
	///*nosotros*/
	// A completar por el alumno...
	if(temporizador==0){
		tmr_startms (tmrt, 1);
		temporizador=1;
		tmr_stop (tmrz);
	}

	piLock (FLAGS_KEY);   //Bloqueamos flags para evitar problemas de concurrencia
	flags &= ~FLAG_TECLA;  // Elimina el flag correspondiente
	flags &= ~FLAG_CONTRASENA;  // Elimina el flag correspondiente
	piUnlock (FLAGS_KEY);  //Desbloqueamos flags al dejar de acceder a ella
	piLock (STD_IO_BUFFER_KEY);  //Bloqueamos la pantalla para evitar problemas de concurrencia
	ActualizaPantalla(&juego.arkanoPi);
	PintaPantallaPorTerminal(&juego.arkanoPi.pantalla);
	piUnlock (STD_IO_BUFFER_KEY);
}

// void MueveRaquetaIzquierda (void): funcion encargada de ejecutar
// el movimiento hacia la izquierda contemplado para la raqueta.
// Debe garantizar la viabilidad del mismo mediante la comprobaci贸n
// de que la nueva posici贸n correspondiente a la raqueta no suponga
// que 茅sta rebase o exceda los l铆mites definidos para el 谩rea de juego
// (i.e. al menos uno de los leds que componen la raqueta debe permanecer
// visible durante todo el transcurso de la partida).
void MueveRaquetaIzquierda (fsm_t *fsm) {
	// FC
	piLock (FLAGS_KEY);  //Bloqueamos flags para evitar problemas de concurrencia
	flags &= ~FLAG_RAQUETA_IZQUIERDA;  // Elimina el flag correspondiente
	flags &= ~FLAG_TECLA;  // Elimina el flag correspondiente
	piUnlock (FLAGS_KEY);  //Desbloqueamos flags al dejar de acceder a ella
	if(juego.arkanoPi.raqueta.x>-2){  // Si no esta en el limite movemos la raqueta a la izquierda
		juego.arkanoPi.raqueta.x--;
	}
	piLock (STD_IO_BUFFER_KEY);  //Bloqueamos la pantalla para evitar problemas de concurrencia
	ActualizaPantalla(&juego.arkanoPi);
	PintaPantallaPorTerminal(&juego.arkanoPi.pantalla);
	piUnlock (STD_IO_BUFFER_KEY);  //Se desbloquea el recurso compartido pantalla
}

// void MueveRaquetaDerecha (void): funci贸n similar a la anterior
// encargada del movimiento hacia la derecha.
void MueveRaquetaDerecha (fsm_t *fsm) {

	piLock (FLAGS_KEY);   //Bloqueamos flags para evitar problemas de concurrencia
	flags &= ~FLAG_RAQUETA_DERECHA;  // Elimina el flag correspondiente
	flags &= ~FLAG_TECLA;  // Elimina el flag correspondiente
	piUnlock (FLAGS_KEY);  //Desbloqueamos flags al dejar de acceder a ella
	if(juego.arkanoPi.raqueta.x<MATRIZ_ANCHO-1){  // Si no esta en el limite movemos la raqueta a la derecha
		juego.arkanoPi.raqueta.x++;
	}
	piLock (STD_IO_BUFFER_KEY);  //Bloqueamos la pantalla para evitar problemas de concurrencia
	ActualizaPantalla(&juego.arkanoPi);
	PintaPantallaPorTerminal(&juego.arkanoPi.pantalla);
	piUnlock (STD_IO_BUFFER_KEY);  //Se desbloquea el recurso compartido pantalla
}

// void MovimientoPelota (void): funci贸n encargada de actualizar la
// posici贸n de la pelota conforme a la trayectoria definida para 茅sta.
// Para ello deber谩 identificar los posibles rebotes de la pelota para,
// en ese caso, modificar su correspondiente trayectoria (los rebotes
// detectados contra alguno de los ladrillos implicar谩n adicionalmente
// la eliminaci贸n del ladrillo). Del mismo modo, deber谩 tambi茅n
// identificar las situaciones en las que se d茅 por finalizada la partida:
// bien porque el jugador no consiga devolver la pelota, y por tanto 茅sta
// rebase el l铆mite inferior del 谩rea de juego, bien porque se agoten
// los ladrillos visibles en el 谩rea de juego.
void MovimientoPelota (fsm_t *fsm) {
	// A completar por el alumno...
	piLock (FLAGS_KEY);   //Bloqueamos flags para evitar problemas de concurrencia
	flags &= ~FLAG_PELOTA;  // Elimina el flag correspondiente
	flags &= ~FLAG_TECLA;   // Elimina el flag correspondiente
	flags &= ~FLAG_DOBLE_PULSACION;   // Elimina el flag correspondiente
	piUnlock (FLAGS_KEY);   //Desbloqueamos flags al dejar de acceder a ella
	if (CalculaLadrillosRestantes(&juego.arkanoPi.ladrillos) == 0){  // Se comprueba si quedan ladrillos sin destruir

			piLock (FLAGS_KEY);
			flags |= FLAG_FINAL_JUEGO;  // Se activa el flag que se馻liza el final del juego
			piUnlock (FLAGS_KEY);
	}
	switch (juego.arkanoPi.pelota.y){
	case 0://si la pelota esta en la fila 0
		if (juego.arkanoPi.pelota.xv == 0){ // si esta moviendose en vertical entonces debajo no podra tener un ladrillo
			juego.arkanoPi.pelota.yv = 1;	//la pelota solo podra bajar
			juego.arkanoPi.pelota.y = 1;
		}
		if(juego.arkanoPi.pelota.xv == 1){		//si la pelota se mueve hacia la derecha
			if(juego.arkanoPi.pelota.x == 9){	//compromprobamos si choca contra la esquina superior derecha sera磇mposible que exista un ladrillo abajo a su izquierda
				juego.arkanoPi.pelota.xv = -1;
				juego.arkanoPi.pelota.yv = 1;	// se le coloca abajo a la izquierda con direccion correcta
				juego.arkanoPi.pelota.x = 8;
				juego.arkanoPi.pelota.y = 1;
			}else if(juego.arkanoPi.pantalla.matriz[juego.arkanoPi.pelota.x+1][juego.arkanoPi.pelota.y+1] == 1){ //comprobamos si en la posicion inferor derecha hay ladrillo
				juego.arkanoPi.ladrillos.matriz[juego.arkanoPi.pelota.x+1][juego.arkanoPi.pelota.y+1] = 0; //quitamos el ladrillo
				softToneWrite (PIN, 500) ; // Suena que se destruye un ladrillo
				tmr_startms (tmrs, 200);
				juego.arkanoPi.pelota.xv = -1;	//la pelota se mantendra en su misma posicon pero cambiara su direccion en x

			}else{ 								// si hay via libre
				juego.arkanoPi.pelota.yv = 1;
				juego.arkanoPi.pelota.x++;		//ocupamos posicion y cogemos direccion correcta
				juego.arkanoPi.pelota.y = 1;
			}
		}

		if(juego.arkanoPi.pelota.xv == -1){ 	//Analogo al de xv = 1
			if(juego.arkanoPi.pelota.x == 0){
				juego.arkanoPi.pelota.xv = 1;
				juego.arkanoPi.pelota.yv = 1;
				juego.arkanoPi.pelota.x = 1;
				juego.arkanoPi.pelota.y = 1;
			}else if(juego.arkanoPi.pantalla.matriz[juego.arkanoPi.pelota.x-1][juego.arkanoPi.pelota.y+1] == 1){
				juego.arkanoPi.ladrillos.matriz[juego.arkanoPi.pelota.x-1][juego.arkanoPi.pelota.y+1] = 0;
				softToneWrite (PIN, 500) ;  // Suena que se destruye un ladrillo
				tmr_startms (tmrs, 200);
				juego.arkanoPi.pelota.xv = 1;

			}else{
				juego.arkanoPi.pelota.yv = 1;
				juego.arkanoPi.pelota.x--;
				juego.arkanoPi.pelota.y = 1;
			}
		}
		break;


	case 1:
		if(juego.arkanoPi.pelota.xv == 0){	// movimiento vertical
			if(juego.arkanoPi.pelota.yv == 1){	//hacia abajo
				juego.arkanoPi.pelota.y = 2;	//baja
			}else{								//si va hacia arriba
				if (juego.arkanoPi.pantalla.matriz[juego.arkanoPi.pelota.x][juego.arkanoPi.pelota.y-1] == 1){ //comprobamos si hay ladrillo
					juego.arkanoPi.ladrillos.matriz[juego.arkanoPi.pelota.x][juego.arkanoPi.pelota.y-1] = 0;	  //elimiamos ladrillo
					softToneWrite (PIN, 500) ;  // Suena que se destruye un ladrillo
					tmr_startms (tmrs, 200);
					juego.arkanoPi.pelota.y = 2;	//la pelota baja
					juego.arkanoPi.pelota.yv = 1;
				}else{
					juego.arkanoPi.pelota.y = 0;  // si no hay ladrillo la pelota sube
				}
			}
		}else if (juego.arkanoPi.pelota.yv == -1){ // si vamos hacia arriba
			if (juego.arkanoPi.pelota.xv == 1){	   // y hacia la derecha
				if(juego.arkanoPi.pelota.x == 9){	// miramos si hay pared o no
					if(juego.arkanoPi.pantalla.matriz[juego.arkanoPi.pelota.x-1][juego.arkanoPi.pelota.y-1] == 1){ //
						juego.arkanoPi.ladrillos.matriz[juego.arkanoPi.pelota.x-1][juego.arkanoPi.pelota.y-1] = 0;
						softToneWrite (PIN, 500) ;  // Suena que se destruye un ladrillo
						tmr_startms (tmrs, 200);
						juego.arkanoPi.pelota.x--;
						juego.arkanoPi.pelota.y++;
						juego.arkanoPi.pelota.xv = -1;
						juego.arkanoPi.pelota.yv = 1;
					}else{
						juego.arkanoPi.pelota.x--;
						juego.arkanoPi.pelota.y--;
						juego.arkanoPi.pelota.xv = -1;
						juego.arkanoPi.pelota.yv = -1;
					}
				}else if(juego.arkanoPi.pantalla.matriz[juego.arkanoPi.pelota.x+1][juego.arkanoPi.pelota.y-1] == 1){
					juego.arkanoPi.ladrillos.matriz[juego.arkanoPi.pelota.x+1][juego.arkanoPi.pelota.y-1] = 0;
					softToneWrite (PIN, 500) ;  // Suena que se destruye un ladrillo
					tmr_startms (tmrs, 200);
					juego.arkanoPi.pelota.x++;
					juego.arkanoPi.pelota.y++;
					juego.arkanoPi.pelota.xv = 1;
					juego.arkanoPi.pelota.yv = 1;
				}else{
					juego.arkanoPi.pelota.x++;
					juego.arkanoPi.pelota.y--;
				}
			}else if (juego.arkanoPi.pelota.xv == -1){
				if(juego.arkanoPi.pelota.x == 0){
					if(juego.arkanoPi.pantalla.matriz[juego.arkanoPi.pelota.x+1][juego.arkanoPi.pelota.y-1] == 1){
						juego.arkanoPi.ladrillos.matriz[juego.arkanoPi.pelota.x+1][juego.arkanoPi.pelota.y-1] = 0;
						softToneWrite (PIN, 500) ;  // Suena que se destruye un ladrillo
						tmr_startms (tmrs, 200);
						juego.arkanoPi.pelota.x++;
						juego.arkanoPi.pelota.y++;
						juego.arkanoPi.pelota.xv = 1;
						juego.arkanoPi.pelota.yv = 1;
					}else{
						juego.arkanoPi.pelota.x++;
						juego.arkanoPi.pelota.y--;
						juego.arkanoPi.pelota.xv = 1;
						juego.arkanoPi.pelota.yv = -1;
					}
				}else if(juego.arkanoPi.pantalla.matriz[juego.arkanoPi.pelota.x-1][juego.arkanoPi.pelota.y-1] == 1){
					juego.arkanoPi.ladrillos.matriz[juego.arkanoPi.pelota.x-1][juego.arkanoPi.pelota.y-1] = 0;
					softToneWrite (PIN, 500) ;  // Suena que se destruye un ladrillo
					tmr_startms (tmrs, 200);
					juego.arkanoPi.pelota.x--;
					juego.arkanoPi.pelota.y++;
					juego.arkanoPi.pelota.xv = -1;
					juego.arkanoPi.pelota.yv = 1;
				}else{
					juego.arkanoPi.pelota.x--;
					juego.arkanoPi.pelota.y--;
				}
			}
		}else{
			if (juego.arkanoPi.pelota.xv == 1){
				if(juego.arkanoPi.pelota.x == 9){
					juego.arkanoPi.pelota.x--;
					juego.arkanoPi.pelota.y++;
					juego.arkanoPi.pelota.xv = -1;
					juego.arkanoPi.pelota.yv = 1;
				}else{
					juego.arkanoPi.pelota.x++;
					juego.arkanoPi.pelota.y++;
				}
			}else if (juego.arkanoPi.pelota.xv == -1){
				if(juego.arkanoPi.pelota.x == 0){
					juego.arkanoPi.pelota.x++;
					juego.arkanoPi.pelota.y++;
					juego.arkanoPi.pelota.xv = 1;
					juego.arkanoPi.pelota.yv = 1;
				}else{
					juego.arkanoPi.pelota.x--;
					juego.arkanoPi.pelota.y++;
				}
			}
			break;

	case 2:
		if (juego.arkanoPi.pelota.yv == -1){
			if(juego.arkanoPi.pelota.xv == 0){
				if (juego.arkanoPi.pantalla.matriz[juego.arkanoPi.pelota.x][juego.arkanoPi.pelota.y-1] == 1){ //comprobamos si hay ladrillo
					juego.arkanoPi.ladrillos.matriz[juego.arkanoPi.pelota.x][juego.arkanoPi.pelota.y-1] = 0;	  //elimiamos ladrillo
					softToneWrite (PIN, 500) ;  // Suena que se destruye un ladrillo
					tmr_startms (tmrs, 200);
					juego.arkanoPi.pelota.y = 3;	//la pelota baja
					juego.arkanoPi.pelota.yv = 1;
				}else{
					juego.arkanoPi.pelota.y = 1;
				}
			}else if (juego.arkanoPi.pelota.xv == 1){
				if (juego.arkanoPi.pelota.x == 9){
					if (juego.arkanoPi.pantalla.matriz[juego.arkanoPi.pelota.x-1][juego.arkanoPi.pelota.y-1] == 1){
						juego.arkanoPi.ladrillos.matriz[juego.arkanoPi.pelota.x-1][juego.arkanoPi.pelota.y-1] = 0;
						softToneWrite (PIN, 500) ;  // Suena que se destruye un ladrillo
						tmr_startms (tmrs, 200);
						juego.arkanoPi.pelota.x--;
						juego.arkanoPi.pelota.y++;
						juego.arkanoPi.pelota.xv = -1;
						juego.arkanoPi.pelota.yv = 1;
					}else{
						juego.arkanoPi.pelota.x--;
						juego.arkanoPi.pelota.y--;
						juego.arkanoPi.pelota.xv = -1;
						juego.arkanoPi.pelota.yv = -1;
					}
				}else if (juego.arkanoPi.pantalla.matriz[juego.arkanoPi.pelota.x+1][juego.arkanoPi.pelota.y-1] == 1){ //comprobamos si hay ladrillo
					juego.arkanoPi.ladrillos.matriz[juego.arkanoPi.pelota.x+1][juego.arkanoPi.pelota.y-1] = 0;	  //elimiamos ladrillo
					softToneWrite (PIN, 500) ;  // Suena que se destruye un ladrillo
					tmr_startms (tmrs, 200);
					juego.arkanoPi.pelota.y = 3;
					juego.arkanoPi.pelota.yv = 1;
					juego.arkanoPi.pelota.x++;
				}else{
					juego.arkanoPi.pelota.y = 1;
					juego.arkanoPi.pelota.x++;
				}
			}else{
				if (juego.arkanoPi.pelota.x == 0){
					if (juego.arkanoPi.pantalla.matriz[juego.arkanoPi.pelota.x+1][juego.arkanoPi.pelota.y-1] == 1){
						juego.arkanoPi.ladrillos.matriz[juego.arkanoPi.pelota.x+1][juego.arkanoPi.pelota.y-1] = 0;
						softToneWrite (PIN, 500) ;  // Suena que se destruye un ladrillo
						tmr_startms (tmrs, 200);
						juego.arkanoPi.pelota.x++;
						juego.arkanoPi.pelota.y++;
						juego.arkanoPi.pelota.xv = +1;
						juego.arkanoPi.pelota.yv = 1;
					}else{
						juego.arkanoPi.pelota.x++;
						juego.arkanoPi.pelota.y--;
						juego.arkanoPi.pelota.xv = +1;
						juego.arkanoPi.pelota.yv = -1;
					}
				}else if (juego.arkanoPi.pantalla.matriz[juego.arkanoPi.pelota.x-1][juego.arkanoPi.pelota.y-1] == 1){ //comprobamos si hay ladrillo
					juego.arkanoPi.ladrillos.matriz[juego.arkanoPi.pelota.x-1][juego.arkanoPi.pelota.y-1] = 0;	  //elimiamos ladrillo
					softToneWrite (PIN, 500) ;  // Suena que se destruye un ladrillo
					tmr_startms (tmrs, 200);
					juego.arkanoPi.pelota.y = 3;
					juego.arkanoPi.pelota.yv = 1;
					juego.arkanoPi.pelota.x--;
				}else{
					juego.arkanoPi.pelota.y = 1;
					juego.arkanoPi.pelota.x--;
				}
			}
		}else{
			if (juego.arkanoPi.pelota.x == 9 && juego.arkanoPi.pelota.xv == 1){
				juego.arkanoPi.pelota.x--;
				juego.arkanoPi.pelota.y++;
				juego.arkanoPi.pelota.xv = -1;
			}else if (juego.arkanoPi.pelota.x == 0 && juego.arkanoPi.pelota.xv == -1){
				juego.arkanoPi.pelota.x++;
				juego.arkanoPi.pelota.y++;
				juego.arkanoPi.pelota.xv = +1;
			}else{
				juego.arkanoPi.pelota.x += juego.arkanoPi.pelota.xv;
				juego.arkanoPi.pelota.y += juego.arkanoPi.pelota.yv;
			}

		}
		break;

	case 3:

		if (juego.arkanoPi.pelota.x == 9 && juego.arkanoPi.pelota.xv == 1){
			juego.arkanoPi.pelota.x--;
			juego.arkanoPi.pelota.y += juego.arkanoPi.pelota.yv;
			juego.arkanoPi.pelota.xv = -1;
		}else if (juego.arkanoPi.pelota.x == 0 && juego.arkanoPi.pelota.xv == -1){
			juego.arkanoPi.pelota.x++;
			juego.arkanoPi.pelota.y += juego.arkanoPi.pelota.yv;
			juego.arkanoPi.pelota.xv = +1;
		}else{
			juego.arkanoPi.pelota.x += juego.arkanoPi.pelota.xv;
			juego.arkanoPi.pelota.y += juego.arkanoPi.pelota.yv;
		}

		break;

	case 4:

		if (juego.arkanoPi.pelota.x == 9 && juego.arkanoPi.pelota.xv == 1){
			juego.arkanoPi.pelota.x--;
			juego.arkanoPi.pelota.y += juego.arkanoPi.pelota.yv;
			juego.arkanoPi.pelota.xv = -1;
		}else if (juego.arkanoPi.pelota.x == 0 && juego.arkanoPi.pelota.xv == -1){
			juego.arkanoPi.pelota.x++;
			juego.arkanoPi.pelota.y += juego.arkanoPi.pelota.yv;
			juego.arkanoPi.pelota.xv = +1;
		}else{
			juego.arkanoPi.pelota.x += juego.arkanoPi.pelota.xv;
			juego.arkanoPi.pelota.y += juego.arkanoPi.pelota.yv;
		}

		break;


	case 5:
		if(juego.arkanoPi.pelota.yv == -1){
			if (juego.arkanoPi.pelota.x == 9 && juego.arkanoPi.pelota.xv == 1){
				juego.arkanoPi.pelota.x--;
				juego.arkanoPi.pelota.y += juego.arkanoPi.pelota.yv;
				juego.arkanoPi.pelota.xv = -1;
			}else if (juego.arkanoPi.pelota.x == 0 && juego.arkanoPi.pelota.xv == -1){
				juego.arkanoPi.pelota.x++;
				juego.arkanoPi.pelota.y += juego.arkanoPi.pelota.yv;
				juego.arkanoPi.pelota.xv = +1;
			}else{
				juego.arkanoPi.pelota.x += juego.arkanoPi.pelota.xv;
				juego.arkanoPi.pelota.y += juego.arkanoPi.pelota.yv;
			}
		}else{
			if (juego.arkanoPi.pelota.xv ==  0){
				if(juego.arkanoPi.pelota.x == juego.arkanoPi.raqueta.x+1){
					softToneWrite (PIN, 200) ;  // Suena que se golpea raqueta
					tmr_startms (tmrs, 200);
					juego.arkanoPi.pelota.y--;
					juego.arkanoPi.pelota.yv = -1;
				}else if(juego.arkanoPi.pelota.x == juego.arkanoPi.raqueta.x){
					softToneWrite (PIN, 200) ;  // Suena que se golpea raqueta
					tmr_startms (tmrs, 200);
					if (juego.arkanoPi.pelota.x == 0){
						juego.arkanoPi.pelota.x++;
						juego.arkanoPi.pelota.y--;
						juego.arkanoPi.pelota.xv = +1;
						juego.arkanoPi.pelota.yv = -1;
					}else{
						juego.arkanoPi.pelota.x--;
						juego.arkanoPi.pelota.y--;
						juego.arkanoPi.pelota.xv = -1;
						juego.arkanoPi.pelota.yv = -1;
					}
				}else if(juego.arkanoPi.pelota.x == juego.arkanoPi.raqueta.x+2){
					softToneWrite (PIN, 200) ;  // Suena que se golpea raqueta
					tmr_startms (tmrs, 200);
					if (juego.arkanoPi.pelota.x == 9){
						juego.arkanoPi.pelota.x--;
						juego.arkanoPi.pelota.y--;
						juego.arkanoPi.pelota.xv = -1;
						juego.arkanoPi.pelota.yv = -1;
					}else{
						juego.arkanoPi.pelota.x++;
						juego.arkanoPi.pelota.y--;
						juego.arkanoPi.pelota.xv = 1;
						juego.arkanoPi.pelota.yv = -1;
					}
				}else{
					juego.arkanoPi.pelota.y++;
				}
			}else if(juego.arkanoPi.pelota.xv == 1){
				if(juego.arkanoPi.pelota.x == juego.arkanoPi.raqueta.x-1){
					softToneWrite (PIN, 200) ;  // Suena que se golpea raqueta
					tmr_startms (tmrs, 200);
					if(juego.arkanoPi.pelota.x != 0){
						juego.arkanoPi.pelota.y--;
						juego.arkanoPi.pelota.yv = -1;
						juego.arkanoPi.pelota.xv = -1;
						juego.arkanoPi.pelota.x--;
					}else{
						juego.arkanoPi.pelota.y--;
						juego.arkanoPi.pelota.yv = -1;
						juego.arkanoPi.pelota.xv = +1;
						juego.arkanoPi.pelota.x++;
					}
				}else if(juego.arkanoPi.pelota.x == juego.arkanoPi.raqueta.x){
					softToneWrite (PIN, 200) ;   // Suena que se golpea raqueta
					tmr_startms (tmrs, 200);
					juego.arkanoPi.pelota.y--;
					juego.arkanoPi.pelota.yv = -1;
					juego.arkanoPi.pelota.xv = 0;
				}else if(juego.arkanoPi.pelota.x == juego.arkanoPi.raqueta.x+1){
					softToneWrite (PIN, 200) ;   // Suena que se golpea raqueta
					tmr_startms (tmrs, 200);
					if (juego.arkanoPi.pelota.x == 9){
						juego.arkanoPi.pelota.x--;
						juego.arkanoPi.pelota.y--;
						juego.arkanoPi.pelota.xv = -1;
						juego.arkanoPi.pelota.yv = -1;
					}else{
						juego.arkanoPi.pelota.x++;
						juego.arkanoPi.pelota.y--;
						juego.arkanoPi.pelota.xv = 1;
						juego.arkanoPi.pelota.yv = -1;
					}
				}else if(juego.arkanoPi.pelota.x == 9 && juego.arkanoPi.raqueta.x == 7){
					softToneWrite (PIN, 200) ;  // Suena que se golpea raqueta
					tmr_startms (tmrs, 200);
					juego.arkanoPi.pelota.xv = 0;
					juego.arkanoPi.pelota.yv = -1;
					juego.arkanoPi.pelota.y --;
				}else if(juego.arkanoPi.pelota.x == 9 && juego.arkanoPi.raqueta.x == 6){
					softToneWrite (PIN, 200) ;  // Suena que se golpea raqueta
					tmr_startms (tmrs, 200);
					juego.arkanoPi.pelota.xv = -1;
					juego.arkanoPi.pelota.yv = -1;
					juego.arkanoPi.pelota.y --;
					juego.arkanoPi.pelota.x--;
				}else{
					if(juego.arkanoPi.pelota.x==9){
						juego.arkanoPi.pelota.y++;
						juego.arkanoPi.pelota.x--;
					}else{

						juego.arkanoPi.pelota.y++;
						juego.arkanoPi.pelota.x++;
					}
				}
			}else{
				if(juego.arkanoPi.pelota.x == juego.arkanoPi.raqueta.x+3){
					softToneWrite (PIN, 200) ;  // Suena que se golpea raqueta
					tmr_startms (tmrs, 200);
					if(juego.arkanoPi.pelota.x != 9){
						juego.arkanoPi.pelota.y--;
						juego.arkanoPi.pelota.yv = -1;
						juego.arkanoPi.pelota.xv = +1;
						juego.arkanoPi.pelota.x++;
					}else{
						juego.arkanoPi.pelota.y--;
						juego.arkanoPi.pelota.yv = -1;
						juego.arkanoPi.pelota.xv = -1;
						juego.arkanoPi.pelota.x--;
					}
				}else if(juego.arkanoPi.pelota.x == juego.arkanoPi.raqueta.x+2){
					softToneWrite (PIN, 200) ;   // Suena que se golpea raqueta
					tmr_startms (tmrs, 200);
					juego.arkanoPi.pelota.y--;
					juego.arkanoPi.pelota.yv = -1;
					juego.arkanoPi.pelota.xv = 0;
				}else if(juego.arkanoPi.pelota.x == juego.arkanoPi.raqueta.x+1){
					softToneWrite (PIN, 200) ;   // Suena que se golpea raqueta
					tmr_startms (tmrs, 200);
					if (juego.arkanoPi.pelota.x == 0){
						juego.arkanoPi.pelota.x++;
						juego.arkanoPi.pelota.y--;
						juego.arkanoPi.pelota.xv = 1;
						juego.arkanoPi.pelota.yv = -1;
					}else{
						juego.arkanoPi.pelota.x--;
						juego.arkanoPi.pelota.y--;
						juego.arkanoPi.pelota.xv = -1;
						juego.arkanoPi.pelota.yv = -1;
					}
				}else if(juego.arkanoPi.pelota.x == 0 && juego.arkanoPi.raqueta.x == 0){
					softToneWrite (PIN, 200) ;   // Suena que se golpea raqueta
					tmr_startms (tmrs, 200);
					juego.arkanoPi.pelota.xv = 0;
					juego.arkanoPi.pelota.yv = -1;
					juego.arkanoPi.pelota.y --;
				}else if(juego.arkanoPi.pelota.x == 0 && juego.arkanoPi.raqueta.x == 1){
					softToneWrite (PIN, 200) ;   // Suena que se golpea raqueta
					tmr_startms (tmrs, 200);
					juego.arkanoPi.pelota.xv = 1;
					juego.arkanoPi.pelota.yv = -1;
					juego.arkanoPi.pelota.y --;
					juego.arkanoPi.pelota.x++;
				}else{
					if(juego.arkanoPi.pelota.x==0){
						juego.arkanoPi.pelota.y++;
						juego.arkanoPi.pelota.x++;
					}else{

						juego.arkanoPi.pelota.y++;
						juego.arkanoPi.pelota.x--;
					}
				}
			}
		}
		}

	}
	piLock (STD_IO_BUFFER_KEY);
	ActualizaPantalla(&juego.arkanoPi);
	PintaPantallaPorTerminal(&juego.arkanoPi.pantalla);
	piUnlock (STD_IO_BUFFER_KEY);
	if(juego.arkanoPi.pelota.x==0||juego.arkanoPi.pelota.x==9||juego.arkanoPi.pelota.y==0){
		softToneWrite (PIN, 1000) ;  // Suena que se ha chocado con una pared
		tmr_startms (tmrs, 200);
	}
	if(juego.arkanoPi.pelota.y==6){
		softToneWrite (PIN, 200) ;  // Suena que se ha perdido
		tmr_startms (tmrs, 1000);
		piLock (FLAGS_KEY);
		flags |= FLAG_FINAL_JUEGO_DERROTA;
		piUnlock (FLAGS_KEY);

	}

}

// void FinalJuego (void): funci贸n encargada de mostrar en la ventana de
// terminal los mensajes necesarios para informar acerca del resultado del juego.
void FinalJuego (fsm_t *fsm) {
	// A completar por el alumno...
	piLock (FLAGS_KEY);  //Bloqueamos flags para evitar problemas de concurrencia
	flags &= ~FLAG_FINAL_JUEGO;  // Elimina el flag correspondiente
	flags &= ~FLAG_TECLA;   // Elimina el flag correspondiente
	piUnlock (FLAGS_KEY);   //Desbloqueamos flags al dejar de acceder a ella

	printf ("GANASTE %d VIDAS RESTANTES", numeroVidas);  // Imprime por pantalla el numero de vidas
	piLock (STD_IO_BUFFER_KEY);
	PintaMensajeInicialPantalla(&juego.arkanoPi.pantalla, &mensaje_final_victoria);  // Imprime el mensaje de victoria
	piUnlock (STD_IO_BUFFER_KEY);


}
void FinalJuegoDerrota (fsm_t *fsm) {
	piLock (FLAGS_KEY);  //Bloqueamos flags para evitar problemas de concurrencia
	flags &= ~FLAG_FINAL_JUEGO_DERROTA;  // Elimina el flag correspondiente
	flags &= ~FLAG_TECLA;   // Elimina el flag correspondiente
	piUnlock (FLAGS_KEY);   //Desbloqueamos flags al dejar de acceder a ella

	printf ("PERDISTE %d VIDAS RESTANTES", numeroVidas);
	piLock (STD_IO_BUFFER_KEY);
	PintaMensajeInicialPantalla(&juego.arkanoPi.pantalla, &mensaje_final_derrota); //Imprime el mensaje de derrota
	piUnlock (STD_IO_BUFFER_KEY);
}

//void ReseteaJuego (void): funci贸n encargada de llevar a cabo la
// reinicializaci贸n de cuantas variables o estructuras resulten
// necesarias para dar comienzo a una nueva partida.
void ReseteaJuego (fsm_t *fsm) {
	//PintaMensajeInicialPantalla();/*nosotros*/
	// A completar por el alumno...
	piLock (FLAGS_KEY);  //Bloqueamos flags para evitar problemas de concurrencia
	flags &= ~FLAG_FINAL_JUEGO;  // Elimina el flag correspondiente
	flags &= ~FLAG_TECLA;  // Elimina el flag correspondiente
	piUnlock (FLAGS_KEY);  //Desbloqueamos flags al dejar de acceder a ella

		InicializaArkanoPi(&juego.arkanoPi);  // Inicializa el juego entero
		piLock (STD_IO_BUFFER_KEY);
		PintaPantallaPorTerminal(&juego.arkanoPi.pantalla); //Pinta la pantalla del nuevo juego
		piUnlock (STD_IO_BUFFER_KEY);

}
void ReseteaJuegoDerrota (fsm_t *fsm) {
	piLock (FLAGS_KEY);  //Bloqueamos flags para evitar problemas de concurrencia
	flags &= ~FLAG_FINAL_JUEGO_DERROTA;  // Elimina el flag correspondiente
	flags &= ~FLAG_TECLA;  // Elimina el flag correspondiente
	piUnlock (FLAGS_KEY);  //Desbloqueamos flags al dejar de acceder a ella
	numeroVidas--;  // Se resta 1 al numero de vidas
    if(numeroVidas==0){  // Si se han acabado las vidas
    	InicializaArkanoPi(&juego.arkanoPi); //Se resetea el juego
    	piLock (STD_IO_BUFFER_KEY);  // Se bloquea el recurso compartido
    	PintaPantallaPorTerminal(&juego.arkanoPi.pantalla); // Se pinta la pantalla
    	piUnlock (STD_IO_BUFFER_KEY); // Se desbloquea el recurso
    	numeroVidas=1;  // Se restablece el numero de vidas
    }
	else{  // Si quedan vidas restantes

		RegeneraArkanoPi(&juego.arkanoPi); // Se mantienen los ladrillos eliminados
		piLock (STD_IO_BUFFER_KEY);  // Bloqueamos flags para evitar concurrencia
	    PintaPantallaPorTerminal(&juego.arkanoPi.pantalla); // Pinta la pantalla
		piUnlock (STD_IO_BUFFER_KEY);  // Se desbloquea el recurso compartido

	}

}
void PintaPantallaPausa(fsm_t *fsm) {
	static tipo_pantalla mensaje_pausa = { {  // Mensaje de pausa
				{0,0,0,0,0,0,0},
				{0,1,1,1,1,1,0},
				{0,1,1,1,1,1,0},
				{0,1,1,1,1,1,0},
				{0,0,0,0,0,0,0},
				{0,0,0,0,0,0,0},
				{0,1,1,1,1,1,0},
				{0,1,1,1,1,1,0},
				{0,1,1,1,1,1,0},
				{0,0,0,0,0,0,0},
		} };

	piLock (FLAGS_KEY);
	flags &= ~FLAG_PAUSA;
	flags &= ~FLAG_DOBLE_PULSACION;
	piUnlock (FLAGS_KEY);
	piLock (STD_IO_BUFFER_KEY);
	PintaMensajeInicialPantalla(&juego.arkanoPi.pantalla, &mensaje_pausa); // Se pinta el mensaje de pausa
	piUnlock (STD_IO_BUFFER_KEY);
}
void PintaPantallaVelocidad (fsm_t *fsm) {
		piLock (FLAGS_KEY);
		flags &= ~FLAG_PAUSA;
		piUnlock (FLAGS_KEY);
		piLock (STD_IO_BUFFER_KEY);
		PintaMensajeInicialPantalla(&juego.arkanoPi.pantalla, &mensaje_velocidadPelota); // Se pinta el mensaje de velocidad
		piUnlock (STD_IO_BUFFER_KEY);
}
void PintaPantallaVelocidadMenos (fsm_t *fsm) {
			piLock (FLAGS_KEY);
			flags &= ~FLAG_RAQUETA_IZQUIERDA;
			piUnlock (FLAGS_KEY);
			piLock (STD_IO_BUFFER_KEY);
			PintaMensajeInicialPantalla(&juego.arkanoPi.pantalla, &mensaje_suma); // Se pinta que se aumenta la velocidad
			piUnlock (STD_IO_BUFFER_KEY);
			if(velocidadPelota > 250){  // Evita que la velocidad sea 0 y la pelota se quede parada
				velocidadPelota = velocidadPelota - 250;
				if(velocidadPelota < 250){  // Comprueba por seguridad que la velocidad no baja de 250
					velocidadPelota = 250;
				}
			}


}
void PintaPantallaVelocidadMas (fsm_t *fsm) {
			piLock (FLAGS_KEY);
			flags &= ~FLAG_RAQUETA_DERECHA;
			piUnlock (FLAGS_KEY);
			piLock (STD_IO_BUFFER_KEY);
			PintaMensajeInicialPantalla(&juego.arkanoPi.pantalla, &mensaje_resta); // Se pinta que se reduce la velocidad
			piUnlock (STD_IO_BUFFER_KEY);
			if(velocidadPelota < 1500){  // Tope superior para no aumentar la velocidad indefinidamente
				velocidadPelota = velocidadPelota + 250;
				if(velocidadPelota > 1500){
					velocidadPelota = 1500;
				}
			}
}
void PintaPantallaVidas (fsm_t *fsm) {
			piLock (FLAGS_KEY);
			flags &= ~FLAG_PAUSA;
			piUnlock (FLAGS_KEY);
			piLock (STD_IO_BUFFER_KEY);
			PintaMensajeInicialPantalla(&juego.arkanoPi.pantalla, &mensaje_vidas);  // Pinta por pantalla un corazon
			piUnlock (STD_IO_BUFFER_KEY);
	}
void PintaPantallaVidasMas (fsm_t *fsm) {
			piLock (FLAGS_KEY);
			flags &= ~FLAG_RAQUETA_DERECHA;
			piUnlock (FLAGS_KEY);
			piLock (STD_IO_BUFFER_KEY);
			PintaMensajeInicialPantalla(&juego.arkanoPi.pantalla, &mensaje_suma_vida);  // Suma vidas
			piUnlock (STD_IO_BUFFER_KEY);
			numeroVidas =numeroVidas + 1;
}

void PintaPantallaVidasMenos (fsm_t *fsm) {
			piLock (FLAGS_KEY);
			flags &= ~FLAG_RAQUETA_IZQUIERDA;
			piUnlock (FLAGS_KEY);
			piLock (STD_IO_BUFFER_KEY);
			PintaMensajeInicialPantalla(&juego.arkanoPi.pantalla, &mensaje_resta_vida);  // Resta vidas
			piUnlock (STD_IO_BUFFER_KEY);
			if(numeroVidas >1){
				numeroVidas =numeroVidas - 1;
			}
}
//------------------------------------------------------
// FUNCIONES DE INICIALIZACION
//------------------------------------------------------

// int systemSetup (void): procedimiento de configuracion del sistema.
// Realizar谩, entra otras, todas las operaciones necesarias para:
// configurar el uso de posibles librer铆as (e.g. Wiring Pi),
// configurar las interrupciones externas asociadas a los pines GPIO,
// configurar las interrupciones peri贸dicas y sus correspondientes temporizadores,
// crear, si fuese necesario, los threads adicionales que pueda requerir el sistema
int systemSetup (void) {
	//int x = 0;
	wiringPiSetupGpio(); // Se inicializan los pines
	piLock (FLAGS_KEY);
	flags = 0;
	piUnlock (FLAGS_KEY);
	/*x= piThreadCreate(thread_explora_teclado);
	if (x != 0){
		piLock (STD_IO_BUFFER_KEY);
		printf ("it didn磘 start!!!\n");
		piUnlock (STD_IO_BUFFER_KEY);
		return -1;
	}*/
	// Se crean los temporizadores
	tmrt = tmr_new (refrescar);
	tmrp = tmr_new (flagPelota);
	tmrm = tmr_new (MenosDe100Milisegundos);
	tmrs = tmr_new (ApagarSonido);
	tmra = tmr_new (Acelerometro);
	tmrz = tmr_new (Teclado);

	tmr_startms (tmrz, 1);
	tmr_startms (tmrp, 1000);
	tmr_startms (tmra, 200);
	//Se definen los pines de las filas como salidas
	pinMode(0,OUTPUT);
	pinMode(1,OUTPUT);
	pinMode(2,OUTPUT);
	pinMode(3,OUTPUT);
	pinMode(4,OUTPUT);
	pinMode(7,OUTPUT);
	pinMode(23,OUTPUT);
	//Se definen los pines de las columnas como salidas
	pinMode(22,OUTPUT);
	pinMode(18,OUTPUT);
	pinMode(17,OUTPUT);
	pinMode(14,OUTPUT);
	//Se definen los botones como entradas
	pinMode (GPIO_BUTTONDER, INPUT);
	pinMode (GPIO_BUTTONIZQ, INPUT);
	pinMode (GPIO_BUTTONPAUSA, INPUT);
	// Se definen otras entradas

	pinMode (5, INPUT);
	pinMode (6, INPUT);
	pinMode (12, INPUT);
	pinMode (13, INPUT);
	// Se define a que metodo llaman cuando se pulsan los botones
	wiringPiISR (GPIO_BUTTONPAUSA, INT_EDGE_FALLING, boton_pausa);
	wiringPiISR (GPIO_BUTTONDER, INT_EDGE_FALLING, boton_izquierda);
	wiringPiISR (GPIO_BUTTONIZQ, INT_EDGE_FALLING, boton_derecha);
	pinMode(24,OUTPUT);
	softToneCreate (PIN) ;
	return 1;

}
static void flagPelota (union sigval value){
	piLock (FLAGS_KEY);
	flags |=FLAG_PELOTA;
	piUnlock (FLAGS_KEY);
	tmr_startms (tmrp, velocidadPelota);
}
static void MenosDe100Milisegundos (union sigval value){
	menosDe100ms=0;  //Cuando han pasado 100 milisegundos se llama a este metodo que apaga la variable

}
static void ApagarSonido (union sigval value){
	softToneWrite (PIN, 0) ;  // Apaga el sonido emitido por PIN
}
static void Acelerometro (union sigval value){
	if((digitalRead (BIT1) == HIGH)&& (digitalRead (BIT2) == LOW)){  // Lee los pines BIT1 y 2 y activa una flag
		piLock (FLAGS_KEY);										 	// u otra en funcion de sus valores
		flags |=FLAG_RAQUETA_IZQUIERDA;
		piUnlock (FLAGS_KEY);
	}
	if ((digitalRead (BIT1) == LOW)&& (digitalRead (BIT2) == HIGH)){
		piLock (FLAGS_KEY);
		flags |=FLAG_RAQUETA_DERECHA;
		piUnlock (FLAGS_KEY);
	}
	tmr_startms (tmra, 200);
}
static void Teclado (union sigval value){

	int i=0;
	for(i=0;i<4;i++){   // Se recorren las filas de la columna en cuestion
			if(digitalRead(filaTeclado[i])==HIGH){
				contrasena[posicion]=matrizTeclado[i][columnaT];
				if(posicion<19){ // Asegura que no nos salimos del array
					posicion++;
				}
				else{
					posicion=0; // Se sobreescribe el array
				}
				while(digitalRead(filaTeclado[i])==HIGH){
					delay(1);
				}
			}
	}
	digitalWrite(columnaT, LOW);
	columnaT= (columnaT+1)%4;
	digitalWrite(columnaT, HIGH);


	int x=0;
	for(x=0;x<20;x++){
		if(contrasena[x]==1&&contrasena[x+1]==9&&contrasena[x+2]==9&&contrasena[x+3]==6){
			flags|=FLAG_CONTRASENA;
			digitalWrite(columnaT, LOW);
		}else{
			tmr_startms (tmrz, 27);
		}
	}
}


void boton_izquierda(void){
	if (millis () < debounceTime) {
		debounceTime = millis () + DEBOUNCE_TIME ;
		return;
	}
	if(menosDe100ms==1){ // Si ha pasado menos de 100 milisegundos se considera doble pulsacion
		piLock (FLAGS_KEY);
		flags |= FLAG_DOBLE_PULSACION;
		piUnlock (FLAGS_KEY);
	}
	else{
	piLock (FLAGS_KEY);
	flags |=FLAG_RAQUETA_IZQUIERDA;
	flags |= FLAG_TECLA;
	piUnlock (FLAGS_KEY);
	menosDe100ms=1;
	tmr_startms (tmrm, 100);
	}
	while (digitalRead (GPIO_BUTTONIZQ) == HIGH) {
		delay (1) ;
	}
	debounceTime = millis () + DEBOUNCE_TIME ;


}
void boton_derecha(void){
	if (millis () < debounceTime) {
		debounceTime = millis () + DEBOUNCE_TIME ;
		return;
	}
	if(menosDe100ms==1){  // Si ha pasado menos de 100 milisegundos se considera doble pulsacion
		piLock (FLAGS_KEY);
		flags |= FLAG_DOBLE_PULSACION;
		piUnlock (FLAGS_KEY);

	}
	else{
	piLock (FLAGS_KEY);
	flags |=FLAG_RAQUETA_DERECHA;
	flags |= FLAG_TECLA;
	piUnlock (FLAGS_KEY);
	menosDe100ms=1;
	tmr_startms (tmrm, 100);
	}
	while (digitalRead (GPIO_BUTTONDER) == HIGH) {
		delay (1) ;
	}
	debounceTime = millis () + DEBOUNCE_TIME ;

}
void boton_pausa(void){
	if (millis () < debounceTime) {
		debounceTime = millis () + DEBOUNCE_TIME ;
		return;
		}
	piLock (FLAGS_KEY);
	flags |=FLAG_PAUSA;  // Activa el flag pausa cuando se pulsa este boton
	piUnlock (FLAGS_KEY);
	while (digitalRead (GPIO_BUTTONPAUSA) == HIGH) {
		delay (1) ;
		}
		debounceTime = millis () + DEBOUNCE_TIME ;

}




static void  refrescar(union sigval value){


	int i;
	int c= 9 - (columna%10);  // c es la columna sobre la que se trabaja

	for(i=0;i<7;i++){   // Se recorren las filas de la columna en cuestion
		if(juego.arkanoPi.pantalla.matriz[c][i] == 1){  //Si es un 1 se apaga el pin correspondiente lo que ilumina el led
			digitalWrite(col[i], LOW);
		}else{
			digitalWrite(col[i], HIGH);
		}
	}
	//col
	digitalWrite(14,c & 0x01);
	digitalWrite(17,c & 0x02);
	digitalWrite(18,c & 0x04);
	digitalWrite(22,c & 0x08);
	tmr_startms (tmrt, 1);
	columna++;
}

int main () {
	unsigned int next;

	fsm_trans_t tabla_juego[] = {  // Se define la maquina de estados
			{ WAIT_CONTRASENA,   CompruebaContrasena,  WAIT_PUSH, InicializaJuego},
			{ WAIT_START,   CompruebaTeclaPulsada,  WAIT_PUSH, InicializaJuego},
			{ WAIT_PUSH,   CompruebaTeclaRaquetaIzquierda,  WAIT_PUSH, MueveRaquetaIzquierda},
			{ WAIT_PUSH,   CompruebaDoblePulsacion,  WAIT_PAUSE, PintaPantallaPausa},
			{ WAIT_PAUSE,   CompruebaTeclaPausa,  WAIT_PAUSE_VELOCIDAD, PintaPantallaVelocidad},
			{ WAIT_PAUSE_VELOCIDAD,   CompruebaTeclaRaquetaIzquierda,  WAIT_PAUSE_VELOCIDAD, PintaPantallaVelocidadMenos},
			{ WAIT_PAUSE_VELOCIDAD,   CompruebaTeclaRaquetaDerecha,  WAIT_PAUSE_VELOCIDAD, PintaPantallaVelocidadMas},
			{ WAIT_PAUSE_VELOCIDAD,   CompruebaTeclaPausa,  WAIT_PAUSE_VIDAS, PintaPantallaVidas},
			{ WAIT_PAUSE_VIDAS,   CompruebaTeclaRaquetaIzquierda,  WAIT_PAUSE_VIDAS, PintaPantallaVidasMenos},
			{ WAIT_PAUSE_VIDAS,   CompruebaTeclaRaquetaDerecha,  WAIT_PAUSE_VIDAS, PintaPantallaVidasMas},
			{ WAIT_PAUSE_VIDAS,   CompruebaTeclaPausa,  WAIT_PAUSE, PintaPantallaPausa},
			{ WAIT_PAUSE,   CompruebaDoblePulsacion,  WAIT_PUSH, MovimientoPelota},
			{ WAIT_PUSH,   CompruebaTeclaPelota,  WAIT_PUSH, MovimientoPelota},
			{ WAIT_PUSH,   CompruebaTeclaRaquetaDerecha,  WAIT_PUSH, MueveRaquetaDerecha },
			{ WAIT_PUSH,   CompruebaFinalJuego,  WAIT_END, FinalJuego},
			{ WAIT_PUSH,   CompruebaFinalJuegoDerrota,  WAIT_END_DERROTA, FinalJuegoDerrota},
			{ WAIT_END,   CompruebaTeclaPulsada,  WAIT_START, ReseteaJuego},
			{ WAIT_END_DERROTA,   CompruebaTeclaPulsada,  WAIT_START, ReseteaJuegoDerrota},
			{ -1, NULL, -1, NULL },
	};


	fsm_t* juego_fsm = fsm_new (WAIT_CONTRASENA, tabla_juego, NULL);
	// Configuracion e inicializacion del sistema

	systemSetup();
	ReseteaJuego(juego_fsm);

	next = millis();
	while (1) {
		fsm_fire (juego_fsm);  // Se dispara la m醧uina de estados
		next += CLK_MS;
		delay_until (next);
	}

	fsm_destroy (juego_fsm);

}
