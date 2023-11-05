#include <string>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>
#include <TimeLib.h>
#include <TimeAlarms.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>
#include <NTPClient.h>
#include <LittleFS.h>
#include <ESPmDNS.h>

// ------------------------- FEATURE TOGGLE ------------------------- //
// Switch different features on and off here.
bool toggleSerial = true; // serial output
bool toggleDisplay = false; // Adafruit Display
bool toggleTimeClient = true; // Timeclient to get NTP time
bool toggleAlarms = true; // Alarms for doing something at some specific times
bool toggleFileSystem = true; // LittleFS for storing alarm times
bool toggleWiFi = true; // WiFi
bool toggleConnectToWiFi = true; // Connect to a local WiFi network
bool toggleSoftAP = true; // Software WiFi access point for webserver
bool toggleWebServer = true; // Webserver for Local AP access
bool toggleLed = true; // External Led brightness controller
bool toggleLedOnboard = true;
bool toggleMDns = true; // ESPmDns: Gives the ESP a domain name in the local WiFi network in the format http://domainname.local
bool toggleLedButton = true; // Enables the button for switching LED on/off manually

// ------------------------- I N I T ------------------------- //

// ------------------------- O L E D   D I S P L A Y------------------------- //
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Global OLED variables
String textLine1 = "";
String textLine2 = "";
String textLine3 = "";
String textLine4 = "";
String textLine5 = "";
String textLine6 = "";

// TIMERS AND DELAYS 
unsigned long timerLoopSerialDelay = 1000;
unsigned long timerLoopSerial = 0;

unsigned long timerBlinkLedOnboardOn = 0;
unsigned long timerBlinkLedOnboardOff = 0;

unsigned long timerTouchRead = 0;

unsigned long timerLedStatusServerCheck = 0;
unsigned long timerLedStatusServerCheckDelay = 500;

// TOUCHREAD SETTINGS
bool touchStarted = false;
uint16_t touchThreshold = 30;
uint16_t touchMinDuration = 40;

// PULLUP BUTTON TO TRIGGER LED MANUALLY
const uint8_t ledButtonPin = 10;


// ESPmDns settings - for local network url in format name.local 
char * mDnsDomainName = "esp32led";

// WiFiMulti & connection timeout in ms
WiFiMulti wifiMulti;
const uint32_t connectTimeoutMs = 15000;
uint32_t MAX_CONNECTION_ATTEMPTS = 5;
uint32_t CONNECTION_RETRY_DELAY = 500;

// ASYNC WEBSERVER
  // Create AsyncWebServer object on port 80
    AsyncWebServer server(80);
  // Create an Event Source on /events
    AsyncEventSource events("/events");
  // Json Variable to Hold Sensor Readings
    JSONVar readings;
  // Timer variables
    unsigned long lastTime = 0;  
    unsigned long timerDelay = 3000;

// WiFi-network to connect to
const char *ssid_1 = "addWiFiSSIDHere";
const char *pw_1 = "addWiFiPasswordHere";

// ESP32 WiFi access point credentials
const char* ap_ssid = "esp32led";
const char* ap_password = "password";

// SET LED PIN & PWM PROPERTIES

