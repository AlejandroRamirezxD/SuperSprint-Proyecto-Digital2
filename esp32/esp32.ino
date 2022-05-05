struct control{
  int Izquierda;
  int Derecha;
  int Acelerador;
  int Freno;
  int Drift;
  }J1,J2;


  int TurnoJ1I = 0;
  int Turno2J1I = 0;
  int Turno3J1I = 0;
  unsigned long tTurnoJ1I ;
  unsigned long t2TurnoJ1I ;
  int REFRESCO = 15;

  int TurnoJ1D = 0;
  int Turno2J1D = 0;
  int Turno3J1D = 0;
  unsigned long tTurnoJ1D ;
  unsigned long t2TurnoJ1D ;

  int TurnoJ1Dr = 0;
  int Turno2J1Dr = 0;
  int Turno3J1Dr = 0;
  unsigned long tTurnoJ1Dr ;
  unsigned long t2TurnoJ1Dr ;

  int TurnoJ1A = 0;
  int Turno2J1A = 0;
  int Turno3J1A = 0;
  unsigned long tTurnoJ1A ;
  unsigned long t2TurnoJ1A ;

  int TurnoJ1F = 0;
  int Turno2J1F = 0;
  int Turno3J1F = 0;
  unsigned long tTurnoJ1F ;
  unsigned long t2TurnoJ1F ;

  int TurnoJ2I = 0;
  int Turno2J2I = 0;
  int Turno3J2I = 0;
  unsigned long tTurnoJ2I ;
  unsigned long t2TurnoJ2I ;
 
  int TurnoJ2D = 0;
  int Turno2J2D = 0;
  int Turno3J2D = 0;
  unsigned long tTurnoJ2D ;
  unsigned long t2TurnoJ2D ;

  int TurnoJ2Dr = 0;
  int Turno2J2Dr = 0;
  int Turno3J2Dr = 0;
  unsigned long tTurnoJ2Dr ;
  unsigned long t2TurnoJ2Dr ;

  int TurnoJ2A = 0;
  int Turno2J2A = 0;
  int Turno3J2A = 0;
  unsigned long tTurnoJ2A ;
  unsigned long t2TurnoJ2A ;

  int TurnoJ2F = 0;
  int Turno2J2F = 0;
  int Turno3J2F = 0;
  unsigned long tTurnoJ2F ;
  unsigned long t2TurnoJ2F ;

  char ReciboDeTiva = 0;
  int pinReset = 35;
  
void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
  pinMode(pinReset, OUTPUT);
  
  // Pines asociados a botones
  J1.Derecha    = 26;
  J1.Izquierda  = 12;
  J1.Acelerador = 33;
  J1.Drift      = 25;
  J1.Freno      = 23;

  // Pines asociados a botones
  J2.Derecha    = 22;
  J2.Izquierda  = 32;
  J2.Acelerador = 13;
  J2.Drift      = 27;
  J2.Freno      = 14;

  //J1
  pinMode(J1.Izquierda, INPUT_PULLUP);
  pinMode(J1.Derecha, INPUT_PULLUP);
  pinMode(J1.Acelerador, INPUT_PULLUP);
  pinMode(J1.Drift, INPUT_PULLUP);
  pinMode(J1.Freno, INPUT_PULLUP);    

  //J2
  pinMode(J2.Izquierda, INPUT_PULLUP);
  pinMode(J2.Derecha, INPUT_PULLUP);
  pinMode(J2.Acelerador, INPUT_PULLUP);
  pinMode(J2.Drift, INPUT_PULLUP);
  pinMode(J2.Freno, INPUT_PULLUP); 
  digitalWrite(pinReset, HIGH);
}

