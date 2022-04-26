/*
 * Recreación del juego SUPER SPRINT por:
 * David Tobar y Alejandro Ramírez
 * 25/04/2022
 * FIUMMMMMMMMMMMMMMMMMMMMMBA
 */

/*
+----------------------------------------------------------------------------------+
|                                    LIBRERIAS                                     |                                   
+----------------------------------------------------------------------------------+
*/
#include <stdint.h>
#include <stdbool.h>
#include <TM4C123GH6PM.h>
#include "Funciones_De_LCD.h"

// Funciones de Juego y control
#include "Funciones_De_Juego.h"

/*
+----------------------------------------------------------------------------------+
|                                     PUERTOS                                      |                                   
+----------------------------------------------------------------------------------+
*/


/*
+----------------------------------------------------------------------------------+
|                                VARIABLES Y STRUCTS                               |                                   
+----------------------------------------------------------------------------------+
*/
// Sub struct de la variables relacionadas al mando 
struct control{
  int Izquierda;
  int Derecha;
  int Acelerador;
  int Freno;
  int Drift;
  int turnoDrift;
  int rateGiro;
  };

// Sub struct de las variables relacionadas al giro
struct giro{
  int enGiro;
  unsigned long tGiro;
  int Posicion_Angular_Actual;
  float Angulo;
  };

// Sub struct de las variables relacionadas al movimiento
struct movimiento{
  float Aceleracion;
  int enMovimiento;
  unsigned long tAceleracion;
  float Velocidad;
  float velX;
  float velY;
  float posX;
  float posY;
  };

// Super struct que contiene a los sub structs
struct Jugador{
  control Control;
  giro Giro;
  movimiento Movimiento;
  int accion;
}J1;

struct limites{
  float xo;
  float xf;
  float yo;
  float yf;
  float xio;
  float xif;
  float yio;
  float yif;
};


// Struct dedicado a los limites de pista 1
struct Pista{
  limites Limites;
}Pista1;

/*
+----------------------------------------------------------------------------------+
|                              PROTOTIPO DE FUNCIONES                              |                                                                      
+----------------------------------------------------------------------------------+
*/


