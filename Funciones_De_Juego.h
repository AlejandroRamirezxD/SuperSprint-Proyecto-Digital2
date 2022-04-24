#ifndef Funciones_De_Juego_h
#define Funciones_De_Juego_h

void Angulo(int Push_Izquierdo,int Push_Derecho,int *Posicion_Angular_Actual,int *Angulo);
int PosicionMap(int Posicion);


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
}



int PosicionMap(int Posicion){
  if(Posicion >= 0){
    return Posicion;
  }else if(Posicion >= -31){
    return Posicion + 32;  
  }
}


#endif
