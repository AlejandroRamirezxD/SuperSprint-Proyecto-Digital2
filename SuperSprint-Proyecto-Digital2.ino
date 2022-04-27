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
  unsigned long tRebote;
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

int   choque = 0;

// Struct dedicado a los limites de pista 1
struct Pista{
  limites Limites;
}Pista1;

/*
+----------------------------------------------------------------------------------+
|                              PROTOTIPO DE FUNCIONES                              |                                                                      
+----------------------------------------------------------------------------------+
*/
void Condiciones_Colisones();
void Giro_Girito();

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
  Pista1.Limites.xio = 71;
  Pista1.Limites.xif = 232;
  Pista1.Limites.yif = 169;
  Pista1.Limites.yio = 73;
  
  // Pines asociados a botones
  J1.Control.Derecha    = PA_4;
  J1.Control.Izquierda  = PA_3;
  J1.Control.Acelerador = PF_0;
  J1.Control.Drift      = PA_2;

  // Valores de maniobra
  J1.Control.rateGiro   = 60;
  J1.Control.turnoDrift = 0;
  J1.Movimiento.Velocidad =  0;
  J1.Movimiento.Aceleracion = 0.0000001;

  // Variables para entrar en las condiciones
  J1.Giro.enGiro = 0;
  J1.Movimiento.enMovimiento = 0;
  J1.Movimiento.tRebote = 0;
  
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
  J1.accion =!digitalRead(J1.Control.Acelerador);
  //choque  = 0;
  if(J1.accion){

    // Se determina el tiempo inicial de la duracion del movimiento (Al acelerar)
    if(!digitalRead(J1.Control.Acelerador) && !J1.Movimiento.enMovimiento){
      J1.Movimiento.tAceleracion = millis();
      J1.Movimiento.enMovimiento = 1;   
    }
    // Se realiza el movimiento, tomando en cuenta la referencia del tiempo anterior (Mientras se acelera)
    else if(!digitalRead(J1.Control.Acelerador) && J1.Movimiento.enMovimiento){
      
      if(!digitalRead(J1.Control.Acelerador)&&(millis()-J1.Movimiento.tAceleracion)>=20){
        Condiciones_Colisones();
         float posX_ini = J1.Movimiento.posX;
         float posY_ini = J1.Movimiento.posY;
         float  vel_ini = J1.Movimiento.Velocidad;

        /* ---------------------------------------------------------------------------------------------
         * **********************************Código de Aceleración**************************************
         * ---------------------------------------------------------------------------------------------
         */
        unsigned long rateVel;
        unsigned long limiteRate = 25;

        J1.Movimiento.Velocidad = J1.Movimiento.Velocidad + J1.Movimiento.Aceleracion*(millis()-J1.Movimiento.tAceleracion);

        if(J1.Movimiento.Velocidad >= 0.015){
            J1.Movimiento.Velocidad = 0.015;
        }
        
        compVelocidad(J1.Movimiento.Velocidad,J1.Giro.Angulo, &J1.Movimiento.velX, &J1.Movimiento.velY);
        movimientoCarro(posX_ini,posY_ini,20, J1.Movimiento.velX, J1.Movimiento.velY, &J1.Movimiento.posX,&J1.Movimiento.posY);          
        LCD_Sprite(J1.Movimiento.posX,J1.Movimiento.posY,16,16,CarritoConPrivilegios,32,J1.Giro.Posicion_Angular_Actual,0,0);
        V_line( J1.Movimiento.posX - posX_ini, 180, 16,  0x632C); 
      }    
    }

    // Al soltar el acelerador, se sale de las condiciones y no se mueve
    if(digitalRead(J1.Control.Acelerador)&&J1.Movimiento.enMovimiento){
      J1.Movimiento.enMovimiento = 0;  
    }
    
    
  }
  else{
    if(!J1.Movimiento.enMovimiento){
      J1.Movimiento.tAceleracion = millis();
      J1.Movimiento.enMovimiento = 1;  
    }
    else if(J1.Movimiento.enMovimiento == 1 && (millis()-J1.Movimiento.tAceleracion)>=2){

      Condiciones_Colisones();
      
      float posX_ini = J1.Movimiento.posX;
      float posY_ini = J1.Movimiento.posY;
      float  vel_ini = J1.Movimiento.Velocidad;
      
      
      J1.Movimiento.Velocidad = vel_ini - J1.Movimiento.Aceleracion*(millis()-J1.Movimiento.tAceleracion)*50;
      
        if(J1.Movimiento.Velocidad <= 0){
           J1.Movimiento.Velocidad = 0;
        }
      
      
      compVelocidad(J1.Movimiento.Velocidad*1000,J1.Giro.Angulo, &J1.Movimiento.velX, &J1.Movimiento.velY);
      movimientoCarro(posX_ini,posY_ini,20, J1.Movimiento.velX/1000, J1.Movimiento.velY/1000, &J1.Movimiento.posX,&J1.Movimiento.posY);   
      LCD_Sprite(J1.Movimiento.posX,J1.Movimiento.posY,16,16,CarritoConPrivilegios,32,J1.Giro.Posicion_Angular_Actual,0,0);
      
      
      
      
      J1.Movimiento.enMovimiento = 0;   
      J1.Movimiento.tAceleracion = millis();
    }
  }

  Giro_Girito();
}

