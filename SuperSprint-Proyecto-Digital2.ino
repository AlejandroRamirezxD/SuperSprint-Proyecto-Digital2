
//***************************************************************************************************************************************
/* Librería para el uso de la pantalla ILI9341 en modo 8 bits
   Basado en el código de martinayotte - https://www.stm32duino.com/viewtopic.php?t=637
   Adaptación, migración y creación de nuevas funciones: Pablo Mazariegos y José Morales
   Con ayuda de: José Guerra
   IE3027: Electrónica Digital 2 - 2019
*/
//***************************************************************************************************************************************

/*
+----------------------------------------------------------------------------------+
|                                    LIBRERIAS                                     |                                   
+----------------------------------------------------------------------------------+
*/
#include <stdint.h>
#include <stdbool.h>
#include <TM4C123GH6PM.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

// Graficos
#include "bitmaps.h"
#include "font.h"
#include "lcd_registers.h"

// Funciones de Juego y control
#include "Funciones_De_Juego.h"


/*
+----------------------------------------------------------------------------------+
|                                     PUERTOS                                      |                                   
+----------------------------------------------------------------------------------+
*/
// Puertos y arreglos de la LCD
#define LCD_RST PD_0
#define LCD_CS PD_1
#define LCD_RS PD_2
#define LCD_WR PD_3
#define LCD_RD PE_1
int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7};

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
void LCD_Init(void);
void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Clear(unsigned int c);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void LCD_Print(String text, int x, int y, int fontSize, int color, int background);

void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset);

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
  J1.Control.rateGiro   = 3;
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
        
        if (posX_ini <= Pista1.Limites.xo || posX_ini >= Pista1.Limites.xf){
          //Serial.print("uwu");
          J1.Giro.Angulo = J1.Giro.Angulo + 2*(90-normAngulo(J1.Giro.Angulo));          
        }
        if(posY_ini < Pista1.Limites.yo || posY_ini > Pista1.Limites.yf){ 
          J1.Movimiento.velY = -J1.Movimiento.velY;        
          //Serial.print("entra");
        }
        compVelocidad(J1.Movimiento.Velocidad, J1.Giro.Angulo, &J1.Movimiento.velX, &J1.Movimiento.velY);
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
        Serial.println(J1.Giro.Posicion_Angular_Actual);
        Angulo(J1.Control.Izquierda, J1.Control.Derecha,&J1.Giro.Posicion_Angular_Actual,&J1.Giro.Angulo);
        compVelocidad(J1.Movimiento.Velocidad, J1.Giro.Angulo, &J1.Movimiento.velX, &J1.Movimiento.velY);
        LCD_Sprite(J1.Movimiento.posX,J1.Movimiento.posY,16,16,CarritoConPrivilegios,32,J1.Giro.Posicion_Angular_Actual,0,0);
        J1.Giro.tGiro = millis();
        Serial.println(J1.Giro.Posicion_Angular_Actual);
        Serial.println();
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