// LED gamma curve for ESP32-Wroom32 with 16-bit resolution
/* const uint32_t ledFadeGammaCurve[1024] = {
     0,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
    100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
    100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
    100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
    100,   100,   100,   100,   100,  100, 103, 106, 109, 112, 115, 118, 121, 124, 127, 131,
   134, 137, 141, 144, 148, 151, 155, 159, 162, 166, 170, 174, 178, 182, 186, 190,
   195, 199, 203, 208, 212, 217, 221, 226, 231, 235, 240, 245, 250, 255, 260, 265,
   271, 276, 281, 287, 292, 298, 303, 309, 315, 321, 326, 332, 339, 345, 351, 357,
   363, 370, 376, 383, 389, 396, 403, 410, 417, 424, 431, 438, 445, 452, 460, 467,
   475, 482, 490, 498, 505, 513, 521, 529, 537, 546, 554, 562, 571, 579, 588, 597,
   605, 614, 623, 632, 641, 651, 660, 669, 679, 688, 698, 708, 717, 727, 737, 747,
   758, 768, 778, 789, 799, 810, 820, 831, 842, 853, 864, 875, 886, 898, 909, 921,
   932, 944, 956, 968, 980, 992,1004,1016,1028,1041,1053,1066,1079,1092,1105,1118,
  1131,1144,1157,1171,1184,1198,1212,1226,1240,1254,1268,1282,1296,1311,1325,1340,
  1355,1370,1385,1400,1415,1430,1446,1461,1477,1492,1508,1524,1540,1556,1573,1589,
  1606,1622,1639,1656,1673,1690,1707,1724,1741,1759,1776,1794,1812,1830,1848,1866,
  1884,1903,1921,1940,1958,1977,1996,2015,2034,2054,2073,2093,2112,2132,2152,2172,
  2192,2212,2233,2253,2274,2295,2315,2336,2358,2379,2400,2422,2443,2465,2487,2509,
  2531,2553,2575,2598,2620,2643,2666,2689,2712,2735,2758,2782,2805,2829,2853,2877,
  2901,2925,2950,2974,2999,3024,3049,3074,3099,3124,3149,3175,3201,3227,3252,3279,
  3305,3331,3358,3384,3411,3438,3465,3492,3519,3547,3574,3602,3630,3658,3686,3714,
  3743,3771,3800,3829,3858,3887,3916,3946,3975,4005,4035,4065,4095,4125,4155,4186,
  4216,4247,4278,4309,4341,4372,4404,4435,4467,4499,4531,4563,4596,4628,4661,4694,
  4727,4760,4794,4827,4861,4894,4928,4962,4997,5031,5065,5100,5135,5170,5205,5240,
  5276,5311,5347,5383,5419,5455,5492,5528,5565,5602,5639,5676,5713,5750,5788,5826,
  5864,5902,5940,5978,6017,6056,6095,6134,6173,6212,6252,6291,6331,6371,6411,6452,
  6492,6533,6574,6615,6656,6697,6739,6780,6822,6864,6906,6949,6991,7034,7077,7119,
  7163,7206,7249,7293,7337,7381,7425,7469,7514,7558,7603,7648,7693,7739,7784,7830,
  7876,7922,7968,8014,8061,8108,8155,8202,8249,8296,8344,8392,8440,8488,8536,8585,
  8633,8682,8731,8780,8830,8879,8929,8979,9029,9079,9129,9180,9231,9282,9333,9384,
  9436,9487,9539,9591,9644,9696,9749,9801,9854,9908,9961,10014,10068,10122,10176,10230,
  10285,10339,10394,10449,10504,10560,10615,10671,10727,10783,10840,10896,10953,11010,11067,11124,
  11181,11239,11297,11355,11413,11472,11530,11589,11648,11707,11767,11826,11886,11946,12006,12066,
  12127,12188,12249,12310,12371,12432,12494,12556,12618,12680,12743,12806,12869,12932,12995,13058,
  13122,13186,13250,13314,13379,13444,13508,13574,13639,13704,13770,13836,13902,13968,14035,14102,
  14168,14236,14303,14370,14438,14506,14574,14643,14711,14780,14849,14918,14987,15057,15127,15197,
  15267,15337,15408,15479,15550,15621,15693,15764,15836,15908,15980,16053,16126,16199,16272,16345,
  16419,16492,16566,16641,16715,16790,16865,16940,17015,17090,17166,17242,17318,17394,17471,17548,
  17625,17702,17780,17857,17935,18013,18091,18170,18249,18328,18407,18486,18566,18646,18726,18806,
  18887,18967,19048,19129,19211,19292,19374,19456,19539,19621,19704,19787,19870,19953,20037,20121,
  20205,20289,20374,20458,20543,20629,20714,20800,20886,20972,21058,21145,21231,21318,21406,21493,
  21581,21669,21757,21845,21934,22023,22112,22201,22291,22381,22471,22561,22651,22742,22833,22924,
  23016,23107,23199,23291,23384,23476,23569,23662,23756,23849,23943,24037,24131,24226,24320,24415,
  24511,24606,24702,24798,24894,24990,25087,25184,25281,25378,25476,25574,25672,25770,25868,25967,
  26066,26166,26265,26365,26465,26565,26666,26766,26867,26969,27070,27172,27274,27376,27479,27581,
  27684,27787,27891,27995,28098,28203,28307,28412,28517,28622,28727,28833,28939,29045,29152,29258,
  29365,29472,29580,29688,29795,29904,30012,30121,30230,30339,30448,30558,30668,30778,30889,30999,
  31110,31222,31333,31445,31557,31669,31782,31894,32008,32121,32234,32348,32462,32577,32691,32806,
  32921,33036,33152,33268,33384,33500,33617,33734,33851,33969,34086,34204,34322,34441,34560,34679,
  34798,34918,35037,35157,35278,35398,35519,35640,35762,35883,36005,36127,36250,36373,36496,36619,
  36742,36866,36990,37115,37239,37364,37489,37614,37740,37866,37992,38119,38246,38373,38500,38627,
  38755,38883,39012,39140,39269,39398,39528,39658,39788,39918,40048,40179,40310,40442,40573,40705,
  40838,40970,41103,41236,41369,41503,41637,41771,41905,42040,42175,42310,42446,42581,42718,42854,
  42991,43127,43265,43402,43540,43678,43816,43955,44094,44233,44372,44512,44652,44793,44933,45074,
  45215,45357,45498,45640,45783,45925,46068,46211,46355,46498,46642,46787,46931,47076,47221,47367,
  47512,47658,47805,47951,48098,48245,48393,48541,48689,48837,48986,49134,49284,49433,49583,49733,
  49883,50034,50185,50336,50488,50640,50792,50944,51097,51250,51403,51557,51711,51865,52019,52174,
  52329,52484,52640,52796,52952,53109,53266,53423,53580,53738,53896,54054,54213,54372,54531,54691,
  54851,55011,55171,55332,55493,55654,55816,55978,56140,56303,56465,56629,56792,56956,57120,57284,
  57449,57614,57779,57945,58110,58277,58443,58610,58777,58944,59112,59280,59449,59617,59786,59955,
  60125,60295,60465,60635,60806,60977,61149,61320,61492,61665,61837,62010,62184,62357,62531,62705,
  62880,63055,63230,63405,63581,63757,63933,64110,64287,64464,64642,64820,64998,65177,65356,65535,
  }; */

