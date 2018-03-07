#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
//#include "secret.h"                       // uncomment if using secret.h file with credentials
#include "twitter.h"

#ifndef WIFICONFIG
const char* ssid = "wifi_ssid";           // WiFi SSID
const char* password = "wifi_password";   // WiFi Password
#endif

const char *HOSTNAME= "TwitterTweet";     // Hostname of your device
std::string search_str = "@debsahu";      // Default search word for twitter
const char *ntp_server = "pool.ntp.org";  // time1.google.com, time.nist.gov, pool.ntp.org
int timezone = -5;                        // US Eastern timezone -05:00 HRS
unsigned long twi_update_interval = 20;   // (seconds) minimum 5s (180 API calls/15 min). Any value less than 5 is ignored!

#ifndef TWITTERINFO  // Obtain these by creating an app @ https://apps.twitter.com/
  static char const consumer_key[]    = "gkyjeH3EF32NJfiuheuyf8623";
  static char const consumer_sec[]    = "HbY5h$N86hg5jjd987HGFsRjJcMkjLaJw44628sOh353gI3H23";
  static char const accesstoken[]     = "041657084136508135-F3BE63U4Y6b346kj6bnkdlvnjbGsd3V";
  static char const accesstoken_sec[] = "bsekjH8YT3dCWDdsgsdHUgdBiosesDgv43rknU4YY56Tj";
#endif

//   Dont change anything below this line    //
///////////////////////////////////////////////

unsigned long api_mtbs = twi_update_interval * 1000; //mean time between api requests
unsigned long api_lasttime = 0; 
bool twit_update = false;
std::string search_msg = "No Message Yet!";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntp_server, timezone*3600, 60000);  // NTP server pool, offset (in seconds), update interval (in milliseconds)
WiFiClientSecure espclient;
ESP8266WebServer server(80);
TwitterClient tcr(espclient, consumer_key, consumer_sec, accesstoken, accesstoken_sec);

void handleRoot() {
  String t;
  t += "<html>";
  t += "<head>";
  t += "<meta name='viewport' content='width=device-width,initial-scale=1' />";
  t += "<meta http-equiv='Content-Type' content='text/html; charset=utf-8' />";
  t += "</had>";
  t += "<body>";
  t += "<h3><center>";
  t += "Post to Twitter";
  t += "</center></h3>";
  t += "<center>";
  t += "<form method='POST' action='/tweet'>";
  t += "<input type=text name=text style='width: 40em;' autofocus placeholder='Twitter Message'>";
  t += "<input type=submit name=submit value='Tweet'>";
  t += "</form>";
  t += "</center>";
  t += "</body>";
  t += "</html>";
  server.send(200, "text/html", t);
}

void getSearchWord() {
  String webpage;
  webpage =  "<html>";
  webpage += "<meta name='viewport' content='width=device-width,initial-scale=1' />";
   webpage += "<head><title>Twitter IOT Scrolling Text Display</title>";
    webpage += "<style>";
     webpage += "body { background-color: #E6E6FA; font-family: Arial, Helvetica, Sans-Serif; Color: blue;}";
    webpage += "</style>";
   webpage += "</head>";
   webpage += "<body>";
    webpage += "<br>";  
    webpage += "<form action='http://"+WiFi.localIP().toString()+"/processreadtweet' method='POST'>";
     webpage += "<center><input type='text' name='search_input' value='"+String(search_str.c_str())+"' placeholder='Twitter Search'></center><br>";
     webpage += "<center><input type='submit' value='Update Search Keyword'></center>";
    webpage += "</form>";
   webpage += "</body>";
  webpage += "</html>";
  server.send(200, "text/html", webpage); // Send a response to the client asking for input
}

