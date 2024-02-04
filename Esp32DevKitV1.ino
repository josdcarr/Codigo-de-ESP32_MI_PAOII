#include <ArduinoJson.h>
#include "configuracionWiFi.h"
#include "control.h"
#include "controlPH.h"
#include "controlORP.h"
//Variable de nivel de agua, para almacenar el valor recibido de JSON(1 o 0) a un booleano (true o false)
bool nivelAguaBool;

// Pines de conexion de los elementos de control, ademas del pin de salida para avisar al arduino uno que muestre los datos en la pantalla LCD
const int Calentador=33;
const int Solenoide=32;
const int LCD=18;
const int B_agua=26;
//----------------Conexion al controlador de motor
const int ENA=23;
const int IN1=22;
const int ENB=19;
const int IN3=21;

void setup() {
  digitalWrite(LCD, LOW);
  Serial.begin(9600);
  configWiFi();
  configTiempo();
  pinMode(Calentador, OUTPUT);
  pinMode(Solenoide, OUTPUT);
  pinMode(LCD, OUTPUT);
  pinMode(B_agua, OUTPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN3, OUTPUT);

  digitalWrite(Calentador, HIGH);
  digitalWrite(Solenoide, HIGH);
  digitalWrite(B_agua, HIGH);
}

void loop() {
  //Funciones de comunicacion serial. Envio de datos al registro y actualizacion de datos a visualizar en la aplicacion movil
  
  
  bool autorizacion= lecturaRTFB_Bolean("Valores de control","Encendido de bomba");
  ControlBombaAgua(B_agua,autorizacion);
  if (Serial.available() > 0) {
    // Leer string
    String jsonString = Serial.readStringUntil('\n');
    // Almacena el JSON string
    StaticJsonDocument<400> doc;
    DeserializationError error = deserializeJson(doc, jsonString);
    // Excepcion en caso de errores
    if (error) {
      //Serial.print("Error de lectura en comunicacion serial: ");
      //Serial.println(error.c_str());
    } else {
      //Serial.println("Establecimiento de comunicacion serial");
      
      //Garantia de que la bomba de agua este activa para el envio de datos
      if( lecturaRTFB_Bolean("Valores de control","Encendido de bomba")){
        //Serial.println("Bomba de agua activa");
        // Acceso a los valores del JSON
        float temperatura = doc["Temperatura"];
        float tds= doc["TDS"];
        bool nivelAgua= doc["Nivel de agua"];
        if(nivelAgua==1){
          nivelAguaBool=true;
        }else{
          nivelAguaBool=false;
        }
        float ph= doc["pH"];
        float orp= doc["ORP"];
        if(temperatura!=0  ){
          //Serial.print("EXITO ENVIO NO NULLS");
          envioRTFB(temperatura,tds,nivelAguaBool,ph,orp);
          envioRTFBActuales(temperatura,tds,nivelAguaBool,ph,orp);
          digitalWrite(LCD, HIGH);
          

          //---------------------------Funciones de control temp, ph, orp, llenado
          //Control temperatura
          bool autorizacion1= lecturaRTFB_Bolean("Valores de control","Control automatico temperatura");
          float valorSeteo= lecturaRTFB_Float("Valores de control","Temperatura");
          ControlTemperatura(Calentador, autorizacion1,  valorSeteo, temperatura);
          //Control llenado automatico
          bool autorizacion2= lecturaRTFB_Bolean("Valores de control","Llenado automatico agua");
          ControlNivelAgua(Solenoide, autorizacion2, nivelAguaBool);
          //Control pH
          float valorSeteoPH=lecturaRTFB_Float("Valores de control","pH");
          int tipoControl=lecturaRTFB_Float("Valores de control","Control");
          bool autorizacionPH= lecturaRTFB_Bolean("Valores de control","Control automatico pH");
          bool autorizacionORP= lecturaRTFB_Bolean("Valores de control","Control automatico ORP");
          ControlpHGeneral(ENA,IN1, ENB, IN3,  valorSeteoPH, ph, tipoControl, autorizacionPH, autorizacionORP);
          //Control ORP
          float valorSeteoORP=lecturaRTFB_Float("Valores de control","ORP");
          ControlORPGeneral(ENB,IN3, valorSeteoORP, orp, tipoControl, autorizacionORP, autorizacionPH);
          Serial.print(valorSeteoPH);
          Serial.print(tipoControl);
          Serial.print(autorizacionORP);
          Serial.print(autorizacionPH);
          Serial.print(valorSeteoORP);
      }
      }else{
        Serial.println("Bomba desactivada ");
        digitalWrite(LCD, LOW);
        analogWrite(ENA, 0);
        digitalWrite(IN1, LOW);
        analogWrite(ENB, 0);
        digitalWrite(IN3, LOW);
        delay(1000);
      }
    }
  }
}