extern uint8_t Mapa_Pista1[];
//extern uint8_t P_Inicio [];
//***************************************************************************************************************************************
// Inicialización
//***************************************************************************************************************************************
void setup() {
  SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
  Serial.begin(115200);
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  Serial.println("Inicio");
  LCD_Init();
  LCD_Clear(0x632C);
  
  //LCD_Bitmap(0, 0, 320, 240, P_Inicio);
  //delay(5000);

  // Limites asociados a la pista1
  // Borde exterior
  Pista1.Limites.xo = 48;
  Pista1.Limites.xf = 255;
  Pista1.Limites.yo = 42;
  Pista1.Limites.yf = 201;

  // Borde interior
  Pista1.Limites.xio = 72;
  Pista1.Limites.xif = 231;
  Pista1.Limites.yio = 169;
  Pista1.Limites.yif = 73;
  
  // Pines asociados a botones
  J1.Control.Derecha    = PA_4;
  J1.Control.Izquierda  = PA_3;
  J1.Control.Acelerador = PF_0;
  J1.Control.Drift      = PA_2;

  // Valores de maniobra
  J1.Control.rateGiro   = 60;
  J1.Control.turnoDrift = 0;
  J1.Movimiento.Velocidad =  0.009;

  // Variables para entrar en las condiciones
  J1.Giro.enGiro = 0;
  J1.Movimiento.enMovimiento = 0;
  
  // Definir pos inicial de carrito 
  J1.Movimiento.posX = 50;
  J1.Movimiento.posY = 180;

  //Definimos el valor de angulo inicial del carro
  J1.Giro.Posicion_Angular_Actual = 0;
  J1.Giro.Angulo = 0;
  compVelocidad(J1.Movimiento.Velocidad, J1.Giro.Angulo, &J1.Movimiento.velX, &J1.Movimiento.velY);
  
  
  //pinMode(Push_Acelerar_J1,INPUT_PULLUP);
  pinMode(J1.Control.Izquierda, INPUT_PULLUP);
  pinMode(J1.Control.Derecha, INPUT_PULLUP);
  pinMode(J1.Control.Acelerador, INPUT_PULLUP);
  pinMode(J1.Control.Drift, INPUT_PULLUP);    

  LCD_Bitmap(0, 0, 320, 240, Mapa_Pista1);
  LCD_Sprite(J1.Movimiento.posX,J1.Movimiento.posY,16,16,CarritoConPrivilegios,32,0,0,0); // Mostrar carrito
}
//***************************************************************************************************************************************
// Loop Infinito
//***************************************************************************************************************************************
void loop() {
  //Primero creamos la variable que nos dice si el usuario toco un boton
  J1.accion = !digitalRead(J1.Control.Izquierda) | !digitalRead(J1.Control.Derecha) | !digitalRead(J1.Control.Acelerador);
  
  /*
  Serial.print("PosX: ");
  Serial.print(J1.Movimiento.posX);
  Serial.print(" PosY: ");
  Serial.println(J1.Movimiento.posY);
*/

  // Borde exterior:
  // x final   255
  // x inicial  45
  // y inicial 203
  // y final    40

  // Borde interior:
  // x inicial 72
  // x final   231
  // y inicial 169
  // y final   73
  
  /*
  Serial.print("Pos angular: ");
  Serial.print(J1.Giro.Posicion_Angular_Actual);
  Serial.print(" Angulo: ");
  Serial.print(J1.Giro.Angulo);

  float AngulitoLIndo = J1.Giro.Angulo*PI/180;
  float Angulote = AngulitoLIndo;

  Serial.print(" Angulote: ");
  Serial.print(Angulote);
  
  Serial.print(" Coseno: ");
  Serial.println(cos(Angulote));
  */
  
  if(J1.accion){

    // Se determina el tiempo inicial de la duracion del movimiento (Al acelerar)
    if(!digitalRead(J1.Control.Acelerador) && !J1.Movimiento.enMovimiento){
      J1.Movimiento.tAceleracion = millis();
      J1.Movimiento.enMovimiento = 1;   
    }
    // Se realiza el movimiento, tomando en cuenta la referencia del tiempo anterior (Mientras se acelera)
    else if(!digitalRead(J1.Control.Acelerador) && J1.Movimiento.enMovimiento){
      if(!digitalRead(J1.Control.Acelerador)&&(millis()-J1.Movimiento.tAceleracion)>=20){
        float posX_ini = J1.Movimiento.posX;
        float posY_ini = J1.Movimiento.posY;

        // El sprite se encuentra dentro del borde exterior en coordenadas x
        if( posX_ini > Pista1.Limites.xo && posX_ini < Pista1.Limites.xf){
          // El sprite se encuentra dentro del borde exterior en coordenadas y
          if(posY_ini > Pista1.Limites.yo && posY_ini < Pista1.Limites.yf){ 
            
          }
        }

        /*
        if (posX_ini <= Pista1.Limites.xo || posX_ini >= Pista1.Limites.xf){          
          J1.Giro.Angulo = J1.Giro.Angulo + 2*(90-normAngulo(J1.Giro.Angulo));
          Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual);          
        }*/

        /*
        // Pasa de pared derecha
        if(posX_ini >= Pista1.Limites.xf){
          J1.Giro.Angulo = J1.Giro.Angulo + 2*(90-normAngulo(J1.Giro.Angulo));
          Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual); 
          J1.Movimiento.velY = -J1.Movimiento.velY;  
        }*/
        //--------------------------Verificacion de limites de las paredes---------------------------
        //*******************************Sentido Antihorario*****************************************
        //Pared inferior
        if(posY_ini>=Pista1.Limites.yf && J1.Giro.Angulo>=270){
          J1.Giro.Angulo = normAngulo(J1.Giro.Angulo);
          Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual); 
        }
        //Pared derecha
        else if(posX_ini>=Pista1.Limites.xf && J1.Giro.Angulo<=90){
          J1.Giro.Angulo = (90-normAngulo(J1.Giro.Angulo))+90;
          Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual);  
        }
        //Pared superior
        else if(posY_ini<=Pista1.Limites.yo && J1.Giro.Angulo<=180 && J1.Giro.Angulo>=90){
          J1.Giro.Angulo = normAngulo(J1.Giro.Angulo)+180;
          Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual); 
        }
        //Pared izquierda
        else if(posX_ini<=Pista1.Limites.xo && J1.Giro.Angulo>=180){
          J1.Giro.Angulo = (90-normAngulo(J1.Giro.Angulo))+270;
          Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual);   
        }
        //--------------------------Verificacion de limites de las paredes---------------------------
        //***********************************Sentido Horario*****************************************
        //Pared inferior
        if(posY_ini>=Pista1.Limites.yf && J1.Giro.Angulo<=270){
          J1.Giro.Angulo = 180 - normAngulo(J1.Giro.Angulo);
          Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual); 
        }
        //Pared derecha
        else if(posX_ini>=Pista1.Limites.xf && J1.Giro.Angulo>=270){
          J1.Giro.Angulo = 90 - (90-normAngulo(J1.Giro.Angulo));
          Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual);  
        }
        //Pared superior
        else if(posY_ini<=Pista1.Limites.yo && J1.Giro.Angulo<=180 && J1.Giro.Angulo<=90){
          J1.Giro.Angulo = 360 - normAngulo(J1.Giro.Angulo);
          Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual); 
        }
        //Pared izquierda
        else if(posX_ini<=Pista1.Limites.xo && J1.Giro.Angulo>=90 && J1.Giro.Angulo<=180){
          J1.Giro.Angulo = 90 - (90-normAngulo(J1.Giro.Angulo));
          Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual);   
        }
        // Sentido reloj, pared izquierda
        if(posX_ini <= Pista1.Limites.xo && (J1.Giro.Angulo >= 90 && J1.Giro.Angulo < 180)){
          
          //J1.Giro.Angulo = normAnguloIx(J1.Giro.Angulo);
          Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual);  
        }
        // Sentido reloj, pared derecha
        else if(posX_ini >= Pista1.Limites.xf && J1.Giro.Angulo >= 180){
          
          //J1.Giro.Angulo = normAnguloIx(J1.Giro.Angulo);
          Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual);  
        }
        // Sentido antireloj, pared derecha
        else if(posX_ini >= Pista1.Limites.xf && J1.Giro.Angulo <= 90){
          
          //J1.Giro.Angulo = normAnguloDx(J1.Giro.Angulo);
          Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual);  
          //J1.Giro.Angulo = J1.Giro.Angulo + 2*(90-normAngulo(J1.Giro.Angulo));
          //Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual); 
          //J1.Movimiento.velY = -J1.Movimiento.velY;  
        }
        // Sentido antireloj, pared izquierda
        else if(posX_ini <= Pista1.Limites.xo && J1.Giro.Angulo >= 180){
          //J1.Giro.Angulo = normAnguloDx(J1.Giro.Angulo);
          Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual); 
          //J1.Giro.Angulo = J1.Giro.Angulo + 2*(90-normAngulo(J1.Giro.Angulo));
          //Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual); 
          //J1.Movimiento.velY = -J1.Movimiento.velY;  
        }
 
        /*
        if(posY_ini < Pista1.Limites.yo || posY_ini > Pista1.Limites.yf){ 
          
          J1.Movimiento.velY = -J1.Movimiento.velY;  
             
          Serial.print("entra");
        }*/

                 
        compVelocidad(J1.Movimiento.Velocidad,J1.Giro.Angulo, &J1.Movimiento.velX, &J1.Movimiento.velY);
        movimientoCarro(posX_ini,posY_ini, 20, J1.Movimiento.velX, J1.Movimiento.velY, &J1.Movimiento.posX,&J1.Movimiento.posY);          
        LCD_Sprite(J1.Movimiento.posX,J1.Movimiento.posY,16,16,CarritoConPrivilegios,32,J1.Giro.Posicion_Angular_Actual,0,0);
        V_line( J1.Movimiento.posX - posX_ini, 180, 16,  0x632C); 
      }    
    }

    // Al soltar el acelerador, se sale de las condiciones y no se mueve
    else if(digitalRead(J1.Control.Acelerador)&&J1.Movimiento.enMovimiento){
      J1.Movimiento.enMovimiento = 0;  
    }
    
    if(J1.accion && !J1.Giro.enGiro){
      /* El usuario pulso un boton de giro
       * por primera vez
       */
      J1.Giro.tGiro = millis();
      J1.Giro.enGiro = 1;
    }else if(J1.accion && J1.Giro.enGiro){
      /*    
       *     El carro gira cada J1.Control.rateGiro ms
       *     J1.Control.turnoDrift
       */
      
      if(!J1.Control.turnoDrift && !digitalRead(J1.Control.Drift)){
        J1.Control.turnoDrift = 1;
        J1.Control.rateGiro   = 20;
                
      }else if(J1.Control.turnoDrift && digitalRead(J1.Control.Drift)){
        //J1.Control.turnoDrift = 0
        J1.Control.rateGiro   = 60;
        J1.Control.turnoDrift = 0;
        
      }
             
      if(digitalRead(J1.Control.Izquierda)&&(millis()-J1.Giro.tGiro)>=J1.Control.rateGiro){
        Angulo(J1.Control.Izquierda, J1.Control.Derecha,&J1.Giro.Posicion_Angular_Actual,&J1.Giro.Angulo);
        compVelocidad(J1.Movimiento.Velocidad, J1.Giro.Angulo, &J1.Movimiento.velX, &J1.Movimiento.velY);
        LCD_Sprite(J1.Movimiento.posX,J1.Movimiento.posY,16,16,CarritoConPrivilegios,32,J1.Giro.Posicion_Angular_Actual,0,0);
        J1.Giro.tGiro = millis();
        /*
        Serial.print("Posicion inicial: ");
        Serial.print(J1.Movimiento.posX);
        Serial.print(",");
        Serial.println(J1.Movimiento.posY);
        Serial.println("");
        */
      }else if(digitalRead(J1.Control.Derecha)&&(millis()-J1.Giro.tGiro)>=J1.Control.rateGiro){
        Angulo(J1.Control.Izquierda, J1.Control.Derecha,&J1.Giro.Posicion_Angular_Actual,&J1.Giro.Angulo);
        compVelocidad(J1.Movimiento.Velocidad, J1.Giro.Angulo, &J1.Movimiento.velX, &J1.Movimiento.velY);
        LCD_Sprite(J1.Movimiento.posX,J1.Movimiento.posY,16,16,CarritoConPrivilegios,32,J1.Giro.Posicion_Angular_Actual,0,0);
        J1.Giro.tGiro = millis();
        /*
        Serial.print("Posicion inicial: ");
        Serial.print(J1.Movimiento.posX);
        Serial.print(",");
        Serial.println(J1.Movimiento.posY);
        Serial.println("");
        */
      }
      
    }else if(!J1.accion && J1.Giro.enGiro){
      J1.Giro.enGiro = 0;  
    }
  }
  /*while(accionBoton){
    unsigned long Tiempo_Transcurrido_Giro = millis() - Tiempo_Inicial_Giro;
    if(digitalRead(Primero.Control_Derecha)== 0 && Tiempo_Transcurrido_Giro >=20){
      Angulo(Primero.Control_Izquierda, Primero.Control_Derecha,&Primero.Posicion_Angular_Actual,&Angulo_V);
      LCD_Sprite(50,180,16,16,CarritoConPrivilegios,32,Primero.Posicion_Angular_Actual,0,0);
      break;
    }
  
    else if(digitalRead(Primero.Control_Izquierda)== 0 && Tiempo_Transcurrido_Giro >=20){
      Angulo(Primero.Control_Izquierda, Primero.Control_Derecha,&Primero.Posicion_Angular_Actual,&Angulo_V);
      LCD_Sprite(50,180,16,16,CarritoConPrivilegios,32,Primero.Posicion_Angular_Actual,0,0);
      break;
    }

    /*else if(Tiempo_Transcurrido_Giro >= 30){
      break;
    }
  }*/
  //Angulo(Push_Izquierdo, Push_Derecho,&Posicion_Angular_Actual,&Angulo_V);
  //LCD_Sprite(50,180,16,16,CarritoConPrivilegios,32,Posicion_Angular_Actual,0,0);

  
