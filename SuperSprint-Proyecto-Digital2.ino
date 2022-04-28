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
  int enFrenado;
  unsigned long tAceleracion;
  unsigned long tFrenado;
  unsigned long tRebote;
  float Velocidad;
  float velX;
  float velY;
  float posX;
  float posY;
  };

// 
struct coordenada{
  float x;
  float y;
};

struct hitBox{
  coordenada a;
  coordenada b;
  coordenada c;
  coordenada d;
};

// Super struct que contiene a los sub structs
struct Jugador{
  hitBox HitBox;
  control Control;
  giro Giro;
  movimiento Movimiento;
  int accion;
}J1,J2;

struct Linea{
  hitBox HitBox;
}Meta;

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

unsigned long tRefLCD;
unsigned long  tRefLCD_2 ;

int drawJ1, drawJ2, choque;

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
void Actualizar_Posicion_HitBox();

extern uint8_t Mapa_Pista1[];
extern uint8_t CarritoConPrivilegios[];
extern uint8_t CarritoSinPrivilegios[];
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
  J1.Control.Derecha    = 0;
  J1.Control.Izquierda  = 0;
  J1.Control.Acelerador = 0;
  J1.Control.Drift      = 0;
  J1.Control.Freno      = 0;

  // Pines asociados a botones
  J2.Control.Derecha    = 0;
  J2.Control.Izquierda  = 0;
  J2.Control.Acelerador = 0;
  J2.Control.Drift      = 0;
  J2.Control.Freno      = 0;

  // Valores de maniobra
  J1.Control.rateGiro   = 60;
  J1.Control.turnoDrift = 0;
  J1.Movimiento.Velocidad =  0;
  J1.Movimiento.Aceleracion = 0.0000001;

   // Valores de maniobra
  J2.Control.rateGiro   = 60;
  J2.Control.turnoDrift = 0;
  J2.Movimiento.Velocidad =  0;
  J2.Movimiento.Aceleracion = 0.0000001;

  // Variables para entrar en las condiciones
  J1.Giro.enGiro = 0;
  J1.Movimiento.enMovimiento = 0;
  J1.Movimiento.enFrenado = 0;
  J1.Movimiento.tRebote = 0;

  J2.Giro.enGiro = 0;
  J2.Movimiento.enMovimiento = 0;
  J2.Movimiento.enFrenado = 0;
  J2.Movimiento.tRebote = 0;
  
  // Definir pos inicial de carrito 
  J1.Movimiento.posX = 50+5;
  J1.Movimiento.posY = 170+5;
  J2.Movimiento.posX = 50+5;
  J2.Movimiento.posY = 190+5;

  // Hit box
  // Esquina superior Izquierda
  J1.HitBox.a.x = J1.Movimiento.posX;
  J1.HitBox.a.y = J1.Movimiento.posY;

  // Esquina superior Derecha
  J1.HitBox.b.x = J1.Movimiento.posX + 32;
  J1.HitBox.b.y = J1.Movimiento.posY;

  // Esquina inferior Derecha
  J1.HitBox.c.x = J1.Movimiento.posX;
  J1.HitBox.c.y = J1.Movimiento.posY + 32;

  // Esquina inferior Izquierda
  J1.HitBox.d.x = J1.Movimiento.posX + 32;
  J1.HitBox.d.y = J1.Movimiento.posY + 32;

  //Definimos el valor de angulo inicial del carro
  J1.Giro.Posicion_Angular_Actual = 0;
  J1.Giro.Angulo = 0;
  compVelocidad(J1.Movimiento.Velocidad, J1.Giro.Angulo, &J1.Movimiento.velX, &J1.Movimiento.velY);

  //Variable de refresco
  tRefLCD = millis();
  drawJ1 = 0;
  drawJ2 = 0;

  LCD_Bitmap(0, 0, 320, 240, Mapa_Pista1);
  FillRect(150, 42, 2, 47, 0xF800); // Meta
  LCD_Sprite(J1.Movimiento.posX,J1.Movimiento.posY,16,16,CarritoConPrivilegios,32,0,0,0); // Mostrar carrito
  LCD_Sprite(J2.Movimiento.posX,J2.Movimiento.posY,16,16,CarritoSinPrivilegios,32,0,0,0); // Mostrar carrito
}
//***************************************************************************************************************************************
// LOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOP
//***************************************************************************************************************************************
void loop() {
  verificacion_Botones();
  //Primero creamos la variable que nos dice si el usuario toco un boton
  J1.accion = J1.Control.Acelerador | J1.Control.Freno;
  J1.accion = J2.Control.Acelerador | J2.Control.Freno;
  Actualizar_Posicion_HitBox();
  
  
  Meta.HitBox.a.x = 150;
  Meta.HitBox.a.y = 42;
  Meta.HitBox.b.x = 150+2;
  Meta.HitBox.b.y = 42;
  Meta.HitBox.c.x = 150;
  Meta.HitBox.c.y = 42+31;
  Meta.HitBox.d.x = 150+2;;
  Meta.HitBox.d.y = 42+31;

  
 
  //choque  = 0;
  accionMovimiento(&J1);
  accionMovimiento(&J2);

  Giro_Girito(&J1);
  Giro_Girito(&J2);

  

  // CarritoConPrivilegios
  if(!drawJ1 && !tRefLCD_2){
    drawJ1 = 1;
    tRefLCD = millis();
  }else if((millis() - tRefLCD )>25){
    LCD_Sprite(J1.Movimiento.posX,J1.Movimiento.posY,16,16,CarritoConPrivilegios,32,J1.Giro.Posicion_Angular_Actual,0,0);
  }else if((millis() - tRefLCD )>25 && drawJ1 == 1){
    drawJ1 = 0;
    tRefLCD_2 = (millis() - tRefLCD );
  }
  
  if(!drawJ2 && tRefLCD_2 > 25){
    drawJ2 = 1;
    tRefLCD = millis();
  }else if((millis() - tRefLCD )>50){
    LCD_Sprite(J2.Movimiento.posX,J2.Movimiento.posY,16,16,CarritoSinPrivilegios,32,J2.Giro.Posicion_Angular_Actual,0,0);
  }else if((millis() - tRefLCD )>25 && drawJ2 == 1){
    drawJ2 = 0;
    tRefLCD_2 = 0;
  }

  
}
//***************************************************************************************************************************************
// LOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOP
//***************************************************************************************************************************************
void Actualizar_Posicion_HitBox(){
    // Hit box J1-------------------------------------------------------------------
  // Esquina superior Izquierda
  J1.HitBox.a.x = J1.Movimiento.posX;
  J1.HitBox.a.y = J1.Movimiento.posY;

  // Esquina superior Derecha
  J1.HitBox.b.x = J1.Movimiento.posX + 32;
  J1.HitBox.b.y = J1.Movimiento.posY;

  // Esquina inferior Derecha
  J1.HitBox.c.x = J1.Movimiento.posX;
  J1.HitBox.c.y = J1.Movimiento.posY + 32;

  // Esquina inferior Izquierda
  J1.HitBox.d.x = J1.Movimiento.posX + 32;
  J1.HitBox.d.y = J1.Movimiento.posY + 32;

  // Hit box J2-------------------------------------------------------------------
  // Esquina superior Izquierda
  J2.HitBox.a.x = J2.Movimiento.posX;
  J2.HitBox.a.y = J2.Movimiento.posY;

  // Esquina superior Derecha
  J2.HitBox.b.x = J2.Movimiento.posX + 32;
  J2.HitBox.b.y = J2.Movimiento.posY;

  // Esquina inferior Derecha
  J2.HitBox.c.x = J2.Movimiento.posX;
  J2.HitBox.c.y = J2.Movimiento.posY + 32;

  // Esquina inferior Izquierda
  J2.HitBox.d.x = J2.Movimiento.posX + 32;
  J2.HitBox.d.y = J2.Movimiento.posY + 32;
  
  /*
  Serial.print("PosX: ");
  Serial.print(J1.Movimiento.posX);
  Serial.print("PosY: ");
  Serial.print(J1.Movimiento.posY);
  
  Serial.print(" a: ");
  Serial.print(J1.HitBox.a.x);
  Serial.print(",");
  Serial.print(J1.HitBox.a.y);

  Serial.print(" b: ");
  Serial.print(J1.HitBox.b.x);
  Serial.print(",");
  Serial.print(J1.HitBox.b.y);

  Serial.print(" c: ");
  Serial.print(J1.HitBox.c.x);
  Serial.print(",");
  Serial.print(J1.HitBox.c.y);

  Serial.print(" d: ");
  Serial.print(J1.HitBox.d.x);
  Serial.print(",");
  Serial.println(J1.HitBox.d.y);*/
}

