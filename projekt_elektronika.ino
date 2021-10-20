
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <dht11.h> // Dołączenie potrzebnych bibliotek

#define SECRET_SSID "Xperia"    // Nazwa sieci 
#define SECRET_PASS "qlvl1028"  // Hasło do sieci

#define SECRET_CH_ID 803992     // Numer kanału na serwerze ThingSpeak
#define SECRET_WRITE_APIKEY "4V9FY8H93CGQDPN3"   // Kod do wgrywania na kanał ThingSpeak


dht11 DHT11;

#define DHT11PIN 16    //przypisanie pinów ESP8266
int sensor_D2 = 5;     
int wartosc_D2;        
int czujnik = A0;
int war = analogRead(czujnik); 

char ssid[] = SECRET_SSID;   
char pass[] = SECRET_PASS;   

WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;



void setup()
{
  pinMode(2, INPUT);
  pinMode(13, OUTPUT);
  Serial.begin(115200);                    //inicjalizacja monitora szeregowego
   
  Serial.println();
  WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client);  // Inicjalizacja do ThingSpeak
}

void loop()
{

if(WiFi.status() != WL_CONNECTED){
    Serial.print(" Próba połączenia z siecią: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\n Połączono.");
  }
// Przypisanie pomiarów do zmiennych
  int number1 = ((float)DHT11.temperature); 
  int number2 = ((float)DHT11.humidity);
  int number3 = wartosc_D2;
  int number4 = analogRead(czujnik);
  String myStatus = ""; // Przypisanie zmiennej do statusu

   // Przypisanie pól z serwera ThingSpeak do zmiennych w.w.
  ThingSpeak.setField(1, number1);
  ThingSpeak.setField(2, number2);
  ThingSpeak.setField(3, number3);
  ThingSpeak.setField(4, number4);
  

  if (war > 250)             // pętla sprawdzająca nasłonecznienie, punkt krytyczny ustalono 250 (słońce czy zachmurzenie)         
  {
    Serial.print("  Slonce");  
    Serial.println(war);
    delay(2000);
  }
  else
  {
    Serial.print("  Zachmurzenie");
    Serial.println(war);
    delay(2000);
  }
  Serial.println("");
  delay(200);              
 

wartosc_D2 = digitalRead(sensor_D2);     // odczyt z czujnika opadów, 1 to brak opadów, a 0 oznacza opady
  
 if (wartosc_D2==1)
 {
  Serial.print("  Brak opadów  ");
  Serial.print("\n");
  delay(2000);
 }
 else 
 {
  Serial.print("  Opady  ");
  Serial.print("\n");
  delay(2000);
  }
 // Ustalenie jaki status wysłać na serwer w zależności od odczytów na czujnikach
  if(number3 == 1 & number4>250){                  
    myStatus = String("Brak opadow oraz słonce"); 
  }
  else if(number3 == 1 & number4<250){
    myStatus = String("Brak opadow oraz zachmurzenie");
  }
   else if(number3 == 0 & number4<250){
    myStatus = String("Opady oraz zachmurzenie");
  }
  else if(number3 == 0 & number4>250){
    myStatus = String("Opady oraz slonce");
  }
  
  int chk = DHT11.read(DHT11PIN);         //sprawdzenie stanu sensora  w pętli switch

  
  switch (chk)
  {
    case DHTLIB_OK: 
    //Serial.print("OK"); 
    break;
    case DHTLIB_ERROR_CHECKSUM: 
    //Serial.println("Błąd sumy kontrolnej"); 
    break;
    case DHTLIB_ERROR_TIMEOUT: 
    //Serial.println("Koniec czasu oczekiwania - brak odpowiedzi"); 
    break;
    default: 
    //Serial.println("Nieznany błąd"); 
    break;
  }
  Serial.print("Wilgotnosc (%): ");              // pomiar wilgoci i temperatury 
  Serial.print((float)DHT11.humidity, 2);       // komenda serial.print uzyta w celu obserwowani wyników w monitorze portu szeregowego
  Serial.print("  ");
  Serial.print("Temperatura (C): ");           
  Serial.println((float)DHT11.temperature, 2);


 ThingSpeak.setStatus(myStatus);              // wysłanie statusu na serwer ThingSpeak
 
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey); // wysłanie wartości pomiarów na serwer ThingSpeak 
  if(x == 200){
    Serial.println("Poprawna aktualizacja pomiarów.");
  }
  else{
    Serial.println("Problem z aktualizacją. HTTP error code " + String(x));
  }
                                  
  delay(20000); //Opóźnienie między kolejnymi odczytami 20s wymagane odgórnie przez serwer
} 
