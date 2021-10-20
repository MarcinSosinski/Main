
#include <ESP8266WiFi.h> // Załączenie potrzebnych bibliotek 
#include <dht11.h>

dht11 DHT11; 

#define DHT11PIN 16    //przypisanie pinów ESP8266
int sensor_D2 = 5;     
int wartosc_D2;        
int czujnik = A0;
int war = analogRead(czujnik);

WiFiClient  client;

#define ssid  "Xperia"  // podanie nazwy sieci oraz hasła
#define password  "qlvl1028"

// Port web server ustawiono na 80
WiFiServer server(80);

// Zmienna do przechowywania HTML request
String header;

void setup() {
  Serial.begin(115200);// na jakiej czestotliwosci dzialaja czujniki oraz komunikacja szeregowa
  Serial.print("Connecting to "); // Połączenie ESP z siecią wifi
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("."); 
  }
  // Wyświetl adres IP i uruchom serwer
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}



void loop()
{
  
   

  
  WiFiClient client = server.available();   // Oczekiwanie na clienta, ESP cały czas oczekuje
// 
  if (client) {                            
    Serial.println("New Client.");          // Kiedy otrzymamy od clienta prośbe o połączenie nastąpi zapis danych,
    String currentLine = "";                // pętla będzie aktywna tak długo jak client bedzie połączony
    while (client.connected()) {            
      if (client.available()) {             // jeśli są dane do odczytu od clienta program je czyta i wyświetla informacje na porcie szeregowym
        char c = client.read();             
        Serial.write(c);                    
        header += c;                        // Zapis nagłówka o dodanie komórki c
        if (c == '\n') {                    
          
          if (currentLine.length() == 0) {  // Wysłanie odpowiedzi, koniec HTTP request
            
            
            client.println("HTTP/1.1 200 OK"); 
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Wykonanie strony
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // Styl strony CSS
            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial;}");             // czcionka, styl, rozmiar itd
            client.println("table { border-collapse: collapse; width:35%; margin-left:auto; margin-right:auto; }"); // wykonanie tabelki
            client.println("th { padding: 12px; background-color: #0043af; color: white; }");
            client.println("tr { border: 1px solid #ddd; padding: 12px; }");
            client.println("tr:hover { background-color: #bcbcbc; }");
            client.println("td { border: none; padding: 12px; }");
            client.println(".sensor { color:white; font-weight: bold; background-color: #bcbcbc; padding: 1px; }");
            
            // Nagłówek strony oraz jej treść
            client.println("</style></head><body><h1>ESP Stacja pogodowa</h1>");
            client.println("<table><tr><th>Pomiary</th><th>Wartosc</th></tr>");
            client.println("<tr><td>Temp. Celcjusza</td><td><span class=\"sensor\">");
            client.println((float)DHT11.temperature,2);
            client.println(" *C</span></td></tr>");  
            client.println("<tr><td>Wilgotnosc</td><td><span class=\"sensor\">");
            client.println((float)DHT11.humidity,2);
            client.println(" %</span></td></tr>");       
            client.println("<tr><td>Opady = 0 </td><td><span class=\"sensor\">");
            client.println(wartosc_D2);
            client.println(" </span></td></tr>");
            client.println("<tr><td>Naslonecznienie</td><td><span class=\"sensor\">");
            client.println(analogRead(czujnik));
            client.println(" </span></td></tr>"); 
            client.println("</body></html>");
            
            // Koniec odpowiedzi HTTP oraz strony internetowej
            client.println();
            // Break z pętli while
            break;
          } else { // czyczenie linii jeśli się pojawi cos nowego
            currentLine = "";
          }
        } else if (c != '\r') {  // zwrot wartości true jesli komórki się różnią
          currentLine += c;       // różnica jeśli wystąpi zostanie dodana do linii
        }
      }
    }
    // Czyszczenie nagłówka
    header = "";
    // Zamknięcie połączenia
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }

 
  
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

    wartosc_D2 = digitalRead(sensor_D2);  // odczyt z czujnika opadów, 1 to brak opadów, a 0 oznacza opady
  
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
}