void loop() {
  
  while(Serial1.available()>0){
    ReciboDeTiva = Serial1.read();
    Serial.print(ReciboDeTiva);
    break;
  }

  if(ReciboDeTiva == 'R'){
    digitalWrite(pinReset, LOW);
    delay(5);
  }else{
    Serial.print(ReciboDeTiva);
  }
  digitalWrite(pinReset, HIGH);
  
  Boton( J1.Izquierda, &TurnoJ1I, &Turno2J1I, &Turno3J1I, tTurnoJ1I, t2TurnoJ1I, REFRESCO, 'A', 'a'); 
  Boton( J1.Derecha, &TurnoJ1D, &Turno2J1D, &Turno3J1D, tTurnoJ1D, t2TurnoJ1D, REFRESCO, 'B', 'b');   
  Boton( J1.Acelerador, &TurnoJ1A, &Turno2J1A, &Turno3J1A, tTurnoJ1A, t2TurnoJ1A, REFRESCO, 'C', 'c'); 
  Boton( J1.Drift, &TurnoJ1Dr, &Turno2J1Dr, &Turno3J1Dr, tTurnoJ1Dr, t2TurnoJ1Dr, REFRESCO, 'E', 'e');     
  Boton( J1.Freno, &TurnoJ1F, &Turno2J1F, &Turno3J1F, tTurnoJ1F, t2TurnoJ1F, REFRESCO, 'D', 'd');  
  
  Boton( J2.Izquierda, &TurnoJ2I, &Turno2J2I, &Turno3J2I, tTurnoJ2I, t2TurnoJ2I, REFRESCO, 'Z', 'z'); 
  Boton( J2.Derecha, &TurnoJ2D, &Turno2J2D, &Turno3J2D, tTurnoJ2D, t2TurnoJ2D, REFRESCO, 'Y', 'y');   
  Boton( J2.Acelerador, &TurnoJ2A, &Turno2J2A, &Turno3J2A, tTurnoJ2A, t2TurnoJ2A, REFRESCO, 'X', 'x'); 
  Boton( J2.Drift, &TurnoJ2Dr, &Turno2J2Dr, &Turno3J2Dr, tTurnoJ2Dr, t2TurnoJ2Dr, REFRESCO, 'V', 'v');     
  Boton( J2.Freno, &TurnoJ2F, &Turno2J2F, &Turno3J2F, tTurnoJ2F, t2TurnoJ2F, REFRESCO, 'W', 'w'); 
}

void Boton( int boton, int *turno1, int *turno2, int *turno3, 
nsigned long t1, unsigned long t2, int resfresco, char L, char l ){
  if(!digitalRead(boton) && !*turno1){
    *turno1 = 1;
    *turno2 = 0;
    *turno3 = 0;
    t1 = millis();
  }else if(!digitalRead(boton) && *turno1 && (millis() -t1)> resfresco){
    if(!digitalRead(boton) && !*turno3){
      Serial2.print(L);
     
      *turno2 = 1;
      *turno3 = 1;
      t2 = millis();
    }
  }
  else if(digitalRead(boton) && *turno2 && (millis()-t2)>resfresco){
    Serial2.print(l);
    
    *turno1 = 0;
    *turno2 = 0;
  }
}


//  Serial22.print("M");
//  Serial2.print(!digitalRead(J1.Izquierda));
//  Serial2.print(!digitalRead(J1.Derecha));
//  Serial2.print(!digitalRead(J1.Acelerador));
//  Serial2.print(!digitalRead(J1.Drift));
//  Serial2.print(!digitalRead(J1.Freno));
//  Serial2.print(!digitalRead(J2.Izquierda));
//  Serial2.print(!digitalRead(J2.Derecha));
//  Serial2.print(!digitalRead(J2.Acelerador));
//  Serial2.print(!digitalRead(J2.Drift));
//  Serial2.print(!digitalRead(J2.Freno));

//   Serial.print("M");
//  Serial.print(!digitalRead(J1.Izquierda));
//  Serial.print(!digitalRead(J1.Derecha));
//  Serial.print(!digitalRead(J1.Acelerador));
//  Serial.print(!digitalRead(J1.Drift));
//  Serial.print(!digitalRead(J1.Freno));
//  Serial.print(!digitalRead(J2.Izquierda));
//  Serial.print(!digitalRead(J2.Derecha));
//  Serial.print(!digitalRead(J2.Acelerador));
//  Serial.print(!digitalRead(J2.Drift));
//  Serial.println(!digitalRead(J2.Freno));
//  delay(15);
