# Twitter Tweeting Server

This example will create a Web-Server to send tweet and search twitter (TwitterClient example). To tweet goto http://TwitterTweet.local/ (Bonjour) or http://<IP_OF_ESP8266>/ and to search goto http://TwitterTweet.local/search (Bonjour) or http://<IP_OF_ESP8266>/search

Install Arduino IDE
* Either update relavent values in TwitterTweet.ino or update sample_secret.h with credentails, rename to secret.h and uncomment the #include "secret.h" file.
* ArduinoJson is not required as parsing is done using text data directly (Method 3 in TwitterClient example)
