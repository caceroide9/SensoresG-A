#include <WiFi.h>
#include <MySQL_Cursor.h>
#include <MySQL_Connection.h>

#define RXD2 16
#define TXD2 17

#define LED 2

///////////////////// WiFi ///////////////////////

char* ssid = "Smart factory Huawei";
char* password = "Lalenca34";

IPAddress local_IP(192, 168, 8, 137);
IPAddress gateway(192, 168, 8, 1);
IPAddress subnet(255, 255, 255, 0);

const int wifi_connection_timeout = 6;
int wifi_connection_timeout_counter = 0;
const int wifi_connection_try_interval = 5000;

///////////////////// MySQL Server ///////////////////////

IPAddress server_addr(xx,xxx,xx,x);  // IP of the MySQL *server* here
char* db_user = "user";              // MySQL user login username
char* db_password = "pass";        // MySQL user login password
char* db_database = "db";             // Base de datos predeterminada  
const int db_port = 3306;

////////////////// Variables ////////////////

const int local_char_variables_length = 9;

char acc_x_local[local_char_variables_length];
char acc_y_local[local_char_variables_length];
char acc_z_local[local_char_variables_length];

char gyro_x_local[local_char_variables_length];
char gyro_y_local[local_char_variables_length];
char gyro_z_local[local_char_variables_length];

char temp_local[local_char_variables_length];

bool received_info_six_axis = false;

String line_to_decode;

int i1 = 0, i2 = 0, i3 = 0, i4 = 0, i5 = 0, i6 = 0, i7 = 0;
String aux1, aux2, aux3, aux4, aux5, aux6, aux7;

void initWiFi(){
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(" Intentando conexion a WiFi " + String(wifi_connection_timeout_counter + 1));
    Serial.print('.');
    if (wifi_connection_timeout_counter == wifi_connection_timeout){
      wifi_connection_timeout_counter = 0;
      Serial.println(" El dispositivo se reiniciara en 4 segundos....");
      delay(4000);
      ESP.restart();
    }
    WiFi.disconnect();
    WiFi.reconnect();
    wifi_connection_timeout_counter++;
    delay(wifi_connection_try_interval);
  }
  wifi_connection_timeout_counter = 0;
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Serial.begin(115200);
  Serial.println("Serial Txd is on pin: "+String(TX));
  Serial.println("Serial Rxd is on pin: "+String(RX));
  initWiFi();
  pinMode(LED,OUTPUT);
  Serial.println(" WiFi Conectado, Verificando Conexión a internet ..................");
  String net_ipaddress = WiFi.localIP().toString();
}

void MySQL_petitions(String _petition){

  //temps_date = rtc.getTime("%F %H:%M:%S");  //2022-12-23 14:22:59
  
  WiFiClient client;
  MySQL_Connection conn(&client);
  MySQL_Cursor* cursor;
  
  if (conn.connect(server_addr, db_port, db_user, db_password) == false){
    Serial.println("Connection to server failed....");
    return;
  }
  else{
    Serial.println("Server Connnection is up....");
    // create MySQL cursor object
    cursor = new MySQL_Cursor(&conn);
    //MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
    
    //////////////////// Querys /////////////////////
    /////////////////////// Envio de temperaturas a BDD //////////////////////
    
    if (_petition == "six_axis"){
      
      char INSERT_SQL[] = "INSERT INTO %s.%s (ufro_acc_x, ufro_acc_y, ufro_acc_z, ufro_gyro_x, ufro_gyro_y, ufro_gyro_z, ufro_temp) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s')";
      char query[256];
      //Insertion
      if (conn.connected()){
        sprintf(query, INSERT_SQL, db_database, "six_axis", acc_x_local, acc_y_local, acc_z_local, gyro_x_local, gyro_y_local, gyro_z_local, temp_local);
        Serial.println(query);
        cursor->execute(query);
        delete cursor;
      }
    }
  }
}

void decode_function(String _string){
  i1 = _string.indexOf('&');
  i2 = _string.indexOf('&', i1 + 1);
  i3 = _string.indexOf('&', i2 + 1);
  i4 = _string.indexOf('&', i3 + 1);
  i5 = _string.indexOf('&', i4 + 1);
  i6 = _string.indexOf('&', i5 + 1);
  //i7 = _string.indexOf('&', i6 + 1);
  
  aux1 = _string.substring(0, i1);                                     // Guarda acc_x
  aux2 = _string.substring(i1 + 1, i2);                                // Guarda acc_y
  aux3 = _string.substring(i2 + 1, i3);                                // Guarda acc_z
  aux4 = _string.substring(i3 + 1, i4);                                // Guarda gyro_x
  aux5 = _string.substring(i4 + 1, i5);                                // Guarda gyro_y
  aux6 = _string.substring(i5 + 1, i6);                                // Guarda gyro_z
  aux7 = _string.substring(i6 + 1, _string.length());                  // Guarda temp
  
  aux1.toCharArray(acc_x_local, local_char_variables_length);
  aux2.toCharArray(acc_y_local, local_char_variables_length);
  aux3.toCharArray(acc_z_local, local_char_variables_length);
  aux4.toCharArray(gyro_x_local, local_char_variables_length);
  aux5.toCharArray(gyro_y_local, local_char_variables_length);
  aux6.toCharArray(gyro_z_local, local_char_variables_length);
  aux7.toCharArray(temp_local, local_char_variables_length);
}

void loop() {
  while (Serial2.available()) {
    line_to_decode = Serial2.readStringUntil('\n');
    if (line_to_decode.length() >= 41){
      
      Serial.println(line_to_decode);
      decode_function(line_to_decode);

      digitalWrite(LED,HIGH);  
      MySQL_petitions("six_axis");
      digitalWrite(LED,LOW);
      
      line_to_decode = "";
    }
  }
  
  delay(100);
}