void Condiciones_Colisones(struct Jugador *carro){
  float posX_ini = carro->Movimiento.posX;
  float posY_ini = carro->Movimiento.posY;
  float  vel_ini = carro->Movimiento.Velocidad;
  
  // El sprite se encuentra dentro del borde exterior en coordenadas x
  if( posX_ini > Pista1.Limites.xo && posX_ini < Pista1.Limites.xf){
    // El sprite se encuentra dentro del borde exterior en coordenadas y
    if(posY_ini > Pista1.Limites.yo && posY_ini < Pista1.Limites.yf){ 
      
    }
  }

  
  //--------------------------Verificacion de limites de las paredes---------------------------
  //*******************************Sentido Antihorario*****************************************
  //Pared inferior
  if(posY_ini>=Pista1.Limites.yf && carro->Giro.Angulo>=270){
    carro->Giro.Angulo = normAngulo(carro->Giro.Angulo);
    Angulo_Cambia_Pos_Angular(carro->Giro.Angulo,&carro->Giro.Posicion_Angular_Actual); 
    choque = 1;
    //Serial.println("1");
  }
  //Pared derecha
  else if(posX_ini>=Pista1.Limites.xf && carro->Giro.Angulo<=90&& carro->Giro.Angulo>=0 ){
    carro->Giro.Angulo = (90-normAngulo(carro->Giro.Angulo))+90;
    Angulo_Cambia_Pos_Angular(carro->Giro.Angulo,&carro->Giro.Posicion_Angular_Actual);
    choque = 1;
    //Serial.println("2");
  }
  //Pared superior
  else if(posY_ini<=Pista1.Limites.yo && carro->Giro.Angulo<=180 && carro->Giro.Angulo>=90){
    carro->Giro.Angulo = normAngulo(carro->Giro.Angulo)+180;
    carro->Movimiento.posY = Pista1.Limites.yo -1;
    Angulo_Cambia_Pos_Angular(carro->Giro.Angulo,&carro->Giro.Posicion_Angular_Actual); 
    choque = 1;
    //Serial.println("3"); 
  }
  //Pared izquierda
  else if(posX_ini<=Pista1.Limites.xo && carro->Giro.Angulo>=180 && carro->Giro.Angulo<=270){
    choque = 1;
    //Serial.println("4"); 
    carro->Giro.Angulo = (90-normAngulo(carro->Giro.Angulo))+270;
    if(carro->Giro.Angulo == 360){
      carro->Giro.Angulo = 0;
    }
    Angulo_Cambia_Pos_Angular(carro->Giro.Angulo,&carro->Giro.Posicion_Angular_Actual);   
  }
  //--------------------------Verificacion de limites de las paredes---------------------------
  //***********************************Sentido Horario*****************************************
  //Pared inferior
  if(posY_ini>=Pista1.Limites.yf && carro->Giro.Angulo<=270 && carro->Giro.Angulo>=180){
    carro->Giro.Angulo = 180 - normAngulo(carro->Giro.Angulo);
    Angulo_Cambia_Pos_Angular(carro->Giro.Angulo,&carro->Giro.Posicion_Angular_Actual);
    choque = 1; 
    //Serial.println("5"); 
    carro->Movimiento.tRebote = millis();
  }
  //Pared derecha
  else if(posX_ini>=Pista1.Limites.xf && carro->Giro.Angulo>=270 && carro->Giro.Angulo<=360){
    //Serial.println("6"); 
    choque = 1;
    carro->Movimiento.tRebote = millis();
    carro->Giro.Angulo = 270 - (90-normAngulo(carro->Giro.Angulo));
    Angulo_Cambia_Pos_Angular(carro->Giro.Angulo,&carro->Giro.Posicion_Angular_Actual);             
  }
  //Pared superior
  else if(posY_ini<=Pista1.Limites.yo && carro->Giro.Angulo<=180 && carro->Giro.Angulo<=90){
    carro->Giro.Angulo = 360 - normAngulo(carro->Giro.Angulo);
    Angulo_Cambia_Pos_Angular(carro->Giro.Angulo,&carro->Giro.Posicion_Angular_Actual); 
    Serial.println("7"); 
    carro->Movimiento.tRebote = millis();
  }
  //Pared izquierda
  else if(posX_ini<=Pista1.Limites.xo && carro->Giro.Angulo>=90 && carro->Giro.Angulo<=180){
    carro->Giro.Angulo = 90 - (90-normAngulo(carro->Giro.Angulo));
    Angulo_Cambia_Pos_Angular(carro->Giro.Angulo,&carro->Giro.Posicion_Angular_Actual); 
    choque = 1;  
    //Serial.println("8"); 
    carro->Movimiento.tRebote = millis();
  }
  //--------------------------Verificacion de limites de borde interior---------------------------
  //***********************************Sentido Horario*****************************************
   //Pared Superior
  if(posY_ini>=Pista1.Limites.yio && posY_ini<=Pista1.Limites.yif && carro->Giro.Angulo>=270 && carro->Giro.Angulo<=360 && posX_ini>=Pista1.Limites.xio && posX_ini<=Pista1.Limites.xif ){
    carro->Giro.Angulo = normAngulo(carro->Giro.Angulo);
    Angulo_Cambia_Pos_Angular(carro->Giro.Angulo,&carro->Giro.Posicion_Angular_Actual); 
    choque = 1;
    carro->Movimiento.tRebote = millis();
    //Serial.println("9"); 
  }
  //Pared derecha
  else if(posX_ini<=Pista1.Limites.xif && posX_ini>=Pista1.Limites.xio && carro->Giro.Angulo>=180 && carro->Giro.Angulo<=270 && posY_ini>=Pista1.Limites.yio && posY_ini<=Pista1.Limites.yif ){
    //Serial.println("10"); 
    choque = 1;
    carro->Movimiento.tRebote = millis();
    carro->Giro.Angulo = (90-normAngulo(carro->Giro.Angulo))+270;
    if(carro->Giro.Angulo == 360){
      carro->Giro.Angulo = 0;
    }
    Angulo_Cambia_Pos_Angular(carro->Giro.Angulo,&carro->Giro.Posicion_Angular_Actual);           
  }
  //Pared inferior
  else if(posY_ini<=Pista1.Limites.yif && posY_ini>=Pista1.Limites.yio && carro->Giro.Angulo>=90&& carro->Giro.Angulo<=180 && posX_ini>=Pista1.Limites.xio && posX_ini<=Pista1.Limites.xif ){
    carro->Giro.Angulo = normAngulo(carro->Giro.Angulo)+180;
    Angulo_Cambia_Pos_Angular(carro->Giro.Angulo,&carro->Giro.Posicion_Angular_Actual); 
    choque = 1;
    //Serial.println("12"); 
    carro->Movimiento.tRebote = millis();
  }
  //Pared izquierda
  else if(posX_ini>=Pista1.Limites.xio && posX_ini<=Pista1.Limites.xif && carro->Giro.Angulo>=0 && carro->Giro.Angulo<=90 && posY_ini>=Pista1.Limites.yio && posY_ini<=Pista1.Limites.yif){
    carro->Giro.Angulo = 90-normAngulo(carro->Giro.Angulo)+90;
    Angulo_Cambia_Pos_Angular(carro->Giro.Angulo,&carro->Giro.Posicion_Angular_Actual);   
    choque = 1;
    //Serial.println("13"); 
    carro->Movimiento.tRebote = millis();
  }

}

