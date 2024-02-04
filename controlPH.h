//-----------------------------Control QUIMICO----------------------------------------------------
//-------------------PH/ORP---------------------------------------------------------------------------

//------------------------------------------Control ON OFF

//EN1 y IN1 para bajar pH
//EN2 Y IN2  para subir pH y dosificar cloro

void ControlOnOffPH(int ENA1, int IN1,int ENA2, int IN2,  float valorSeteo,float valorActual){
  if( valorActual<(valorSeteo-valorSeteo*0.01)){
    analogWrite(ENA2, 255);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA1, 0);
    digitalWrite(IN1, LOW);
    delay(1000);
  }else if(valorActual> (valorSeteo+valorSeteo*0.01)){
    analogWrite(ENA2, 0);
    digitalWrite(IN2, LOW);
    analogWrite(ENA1, 255);
    digitalWrite(IN1, HIGH);
    delay(1000);
  }else{
    analogWrite(ENA2, 0);
    digitalWrite(IN2, LOW);
    analogWrite(ENA1, 0);
    digitalWrite(IN1, LOW);
    delay(1000);
  }
}
//CONTROL PROPORCIONAL
void ControlProporcionalPH(int ENA1, int IN1,int ENA2, int IN2,  float valorSeteo,float valorActual){
  if( valorActual<valorSeteo){//Subir pH
    analogWrite(ENA1, 0);
    digitalWrite(IN1, LOW);
    delay(500);
    float porcentajeSeparacion=((1-valorActual/valorSeteo)*100)/10;
    if(porcentajeSeparacion<1 && porcentajeSeparacion>0 ){//dentro de zona proporcional
      int senalRedondeada=  round(255*porcentajeSeparacion);

      if(senalRedondeada>220){
        analogWrite(ENA2, senalRedondeada);
        digitalWrite(IN2, HIGH);
        delay(1000);
      }else{
        analogWrite(ENA2, 220);
        digitalWrite(IN2, HIGH);
        delay(1000);
      }
    }else if(valorActual>=valorSeteo){//Se llego al valor deseado
      analogWrite(ENA2, 0);
      digitalWrite(IN2, LOW);
      delay(1000);
    }else {//Por fuera de la zona proporcional
      analogWrite(ENA2, 255);
      digitalWrite(IN2, HIGH);
      delay(1000);
    }
  }
  if( valorActual>valorSeteo){//Bajar pH
    analogWrite(ENA2, 0);
    digitalWrite(IN2, LOW);
    float porcentajeSeparacion=((valorActual/valorSeteo-1)*100)/10;
    if(porcentajeSeparacion<1 ){//dentro de zona proporcional
      int senalRedondeada=  round(255*porcentajeSeparacion);
      if(senalRedondeada>220){
        analogWrite(ENA1, senalRedondeada);
        digitalWrite(IN1, HIGH);
        delay(1000);
      }else{
        analogWrite(ENA1, 220);
        digitalWrite(IN1, HIGH);
        delay(1000);
      }
    }else if(valorActual<=valorSeteo){//Se llego al valor deseado
      analogWrite(ENA1, 0);
      digitalWrite(IN1, LOW);
      delay(1000);
    }else {//Por fuera de la zona proporcional
      analogWrite(ENA1, 255);
      digitalWrite(IN1, HIGH);
      delay(1000);
    }
  }
}
//----------------------------------------------------CONTROL PID

long T_previo_pH = 0;
float error_Previo_pH = 0;
float eintegral_pH = 0;

void ControlpHPID(int ENA1, int IN1,int ENA2, int IN2,float setPoint,float valorReal){//Pines de control ENA e IN, valor deseado y valor actual

  float kp = 0.1116; //Segun el sistema a controlar
  float kd = 0.837;
  float ki = 0.00372;
  // diferencial de tiempo
  long T_actual = micros();
  float T_delta = ((float) (T_actual - T_previo_pH))/( 1.0e6 );
   T_previo_pH = T_actual;
  // error
  float error =valorReal-setPoint;
  // Derivativo
  float dedt = (error-error_Previo_pH)/(T_delta);
  // Integral
   eintegral_pH = eintegral_pH + error*T_delta;

  // Ecuacion ziegler nichols
  float u = kp*error + kd*dedt + ki*eintegral_pH;
  // Señal PWM
  float pwr = fabs(u);
  if( pwr > 255 ){
    pwr = 255;
  }
  if( pwr < 220 ){
    pwr = 220;
  }
  if(u<0){//SUBIR PH
   analogWrite(ENA2, pwr);
   digitalWrite(IN2, HIGH);
   analogWrite(ENA1, 0);
   digitalWrite(IN1, LOW);
   delay(1000);
  }else{//BAJAR PH
   analogWrite(ENA2, 0);
   digitalWrite(IN2, LOW);
   analogWrite(ENA1, pwr);
   digitalWrite(IN1, HIGH);
   delay(1000);
  }
    error_Previo_pH = error;
}

// CONTROL GENERAL PH
void ControlpHGeneral(int ENA1, int IN1,int ENA2, int IN2,float setPoint,float valorReal, int tipoControl,bool autorizacion,bool autorizacion_2){//tipo de control: ON OFF(0), PROPORCIONAL(1) O PID (2),

    if(!autorizacion_2){

        if(autorizacion){
          if( tipoControl==0){
            ControlOnOffPH(ENA1,IN1,ENA2,IN2,setPoint,valorReal);
          }else if(tipoControl==1){
            ControlProporcionalPH(ENA1,IN1,ENA2,IN2,setPoint,valorReal);
          }else if(tipoControl==2){
          ControlpHPID(ENA1,IN1,ENA2,IN2,setPoint,valorReal);
          }
        }else if(autorizacion==false){
              analogWrite(ENA1, 0);
              digitalWrite(IN1, LOW);
                Serial.println("Sin autorizacion");
        }
    }
    }