//***************************************************************************************************************************************
// Función para inicializar LCD
//***************************************************************************************************************************************
void LCD_Init(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++) {
    pinMode(DPINS[i], OUTPUT);
  }
  //****************************************
  // Secuencia de Inicialización
  //****************************************
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(20);
  digitalWrite(LCD_RST, HIGH);
  delay(150);
  digitalWrite(LCD_CS, LOW);
  //****************************************
  LCD_CMD(0xE9);  // SETPANELRELATED
  LCD_DATA(0x20);
  //****************************************
  LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
  delay(100);
  //****************************************
  LCD_CMD(0xD1);    // (SETVCOM)
  LCD_DATA(0x00);
  LCD_DATA(0x71);
  LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0xD0);   // (SETPOWER)
  LCD_DATA(0x07);
  LCD_DATA(0x01);
  LCD_DATA(0x08);
  //****************************************
  LCD_CMD(0x36);  // (MEMORYACCESS)
  LCD_DATA(0x40 | 0x80 | 0x20 | 0x08); // LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
  LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
  //****************************************
  LCD_CMD(0xC1);    // (POWERCONTROL2)
  LCD_DATA(0x10);
  LCD_DATA(0x10);
  LCD_DATA(0x02);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
  LCD_DATA(0x00);
  LCD_DATA(0x35);
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
  LCD_DATA(0x04); // 72Hz
  //****************************************
  LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
  LCD_DATA(0x01);
  LCD_DATA(0x44);
  //****************************************
  LCD_CMD(0xC8); //Set Gamma  (GAMMASET)
  LCD_DATA(0x04);
  LCD_DATA(0x67);
  LCD_DATA(0x35);
  LCD_DATA(0x04);
  LCD_DATA(0x08);
  LCD_DATA(0x06);
  LCD_DATA(0x24);
  LCD_DATA(0x01);
  LCD_DATA(0x37);
  LCD_DATA(0x40);
  LCD_DATA(0x03);
  LCD_DATA(0x10);
  LCD_DATA(0x08);
  LCD_DATA(0x80);
  LCD_DATA(0x00);
  //****************************************
  LCD_CMD(0x2A); // Set_column_address 320px (CASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x3F);
  //****************************************
  LCD_CMD(0x2B); // Set_page_address 480px (PASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0xE0);
  //  LCD_DATA(0x8F);
  LCD_CMD(0x29); //display on
  LCD_CMD(0x2C); //display on

  LCD_CMD(ILI9341_INVOFF); //Invert Off
  delay(120);
  LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  LCD_CMD(ILI9341_DISPON);    //Display on
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar comandos a la LCD - parámetro (comando)
//***************************************************************************************************************************************
void LCD_CMD(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = cmd;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar datos a la LCD - parámetro (dato)
//***************************************************************************************************************************************
void LCD_DATA(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = data;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para definir rango de direcciones de memoria con las cuales se trabajara (se define una ventana)
//***************************************************************************************************************************************
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
  LCD_CMD(0x2a); // Set_column_address 4 parameters
  LCD_DATA(x1 >> 8);
  LCD_DATA(x1);
  LCD_DATA(x2 >> 8);
  LCD_DATA(x2);
  LCD_CMD(0x2b); // Set_page_address 4 parameters
  LCD_DATA(y1 >> 8);
  LCD_DATA(y1);
  LCD_DATA(y2 >> 8);
  LCD_DATA(y2);
  LCD_CMD(0x2c); // Write_memory_start
}
//***************************************************************************************************************************************
// Función para borrar la pantalla - parámetros (color)
//***************************************************************************************************************************************
void LCD_Clear(unsigned int c) {
  unsigned int x, y;
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  SetWindows(0, 0, 319, 239); // 479, 319);
  for (x = 0; x < 320; x++)
    for (y = 0; y < 240; y++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);
    }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea horizontal - parámetros ( coordenada x, cordenada y, longitud, color)
//***************************************************************************************************************************************
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + x;
  SetWindows(x, y, l, y);
  j = l;// * 2;
  for (i = 0; i < l; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea vertical - parámetros ( coordenada x, cordenada y, longitud, color)
//***************************************************************************************************************************************
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + y;
  SetWindows(x, y, x, l);
  j = l; //* 2;
  for (i = 1; i <= j; i++) {
    LCD_DATA(c >> 8);
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  H_line(x  , y  , w, c);
  H_line(x  , y + h, w, c);
  V_line(x  , y  , h, c);
  V_line(x + w, y  , h, c);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo relleno - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
/*void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  unsigned int i;
  for (i = 0; i < h; i++) {
    H_line(x  , y  , w, c);
    H_line(x  , y+i, w, c);
  }
  }
*/

void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 = x + w;
  y2 = y + h;
  SetWindows(x, y, x2 - 1, y2 - 1);
  unsigned int k = w * h * 2 - 1;
  unsigned int i, j;
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);

      //LCD_DATA(bitmap[k]);
      k = k - 2;
    }
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar texto - parámetros ( texto, coordenada x, cordenada y, color, background)
//***************************************************************************************************************************************
void LCD_Print(String text, int x, int y, int fontSize, int color, int background) {
  int fontXSize ;
  int fontYSize ;

  if (fontSize == 1) {
    fontXSize = fontXSizeSmal ;
    fontYSize = fontYSizeSmal ;
  }
  if (fontSize == 2) {
    fontXSize = fontXSizeBig ;
    fontYSize = fontYSizeBig ;
  }

  char charInput ;
  int cLength = text.length();
  Serial.println(cLength, DEC);
  int charDec ;
  int c ;
  int charHex ;
  char char_array[cLength + 1];
  text.toCharArray(char_array, cLength + 1) ;
  for (int i = 0; i < cLength ; i++) {
    charInput = char_array[i];
    Serial.println(char_array[i]);
    charDec = int(charInput);
    digitalWrite(LCD_CS, LOW);
    SetWindows(x + (i * fontXSize), y, x + (i * fontXSize) + fontXSize - 1, y + fontYSize );
    long charHex1 ;
    for ( int n = 0 ; n < fontYSize ; n++ ) {
      if (fontSize == 1) {
        charHex1 = pgm_read_word_near(smallFont + ((charDec - 32) * fontYSize) + n);
      }
      if (fontSize == 2) {
        charHex1 = pgm_read_word_near(bigFont + ((charDec - 32) * fontYSize) + n);
      }
      for (int t = 1; t < fontXSize + 1 ; t++) {
        if (( charHex1 & (1 << (fontXSize - t))) > 0 ) {
          c = color ;
        } else {
          c = background ;
        }
        LCD_DATA(c >> 8);
        LCD_DATA(c);
      }
    }
    digitalWrite(LCD_CS, HIGH);
  }
}
//***************************************************************************************************************************************
// Función para dibujar una imagen a partir de un arreglo de colores (Bitmap) Formato (Color 16bit R 5bits G 6bits B 5bits)
//***************************************************************************************************************************************
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 = x + width;
  y2 = y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  unsigned int k = 0;
  unsigned int i, j;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k + 1]);
      //LCD_DATA(bitmap[k]);
      k = k + 2;
    }
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una imagen sprite - los parámetros columns = número de imagenes en el sprite, index = cual desplegar, flip = darle vuelta
//***************************************************************************************************************************************
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[], int columns, int index, char flip, char offset) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);

  unsigned int x2, y2;
  x2 =   x + width;
  y2 =    y + height;
  SetWindows(x, y, x2 - 1, y2 - 1);
  int k = 0;
  int ancho = ((width * columns));
  if (flip) {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width - 1 - offset) * 2;
      k = k + width * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k + 1]);
        k = k - 2;
      }
    }
  } else {
    for (int j = 0; j < height; j++) {
      k = (j * (ancho) + index * width + 1 + offset) * 2;
      for (int i = 0; i < width; i++) {
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k + 1]);
        k = k + 2;
      }
    }


  }
  digitalWrite(LCD_CS, HIGH);
}
