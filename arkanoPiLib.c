#include "arkanoPiLib.h"

int ladrillos_basico[MATRIZ_ANCHO][MATRIZ_ALTO] = {
		{1,1,0,0,0,0,0},
		{1,1,0,0,0,0,0},
		{1,1,0,0,0,0,0},
		{1,1,0,0,0,0,0},
		{1,1,0,0,0,0,0},
		{1,1,0,0,0,0,0},
		{1,1,0,0,0,0,0},
		{1,1,0,0,0,0,0},
		{1,1,0,0,0,0,0},
		{1,1,0,0,0,0,0},
};


//------------------------------------------------------
// FUNCIONES DE INICIALIZACION / RESET
//------------------------------------------------------

void ReseteaMatriz(tipo_pantalla *p_pantalla) {
	int i, j = 0;

	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			p_pantalla->matriz[i][j] = 0;
		}
	}
}
void RegeneraMatriz(tipo_pantalla *p_pantalla) {
	int i, j = 0;

	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=2;j<MATRIZ_ALTO;j++) {
			p_pantalla->matriz[i][j] = 0;
		}
	}
}

void ReseteaLadrillos(tipo_pantalla *p_ladrillos) {
	int i, j = 0;

	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			p_ladrillos->matriz[i][j] = ladrillos_basico[i][j];
		}
	}
}

void ReseteaPelota(tipo_pelota *p_pelota) {
	// Pelota inicialmente en el centro de la pantalla
	p_pelota->x = MATRIZ_ANCHO/2 - 1;
	p_pelota->y = MATRIZ_ALTO/2;

	// Trayectoria inicial
	p_pelota->xv = 0;
	p_pelota->yv = -1;

}

void ReseteaRaqueta(tipo_raqueta *p_raqueta) {
	// Raqueta inicialmente en el centro de la pantalla
	p_raqueta->x = MATRIZ_ANCHO/2 - p_raqueta->ancho/2 -2;
	p_raqueta->y = MATRIZ_ALTO - 1;
	p_raqueta->ancho = RAQUETA_ANCHO;
	p_raqueta->alto = RAQUETA_ALTO;
}

//------------------------------------------------------
// FUNCIONES DE VISUALIZACION (ACTUALIZACION DEL OBJETO PANTALLA QUE LUEGO USARA EL DISPLAY)
//------------------------------------------------------

// void PintaMensajeInicialPantalla (...): metodo encargado de aprovechar
// el display para presentar un mensaje de bienvenida al usuario
void PintaMensajeInicialPantalla (tipo_pantalla *p_pantalla, tipo_pantalla *p_pantalla_inicial) {
	//FC
	int i,j = 0;
	for(i=0;i<MATRIZ_ANCHO;i++) {
			for(j=0;j<MATRIZ_ALTO;j++) {
				p_pantalla->matriz[i][j] = p_pantalla_inicial->matriz[i][j];
			}
	    }
}

// void PintaPantallaPorTerminal (...): metodo encargado de mostrar
// el contenido o la ocupaciÃ³n de la matriz de leds en la ventana de
// terminal o consola. Este mÃ©todo sera fundamental para facilitar
// la labor de depuraciÃ³n de errores (por ejemplo, en la programaciÃ³n
// de los diferentes movimientos tanto de la raqueta como de la pelota).
void PintaPantallaPorTerminal  (tipo_pantalla *p_pantalla) {
	// FC
	/*ir recorriendo la matriz posicion por posicion y hacer un printf*
	 */
	/*int i, j=0;
	puts("[PANTALLA]\n");
	for(i=0;i<MATRIZ_ALTO;i++) {
				for(j=MATRIZ_ANCHO-1;j>=0;j--) {
					printf("%d",p_pantalla->matriz[j][i]);
				}
				printf("\n");

	}
}
*/
	int i, j=0;
		puts("[PANTALLA]\n");
		for(i=0;i<MATRIZ_ALTO;i++) {
					for(j=0;j<MATRIZ_ANCHO;j++) {
						printf("%d",p_pantalla->matriz[j][i]);
					}
					printf("\n");

		}
	}

// void PintaLadrillos(...): funcion encargada de â€œpintarâ€� los ladrillos
// en sus correspondientes posiciones dentro del Ã¡rea de juego
void PintaLadrillos(tipo_pantalla *p_ladrillos, tipo_pantalla *p_pantalla) {
	int i, j = 0;

	for(i=0;i<MATRIZ_ANCHO;i++) {
		for(j=0;j<MATRIZ_ALTO;j++) {
			p_pantalla->matriz[i][j] = p_ladrillos->matriz[i][j];
		}
    }
}

// void PintaRaqueta(...): funcion encargada de â€œpintarâ€� la raqueta
// en su posicion correspondiente dentro del Ã¡rea de juego
void PintaRaqueta(tipo_raqueta *p_raqueta, tipo_pantalla *p_pantalla) {
	int i, j = 0;

	for(i=0;i<RAQUETA_ANCHO;i++) {
		for(j=0;j<RAQUETA_ALTO;j++) {
			if (( (p_raqueta->x+i >= 0) && (p_raqueta->x+i < MATRIZ_ANCHO) ) &&
					( (p_raqueta->y+j >= 0) && (p_raqueta->y+j < MATRIZ_ALTO) ))
				p_pantalla->matriz[p_raqueta->x+i][p_raqueta->y+j] = 1;
		}
	}
}

