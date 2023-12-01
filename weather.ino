//Written  By Frenoy Osburn
// Modified by Emmanuel Odunlade
// Modified by Jayant Solanki 12/01/2023, Replaced client with HTTPClient for parsing json
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <stdio.h>
#include <ArduinoJson.h>
#include <U8g2lib.h>
#define SUN  0
#define SUN_CLOUD  1
#define CLOUD 2
#define RAIN 3
#define THUNDER 4
U8G2_SSD1306_64X48_ER_2_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display
//initiate the WifiClient
WiFiClient client;
const char* ssid = "Wifissid";                          
const char* password = "wifipass@717";                 
unsigned long lastCallTime = 0;               // last time you called the updateWeather function, in milliseconds
const unsigned long postingInterval = 30L * 1000L;  // delay between updates, in milliseconds
String APIKEY = "yourapikey";
String NameOfCity = "Sunnyvale,US"; //e.g Lagos, NG if your city is Lagos in Nigeria. 
String cityId = "5400075" ;
const char *weatherHost = "http://api.openweathermap.org";
const char *weatherLang = "&lang=en";
int lineht = 0;
void setup()
{  
  u8g2.begin();
  
  u8g2.firstPage();
  do {
    // u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.setFont(u8g2_font_6x13_tr);
    lineht = u8g2.getMaxCharHeight();
    // u8g2.drawStr(0,20,"Online");
    // u8g2.drawStr(28,40,"Weather");
    // u8g2.drawStr(56,60,"Display");
    u8g2.drawStr(0,9,"Online");
    u8g2.drawStr(0,9 + lineht,"Weather");
    u8g2.drawStr(0,9 + lineht*2,"Display");
  } while ( u8g2.nextPage() );
  
  u8g2.enableUTF8Print();   //for the degree symbol
  
  Serial.begin(115200);
  Serial.println("\n\nOnline Weather Display\n");
  Serial.println("Connecting to network");
  WiFi.begin(ssid, password);
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(200);    
    if (++counter > 100) 
      ESP.restart();
    Serial.print( "." );
  }
  Serial.println("\nWiFi connected");
  printWifiStatus();
}
void loop() 
{    
  if (millis() - lastCallTime > postingInterval) 
  {
    updateWeather();
  }
}
void updateWeather()
{     

  String url = String(weatherHost) + "/data/2.5/weather?q=" + NameOfCity + "&units=metric&appid=" + APIKEY + weatherLang;
  String line;

  HTTPClient http;  // Declare an object of class HTTPClient
  http.begin(client, url);  // Specify request destination
  int httpCode = http.GET(); //Send the request
  if (httpCode > 0) // Check the returning code
  {
    line = http.getString();   // Get the request response payload
    // Allocate the JSON document
    // Use arduinojson.org/v6/assistant to compute the capacity.
    const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 2*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + 2*JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(13) + 270;
    DynamicJsonDocument doc(capacity);
    
    // Parse JSON object
    DeserializationError error = deserializeJson(doc, line);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    }
    Serial.println(line);
        
    int weatherId = doc["weather"][0]["id"].as<int>();
    float weatherTemperature = doc["main"]["temp"].as<float>();
    int weatherHumidity = doc["main"]["humidity"].as<int>();
    
    //Disconnect
    client.stop();
    
    Serial.println(F("Response:"));
    Serial.print("Weather: ");
    Serial.println(weatherId);
    Serial.print("Temperature: ");
    Serial.println(weatherTemperature);
    Serial.print("Humidity: ");
    Serial.println(weatherHumidity);
    Serial.println();
    
    char scrollText[15];
    sprintf(scrollText, "Humidity:%3d%%", weatherHumidity);
    if(weatherId == 800)    //clear
    {
      draw(scrollText, SUN, weatherTemperature);
    }
    else
    {
      switch(weatherId/100)
      {
        case 2:     //Thunderstorm
            draw(scrollText, THUNDER, weatherTemperature);
            break;
    
        case 3:     //Drizzle
        case 5:     //Rain
            draw(scrollText, RAIN, weatherTemperature);
            break;
    
        case 7:     //Sun with clouds
            draw(scrollText, SUN_CLOUD, weatherTemperature);
            break;
        case 8:     //clouds
            draw(scrollText, CLOUD, weatherTemperature);
            break;
        
        default:    //Sun with clouds           
            draw(scrollText, SUN_CLOUD, weatherTemperature);
            break;
      }    
    }
  }
  else
  {
    Serial.println("HTTP response code is no good");
    http.end();
    return;
  }
  http.end();   //Close connection
  // Serial.print("Response :"); Serial.println(line); // debug info



  // note the time that this function was called
   lastCallTime = millis();
}
void printWifiStatus() 
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
void drawWeatherSymbol(u8g2_uint_t x, u8g2_uint_t y, uint8_t symbol)
{
  // fonts used:
  // u8g2_font_open_iconic_embedded_6x_t
  // u8g2_font_open_iconic_weather_6x_t
  // encoding values, see: https://github.com/olikraus/u8g2/wiki/fntgrpiconic
  
  switch(symbol)
  {
    case SUN:
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);//32 pixel height
      u8g2.drawGlyph(x, y, 69);  
      break;
    case SUN_CLOUD:
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(x, y, 65); 
      break;
    case CLOUD:
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(x, y, 64); 
      break;
    case RAIN:
      u8g2.setFont(u8g2_font_open_iconic_weather_4x_t);
      u8g2.drawGlyph(x, y, 67); 
      break;
    case THUNDER:
      u8g2.setFont(u8g2_font_open_iconic_embedded_4x_t);
      u8g2.drawGlyph(x, y, 67);
      break;      
  }
}
void drawWeather(uint8_t symbol, int degree)
{
  drawWeatherSymbol(0, 32, symbol);
  u8g2.setFont(u8g2_font_t0_13_tf);//9 pixels height
  u8g2.setCursor(32, 12);
  u8g2.print(degree);
  u8g2.print("°C");   // requires enableUTF8Print()
}
/*
  Draw a string with specified pixel offset. 
  The offset can be negative.
  Limitation: The monochrome font with 8 pixel per glyph
*/
void drawScrollString(int16_t offset, const char *s)
{
  static char buf[36];  // should for screen with up to 256 pixel width 
  size_t len;
  size_t char_offset = 0;
  u8g2_uint_t dx = 0;
  size_t visible = 0;
  len = strlen(s);
  if ( offset < 0 )
  {
    char_offset = (-offset)/8;
    dx = offset + char_offset*8;
    if ( char_offset >= u8g2.getDisplayWidth()/8 )
      return;
    visible = u8g2.getDisplayWidth()/8-char_offset+1;
    strncpy(buf, s, visible);
    buf[visible] = '\0';
    u8g2.setFont(u8g2_font_8x13_mf);
    u8g2.drawStr(char_offset*8-dx, 45, buf);
  }
  else
  {
    char_offset = offset / 8;
    if ( char_offset >= len )
      return; // nothing visible
    dx = offset - char_offset*8;
    visible = len - char_offset;
    if ( visible > u8g2.getDisplayWidth()/8+1 )
      visible = u8g2.getDisplayWidth()/8+1;
    strncpy(buf, s+char_offset, visible);
    buf[visible] = '\0';
    u8g2.setFont(u8g2_font_8x13_mf);
    u8g2.drawStr(-dx, 45, buf);
  }
}
void draw(const char *s, uint8_t symbol, int degree)
{
  int16_t offset = -(int16_t)u8g2.getDisplayWidth();
  int16_t len = strlen(s);
  for(;;)
  {
    u8g2.firstPage();
    do {
      drawWeather(symbol, degree);      
      drawScrollString(offset, s);
    } while ( u8g2.nextPage() );
    delay(20);
    offset+=2;
    if ( offset > len*8+1 )
      break;
  }
}