void Giro_Girito(struct Jugador *carro){
  carro->accion = carro->Control.Izquierda | carro->Control.Derecha;
    if(carro->accion && !carro->Giro.enGiro){
      /* El usuario pulso un boton de giro
       * por primera vez
       */
      carro->Giro.tGiro = millis();
      carro->Giro.enGiro = 1;
    }else if(carro->accion && carro->Giro.enGiro){
      /*    
       *     El carro gira cada carro->Control.rateGiro ms
       *     carro->Control.turnoDrift
       */
      
      if(!carro->Control.turnoDrift && carro->Control.Drift){
        carro->Control.turnoDrift = 1;
        carro->Control.rateGiro   = 20;
                
      }else if(carro->Control.turnoDrift && carro->Control.Drift){
        //carro->Control.turnoDrift = 0
        carro->Control.rateGiro   = 60;
        carro->Control.turnoDrift = 0;
        
      }
             
      if(carro->Control.Izquierda&&(millis()-carro->Giro.tGiro)>=carro->Control.rateGiro){
        FillRect(150, 42, 2, 47, 0xF800); // Meta
        Angulo(carro->Control.Izquierda, carro->Control.Derecha,&carro->Giro.Posicion_Angular_Actual,&carro->Giro.Angulo);
        compVelocidad(carro->Movimiento.Velocidad, carro->Giro.Angulo, &carro->Movimiento.velX, &carro->Movimiento.velY);
        carro->Giro.tGiro = millis();
      }else if(carro->Control.Derecha && (millis()-carro->Giro.tGiro)>=carro->Control.rateGiro){
        FillRect(150, 42, 2, 47, 0xF800); // Meta
        Angulo(carro->Control.Izquierda, carro->Control.Derecha,&carro->Giro.Posicion_Angular_Actual,&carro->Giro.Angulo);
        compVelocidad(carro->Movimiento.Velocidad, carro->Giro.Angulo, &carro->Movimiento.velX, &carro->Movimiento.velY);
        carro->Giro.tGiro = millis();
      }
      
    }
    if(!carro->accion && carro->Giro.enGiro){
      carro->Giro.enGiro = 0;  
    } 
}

