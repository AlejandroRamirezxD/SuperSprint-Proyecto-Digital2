#ifndef Funciones_De_Juego_h
#define Funciones_De_Juego_h

int PosicionMap(int Posicion);
void Angulo(int Push_Izquierdo,int Push_Derecho,int *Posicion_Angular_Actual,int *Angulo);
void compVelocidad(float velocidad, float angulo, float *velocidadX, float *velocidadY);
void movimientoCarro(float x_ini, float y_ini,unsigned long t_refresco,float velocidad, float angulo, float *x_f, float *y_f);

int PosicionMap(int Posicion){
  if(Posicion >= 0){
    return Posicion;
  }else if(Posicion >= -31){
    return Posicion + 32;  
  }
}

void Angulo (int Push_Izquierdo, int Push_Derecho, int *Posicion_Angular_Actual, float *Angulo ){
  if(!digitalRead(Push_Izquierdo)){
    *Posicion_Angular_Actual = *Posicion_Angular_Actual + 1;
    
    if(*Posicion_Angular_Actual  > 31){
      *Posicion_Angular_Actual = 0;
    }
    
    *Posicion_Angular_Actual = PosicionMap(*Posicion_Angular_Actual);
  }
  else if(!digitalRead(Push_Derecho)){
    *Posicion_Angular_Actual = *Posicion_Angular_Actual - 1;
    if(*Posicion_Angular_Actual  < -31){
      *Posicion_Angular_Actual = 0;
    }
    *Posicion_Angular_Actual = PosicionMap(*Posicion_Angular_Actual);
  }

  if(*Posicion_Angular_Actual == 0){
    *Angulo = 0;    
  }
  else if(*Posicion_Angular_Actual > 0 && *Posicion_Angular_Actual <= 26){
    *Angulo = (*Posicion_Angular_Actual)*10 + 10; 
  }
  else if(*Posicion_Angular_Actual > 26 && *Posicion_Angular_Actual <= 31){
    *Angulo = (*Posicion_Angular_Actual-26)*15 + 270;
  }
}

void compVelocidad(float velocidad, float angulo, float *velocidadX, float *velocidadY){
  *velocidadX =  velocidad*cos(angulo*PI/180);
  *velocidadY =  velocidad*sin(angulo*PI/180);
}

void movimientoCarro(float x_ini, float y_ini,unsigned long t_refresco,float velocidadX, float velocidadY, float *x_f, float *y_f){
  *x_f = x_ini + t_refresco*velocidadX;
  *y_f = y_ini - t_refresco*velocidadY;
}

#endif
