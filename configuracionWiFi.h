#include <ArduinoJson.h>
#include <Arduino.h>
#include <WiFi.h>
#include "time.h"
#include <Firebase_ESP_Client.h>
//Generacion de informacion de los tokens.
#include "addons/TokenHelper.h"
//Provee las funciones para subir los datos.
#include "addons/RTDBHelper.h"

// Credenciales wifi
#define WIFI_SSID "HUAWEI Y7p"
#define WIFI_PASSWORD "d3m355555"
//Sitio web de donde se saca la hora del GMT
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -18000;//Desface horario en segundos
const int   daylightOffset_sec = 3600;

// Llave API de BD
#define API_KEY "AIzaSyDKF5rpyd2-GvOxe8BQAfE0_04GodHXSZk"

// URL de BD
#define DATABASE_URL "https://tesis-706e5-default-rtdb.firebaseio.com/" 

//Objeto firebase
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

bool signupOK = false;
void configWiFi(){
 
  WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    //Serial.print(".");
    delay(300);
  }
  //Serial.println();
  //Serial.print("Connected with IP: ");
  //Serial.println(WiFi.localIP());
 // Serial.println();

  /* Asignar llave API */
  config.api_key = API_KEY;

  /* Asignar URL de BS(required) */
  config.database_url = DATABASE_URL;

  /* Ingreso */
  if (Firebase.signUp(&config, &auth, "", "")){
   // Serial.println("ok");
    signupOK = true;
  }
  else{
   // Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}
String enviarTiempo(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
   // Serial.println("Error al obtener tiempo ");
    return "ERROR";
  }
   char tiempo[40];
  strftime(tiempo,40, "%B-%d-%Y-%H:%M", &timeinfo); 
  
  return String(tiempo);

}
String enviarMes(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
   
    return "ERROR";
  }
   char tiempo[12];
  strftime(tiempo,12, "%B", &timeinfo); 
  
  return String(tiempo);

}
String enviarDia(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){

    return "ERROR";
  }
   char tiempo[10];
  strftime(tiempo,10, "%d", &timeinfo); 
  
  return String(tiempo);

}

String enviarHora(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){

    return "ERROR";
  }
   char tiempo[10];
  strftime(tiempo,10, "%H:%M", &timeinfo); 
  
  return String(tiempo);

}


