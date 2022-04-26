#ifndef Funciones_De_Juego_h
#define Funciones_De_Juego_h

int PosicionMap(int Posicion);
void Angulo(int Push_Izquierdo,int Push_Derecho,int *Posicion_Angular_Actual,int *Angulo);
void compVelocidad(float velocidad, float angulo, float *velocidadX, float *velocidadY);
void movimientoCarro(float x_ini, float y_ini,unsigned long t_refresco,float velocidad, float angulo, float *x_f, float *y_f);

void Angulo_Cambia_Pos_Angular(float Angulo, int *Pos_Angular){
    if(Angulo == 0){
    *Pos_Angular = 0;  
  }
  else if(Angulo>=0 && Angulo <=270){
    *Pos_Angular = (Angulo - 10)/10;
  }
  else if(Angulo>270 && Angulo <360){
    *Pos_Angular = (Angulo - 270)/15 + 26;  
  }
}

float normAngulo(float angulo){
  float anguloNormalizado;
  if(angulo<=90){
    return angulo;  
  }
  else if(angulo <=180){
    anguloNormalizado = 180-angulo;
    return anguloNormalizado;  
  }
  else if(angulo <=270){
    anguloNormalizado = angulo-180;
    return anguloNormalizado;  
  }
  else if(angulo <=360){
    anguloNormalizado = 360-angulo;
    return anguloNormalizado;  
  }
  else{
    return angulo;  
  }  
}



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