// LED gamma curve for ESP32-C3 Supermini with 13-bit resolution
  const uint16_t ledFadeGammaCurve[1024] = {
     0,   0,   0,   0,   1,   1,   1,   2,   2,   3,   3,   4,   4,   5,   6,   6,
     7,   8,   9,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,
    23,  24,  25,  26,  28,  29,  30,  32,  33,  35,  36,  37,  39,  40,  42,  44,
    45,  47,  48,  50,  52,  53,  55,  57,  59,  60,  62,  64,  66,  68,  70,  72,
    74,  76,  78,  80,  82,  84,  86,  88,  90,  92,  94,  96,  99, 101, 103, 105,
   108, 110, 112, 115, 117, 119, 122, 124, 127, 129, 131, 134, 136, 139, 142, 144,
   147, 149, 152, 155, 157, 160, 163, 165, 168, 171, 174, 176, 179, 182, 185, 188,
   191, 194, 196, 199, 202, 205, 208, 211, 214, 217, 220, 224, 227, 230, 233, 236,
   239, 242, 246, 249, 252, 255, 259, 262, 265, 269, 272, 275, 279, 282, 285, 289,
   292, 296, 299, 303, 306, 310, 313, 317, 320, 324, 328, 331, 335, 339, 342, 346,
   350, 353, 357, 361, 365, 368, 372, 376, 380, 384, 388, 391, 395, 399, 403, 407,
   411, 415, 419, 423, 427, 431, 435, 439, 443, 447, 452, 456, 460, 464, 468, 472,
   477, 481, 485, 489, 494, 498, 502, 507, 511, 515, 520, 524, 528, 533, 537, 542,
   546, 551, 555, 560, 564, 569, 573, 578, 582, 587, 591, 596, 601, 605, 610, 615,
   619, 624, 629, 634, 638, 643, 648, 653, 657, 662, 667, 672, 677, 682, 687, 692,
   696, 701, 706, 711, 716, 721, 726, 731, 736, 741, 747, 752, 757, 762, 767, 772,
   777, 782, 788, 793, 798, 803, 808, 814, 819, 824, 830, 835, 840, 846, 851, 856,
   862, 867, 872, 878, 883, 889, 894, 900, 905, 911, 916, 922, 927, 933, 938, 944,
   950, 955, 961, 966, 972, 978, 983, 989, 995,1001,1006,1012,1018,1024,1029,1035,
  1041,1047,1053,1058,1064,1070,1076,1082,1088,1094,1100,1106,1112,1118,1124,1130,
  1136,1142,1148,1154,1160,1166,1172,1178,1184,1191,1197,1203,1209,1215,1222,1228,
  1234,1240,1247,1253,1259,1265,1272,1278,1284,1291,1297,1303,1310,1316,1323,1329,
  1336,1342,1349,1355,1361,1368,1375,1381,1388,1394,1401,1407,1414,1421,1427,1434,
  1440,1447,1454,1460,1467,1474,1481,1487,1494,1501,1508,1514,1521,1528,1535,1542,
  1549,1555,1562,1569,1576,1583,1590,1597,1604,1611,1618,1625,1632,1639,1646,1653,
  1660,1667,1674,1681,1688,1695,1702,1709,1717,1724,1731,1738,1745,1753,1760,1767,
  1774,1781,1789,1796,1803,1811,1818,1825,1833,1840,1847,1855,1862,1870,1877,1884,
  1892,1899,1907,1914,1922,1929,1937,1944,1952,1959,1967,1974,1982,1990,1997,2005,
  2012,2020,2028,2035,2043,2051,2058,2066,2074,2082,2089,2097,2105,2113,2121,2128,
  2136,2144,2152,2160,2168,2175,2183,2191,2199,2207,2215,2223,2231,2239,2247,2255,
  2263,2271,2279,2287,2295,2303,2311,2319,2327,2335,2344,2352,2360,2368,2376,2384,
  2393,2401,2409,2417,2425,2434,2442,2450,2459,2467,2475,2483,2492,2500,2509,2517,
  2525,2534,2542,2550,2559,2567,2576,2584,2593,2601,2610,2618,2627,2635,2644,2652,
  2661,2669,2678,2687,2695,2704,2712,2721,2730,2738,2747,2756,2765,2773,2782,2791,
  2799,2808,2817,2826,2834,2843,2852,2861,2870,2879,2887,2896,2905,2914,2923,2932,
  2941,2950,2959,2968,2977,2986,2995,3004,3013,3022,3031,3040,3049,3058,3067,3076,
  3085,3094,3103,3112,3122,3131,3140,3149,3158,3167,3177,3186,3195,3204,3214,3223,
  3232,3241,3251,3260,3269,3279,3288,3297,3307,3316,3326,3335,3344,3354,3363,3373,
  3382,3392,3401,3411,3420,3430,3439,3449,3458,3468,3477,3487,3496,3506,3516,3525,
  3535,3544,3554,3564,3573,3583,3593,3602,3612,3622,3632,3641,3651,3661,3671,3680,
  3690,3700,3710,3720,3730,3739,3749,3759,3769,3779,3789,3799,3809,3819,3829,3838,
  3848,3858,3868,3878,3888,3898,3908,3919,3929,3939,3949,3959,3969,3979,3989,3999,
  4009,4020,4030,4040,4050,4060,4070,4081,4091,4101,4111,4122,4132,4142,4152,4163,
  4173,4183,4194,4204,4214,4225,4235,4245,4256,4266,4277,4287,4297,4308,4318,4329,
  4339,4350,4360,4371,4381,4392,4402,4413,4423,4434,4445,4455,4466,4476,4487,4498,
  4508,4519,4530,4540,4551,4562,4572,4583,4594,4605,4615,4626,4637,4648,4658,4669,
  4680,4691,4702,4712,4723,4734,4745,4756,4767,4778,4789,4799,4810,4821,4832,4843,
  4854,4865,4876,4887,4898,4909,4920,4931,4942,4953,4964,4976,4987,4998,5009,5020,
  5031,5042,5053,5065,5076,5087,5098,5109,5121,5132,5143,5154,5165,5177,5188,5199,
  5211,5222,5233,5245,5256,5267,5279,5290,5301,5313,5324,5335,5347,5358,5370,5381,
  5393,5404,5416,5427,5439,5450,5462,5473,5485,5496,5508,5519,5531,5542,5554,5566,
  5577,5589,5601,5612,5624,5635,5647,5659,5671,5682,5694,5706,5717,5729,5741,5753,
  5764,5776,5788,5800,5812,5823,5835,5847,5859,5871,5883,5895,5907,5918,5930,5942,
  5954,5966,5978,5990,6002,6014,6026,6038,6050,6062,6074,6086,6098,6110,6122,6134,
  6146,6159,6171,6183,6195,6207,6219,6231,6243,6256,6268,6280,6292,6304,6317,6329,
  6341,6353,6366,6378,6390,6403,6415,6427,6439,6452,6464,6477,6489,6501,6514,6526,
  6538,6551,6563,6576,6588,6601,6613,6625,6638,6650,6663,6675,6688,6700,6713,6726,
  6738,6751,6763,6776,6788,6801,6814,6826,6839,6852,6864,6877,6890,6902,6915,6928,
  6940,6953,6966,6979,6991,7004,7017,7030,7042,7055,7068,7081,7094,7106,7119,7132,
  7145,7158,7171,7184,7197,7209,7222,7235,7248,7261,7274,7287,7300,7313,7326,7339,
  7352,7365,7378,7391,7404,7417,7430,7443,7457,7470,7483,7496,7509,7522,7535,7548,
  7562,7575,7588,7601,7614,7628,7641,7654,7667,7681,7694,7707,7720,7734,7747,7760,
  7774,7787,7800,7814,7827,7840,7854,7867,7880,7894,7907,7921,7934,7948,7961,7974,
  7988,8001,8015,8028,8042,8055,8069,8082,8096,8109,8123,8137,8150,8164,8177,8191,
  };

