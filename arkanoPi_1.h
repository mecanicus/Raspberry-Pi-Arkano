#ifndef _ARKANOPI_H_
#define _ARKANOPI_H_
// Se cargan las librerías
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <wiringPi.h>
#include "arkanoPiLib.h"
#include <softTone.h>
#include "kbhit.h" // para poder detectar teclas pulsadas sin bloqueo y leer las teclas pulsadas

#include "fsm.h" // para poder crear y ejecutar la mÃ¡quina de estados
#include "tmr.h"
#define CLK_MS 10// PERIODO DE ACTUALIZACION DE LA MAQUINA ESTADOS

// FLAGS DEL SISTEMA
#define FLAG_TECLA					0x01
#define FLAG_PELOTA					0x02
#define FLAG_RAQUETA_IZQUIERDA 		0x04
#define FLAG_RAQUETA_DERECHA 		0x08
#define FLAG_FINAL_JUEGO 			0x10
#define FLAG_PAUSA                  0x20
#define FLAG_DOBLE_PULSACION        0x40
#define FLAG_FINAL_JUEGO_DERROTA    0x80
#define FLAG_CONTRASENA 0x100
#define GPIO_BUTTONIZQ 19   //8 de las entradas de la placa TL-04
#define GPIO_BUTTONDER 16   //7 de las entradas de la placa TL-04
#define GPIO_BUTTONPAUSA 20   //15 de las entradas de la placa TL-04
#define PIN 24    //20 de las salidas de la placa TL-04
#define BIT1 26  //16 de las entradas de la placa TL-04
#define BIT2 27  //17 de las entradas de la placa TL-04


#define DEBOUNCE_TIME 122

#define	FLAGS_KEY	1
#define	STD_IO_BUFFER_KEY	2


/*typedef enum {
	WAIT_START,
	WAIT_PUSH,
	WAIT_END} tipo_estados_juego;
*/
typedef struct {
	tipo_arkanoPi arkanoPi;
	/*tipo_estados_juego estado;*/
	char teclaPulsada;
} tipo_juego;

//------------------------------------------------------
// FUNCIONES DE ACCION
//------------------------------------------------------

void InicializaJuego (fsm_t *fsm);
void MueveRaquetaIzquierda (fsm_t *fsm);
void MueveRaquetaDerecha (fsm_t *fsm);
void MovimientoPelota (fsm_t *fsm);
void FinalJuego (fsm_t *fsm);
void ReseteaJuego (fsm_t *fsm);
void boton_derecha (void);
void boton_izquierda(void);
void boton_pausa(void);
void PintaPantallaPausa(fsm_t *fsm);
static void refrescar(union sigval value);
static void flagPelota (union sigval value);
static void MenosDe100Milisegundos (union sigval value);
static void ApagarSonido (union sigval value);
static void Acelerometro (union sigval value);
static void Teclado (union sigval value);
//------------------------------------------------------
// FUNCIONES DE INICIALIZACION
//------------------------------------------------------
int systemSetup (void);

#endif /* ARKANOPI_H_ */