void envioRTFB(float temperatura, float tds, bool nivelAgua, float ph, float orp ){
  if (Firebase.ready() && signupOK ){
    
    // Escribe las lecturas en la BD
    Firebase.RTDB.setFloat(&fbdo,"/Valores medidos/"+enviarTiempo()+"/Sensor Temperatura", temperatura);
    Firebase.RTDB.setFloat(&fbdo,"/Valores medidos/"+enviarTiempo()+"/Sensor TDS", tds);
    Firebase.RTDB.setBool(&fbdo, "/Valores medidos/"+enviarTiempo()+"/Sensor Nivel de agua", nivelAgua);
    Firebase.RTDB.setFloat(&fbdo, "/Valores medidos/"+enviarTiempo()+"/Sensor PH", ph);
    Firebase.RTDB.setFloat(&fbdo, "/Valores medidos/"+enviarTiempo()+"/Sensor ORP", orp);
    Firebase.RTDB.setString(&fbdo,"/Valores medidos/"+enviarTiempo()+"/Día", enviarDia());
    Firebase.RTDB.setString(&fbdo,"/Valores medidos/"+enviarTiempo()+"/Mes", enviarMes());
    Firebase.RTDB.setString(&fbdo, "/Valores medidos/"+enviarTiempo()+"/Hora", enviarHora());
   // Serial.println("enviado");
  }else{
   // Serial.println("error");

  }
}
void envioRTFBActuales(float temperatura, float tds, bool nivelAgua,float ph, float orp ){
  if (Firebase.ready() && signupOK ){
    
    // Escribe las lecturas en la BD
    Firebase.RTDB.setFloat(&fbdo,"/Valores actuales/Temperatura", temperatura);
    Firebase.RTDB.setFloat(&fbdo,"/Valores actuales/TDS", tds);
    Firebase.RTDB.setBool(&fbdo, "/Valores actuales/Nivel agua", nivelAgua);
    Firebase.RTDB.setFloat(&fbdo, "/Valores actuales/ph", ph);
    Firebase.RTDB.setFloat(&fbdo, "/Valores actuales/ORP", orp);
  //  Serial.println("enviado");
  }else{
   // Serial.println("error");

  }
}
void configTiempo(){
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

}
float lecturaRTFB_Temperatura(){
  if(Firebase.RTDB.getFloat(&fbdo, "/Valores de control/Temperatura")){
    if(fbdo.dataType()=="float"||fbdo.dataType()=="int"){
  float lecturaTemperatura= fbdo.floatData() ;
  return lecturaTemperatura;
  }else{
    return 0;
  }
  }else{
    return 0;
  }
}
float lecturaRTFB_ORP(){
  if(Firebase.RTDB.getFloat(&fbdo, "/Valores de control/ORP")){
    if(fbdo.dataType()=="float"){
  float lecturaORP= fbdo.floatData();
  return lecturaORP;
  }else{
    return 0;
  }
  }else{
    return 0;
  }
}
float lecturaRTFB_PH(){
  if(Firebase.RTDB.getFloat(&fbdo, "/Valores de control/pH")){
    if(fbdo.dataType()=="float"){
  float lecturaPH= fbdo.floatData();
  return lecturaPH;
  }else{
    return 0;
  }
  }else{
    return 0;
  }
  
}
bool lecturaRTFB_Control_NivelAgua(){
  if(Firebase.RTDB.getFloat(&fbdo, "/Valores de control/Llenado automatico agua")){
    if(fbdo.dataType()=="bool"){
  bool lecturaNivel= fbdo.boolData();
  return lecturaNivel;
  }else{
    return false;
  }
  }else{
    return false;
  }

}
bool lecturaRTFB_Control_Temperatura(){
  if(Firebase.RTDB.getFloat(&fbdo, "/Valores de control/Control automatico temperatura")){
    if(fbdo.dataType()=="bool"){
  bool lecturaCT= fbdo.boolData();
  return lecturaCT;
  }else{
    return false;
  }
  }else{
    return false;
  }

}
bool lecturaRTFB_Control_Ph(){
  if(Firebase.RTDB.getFloat(&fbdo, "/Valores de control/Control automatico pH")){
    if(fbdo.dataType()=="bool"){
  bool lecturaCPH= fbdo.boolData();
  return lecturaCPH;
  }else{
    return false;
  }
  }else{
    return false;
  }

}
bool lecturaRTFB_Control_ORP(){
  if(Firebase.RTDB.getFloat(&fbdo, "/Valores de control/Control automatico ORP")){
    if(fbdo.dataType()=="bool"){
  bool lecturaCORP= fbdo.boolData();
  return lecturaCORP;
  }else{
    return false;
  }
  }else{
    return false;
  }

}
bool lecturaRTFB_Control_Bomba(){
  if(Firebase.RTDB.getBool(&fbdo, "/Valores de control/Encendido de bomba")){
    if(fbdo.dataType()=="boolean"){
  //    Serial.println("exito");
  bool lecturaCBomba= fbdo.boolData();
  return lecturaCBomba;
  }else{
   // Serial.println("error lectura");
    
    return false;
  }
  }else{// Serial.println("No reconoce rama");
    return false;
  }

}

bool lecturaRTFB_Bolean(String rama, String subrama){
  if(Firebase.RTDB.getBool(&fbdo, "/"+rama+"/"+subrama)){
    if(fbdo.dataType()=="boolean"){
  //    Serial.println("exito");
      
  bool lecturaCBomba= fbdo.boolData();
  return lecturaCBomba;
  }else{
  //  Serial.println("error lectura");
    return false;
  }
  }else{ 
  //  Serial.println("No reconoce rama");
    return false;
  }

}

float lecturaRTFB_Float(String rama, String subrama){
  if(Firebase.RTDB.getFloat(&fbdo, "/"+rama+"/"+subrama)){
    if(fbdo.dataType()=="float" || fbdo.dataType()=="int"){
      
  float lecturaCBomba= fbdo.floatData();
  return lecturaCBomba;
  }else{

    return 0;
  }
  }else{ 
    return 0;
  }

}