// void PintaPelota(...): funcion encargada de â€œpintarâ€� la pelota
// en su posicion correspondiente dentro del Ã¡rea de juego
void PintaPelota(tipo_pelota *p_pelota, tipo_pantalla *p_pantalla) {
	if( (p_pelota->x >= 0) && (p_pelota->x < MATRIZ_ANCHO) ) {
		if( (p_pelota->y >= 0) && (p_pelota->y < MATRIZ_ALTO) ) {
			p_pantalla->matriz[p_pelota->x][p_pelota->y] = 1;
		}
		else {
			printf("\n\nPROBLEMAS!!!! posicion y=%d de la pelota INVALIDA!!!\n\n", p_pelota->y);
			fflush(stdout);
		}
	}
	else {
		printf("\n\nPROBLEMAS!!!! posicion x=%d de la pelota INVALIDA!!!\n\n", p_pelota->x);
		fflush(stdout);
	}
}

// void ActualizaPantalla(...): metodo cuya ejecucion estara ligada a
// cualquiera de los movimientos de la raqueta o de la pelota y que
// sera el encargado de actualizar convenientemente la estructura de datos
// en memoria que representa el Ã¡rea de juego y su correspondiente estado.
void ActualizaPantalla(tipo_arkanoPi* p_arkanoPi) {
    // Borro toda la pantalla
	//FC
	ReseteaMatriz((tipo_pantalla*)(&(p_arkanoPi->pantalla)));

	PintaLadrillos(&(p_arkanoPi->ladrillos), &(p_arkanoPi->pantalla));
	PintaPelota(&(p_arkanoPi->pelota), &(p_arkanoPi->pantalla));
	PintaRaqueta(&(p_arkanoPi->raqueta), &(p_arkanoPi->pantalla));
}

// void InicializaArkanoPi(...): metodo encargado de la inicializaciÃ³n
// de toda variable o estructura de datos especificamente ligada al
// desarrollo del juego y su visualizacion.
void InicializaArkanoPi(tipo_arkanoPi *p_arkanoPi) {
	//FC
	static tipo_pantalla mensaje_inicial = { {
			{0,0,0,0,0,0,0},
			{0,0,0,1,0,0,0},
			{1,1,0,1,1,0,0},
			{1,1,0,0,1,1,0},
			{0,0,0,0,0,1,1},
			{0,0,0,0,0,1,1},
			{1,1,0,0,1,1,0},
			{1,1,0,1,1,0,0},
			{0,0,0,1,0,0,0},
			{0,0,0,0,0,0,0},
	} };
	ReseteaMatriz((tipo_pantalla*)(&(p_arkanoPi->pantalla)));

	ReseteaLadrillos((tipo_pantalla*)(&(p_arkanoPi->ladrillos)));
	ReseteaPelota((tipo_pelota*)(&(p_arkanoPi->pelota)));
	ReseteaRaqueta((tipo_raqueta*)(&(p_arkanoPi->raqueta)));

	PintaMensajeInicialPantalla(&p_arkanoPi->pantalla, &mensaje_inicial);

}
void RegeneraArkanoPi(tipo_arkanoPi *p_arkanoPi) {
	static tipo_pantalla mensaje_inicial = { {
			{0,0,0,0,0,0,0},
			{0,0,0,1,0,0,0},
			{0,0,0,1,1,0,0},
			{0,0,0,0,1,1,0},
			{0,0,0,0,0,1,1},
			{0,0,0,0,0,1,1},
			{0,0,0,0,1,1,0},
			{0,0,0,1,1,0,0},
			{0,0,0,1,0,0,0},
			{0,0,0,0,0,0,0},
	} };
	RegeneraMatriz((tipo_pantalla*)(&(p_arkanoPi->pantalla)));
	ReseteaPelota((tipo_pelota*)(&(p_arkanoPi->pelota)));
	ReseteaRaqueta((tipo_raqueta*)(&(p_arkanoPi->raqueta)));
	PintaMensajeInicialPantalla(&p_arkanoPi->pantalla, &mensaje_inicial);

}

// int CalculaLadrillosRestantes(...): funciÃ³n encargada de evaluar
// el estado de ocupacion del area de juego por los ladrillos y
// devolver el numero de estos
int CalculaLadrillosRestantes(/*volatile*/ tipo_pantalla *p_ladrillos) {
	//FC
	int num_ladrillos_restantes = 0;
	int i=0;
	int j=0;
	for(i=0;i<MATRIZ_ANCHO;i++) {
			for(j=0;j<2;j++) {
				if(p_ladrillos->matriz[i][j]==1 ){
					num_ladrillos_restantes ++;
				}
			}
	}

	return num_ladrillos_restantes;
}