const int pwmOutput = 5;
const int freq = 1000; // 8bit: 5KHz, 10bit: 1KHz/8KHz, 13bit: 1KHz, 10KHz, 14bit: 5KHz, 16bit: 1KHz
const int ledChannel = 0; 
const int resolution = 13; // 8bit: 256, 10bit: 2048, 13bit: 8191, 16bit: 65535
const int maxDutyCycle = 8191;
const int cycleDelay = 1; // how many milliseconds to delay between each cycle
volatile unsigned int brightness = 0;
bool stopDim = false;
bool ledOn = false;
int sizeOfGammaArray = sizeof(ledFadeGammaCurve)/sizeof(uint16_t); // use uint32_r for ESP32-Wroom32 with 16-bit resolution

void initLed() {
// configure LED PWM functionalitites
  ledcSetup(ledChannel, freq, resolution);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(pwmOutput, ledChannel);
}

// INIT ONBOARD LED
const uint8_t ledOnboard = 8;
void initLedOnboard() {
  pinMode(ledOnboard, OUTPUT);
}

// List of differences in minutes for different alarms [AlarmId] = difference
AlarmId differences[10000]; // Currently random 10Kb allocation of memory. Should be sufficient.

// SETUP GETTING TIME
const long utcOffsetInSeconds = 7200; //Offset from UTC time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void maintainWiFi();
void initWiFi();
void getDateTime();
void writeOnDisplayLine(int lineNumber, String text);
void updateDisplay(int lineNumber, String text);
void increaseBrightness(float minutes);
void decreaseBrightness(float minutes);
void initFS();
void initWebServer();
void initSoftAP();
void createAlarms();
bool isSummerTime(int hour, int month, int tm_mday, int tm_wday);
void blinkOnboardLed(uint32_t delay);
void blinkOnboardLedTimes(uint32_t amount);
void touchLedOnOff();
void buttonLedOnOff();
void initMDNS();