void Condiciones_Colisones(){
  float posX_ini = J1.Movimiento.posX;
  float posY_ini = J1.Movimiento.posY;
  float  vel_ini = J1.Movimiento.Velocidad;
  
  // El sprite se encuentra dentro del borde exterior en coordenadas x
  if( posX_ini > Pista1.Limites.xo && posX_ini < Pista1.Limites.xf){
    // El sprite se encuentra dentro del borde exterior en coordenadas y
    if(posY_ini > Pista1.Limites.yo && posY_ini < Pista1.Limites.yf){ 
      
    }
  }

  
  //--------------------------Verificacion de limites de las paredes---------------------------
  //*******************************Sentido Antihorario*****************************************
  //Pared inferior
  if(posY_ini>=Pista1.Limites.yf && J1.Giro.Angulo>=270){
    J1.Giro.Angulo = normAngulo(J1.Giro.Angulo);
    Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual); 
    choque = 1;
    //Serial.println("1");
  }
  //Pared derecha
  else if(posX_ini>=Pista1.Limites.xf && J1.Giro.Angulo<=90&& J1.Giro.Angulo>=0 ){
    J1.Giro.Angulo = (90-normAngulo(J1.Giro.Angulo))+90;
    Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual);
    choque = 1;
    //Serial.println("2");
  }
  //Pared superior
  else if(posY_ini<=Pista1.Limites.yo && J1.Giro.Angulo<=180 && J1.Giro.Angulo>=90){
    J1.Giro.Angulo = normAngulo(J1.Giro.Angulo)+180;
    J1.Movimiento.posY = Pista1.Limites.yo -1;
    Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual); 
    choque = 1;
    //Serial.println("3"); 
  }
  //Pared izquierda
  else if(posX_ini<=Pista1.Limites.xo && J1.Giro.Angulo>=180 && J1.Giro.Angulo<=270){
    choque = 1;
    //Serial.println("4"); 
    J1.Giro.Angulo = (90-normAngulo(J1.Giro.Angulo))+270;
    if(J1.Giro.Angulo == 360){
      J1.Giro.Angulo = 0;
    }
    Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual);   
  }
  //--------------------------Verificacion de limites de las paredes---------------------------
  //***********************************Sentido Horario*****************************************
  //Pared inferior
  if(posY_ini>=Pista1.Limites.yf && J1.Giro.Angulo<=270 && J1.Giro.Angulo>=180){
    J1.Giro.Angulo = 180 - normAngulo(J1.Giro.Angulo);
    Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual);
    choque = 1; 
    //Serial.println("5"); 
    J1.Movimiento.tRebote = millis();
  }
  //Pared derecha
  else if(posX_ini>=Pista1.Limites.xf && J1.Giro.Angulo>=270 && J1.Giro.Angulo<=360){
    //Serial.println("6"); 
    choque = 1;
    J1.Movimiento.tRebote = millis();
    J1.Giro.Angulo = 270 - (90-normAngulo(J1.Giro.Angulo));
    Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual);             
  }
  //Pared superior
  else if(posY_ini<=Pista1.Limites.yo && J1.Giro.Angulo<=180 && J1.Giro.Angulo<=90){
    J1.Giro.Angulo = 360 - normAngulo(J1.Giro.Angulo);
    Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual); 
    Serial.println("7"); 
    J1.Movimiento.tRebote = millis();
  }
  //Pared izquierda
  else if(posX_ini<=Pista1.Limites.xo && J1.Giro.Angulo>=90 && J1.Giro.Angulo<=180){
    J1.Giro.Angulo = 90 - (90-normAngulo(J1.Giro.Angulo));
    Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual); 
    choque = 1;  
    //Serial.println("8"); 
    J1.Movimiento.tRebote = millis();
  }
  //--------------------------Verificacion de limites de borde interior---------------------------
  //***********************************Sentido Horario*****************************************
   //Pared Superior
  if(posY_ini>=Pista1.Limites.yio && posY_ini<=Pista1.Limites.yif && J1.Giro.Angulo>=270 && J1.Giro.Angulo<=360 && posX_ini>=Pista1.Limites.xio && posX_ini<=Pista1.Limites.xif ){
    J1.Giro.Angulo = normAngulo(J1.Giro.Angulo);
    Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual); 
    choque = 1;
    J1.Movimiento.tRebote = millis();
    //Serial.println("9"); 
  }
  //Pared derecha
  else if(posX_ini<=Pista1.Limites.xif && posX_ini>=Pista1.Limites.xio && J1.Giro.Angulo>=180 && J1.Giro.Angulo<=270 && posY_ini>=Pista1.Limites.yio && posY_ini<=Pista1.Limites.yif ){
    //Serial.println("10"); 
    choque = 1;
    J1.Movimiento.tRebote = millis();
    J1.Giro.Angulo = (90-normAngulo(J1.Giro.Angulo))+270;
    if(J1.Giro.Angulo == 360){
      J1.Giro.Angulo = 0;
    }
    Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual);           
  }
  //Pared inferior
  else if(posY_ini<=Pista1.Limites.yif && posY_ini>=Pista1.Limites.yio && J1.Giro.Angulo>=90&& J1.Giro.Angulo<=180 && posX_ini>=Pista1.Limites.xio && posX_ini<=Pista1.Limites.xif ){
    J1.Giro.Angulo = normAngulo(J1.Giro.Angulo)+180;
    Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual); 
    choque = 1;
    //Serial.println("12"); 
    J1.Movimiento.tRebote = millis();
  }
  //Pared izquierda
  else if(posX_ini>=Pista1.Limites.xio && posX_ini<=Pista1.Limites.xif && J1.Giro.Angulo>=0 && J1.Giro.Angulo<=90 && posY_ini>=Pista1.Limites.yio && posY_ini<=Pista1.Limites.yif){
    J1.Giro.Angulo = 90-normAngulo(J1.Giro.Angulo)+90;
    Angulo_Cambia_Pos_Angular(J1.Giro.Angulo,&J1.Giro.Posicion_Angular_Actual);   
    choque = 1;
    //Serial.println("13"); 
    J1.Movimiento.tRebote = millis();
  }

}

void Giro_Girito(){
  J1.accion = !digitalRead(J1.Control.Izquierda) | !digitalRead(J1.Control.Derecha);
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
      }else if(digitalRead(J1.Control.Derecha)&&(millis()-J1.Giro.tGiro)>=J1.Control.rateGiro){
        Angulo(J1.Control.Izquierda, J1.Control.Derecha,&J1.Giro.Posicion_Angular_Actual,&J1.Giro.Angulo);
        compVelocidad(J1.Movimiento.Velocidad, J1.Giro.Angulo, &J1.Movimiento.velX, &J1.Movimiento.velY);
        LCD_Sprite(J1.Movimiento.posX,J1.Movimiento.posY,16,16,CarritoConPrivilegios,32,J1.Giro.Posicion_Angular_Actual,0,0);
        J1.Giro.tGiro = millis();
      }
      
    }
    if(!J1.accion && J1.Giro.enGiro){
      J1.Giro.enGiro = 0;  
    } 
}
