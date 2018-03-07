//WiFi
#define WIFICONFIG
#ifdef WIFICONFIG
const char* ssid = "wifi1";  //WiFi SSID
const char* password = "wifi1password";  //WiFi Password
#endif

// Twitter info
#define TWITTERINFO
#define TWITTERMETHOD
// Do not use these credentials as they are fake data as placeholder
// Obtain these by creating an app @ https://apps.twitter.com/
#ifdef TWITTERINFO
  #define METHOD3    // Use either METHOD1 or METHOD2 or METHOD3 or METHOD4, 3 is most reliable (3>4>2>1)
  static char const consumer_key[]    = "gkyjeH3EF32NJfiuheuyf8623";
  static char const consumer_sec[]    = "HbY5h$N86hg5jjd987HGFsRjJcMkjLaJw44628sOh353gI3H23";
  static char const accesstoken[]     = "041657084136508135-F3BE63U4Y6b346kj6bnkdlvnjbGsd3V";
  static char const accesstoken_sec[] = "bsekjH8YT3dCWDdsgsdHUgdBiosesDgv43rknU4YY56Tj";
#endif