// ------------------------- S E T U P ------------------------- //

void setup() {
if (toggleSerial) {
    Serial.begin(9600);
    delay(500);
    Serial.println("Hello world!");
    }

  if (toggleLedOnboard) {
    if (toggleSerial) {Serial.println("Init ledOnBoard!");}  
    initLedOnboard();
    blinkOnboardLedTimes(2);
    analogWrite(ledOnboard, 255); // Turns the LED off. AnalogWrite 255 = off and 0 = max.
  }

  if (toggleDisplay) {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    delay(1000);
    }

  if (toggleFileSystem) {
    if (toggleSerial) {Serial.println("Init LittleFS...");}
    initFS();
    delay(1000);
    }

  if (toggleWiFi) {
    if (toggleDisplay) {updateDisplay(1, "Init WiFi!");}
    if (toggleSerial) {Serial.println("Init WiFi!");}
    initWiFi();
    if (toggleDisplay) {updateDisplay(1, "WiFi " + WiFi.localIP().toString());}
    if (toggleSerial) {Serial.println("WiFi" + WiFi.localIP().toString());}
    }

  if (toggleMDns) {
    initMDNS();
    }

  if (toggleTimeClient) {
    timeClient.begin();
    if (toggleDisplay) {updateDisplay(3, "Init DateTime!");}
    getDateTime();
    delay(1000);
    }

  if (toggleSoftAP) {
    if (toggleDisplay) {updateDisplay(3, "Init WiFi-AP!");}
    initSoftAP();
    }
    
  if (toggleWebServer) {
    initWebServer();
    }

  if (toggleDisplay) {updateDisplay(3, "Init LED PWM!");}
  if (toggleLed) {initLed();}

  if (toggleLedButton) {
    pinMode(ledButtonPin, INPUT_PULLUP);
    }


  if (toggleDisplay) {updateDisplay(3, "Init timers!");}
  if (toggleAlarms) {createAlarms();}
}

// ------------------------- L O O P ------------------------- //

void loop() {
  // MAINTAIN WIFI CONNECTION
  if (toggleConnectToWiFi) {maintainWiFi();}
  if (toggleLedOnboard) {blinkOnboardLed(2000);}
  if (toggleDisplay) {updateDisplay(3, timeClient.getFormattedTime());}

  if (toggleDisplay) {updateDisplay(5, String(brightness));}
  if (toggleSerial && millis() - timerLoopSerial >= timerLoopSerialDelay) { 
      Serial.println("brightness: " + String(brightness));
      Serial.println(timeClient.getFormattedTime());
      timerLoopSerial = millis();
    }

  // Trigger LED with ledButton
  if (toggleLedButton) {
    buttonLedOnOff();
  }

  Alarm.delay(0);
}

void alarmStarting() {
  AlarmId id;
  float difference;
  id = Alarm.getTriggeredAlarmId();
  difference = differences[id];
  if (toggleDisplay) {updateDisplay(5, "ALARM STARTING!");}
  if (toggleSerial) {Serial.println("ALARM id: " + String(id) + "difference: " + String(difference) + " STARTING!");}
  increaseBrightness(difference);
}

void alarmEnding() {
  AlarmId id;
  float difference;
  id = Alarm.getTriggeredAlarmId();
  difference = differences[id];
  if (toggleDisplay) {updateDisplay(5, "ALARM ENDING!");}
  if (toggleSerial) {Serial.println("ALARM id: " + String(id) + "difference: " + String(difference) + " ENDING!");}
  decreaseBrightness(difference);
}