void accionMovimiento(struct Jugador *carro){
  if(carro->accion){
    Serial.println("Entro");
    //**************************************************************************************************
    //**************************************************************************************************
    //**************************************BOTON DE ACELERADOR*****************************************
    //**************************************************************************************************
    //**************************************************************************************************
    // Se determina el tiempo inicial de la duracion del movimiento (Al acelerar)
    if(carro->Control.Acelerador && !carro->Movimiento.enMovimiento){
      carro->Movimiento.tAceleracion = millis();
      carro->Movimiento.enMovimiento = 1;   
    }
    // Se realiza el movimiento, tomando en cuenta la referencia del tiempo anterior (Mientras se acelera)
    else if(carro->Control.Acelerador && carro->Movimiento.enMovimiento){
      
      if(carro->Control.Acelerador && (millis()-carro->Movimiento.tAceleracion)>=40){
        Condiciones_Colisones(carro);
         float posX_ini = carro->Movimiento.posX;
         float posY_ini = carro->Movimiento.posY;
         float  vel_ini = carro->Movimiento.Velocidad;

        /* ---------------------------------------------------------------------------------------------
         * **********************************Código de Aceleración**************************************
         * ---------------------------------------------------------------------------------------------
         */
  
        carro->Movimiento.Velocidad = carro->Movimiento.Velocidad + carro->Movimiento.Aceleracion*(millis()-carro->Movimiento.tAceleracion);

        if(carro->Movimiento.Velocidad >= 0.015){
            carro->Movimiento.Velocidad = 0.015;
        }

        
        FillRect(150, 42, 2, 47, 0xF800); // Meta
        compVelocidad(carro->Movimiento.Velocidad,carro->Giro.Angulo, &carro->Movimiento.velX, &carro->Movimiento.velY);
        movimientoCarro(posX_ini,posY_ini,40, carro->Movimiento.velX, carro->Movimiento.velY, &carro->Movimiento.posX,&carro->Movimiento.posY);          
        V_line( carro->Movimiento.posX - posX_ini, 180, 16,  0x632C); 
      }    
    }

    // Al soltar el acelerador, se sale de las condiciones y no se mueve
    if(!carro->Control.Acelerador && carro->Movimiento.enMovimiento){
      carro->Movimiento.enMovimiento = 0;  
    }

    //**************************************************************************************************
    //**************************************************************************************************
    //****************************************BOTON DE FRENO********************************************
    //**************************************************************************************************
    //**************************************************************************************************
    // Se determina el tiempo inicial de la duracion del movimiento (Al acelerar)
    Serial.println(carro->Movimiento.enFrenado);
    if(carro->Control.Freno && !carro->Movimiento.enFrenado){
      Serial.println("Frenando");
      carro->Movimiento.tFrenado = millis();
      carro->Movimiento.enFrenado = 1;   
    }
    // Se realiza el movimiento, tomando en cuenta la referencia del tiempo anterior (Mientras se acelera)
    else if(carro->Control.Freno && carro->Movimiento.enFrenado){
      
      if(carro->Control.Freno&&(millis()-carro->Movimiento.tFrenado)>=40){
         Condiciones_Colisones(carro);
         float posX_ini = carro->Movimiento.posX;
         float posY_ini = carro->Movimiento.posY;
         float  vel_ini = carro->Movimiento.Velocidad;

        /* ---------------------------------------------------------------------------------------------
         * **********************************Código de Frenado******************************************
         * ---------------------------------------------------------------------------------------------
         */
  
        carro->Movimiento.Velocidad = carro->Movimiento.Velocidad - 2*carro->Movimiento.Aceleracion*(millis()-carro->Movimiento.tFrenado);

        if(carro->Movimiento.Velocidad <= 0){
            carro->Movimiento.Velocidad = 0;
        }

        
        FillRect(150, 42, 2, 47, 0xF800); // Meta
        compVelocidad(carro->Movimiento.Velocidad,carro->Giro.Angulo, &carro->Movimiento.velX, &carro->Movimiento.velY);
        movimientoCarro(posX_ini,posY_ini,40, carro->Movimiento.velX, carro->Movimiento.velY, &carro->Movimiento.posX,&carro->Movimiento.posY);          
        V_line( carro->Movimiento.posX - posX_ini, 180, 16,  0x632C); 
      }    
    }

    // Al soltar el acelerador, se sale de las condiciones y no se mueve
    if(!carro->Control.Freno && carro->Movimiento.enFrenado){
      carro->Movimiento.enFrenado = 0;  
    }
    
  }
  else{
    if(!carro->Movimiento.enMovimiento){
      carro->Movimiento.tAceleracion = millis();
      carro->Movimiento.enMovimiento = 1;  
    }
    else if(carro->Movimiento.enMovimiento == 1 && (millis()-carro->Movimiento.tAceleracion)>=2){

      Condiciones_Colisones(carro);
      
      float posX_ini = carro->Movimiento.posX;
      float posY_ini = carro->Movimiento.posY;
      float  vel_ini = carro->Movimiento.Velocidad;
      
      
      carro->Movimiento.Velocidad = vel_ini - carro->Movimiento.Aceleracion*(millis()-carro->Movimiento.tAceleracion)*50;
      
        if(carro->Movimiento.Velocidad <= 0){
           carro->Movimiento.Velocidad = 0;
        }
      
      FillRect(150, 42, 2, 47, 0xF800); // Meta
      compVelocidad(carro->Movimiento.Velocidad*1000,carro->Giro.Angulo, &carro->Movimiento.velX, &carro->Movimiento.velY);
      movimientoCarro(posX_ini,posY_ini,40, carro->Movimiento.velX/1000, carro->Movimiento.velY/1000, &carro->Movimiento.posX,&carro->Movimiento.posY);   
      
      
      
      
      carro->Movimiento.enMovimiento = 0;   
      carro->Movimiento.tAceleracion = millis();
    }
  }
}

