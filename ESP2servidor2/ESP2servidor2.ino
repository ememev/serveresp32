#include <WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h> //Libreria para importar archivos css, html y js externos.
//--Agregando comentario para calar el git
//--Agregando ccomentario para ver el git diff -staged
//--creando una nueva rama para no afectar al main.
//------------------Servidor Web en puerto 80---------------------

//WiFiServer server(80);
//De otra forma 
AsyncWebServer server(80);

//---------------------Credenciales de WiFi-----------------------

const char* ssid     = "-----";
const char* password = "-----";

//---------------------VARIABLES GLOBALES-------------------------
int contconexion = 0;

String header; // Variable para guardar el HTTP request

String Estado_Fotogoniometro = Apagado;

const int salida = 2;

//----Función Processor para regresar valores a página Web-------------------------

String processor(const String& var) {
  if(var == "ESTADO_RELE")
    {
     if(digitalRead(Rele_Pin)==1)
     {
      Estado_Fotogoniometro= "Encendido";
     }
     else
     {
      Estado_Fotogoniometro = "Apagado";
     }
    return Estado_Fotogoniometro;
    }
  
}

//---------------------------SETUP--------------------------------
void setup() {
  Serial.begin(115200);
  Serial.println("");
  
  pinMode(salida, OUTPUT); 
  digitalWrite(salida, LOW);

  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Conexión WIFI
  WiFi.begin(ssid, password);
  //Cuenta hasta 50 si no se puede conectar lo cancela
  while (WiFi.status() != WL_CONNECTED and contconexion <10) { 
    ++contconexion;
    delay(500);
    Serial.print("Conectandose a internet...");
  }
  
  if (contconexion <10) {
      //para usar con ip fija
      //IPAddress ip(192,168,1,180); 
      //IPAddress gateway(192,168,1,1); 
      //IPAddress subnet(255,255,255,0); 
      //WiFi.config(ip, gateway, subnet); 
      
      Serial.println("");
      Serial.println("WiFi conectado");
      Serial.println(WiFi.localIP());
      
       //A partir de aqui vamos a resolver las solicitudes del cliente

  //---------------------Cuando ingresen a nuestra página------------------------
  //Primera opcion
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  //Segunda opcion
  //server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  
  //--------Cuando nuestro archivo Html solicite el archivo Css------------------
  server.on("/estilos.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/estilos.css", "text/css");
  });

  //-----------------Cuando pulsemos el botón de Iniciar-------------------------
  server.on("/RELE=ON", HTTP_GET, [](AsyncWebServerRequest *request){
    //Aqui enviaremos el codigo de la rutina al arduino
        
    request->send(SPIFFS, "/index.html", String(), false, processor);// Funcion que envia el estado del fotogoniometro
  });
  
  //----------------Cuando pulsemos el botón de apagar---------------------------
  server.on("/RELE=OFF", HTTP_GET, [](AsyncWebServerRequest *request){  
    //Aqui enviaremos el M112 al arduino
    
    request->send(SPIFFS, "/index.html", String(), false, processor); // Funcion que envia el estado del fotogoniometro
  });

  //-----------------Cuando mandamos un codigo libre /SetText---------------------
  server.on("/SetText", HTTP_POST, [](AsyncWebServerRequest *request){    
    String codigoLibre = request->arg("codigo");
    //Aqui enviamos el codigo libre al arduino

    request->redirect("/");
  });
    
  //Iniciamos servidor
      server.begin(); 
     
  }
//Si despues de los 10 intentos no se conecta al servidor.
  else { 
      Serial.println("");
      Serial.println("Error de conexion");
  }
}