void createAlarms() {
    // Clean up previous alarms
      uint8_t alarmCount = Alarm.count();
      if (toggleSerial) {Serial.println("1: alarmCount = " + String(alarmCount));}
      if (alarmCount > 0) {
        for (int i = 0; i < alarmCount; i++) {
          Alarm.free(i);
        }
      }
      alarmCount = Alarm.count();
      if (toggleSerial) {Serial.println("2: alarmCount = " + String(alarmCount));}

    // Create new alarms
    if (toggleSerial) {Serial.println("Creating new alarms!");}
    File file = LittleFS.open("/data.csv", "r");
    int lineCount = 0;
    String line;
    while (file.available()) {
      char c = file.read();
      if (c == '\n') {
        lineCount++;
        if (toggleSerial) {Serial.println("lineCount1:" + String(lineCount));}
      }
    }
    file.seek(0); // reset file position to beginning
    if (toggleSerial) {Serial.println("lineCount2:" + String(lineCount));}

  for (int i = 0; i < lineCount; i++) {
    String line = file.readStringUntil('\n');
    int commaIndex = line.indexOf(',');
    String status = line.substring(0, commaIndex);
    String startTime = line.substring(commaIndex + 1, commaIndex + 6);
    String endTime = line.substring(commaIndex + 7, commaIndex + 12);
    AlarmId id;

    int startMinutes = (startTime.substring(0,2).toInt()*60) + startTime.substring(3,5).toInt();
    int endMinutes = (endTime.substring(0,2).toInt()*60) + endTime.substring(3,5).toInt();
    int hour = startTime.substring(0, 2).toInt();
    int minute = startTime.substring(3, 5).toInt();
    int difference = endMinutes - startMinutes;

    if (status == "On" || status == "Off") {
        
        if (toggleSerial) {
          Serial.println("i: " + String(i));
          Serial.println("Status: " + String(status));
          Serial.println("startTime: " + String(startTime));
          Serial.println("endTime: " + String(endTime));
          Serial.println("difference: " + String(difference));}
        if (status == "On") {
          id = Alarm.alarmRepeat(hour,minute,0,alarmStarting);
          differences[id] = difference;
          if (toggleSerial) {Serial.println("Alarm On set at " + String(hour) + ':' + String(minute)) + ". id: " + String(id) + "difference: " + String(difference);}
        }
        if (status == "Off") {
          id = Alarm.alarmRepeat(hour,minute,0, alarmEnding);
          differences[id] = difference;
          if (toggleSerial) {Serial.println("Alarm Off set at " + String(hour) + ':' + String(minute)) + ". id: " + String(id) + "difference: " + String(difference);}
        }
      }
  }
  file.close();
  blinkOnboardLedTimes(2);
}

// Initialize WiFi
void initWiFi()
{
  WiFi.mode(WIFI_AP_STA);

  if (toggleConnectToWiFi) {
    if (toggleSerial) {Serial.println("Connecting to WiFi ..");}
    while (wifiMulti.run(connectTimeoutMs) != WL_CONNECTED)
    {
      for (int i = 0; i < 10; i++)
      {
        wifiMulti.addAP(ssid_1, pw_1);
        // Add more wifiMulti.addAP(ssid, pw); here if you have more wifi-networks you want to connect to.
        if (toggleSerial) {Serial.print('.');}
        if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED)
        {
          if (toggleSerial) {
            Serial.println(' ');
            Serial.print("SSID: "); Serial.println(WiFi.SSID());
            Serial.print("LOCAL IP: "); Serial.println(WiFi.localIP());
            Serial.print("GATEWAY: "); Serial.println(WiFi.gatewayIP());
          }
          break;
        }
      }
    }
  }
}

// Maintain Wifi connection
void maintainWiFi()
{
  while (wifiMulti.run(connectTimeoutMs) != WL_CONNECTED)
  {
    if (toggleSerial) {Serial.println("DISCONNECTED! ReConnecting to WiFi ..");}
    for (int i = 0; i < 10; i++)
    {
      if (toggleSerial) {Serial.print('.');}
      if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED)
      {
        if (toggleSerial) {
          Serial.println(' ');
          Serial.println(WiFi.status());
          Serial.println(WiFi.SSID());
          Serial.println(WiFi.localIP());
          }
        break;
      }
      if (wifiMulti.run(connectTimeoutMs) == WL_NO_SSID_AVAIL) {
        if (toggleSerial) {Serial.println("NO SSID AVAILABLE");}
      }
    }
  }
}

// GETTING DATE
void getDateTime()
{
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime(); // GET epochtime
  struct tm *ptm = gmtime((time_t *)&epochTime);
  int monthDay = ptm->tm_mday;
  int weekDay = ptm->tm_wday;
  int currentMonth = ptm->tm_mon + 1;
  int currentYear = ptm->tm_year + 1900;
  int hour = ptm->tm_hour;
  int minute = ptm->tm_min;
  isSummerTime(hour, currentMonth, monthDay, weekDay) ? 
    timeClient.setTimeOffset(10800) : 
    timeClient.setTimeOffset(7200);
  if (toggleDisplay) {isSummerTime(hour, currentMonth, monthDay, weekDay) ? updateDisplay(4, "It's summer :)") : updateDisplay(4, "It's winter :(");}
  if (toggleSerial) {isSummerTime(hour, currentMonth, monthDay, weekDay) ? Serial.println("It's summer :)") : Serial.println("It's winter :(");}
  timeClient.update();
  String currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
  if (toggleSerial) {Serial.println(epochTime);}
  if (toggleSerial) {Serial.println(currentDate);}
  setTime(timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds(), monthDay, currentMonth, currentYear);
}