//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
//------------------------------------ACTUALIZACIÓN DEL ESTADO DE LOS BOTONES----------------------------------------
//-------------------------------------------------------------------------------------------------------------------
void ascii_to_BSignal(unsigned char character){
  //A partir de un switch determinamos la acción a realizar
  switch(character){
    //Caso 0: presiona el boton de izquierda J1
    case 'A':
      J1.Control.Izquierda = 1;
    break;
    //Caso 1: suelta el boton de izquierda J1
    case 'a':
      J1.Control.Izquierda = 0;
    break;
    //Caso 2: presiona el boton de derecha J1
    case 'B':
      J1.Control.Derecha = 1;
    break;
    //Caso 3: suelta el boton derecha J1
    case 'b':
      J1.Control.Derecha = 0;
    break;
    //Caso 4: presiona el boton acelerador J1
    case 'C':
      J1.Control.Acelerador = 1;
    break;
    //Caso 5: suelta el boton acelerador J1
    case 'c':
      J1.Control.Acelerador = 0;
    break;
    //Caso 6: presiona el boton freno J1
    case 'D':
      J1.Control.Freno = 1;
    break;
    //Caso 7: suelta el boton freno J1
    case 'd':
      J1.Control.Freno = 0;
    break;
    //Caso 8: presiona el botn drift J1
    case 'E':
      J1.Control.Drift = 1;
    break;
    //Caso 9: suelta el botn drift J1
    case 'e':
      J1.Control.Drift = 0;
    break;
    //Caso 10: presiona el boton de izquierda J2
    case 'Z':
      J2.Control.Izquierda = 1;
    break;
    //Caso 11: suelta el boton de izquierda J2
    case 'z':
      J2.Control.Izquierda = 0;
    break;
    //Caso 12: presiona el boton de derecha J2
    case 'Y':
      J2.Control.Derecha = 1;
    break;
    //Caso 13: suelta el boton derecha J2
    case 'y':
      J2.Control.Derecha = 0;
    break;
    //Caso 14: presiona el boton acelerador J2
    case 'X':
      J2.Control.Acelerador = 1;
    break;
    //Caso 15: suelta el boton acelerador J2
    case 'x':
      J2.Control.Acelerador = 0;
    break;
    //Caso 16: presiona el boton freno J2
    case 'W':
      J2.Control.Freno = 1;
    break;
    //Caso 17: suelta el boton freno J2
    case 'w':
      J2.Control.Freno = 0;
    break;
    //Caso 18: presiona el botn drift J2
    case 'V':
      J2.Control.Drift = 1;
    break;
    //Caso 19: suelta el botn drift J2
    case 'v':
      J2.Control.Drift = 0;
    break;
  }  
}

void verificacion_Botones(void){
  while(Serial3.available()){
    unsigned char caracter = Serial3.read();
    ascii_to_BSignal(caracter);  
  }
}
