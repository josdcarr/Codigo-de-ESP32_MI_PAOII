//-----------------------------Control QUIMICO----------------------------------------------------
//-------------------ORP---------------------------------------------------------------------------

//------------------------------------------Control ON OFF


//EN2 Y IN2  para subir pH y dosificar cloro
void ControlOnOffORP(int ENA2, int IN2,  float valorSeteo,float valorActual){
  if( valorActual<(valorSeteo-valorSeteo*0.01)){
    analogWrite(ENA2, 255);
    digitalWrite(IN2, HIGH);
    delay(1000);

  }else{
    analogWrite(ENA2, 0);
    digitalWrite(IN2, LOW);
    delay(1000);

  }
}
//CONTROL PROPORCIONAL

void ControlProporcionalORP(int ENA2, int IN2,  float valorSeteo,float valorActual){

  if( valorActual<valorSeteo){//Subir ORP
    
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


}
//----------------------------------------------------CONTROL PID

long T_previo_ORP = 0;
float error_Previo_ORP = 0;
float eintegral_ORP = 0;
void ControlORPPID(int ENA2, int IN2,float setPoint,float valorReal){//Pines de control ENA e IN, valor deseado y valor actual
  float error_Previo_ORP = 0;
  float kp = 0.0018; //Segun el sistema a controlar
  float kd = 0.0144;
  float ki = 0.00005625;
  // diferencial de tiempo
  long T_actual = micros();
  float T_delta = ((float) (T_actual - T_previo_ORP))/( 1.0e6 );
  long T_previo_ORP = T_actual;
  // error
  float error =valorReal-setPoint;
  // Derivativo
  float dedt = (error-error_Previo_ORP)/(T_delta);
  // Integral
  float eintegral_ORP = eintegral_ORP + error*T_delta;

  // Ecuacion ziegler nichols
  float u = kp*error + kd*dedt + ki*eintegral_ORP;
  // Señal PWM
  float pwr = fabs(u);
  if( pwr > 255 ){
    pwr = 255;
  }
  if( pwr < 220 ){
    pwr = 220;
  }
  if(u<0){//SUBIR ORP
   analogWrite(ENA2, pwr);
   digitalWrite(IN2, HIGH);
    delay(1000);
  }else{//BAJAR ORP
   analogWrite(ENA2, 0);
   digitalWrite(IN2, LOW);
   delay(1000);
  }
    error_Previo_ORP = error;
}

// CONTROL GENERAL ORP

void ControlORPGeneral(int ENA2, int IN2,float setPoint,float valorReal, int tipoControl,bool autorizacion,bool autorizacion_2){//tipo de control: ON OFF(1), PROPORCIONAL(2) O PID (3),

    if(!autorizacion_2){
        if(autorizacion){
          if( tipoControl==0){
            ControlOnOffORP(ENA2,IN2,setPoint,valorReal);
            delay(1000);
          }else if(tipoControl==1){  
            ControlProporcionalORP(ENA2,IN2,setPoint,valorReal);
            delay(1000);
        }else if(tipoControl==2){
            ControlORPPID(ENA2,IN2,setPoint,valorReal);
            delay(1000);
          }
    }else if(autorizacion==false){
              analogWrite(ENA2, 0);
              digitalWrite(IN2, LOW);
              Serial.println("Sin autorizacion");
        }

    }
}