// IS IT SUMMERTIME OR WINTERTIME?
bool isSummerTime(int hour, int month, int tm_mday, int tm_wday) {
  int previousSunday;
  tm_wday == 0 ? previousSunday = tm_mday - 7 : previousSunday = tm_mday - tm_wday;
    //If today is sunday, the previous sunday was 7 days ago. If it's any other day, sunday was weekdays amount of days ago
    
  if (month < 3 || month > 10) {
    return false; // Between november and february (including), it's always winter
  } 
  if (month > 3 && month < 10) {
    return true; // Between april and september (including), it's always summer
  } 
   // If it's march or october...
  if (tm_mday <= 24) { // ...and date is 24 or less (24th can be maximally second last sunday)...
      return (month != 3); // ...winter if month 3, summer if else.
    }
  if (previousSunday == 18 && tm_mday == 25 || previousSunday == 24 && tm_mday == 31) { 
        // ...and previousSunday was 18th (or 24th) and today is 25th (or 31st) and hour is more/equal than 3 (minimum/maximum last sunday)...
      /* if (hour < 3) {
          return (month != 3); // ...winter if month 3, summer if else.
      } else {
          return (month == 3); // ...summer if month 3, winter if else.
      } */
      return (hour < 3) ? month != 3 : month == 3;
    }
  if (previousSunday > 18 && previousSunday < 24) { // ...and previousSunday was between 19 and 23... (any sunday in this range is the second last sunday)
    if (previousSunday + 7 == tm_mday) { // ...and previousSunday was exactly a week ago (today is the last sunday)...
        if (hour < 3) {
          return (month != 3); // winter if month 3, summer if else
        } 
        if (hour >= 3) {
            return (month == 3); // summer if month 3, winter, if else
        }
      }  
    }
    return (month == 3) ? previousSunday >= 25 : previousSunday < 25;
    // If it's march, and last sunday was on or after 25th, it's summer! If not, it's winter.
    // If it's october, and last sunday was before 25th, it's still summer. If not, it's winter.
}

// DISPLAY ON LINE
void writeOnDisplayLine(int lineNumber, String text) {
  display.setCursor(0, (lineNumber - 1) * 10);
  display.println(text);
}

void updateDisplay(int lineNumber, String text) {
  lineNumber == 1 ? textLine1 = text : 
  lineNumber == 2 ? textLine2 = text :
  lineNumber == 3 ? textLine3 = text :
  lineNumber == 4 ? textLine4 = text : 
  lineNumber == 5 ? textLine5 = text : 
  textLine6 = text;
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  writeOnDisplayLine(1, textLine1);
  writeOnDisplayLine(2, textLine2);
  writeOnDisplayLine(3, textLine3);
  writeOnDisplayLine(4, textLine4);
  writeOnDisplayLine(5, textLine5);
  writeOnDisplayLine(6, textLine6);
  display.display();
}

void increaseBrightness(float minutes) {
    if (toggleSerial) {Serial.println("Increase brightness triggered!");}
    stopDim = false;
    int seconds = minutes * 60;
    int milliseconds = seconds * 1000;
    int cycles = milliseconds / cycleDelay;

    for(int i = 0; i <= cycles; i++) {
          // Finalize increase quickly if button clicked while increasing
          if (digitalRead(ledButtonPin) == LOW && i > 500) {
            if (toggleSerial) {Serial.println("Increase interrupted!");}
            ledOn ? decreaseBrightness(0.03) : increaseBrightness(0.03);
            break;
          }
          // Increase brightness
          float gammaCurrent = 
            (float(i)/float(cycles))*
            (sizeOfGammaArray - 1); // Find the current element float of the array at i/cycles
          int gammaCurrentInt = trunc(gammaCurrent); // Remove the float
        brightness = int(
            map(
              gammaCurrent, // current float array element 
              gammaCurrentInt, // previous real array element
              gammaCurrentInt+1, // next real array element
              ledFadeGammaCurve[gammaCurrentInt], // previous brightness value at gammaCurrent element of array
              ledFadeGammaCurve[gammaCurrentInt+1] // next brightness value at gammaCurrent element of array
            ));
      ledcWrite(ledChannel, brightness);
      delay(cycleDelay);
  }
      ledOn = true;
      if (toggleSerial) {Serial.println("Brightness increased to max!");}
}

