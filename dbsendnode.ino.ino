#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <DHT.h>

#define HOST "10.0.0.55"

#define ssid "MySSID"  // replace MySSID with your WiFi network name
#define pass "MyPassowrd"  // replace MyPassword with your WiFi password
#define ONE_WIRE_BUS 2    
#define DHTPIN 14
#define DHTTYPE DHT11

WiFiClient client;

// ESP8266WebServer server(80);

DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27, 20, 4);

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

String sendval, sendval2, sendval3, sendval4, postData;

void setup(){
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  WiFi.begin(ssid, pass);
  Serial.print("\n\r \n\rWorking to connect");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); 
     Serial.print(".");
    }
  Serial.println("");
  Serial.println("ESP8266 Webpage");
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  // server.on("/", handleRoot); 
  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{

  HTTPClient http; 


  int lightValue = analogRead(A0);
  server.handleClient();
  sensors.requestTemperatures();  // Send the command to get temperatures


  float val = sensors.getTempFByIndex(0);
  float val2 = sensors.getTempFByIndex(1);
  float val3 = dht.readHumidity();
  float val4 = dht.readTemperature(true);

  sendval = String(val);  
  sendval2 = String(val2);  
  sendval3 = String(val3);  
  sendval4 = String(val4); 

  postData = "sendval=" + sendval + "&sendval2=" + sendval2 + "&sendval3=" + sendval3 + "&sendval4=" + sendval4;


  http.begin(client, "http://10.0.0.55/dbwrite.php");              
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpCode = http.POST(postData);   // Send POST request to php file and store server response code in variable named httpCode
  Serial.println("Values are, sendval = " + sendval + " and sendval2 = " + sendval2 );
  delay(5000);

if (httpCode == 200) { 
  Serial.println("Values uploaded successfully."); 
    Serial.println(httpCode); 
      String webpage = http.getString();    // Get html webpage output and store it in a string
        Serial.println(webpage + "\n"); 
}

// if failed to connect then return and restart

else { 
    Serial.println(httpCode); 
        Serial.println("Failed to upload values. \n"); 
            http.end(); 
  return; 
}

  lcd.begin(16, 2);
  lcd.print("Left") + lcd.print(" ") + lcd.print(val) + lcd.print("\337F");
  lcd.setCursor(0, 1);
  lcd.print("Right") + lcd.print(" ") + lcd.print(val2) + lcd.print("\337F");


  if (lightValue <= 500) {
    lcd.noBacklight();
  } else {
    lcd.backlight();
  }
}
