# Twitter Client
This will connect to Twitter and search for a term. The reply is posted on Serial. There are several method to parse the data. Since the data from Twitter's reply is huge, ArduinoJson DynamicBuffer struggles. So there are multiple ways of reading the data. Use any method that works best for you. Method 3 is the most reliable that parses text data directly. A few tests that have given me this order of relibility to read data from Twitter Method 3>4>2>1.

Install Arduino IDE
* Either update TwitterClient.ino with relavent value or update sample_secret.h with credentails and rename to secret.h. Uncomment the #include "secret.h" file.
* Install ArduinoJson from https://github.com/bblanchon/ArduinoJson
