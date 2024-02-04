//Control ON OFF temperatura
void ControlTemperatura(int pinEntrada, bool autorizacion, float valorSeteo,float valorActual){//Pide el pin de salida digital que enviara  la señal de control,autorizacion de control, valor deseado y valor actual
  if(autorizacion){
    if( valorActual<(valorSeteo-valorSeteo*0.10)){//Control OnOff con una zona muerta de 10%
      digitalWrite(pinEntrada, LOW);
      //Serial.println("No se llega a la temperatura");
    }else if(valorActual>=(valorSeteo+valorSeteo*0.10)){
      digitalWrite(pinEntrada, HIGH);

      
    }

  }else{
    digitalWrite(pinEntrada, HIGH);

  
}
}
//Nivel de agua
void ControlNivelAgua(int pinEntrada, bool autorizacion, bool nivelNormal){//Pide el pin de salida digital que enviara  la señal de control,autorizacion de control y el valor actual del nivel de agua
  if(autorizacion){
    if(nivelNormal){
      digitalWrite(pinEntrada, HIGH);
      // Serial.println("Piscina llena");
    }else{
      digitalWrite(pinEntrada, LOW);
      // Serial.println("Piscina llenandose");
    }

  }else{ 
    digitalWrite(pinEntrada, HIGH);
   // Serial.println("Sin autorizacion");
  
}
}
//Encendido de la bomba de agua principal
void ControlBombaAgua(int pinEntrada, bool encendido){//Encendido y apagadod de la bomba según el usuario
 
    if(encendido){
      digitalWrite(pinEntrada, LOW);
      // Serial.println("Bomba Activa");
    }else{
      digitalWrite(pinEntrada, HIGH);
      // Serial.println("Bomba desactivada");
    }

}





