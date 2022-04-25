#ifndef Funciones_De_Juego_h
#define Funciones_De_Juego_h

int PosicionMap(int Posicion);
void Angulo(int Push_Izquierdo,int Push_Derecho,int *Posicion_Angular_Actual,int *Angulo);

int PosicionMap(int Posicion){
  if(Posicion >= 0){
    return Posicion;
  }else if(Posicion >= -31){
    return Posicion + 32;  
  }
}

void Angulo(int Push_Izquierdo, int Push_Derecho, int *Posicion_Angular_Actual, int *Angulo){
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

#endif