// Medio aceleron, esta no tiene implementado el giro. Y no toma en cuenta la velocidad inicial al acelerar OJO
/*
  // Bucle cuando se preciona acelerador
  while(!val_Push_Acelerar_J1){
    
     // La unica funcion de este while que contiene otro while, es contar con una variable
     // que tenga el tiempo inicial (variable Tiempo_Inicial_J1) y luego compararla con la 
     // variable de tiempo final (Tiempo_Final_J1). Con una resta de estos tiempos, se 
     // obtiene el tiempo de duracion. Esta resta se actualiza constantemente dentro del 
     // while de bajo es el que va actualizando el valor de la duracion del boton apachado.
    
    val_Push_Acelerar_J1 = digitalRead(Push_Acelerar_J1); // Leer estado boton acelerador
    Tiempo_Inicial_J1 = millis(); //Pasar el valor del millis al tiempo inicial
    //Duracion_Boton_J1 = 0;
    // Actualiza la resta de tiempos constantemente
    while(!val_Push_Acelerar_J1){
      val_Push_Acelerar_J1 = digitalRead(Push_Acelerar_J1);
      // Actualiza el tiempo presente para luego comparar con el tiempo inicial al pulsar
      Tiempo_Final_J1 = millis(); 
      Duracion_Boton_J1 = Tiempo_Final_J1 - Tiempo_Inicial_J1; // Determina el tiempo 
      //Turno_Boton_J1    = HIGH;

      // Posicion a partir de la duracion del pulso
      PosI_J1 = PosI_J1 + 0.5*AclI_J1*(Duracion_Boton_J1)*(Duracion_Boton_J1);
      // VelI_J1 = PosI_J1/Duracion_Boton_J1;
      
      // Mover Sprite
      LCD_Sprite(PosI_J1,180,16,16,CarritoConPrivilegios,32,0,0,0);
      V_line( PosI_J1 -1, 180, 16,  0x632C);

      // Imprimir tiempo pulsado
      Serial.print("Tiempo pulsado: ");
      Serial.print(Duracion_Boton_J1);
      Serial.print(" Pos: ");
      Serial.print(PosI_J1);
      Serial.print(" Vel: ");
      Serial.println(VelI_J1);

      // Al soltar boton salir de while
      if(val_Push_Acelerar_J1){
        VelI_J1 = PosI_J1/Duracion_Boton_J1;
        Serial.print(" Vel: ");
        Serial.println(VelI_J1);
        //Duracion_Boton_J1 = 0;
        break;
      }
    }
    // Al soltar boton salir del while
    if(val_Push_Acelerar_J1){
        break;
    }
  }

*/
}
  
  /*
  for (int x = 50; x < 270 - 16; x++) {
    int anim2 = (x / 10) % 32;
    LCD_Sprite(x,180,16,16,CarritoConPrivilegios,32,anim2,0,0);
    V_line( x -1, 180, 16,  0x632C);
    delay(15);
  }
  for (int x = 270-16; x > 50; x--) {
    int anim2 = (x / 10) % 32;
    LCD_Sprite(x,180,16,16,CarritoConPrivilegios,32,anim2,1,0);
    V_line( x +16, 180, 16,   0x632C);
    delay(15);
  }
  
  */
  /* for(int x = 0; x <320-32; x++){
     delay(15);
     int anim2 = (x/35)%2;

     LCD_Sprite(x,100,16,24,planta,2,anim2,0,1);
     V_line( x -1, 100, 24, 0x421b);

     //LCD_Bitmap(x, 100, 32, 32, prueba);

     int anim = (x/11)%8;


     int anim3 = (x/11)%4;

     LCD_Sprite(x, 20, 16, 32, mario,8, anim,1, 0);
     V_line( x -1, 20, 32, 0x421b);

     //LCD_Sprite(x,100,32,32,bowser,4,anim3,0,1);
     //V_line( x -1, 100, 32, 0x421b);


     LCD_Sprite(x, 140, 16, 16, enemy,2, anim2,1, 0);
     V_line( x -1, 140, 16, 0x421b);

     LCD_Sprite(x, 175, 16, 32, luigi,8, anim,1, 0);
     V_line( x -1, 175, 32, 0x421b);
    }
    for(int x = 320-32; x >0; x--){
     delay(5);
     int anim = (x/11)%8;
     int anim2 = (x/11)%2;

     LCD_Sprite(x,100,16,24,planta,2,anim2,0,0);
     V_line( x + 16, 100, 24, 0x421b);

     //LCD_Bitmap(x, 100, 32, 32, prueba);

     //LCD_Sprite(x, 140, 16, 16, enemy,2, anim2,0, 0);
     //V_line( x + 16, 140, 16, 0x421b);

     //LCD_Sprite(x, 175, 16, 32, luigi,8, anim,0, 0);
     //V_line( x + 16, 175, 32, 0x421b);

     //LCD_Sprite(x, 20, 16, 32, mario,8, anim,0, 0);
     //V_line( x + 16, 20, 32, 0x421b);
    }
  */