void handleTweet() {
  if (server.method() == HTTP_POST) {
    std::string text;
    bool submit = false;
    for (uint8_t i=0; i<server.args(); i++){
      if (server.argName(i) == "text") {
        String s = server.arg(i);
        text = std::string(s.c_str(), s.length());
      } else if (server.argName(i) == "submit") {
        submit = true;
      }
    }
   if (submit && !text.empty()) tcr.tweet(text);
   server.sendHeader("Location", "/", true);
   server.send(302, "text/plain", "");
  }
}

void readTweet(){
  String t;
  t += "<html>";
  t += "<head>";
  t += "<meta name='viewport' content='width=device-width,initial-scale=1' />";
  t += "<meta http-equiv='Content-Type' content='text/html; charset=utf-8' />";
  t += "</had>";
  t += "<body>";
  t += "<center><p>Searching Twitter for: " + String(search_str.c_str()) + "</p></center>";
  t += "<center><p>Latest Message: " + String(search_msg.c_str()) + "</p></center>";
  t += "<br><center><a href='http://" + WiFi.localIP().toString() + "/search'>Update Search Term?</a></center>";
  t += "</form>";
  t += "</body>";
  t += "</html>";
  server.send(200, "text/html", t);
}

void processReadTweet(){
  if (server.args() > 0 and server.method() == HTTP_POST) { // Arguments were received
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      Serial.print(server.argName(i)); // Display the argument
      if (server.argName(i) == "search_input") {
        search_str=std::string(server.arg(i).c_str());
      }
    }
  }
  
  String t;
  t += "<html>";
  t += "<head>";
  t += "<meta name='viewport' content='width=device-width,initial-scale=1' />";
  t += "<meta http-equiv='Content-Type' content='text/html; charset=utf-8' />";
  t += "</had>";
  t += "<body>";
  t += "<center><p>Updated search term: " + String(search_str.c_str()) + "</p></center>";
  t += "<br><center><a href='http://" + WiFi.localIP().toString() + "/search'>Update again?</a></center>";
  t += "</form>";
  t += "</body>";
  t += "</html>";
  server.send(200, "text/html", t);
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void){
  if (twi_update_interval < 5) api_mtbs = 5000; // Cant update faster than 5s.
  //Begin Serial
  Serial.begin(115200);
  // WiFi Connection
  WiFi.begin(ssid, password);
  Serial.print("\nConnecting to ");
  Serial.print(ssid);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected. yay!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(100);
  // Connect to NTP
  timeClient.begin();
  timeClient.forceUpdate();
  timevalue = (time_t) timeClient.getEpochTime();
  Serial.println("NTP Synced");
  delay(100);
  // Setup internal LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // MDNS Server
  if (MDNS.begin(HOSTNAME)) {              // Start the mDNS responder for esp8266.local
    Serial.println("mDNS responder started.");
    Serial.print("Goto http://");
    Serial.print(HOSTNAME);
    Serial.println(".local/ on web-browser if you are on the same network!");
    Serial.print("or goto http://");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Error setting up MDNS responder!");
  }

  server.on("/", handleRoot);
  server.on("/search", getSearchWord);
  server.on("/tweet", handleTweet);
  server.on("/readtweet",readTweet);
  server.on("/processreadtweet",processReadTweet);
  server.onNotFound(handleNotFound);
  server.begin();

  MDNS.addService("http", "tcp", 80);
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient(); // WebServer
  
  if (millis() > api_lasttime + api_mtbs)  {
    digitalWrite(LED_BUILTIN, LOW);
    timeClient.update();   // NTP time update
    timevalue = (time_t) timeClient.getEpochTime();
    Serial.print("Epoch: "); Serial.println(timeClient.getEpochTime());
    String tmsg = tcr.searchtwitter(search_str);
    if (tmsg.length()>0 and tmsg.length()<500) {
      search_msg = std::string(tmsg.c_str(), tmsg.length());
    }
    Serial.print("Search: ");
    Serial.println(search_str.c_str());
    Serial.print("MSG: ");
    Serial.println(search_msg.c_str());
    api_lasttime = millis();
  }
  yield();
  digitalWrite(LED_BUILTIN, HIGH);
}