void decreaseBrightness(float minutes) {
    if (toggleSerial) {Serial.println("Decrease brightness triggered!");}
    int seconds = minutes * 60;
    int milliseconds = seconds * 1000;
    int cycles = milliseconds / cycleDelay;
    
      for(int i = cycles; i >= 0; i--) {
        // Finalize decrease quickly if button clicked while decreasing
          if (digitalRead(ledButtonPin) == LOW && i < cycles - 500) {
            if (toggleSerial) {Serial.println("Decrease interrupted!");}
            ledOn ? decreaseBrightness(0.03) : increaseBrightness(0.03);
            break;
          }
        // Decrease brightness
        float gammaCurrent = (float(i)/float(cycles))*(sizeOfGammaArray - 1);
        int gammaCurrentInt = trunc(gammaCurrent);

        brightness = int(
            map(
              gammaCurrent, // current brightness in gamma array
              gammaCurrentInt,
              gammaCurrentInt+1,
              ledFadeGammaCurve[gammaCurrentInt],
              ledFadeGammaCurve[gammaCurrentInt+1]
            ));
        ledcWrite(ledChannel, brightness); 
        delay(cycleDelay);
  }
    ledOn = false;
    if (toggleSerial) {Serial.println("Brightness decreased to min!");}
}

// Initialize LittleFS
void initFS() {
  if (!LittleFS.begin(true)) {
    if (toggleSerial) {Serial.println("An error has occurred while mounting LittleFS");}
  } else {
    if (toggleSerial) {Serial.println("LittleFS mounted successfully");}
  }
}

void initSoftAP() {
  if (toggleDisplay) {updateDisplay(2, "SETUP WiFi-AP");}
  WiFi.softAP(ap_ssid, ap_password);
  if (toggleDisplay) {updateDisplay(2, "WiFi-AP " + WiFi.softAPIP().toString());}
}

// INIT WEBSERVER
void initWebServer() {
  
  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html");
  });

  server.serveStatic("/", LittleFS, "/");

  server.on("/lightswitch", HTTP_POST, [](AsyncWebServerRequest *request) {
    !ledOn ? increaseBrightness(0.03) : decreaseBrightness(0.03);
    request->send(200, "text/plain", "200 OK");
  });

      server.on("/save", HTTP_POST, [](AsyncWebServerRequest *request){
    String data = request->arg("plain");
    File file = LittleFS.open("/data.csv", FILE_WRITE);
    if (file) {
      file.println(data);
      file.close();
      request->send(200, "text/plain", "Data saved successfully");
      if (toggleDisplay) {updateDisplay(4, "Data save success!!!");}

       File file = LittleFS.open("/data.csv", FILE_READ);
        if (file) {
          String data = file.readString();
          if (toggleSerial) {
            Serial.println("Creatig new alarms from:");
            Serial.println(data);
            }
          file.close();
          createAlarms();
        } else {
          if (toggleSerial) {Serial.println("Error opening file");}
        }
      
    } else {
      request->send(500, "text/plain", "Error saving data");
      if (toggleDisplay) {updateDisplay(4, "Data save error!!!");}
    }
  });

    server.on("/data.csv", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(LittleFS, "/data.csv", "text/csv");
      if (toggleDisplay) {updateDisplay(4, "Data requested!");}
  });

  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      if (toggleSerial) {Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());}
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 3000);
  });
  server.addHandler(&events);
  
  // Start server
  server.begin();  
}

void blinkOnboardLed(uint32_t delay) {
  static unsigned long previousMillis = 0;
  static bool ledState = false;

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= delay) {
    previousMillis = currentMillis;
    ledState = !ledState;
    blinkOnboardLedTimes(3);
  }
}

void blinkOnboardLedTimes(uint32_t amount) {
  for (int i = 0; i < amount; i++) {
    analogWrite(ledOnboard, 255);
    Alarm.delay(50);
    analogWrite(ledOnboard, 220);
    Alarm.delay(100);
    analogWrite(ledOnboard, 255);
  }
}

// Using capacitative touch sensor on ESP32-Wroom32 to trigger led on/off
/* void touchLedOnOff() {
    int t = touchRead(13);
    if (t < touchThreshold && !touchStarted) { // When touch starts
      touchStarted = true;
      timerTouchRead = millis();
    }
    if (millis() - timerTouchRead >= touchMinDuration && touchStarted) { // If it's been touched long enough
      if (toggleSerial) {Serial.println("Touchread triggered!");}
      if (ledOn == false) {
        increaseBrightness(0.03);
        touchStarted = false;
        return;
      }
      if (ledOn == true) {
        decreaseBrightness(0.03);
        touchStarted = false;
        return;
      }
    }
    if (t > touchThreshold && millis() - timerTouchRead < touchMinDuration && touchStarted) { // If it's touchThreshold changes too soon
     if (toggleSerial) {Serial.println("False touchRead!");} 
     touchStarted = false;
    }
  } */

  void initMDNS() {
    if (!MDNS.begin(mDnsDomainName)) {
      if (toggleSerial) {Serial.println("MDNS FAILED!");}
    } else {
      if (toggleSerial) {Serial.println("MDNS Started");}
    }
  }

  void buttonLedOnOff() {
    if (digitalRead(ledButtonPin) == LOW) {
      ledOn ? decreaseBrightness(0.03) : increaseBrightness(0.03);
    }
  }