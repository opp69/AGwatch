#include <pebble.h>
  
//#define WX_ONLY
//#define LOG_LEVEL_I    
//#define LOG_LEVEL_I2
  
  
 /*#define TupletStaticCString(_key, _cstring, length) \
  ((const Tuplet) { .type = TUPLE_CSTRING, .key = _key, .cstring = { .data = _cstring, .length = length + 1 }})

#define MyTupletCString(_key, _cstring) \
  ((const Tuplet) { .type = TUPLE_CSTRING, .key = _key, .cstring = { .data = _cstring, .length = strlen(_cstring) + 1 }})
*/
  
/*================================================================*/
#define AGwVersion  "v4.6"
/*================================================================*/
    
#ifdef PBL_COLOR
  #define PHY_COLOR GColorMelon  
  #define EMO_COLOR GColorCeleste  
  #define INT_COLOR GColorMintGreen      
  
  #define BACK_ALLDAY_COLOR GColorBlack  
  #define BACK_7DAY_COLOR GColorOxfordBlue
  #define BACK_1DAY_COLOR GColorBulgarianRose
  
  #define LINE_ALLDAY_COLOR GColorInchworm //GColorRajah
  #define LINE_7DAY_COLOR GColorPictonBlue   
  #define LINE_1DAY_COLOR GColorSunsetOrange
  
  #define RAIN_COLOR GColorVividCerulean
  #define SNOW_COLOR GColorFolly  
  #define THUNDER_COLOR GColorYellow
#else
  #define PHY_COLOR GColorWhite  
  #define INT_COLOR GColorWhite    
  #define EMO_COLOR GColorWhite      
  #define BACK_ALLDAY_COLOR GColorBlack
  #define BACK_7DAY_COLOR GColorBlack
  #define BACK_1DAY_COLOR GColorBlack

  #define LINE_ALLDAY_COLOR GColorWhite
  #define LINE_7DAY_COLOR GColorWhite
  #define LINE_1DAY_COLOR GColorWhite  

  #define RAIN_COLOR GColorWhite
  #define SNOW_COLOR GColorWhite    
  #define THUNDER_COLOR GColorWhite
#endif  
  
  
#include "xconfig.h"
#include "netdownload.h"

Window *s_main_window;

int firstrun = 1; // Variable to check for first run. 1 - YES. 0 - NO.

TextLayer *text_date_layer;     // zone 1
TextLayer *text_week_layer;

TextLayer *text_time_layer;			// zone 2 de texte où l'heure sera affichée

TextLayer *text_zodSIGN_layer;        // zone 3
TextLayer *text_zodMOON_layer;
TextLayer *text_zodiac_layer;

TextLayer *text_bioNAME_layer;
TextLayer *text_bioPHY_layer;
TextLayer *text_bioEMO_layer;
TextLayer *text_bioINT_layer;

TextLayer *text_wxCITY_layer;
TextLayer *text_wxTEMP_layer;  
#ifdef PBL_PLATFORM_BASALT
TextLayer *text_wxICON_layer;  
#endif
TextLayer *text_wxINFO_layer;  



TextLayer *text_msg_layer;      // zone 4
TextLayer *text_msg2_layer;
TextLayer *text_msg3_layer;

TextLayer *ag_back_layer;
Layer *line_layer;
Layer *biograph_layer;
Layer *graph_wxRAIN_layer;


#define THRESHOLD_DAY7 8
#define THRESHOLD_DAY1 2

#define SECONDS_BETWEEN_WEATHER_UPDATES_NIGHT 3000
#define SECONDS_BETWEEN_WEATHER_UPDATES_DAY 900  
#define SECONDS_AFTER_WEATHER_UNVALID 6000
#define SECONDS_AFTER_WEATHER_WARNING 180



static const char *day_names_fr[] = {"Dimanche","Lundi","Mardi","Mercredi","Jeudi","Vendredi","Samedi"};
//static const char *month_names[] = {"Janvier","Février","Mars","Avril","Mai","Juin","Juillet",
//                                    "Août","Septembre","Octobre","Novembre","Décembre"};
static char day_char[] =  {'J','D'};      // 0 = FR, 1 = US
static char year_char[] = "AY";


static char time_text[] = "00:00";
static char date_text[] = "XXX 00.12.1223";
static char week_text[] = "s00"; 
static char zodiac_sign[] = "X"; 
static char moon_sign[]   = " ";
static char zodiac_text[] = "Xxxxxxxxxx"; 
static char bio_text[] = "              ";
static char bioPHY_text[] = "P:-100";
static char bioEMO_text[] = "E:-100";
static char bioINT_text[] = "I:-100";
static char msg_text[]  = "J+XX: 12345678901234567890";
static char msg2_text[] = "J+XX: 12345678901234567890";
static char msg3_text[] = "J+XX: 12345678901234567890";

static char wxTemp_text[] = "---°";
static char wxCity_text[] = "123456789012345";
#ifdef PBL_PLATFORM_BASALT
static char wxIcon_text[] = "\uf055";
#endif
static char wxInfo_text[] = "-xx/-xx° xxx%";


enum KEY_Settings { KEY_LANGUAGE = 1, KEY_VIBRATE, KEY_AGURL, KEY_TEMPUNIT };
static enum SettingLanguage { language_FR = 0, language_US, language_count } config_language;
static enum SettingVibrate  { vibrate_NONE = 0, vibrate_12AM8PM, vibrate_8AMTO8PM, vibrate_ALWAYS, vibrate_count } config_vibrate;
static enum SettingTempUnit { tempunit_CELSIUS = 0, tempunit_FAHRENHEIT, tempunit_count } config_tempunit;
static char config_agurl[128];

static enum LayersMode { ZOD_mode = 1, BIO_mode, WX_mode} layers_mode;
int         layers_duration = 1;      // in Minutes

time_t wxdata_timeOfLastResponse = 0;
time_t wxdata_timeOfLastRequest = 0;
int wxdata_temp100x = 0;
int wxdata_humidity = 0;
static char wxdata_city[30];
int wxdata_condition = 0;
int wxdata_clouds = 0;
static char wxdata_icon[] = "000";

int wxdata_day1time = 0;
int wxdata_day1tempmin = 0;
int wxdata_day1tempmax = 0;
int wxdata_day2time = 0;
int wxdata_day2tempmin = 0;
int wxdata_day2tempmax = 0;

int wxdata_h0conditions = 0;
int wxdata_h3conditions = 0;
int wxdata_h6conditions = 0;
int wxdata_h9conditions = 0;
int wxdata_h12conditions = 0;



int bio_deltaJ = 0;
int global_jdiff = 99;

// PERSISTANT KEY TO STORE THE AGLINES
#define KEY_AG_NBLINE   999
#define KEY_AG_LINES    KEY_AG_NBLINE+1

#define KEY_BIO_NBLINE   1999
#define KEY_BIO_LINES    KEY_BIO_NBLINE+1

#define KEY_WX_GETWEATHER 1000000
#define KEY_WX_TEMP100X   1000010
#define KEY_WX_HUMIDITY  1000011 
#define KEY_WX_ICON      1000012
#define KEY_WX_CITYNAME  1000013
#define KEY_WX_CONDITION 1000014
#define KEY_WX_CLOUDS    1000015
#define KEY_WX_DAY1_TIME   1000020
#define KEY_WX_DAY1TEMPMIN 1000022
#define KEY_WX_DAY1TEMPMAX 1000023
#define KEY_WX_DAY2_TIME   1000030  
#define KEY_WX_DAY2TEMPMIN 1000032
#define KEY_WX_DAY2TEMPMAX 1000033
  
#define KEY_WX_H0_CONDITIONS 1000040
#define KEY_WX_H3_CONDITIONS 1000041  
#define KEY_WX_H6_CONDITIONS 1000042
#define KEY_WX_H9_CONDITIONS 1000043  
#define KEY_WX_H12_CONDITIONS 1000044    
  
  
//static enum SettingTimeFormat { timeformat_12 = 0, timeformat_24, timeformat_count } config_timeformat;
//static AppSync s_sync;
//static uint8_t s_sync_buffer[256];

static BatteryChargeState myBatState;
static bool myBTState = false;    // unknown

#define wxI_w 38
#define wxT_w 40



#include "xmath.h"
#include "agenda.h"


  
void DrawBIOSin(GContext* ctx, int periode) {
  int32_t y1;
  int32_t y2; 
  int i ;
  for (i=0; i<44; i++) {
    int32_t angle = (i/2+bio_deltaJ-14/2) *TRIG_MAX_ANGLE/periode;
    y1 =-sin_lookup(angle) * 16 / TRIG_MAX_RATIO+17;
    angle = ((i+1)/2+bio_deltaJ-14/2)*TRIG_MAX_ANGLE/periode;
    y2 =-sin_lookup(angle) * 16 / TRIG_MAX_RATIO+17;   
    graphics_draw_line(ctx, GPoint(i, y1), GPoint(i+1,y2));
  } 
  
}
 

void biograph_layer_update_callback(Layer *me, GContext* ctx) {
  
  graphics_context_set_stroke_color(ctx, PHY_COLOR);
  DrawBIOSin(ctx, 23);
  
  graphics_context_set_stroke_color(ctx, EMO_COLOR);
  DrawBIOSin(ctx, 28);  
 
  graphics_context_set_stroke_color(ctx, INT_COLOR);
  DrawBIOSin(ctx, 33);  
  
  graphics_context_set_stroke_color(ctx, GColorWhite);
  //graphics_draw_rect(ctx, GRect(0,0,44,35) );
  graphics_draw_line(ctx, GPoint(0,17), GPoint(44,17));
  graphics_draw_line(ctx, GPoint(14,0), GPoint(14, 35));                    
 
}
  



void graph_wxRAIN_layer_update_callback(Layer *me, GContext* ctx) {
   
  #ifdef LOG_LEVEL_I
  APP_LOG(APP_LOG_LEVEL_INFO, "Rain Layer update");
  #endif
  if (wxdata_h0conditions!=0) {
    draw_rain(ctx, 0 ,wxdata_h0conditions);
    draw_rain(ctx, 1 ,wxdata_h3conditions);  
    draw_rain(ctx, 2 ,wxdata_h6conditions);    
    draw_rain(ctx, 3 ,wxdata_h9conditions);      
    draw_rain(ctx, 4 ,wxdata_h12conditions);        
  };
  //graphics_context_set_stroke_color(ctx, LINE_1DAY_COLOR);
  //graphics_context_set_fill_color(ctx, LINE_7DAY_COLOR);
  //graphics_draw_rect(ctx, GRect(0,0, wxI_w, 7));
}


 //Initialise screen = 144 x 168
void line_layer_update_callback(Layer *me, GContext* ctx) {

  #ifdef LOG_LEVEL_I
  APP_LOG(APP_LOG_LEVEL_INFO, "Line Layer update");
  #endif
  
  if (global_jdiff < THRESHOLD_DAY1) {
    graphics_context_set_stroke_color(ctx, LINE_1DAY_COLOR);
  }
  else if (global_jdiff < THRESHOLD_DAY7) {
    graphics_context_set_stroke_color(ctx, LINE_7DAY_COLOR);
  }
  else {
    graphics_context_set_stroke_color(ctx, LINE_ALLDAY_COLOR);
  } 
    
#ifdef PBL_COLOR
  //graphics_context_set_stroke_color(ctx, GColorChromeYellow);
#else
  //graphics_context_set_stroke_color(ctx, GColorWhite);
#endif
  

 graphics_draw_line(ctx, GPoint(6, 22), GPoint(144-6, 22));
 graphics_draw_line(ctx, GPoint(6, 107), GPoint(144-6, 107));  
//  graphics_draw_line(ctx, GPoint(6, 59), GPoint(132, 59));

#ifdef PBL_COLOR
  //graphics_context_set_stroke_color(ctx, GColorRajah);
#endif
  graphics_draw_line(ctx, GPoint(6, 70), GPoint(144-6, 70));
  graphics_draw_line(ctx, GPoint(6, 71), GPoint(144-6, 71));

 
  graphics_context_set_stroke_color(ctx, GColorWhite);
  if (myBTState == true) {
    const int BTH = 8;
    const int BTH4 = 2;
    
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_draw_line(ctx, GPoint(6, 25),               GPoint(6, 25+BTH));
    graphics_draw_line(ctx, GPoint(6, 25),               GPoint(6+BTH4, 25+BTH4));
    graphics_draw_line(ctx, GPoint(6+BTH4, 25+BTH4),     GPoint(6-BTH4, 25+BTH4*3));    
    
    graphics_draw_line(ctx, GPoint(6, 25+BTH),           GPoint(6+BTH4, 25+BTH-BTH4));
    graphics_draw_line(ctx, GPoint(6+BTH4, 25+BTH-BTH4), GPoint(6-BTH4, 25+BTH-BTH4*3) );    
  }
  
  if ((myBatState.is_charging == true) || (myBatState.charge_percent<=BATT_PERCENT)) {
    graphics_draw_line(ctx, GPoint(5, 54), GPoint(7, 54));
    graphics_draw_rect(ctx, GRect(3, 55, 7, 13));
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_rect(ctx, GRect(3+2, 55+13-2, 3, -myBatState.charge_percent/10), 0, GCornerNone);
  }
  
  
  if (myBatState.is_charging == true) {
    graphics_draw_line(ctx, GPoint(5, 50), GPoint(7, 48));
    graphics_draw_line(ctx, GPoint(7, 48), GPoint(5, 48));    
    graphics_draw_line(ctx, GPoint(5, 48), GPoint(7, 46));        
  }
}


void display_layers_mode(enum LayersMode _mode ) {
  
  layer_set_hidden(text_layer_get_layer(text_zodiac_layer),   _mode != ZOD_mode);
  layer_set_hidden(text_layer_get_layer(text_zodSIGN_layer),  _mode != ZOD_mode);    
  layer_set_hidden(text_layer_get_layer(text_zodMOON_layer),  _mode != ZOD_mode);
    
  layer_set_hidden(text_layer_get_layer(text_bioNAME_layer),  _mode != BIO_mode); 
  layer_set_hidden(text_layer_get_layer(text_bioINT_layer),   _mode != BIO_mode);     
  layer_set_hidden(text_layer_get_layer(text_bioEMO_layer),   _mode != BIO_mode);         
  layer_set_hidden(text_layer_get_layer(text_bioPHY_layer),   _mode != BIO_mode);           
  layer_set_hidden(biograph_layer,                            _mode != BIO_mode);           
  
  layer_set_hidden(text_layer_get_layer(text_wxCITY_layer),   _mode != WX_mode); 
  layer_set_hidden(text_layer_get_layer(text_wxTEMP_layer),   _mode != WX_mode);     
#ifdef PBL_PLATFORM_BASALT
  layer_set_hidden(text_layer_get_layer(text_wxICON_layer),   _mode != WX_mode); 
#endif  
  layer_set_hidden(text_layer_get_layer(text_wxINFO_layer),   _mode != WX_mode);       
  layer_set_hidden(graph_wxRAIN_layer,                        _mode != WX_mode);         
};


void request_weather() {    
  #ifdef LOG_LEVEL_I
    APP_LOG(APP_LOG_LEVEL_INFO, "Pebble Request WX"); 
  #endif
  
  if  (difftime(time(NULL), wxdata_timeOfLastRequest) > 100) {   // wait min 100 sec between
    #ifdef LOG_LEVEL_I
      APP_LOG(APP_LOG_LEVEL_INFO, "WX to old"); 
    #endif
    
    wxdata_timeOfLastRequest = time(NULL);
   
    if (myBTState==true) {
      APP_LOG(APP_LOG_LEVEL_INFO, "WX to old and BT Ok : Request to JS"); 
    
      DictionaryIterator *iter;
      app_message_outbox_begin(&iter);
      Tuplet value = TupletInteger(KEY_WX_GETWEATHER, 0);
      dict_write_tuplet(iter, &value);
      app_message_outbox_send();
    }
  }
}


void update_weathertext() {
  
  #ifdef LOG_LEVEL_I
    APP_LOG(APP_LOG_LEVEL_INFO, "Updating WX text"); 
  #endif
    

  int tempC = round100(kelvin100toTU(wxdata_temp100x, config_tempunit));
    
  snprintf(wxTemp_text, sizeof(wxTemp_text), "%d°",  tempC );
  snprintf(wxCity_text, sizeof(wxCity_text), "%s",   wxdata_city );
    
 
  int d1tempmin = floor100(kelvin100toTU(wxdata_day1tempmin, config_tempunit));   // for rounding  
  int d1tempmax = ceil100(kelvin100toTU(wxdata_day1tempmax, config_tempunit));  
  int d2tempmin = floor100(kelvin100toTU(wxdata_day2tempmin, config_tempunit));
  int d2tempmax = ceil100(kelvin100toTU(wxdata_day2tempmax, config_tempunit));
  
  #ifdef LOG_LEVEL_I2
    APP_LOG(APP_LOG_LEVEL_INFO, "%d %d %d %d", d1tempmin, d1tempmax, d2tempmin, d2tempmax); 
  #endif
  
  if (wxdata_day1time > 0) { 
    // Forecast Response
    time_t currentTime = time(NULL);
    struct tm *currentCalendarTime = localtime(&currentTime);
    int dayOfMonthCurrent = currentCalendarTime->tm_mday;
      
    time_t day1Date_t = wxdata_day1time;
    struct tm *day1CalendarTime = localtime(&day1Date_t);
    int dayOfMonth1 = day1CalendarTime->tm_mday;
      
    time_t day2Date_t = wxdata_day2time;
    struct tm *day2CalendarTime = localtime(&day2Date_t);
    int dayOfMonth2 = day2CalendarTime->tm_mday;
      
    if (dayOfMonthCurrent == dayOfMonth1) {     
        snprintf(wxInfo_text, sizeof(wxInfo_text),"%d/%d° %d%%", d1tempmin, d1tempmax, wxdata_humidity);       

      }
      else if (dayOfMonthCurrent == dayOfMonth2) {
        // Day 2 is Today's Date 
        snprintf(wxInfo_text, sizeof(wxInfo_text),"%d/%d° %d%%", d2tempmin, d2tempmax, wxdata_humidity);      
    }
    else snprintf(wxInfo_text, sizeof(wxInfo_text),"?/?° %d%%", wxdata_humidity);
  } // (day1Date > 0)
  else snprintf(wxInfo_text, sizeof(wxInfo_text),"%d%%", wxdata_humidity);    
  
  #ifdef LOG_LEVEL_I2
     APP_LOG(APP_LOG_LEVEL_INFO, "infotext= %s", wxInfo_text);
  #endif
  
 // if (wxdata_day1tempmin==0) snprintf(wxInfo_text, sizeof(wxInfo_text),"%d%%", wxdata_humidity);  
//else snprintf(wxInfo_text, sizeof(wxInfo_text),"%d/%d° %d%%", d1tempmin, d1tempmax, wxdata_humidity);
#ifdef PBL_PLATFORM_BASALT  
  bool isDay = true;
  bool isCloudy = true;
  if (strlen(wxdata_icon)>1 && wxdata_icon[strlen(wxdata_icon)-1]=='n') isDay = false;
  if (wxdata_clouds<70) isCloudy = false;
  
  switch (wxdata_condition) {
    case 200:
    case 201:
    case 202:
      if (isCloudy==true)   snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf01e");  
      else if (isDay==true) snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf010");  
      else                  snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf02d");  
      break;
    case 210:
    case 211:    
      if (isDay==true) snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf005");  
      else             snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf025");  
      break;
    case 212:
    case 221:
      snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf016");  
      break;
    case 230:
    case 231:
    case 232:
      if (isCloudy==true)   snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf01d");  
      else if (isDay==true) snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf00e");  
      else                  snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf02c");  
      break;
    case 300:
    case 301:
    case 302:
    case 310:
    case 311:    
    case 312:
    case 313:
    case 314:
    case 321:
     if (isCloudy==true)   snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf01c");  
      else if (isDay==true) snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf00b");  
      else                  snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf02b");  
      break;
    case 500:
    case 501:
    case 502:    
    case 503:
    case 504:    
      if (isCloudy==true)   snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf019");  
      else if (isDay==true) snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf008");  
      else                  snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf028");  
      break;
     case 520:    
     case 521:
     case 522:
     case 531: 
      if (isCloudy==true)   snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf01a");  
      else if (isDay==true) snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf009");  
      else                  snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf029");  
      break;    
    //==================================================
    case 600:  //snow
    case 601:
    case 602:
    case 621:
    case 622:
      if (isCloudy==true)   snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf01b");  
      else if (isDay==true) snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf00a");  
      else                  snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf02a");  
      break; 
    case 611: //Sleet
    case 612:
      if (isCloudy==true)   snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf0b5");  
      else if (isDay==true) snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf0b2");  
      else                  snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf0b4");  
      break; 
    case 615: //Rain & snow
    case 616:
      if (isCloudy==true)   snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf017");  
      else if (isDay==true) snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf006");  
      else                  snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf026");  
      break; 
    //==================================================
    case 701: // mist
      if (isCloudy==true)   snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf014");  
      else if (isDay==true) snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf003");  
      else                  snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf04a");
      break;
    case 711: // smoke
      snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf062");
      break;
    //==================================================
    case 800: // clear sky
      if (isDay==true) snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf00d");  
      else             snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf02e");
      break;
    case 801: // few clouds 02d
      if (isDay==true) snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf00c");  
      else             snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf083");
      break;
    case 802: // scatted clouds 03d
      if (isDay==true) snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf002");  
      else             snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf086");
      break;
    case 803: // broken clouds 04d
      snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf041");
      break;
    case 804: // overcast clouds 04d
      snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf013");
      break;
    //==================================================
    case 900: // Tornado
      snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf056");
      break;
    case 901: // Tropical Storm
    case 902: // Hurricane
      snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf073");
      break;
    case 903: // Cold
      snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf076");
      break;            
    case 904: // Hot
      snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf072");
      break;        
    case 905: // Windy
      snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf050");
      break;         
    case 906: // hail
      if (isCloudy==true)   snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf015");  
      else if (isDay==true) snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf004");  
      else                  snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf024");  
      break;
    case 951: // Calm
      snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf0b7");
      break;         
    case 952: // Light Breeze
      snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf0b9");
      break;         
    case 953: // Calm Breeze
      snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf0ba");
      break;         
    case 954: // Moderate Breeze
      snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf0bb");
      break;         
    case 955: // Moderate Breeze
      snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf0bc");
      break;                
    case 956: // Moderate Breeze
      snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf0bd");
      break;                
    case 957: // Moderate Breeze
      snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf0be");
      break;                
    case 958: // Moderate Breeze
      snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf0bf");
      break;                
    case 959: // Moderate Breeze
      snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf0c0");
      break;                
    case 960: // Moderate Breeze
      snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf0c1");
      break;                
    case 961: // Moderate Breeze
      snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf0c2");
      break;                
    case 962: // Moderate Breeze
      snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf0c3");
      break;                
            
    
    default:
      snprintf(wxIcon_text, sizeof(wxIcon_text), "%s",   "\uf055");  
      snprintf(wxInfo_text, sizeof(wxInfo_text),"? %d %s", wxdata_condition, wxdata_icon);
  }
#endif
 
}


void SetLayerMode(enum LayersMode _mode) {
  layers_mode = _mode;
  switch (_mode) {
    case WX_mode:
      layers_duration=2;
      break;  
    default: layers_duration=1;
  }
};

void text_layers_update(struct tm *t) {

  // Update Every Minute
  if (t->tm_sec == 0 || firstrun == 1) {
    
    // Hour & Minute Formatting Type
    char *time_format;
    time_format = (clock_is_24h_style()) ? "%R" : "%I:%M" ;			// %R = %H:%M
    strftime(time_text, sizeof(time_text), time_format, t);
    if (!clock_is_24h_style() && (time_text[0] == '0')) {
     memmove(time_text, &time_text[1], sizeof(time_text) - 1);			// en AM/PM 02:12 -> 2:13
    };

    // Set the TextLayer text
    text_layer_set_text(text_time_layer, time_text); 
    
    
    // LAYERS TOOGLES
    layers_duration--;    
    if (layers_duration<1) {
      if (layers_mode==ZOD_mode) SetLayerMode(WX_mode);
      else if (layers_mode==WX_mode) SetLayerMode(BIO_mode); 
      else if (layers_mode==BIO_mode) SetLayerMode(ZOD_mode);
    }
    
    // Change the mode if no data
    if ((wxdata_timeOfLastResponse==0 || difftime(time(NULL), wxdata_timeOfLastResponse)>SECONDS_AFTER_WEATHER_UNVALID) 
        && (layers_mode==WX_mode)) SetLayerMode(BIO_mode);
    
    if (difftime(wxdata_timeOfLastRequest, wxdata_timeOfLastResponse)>SECONDS_AFTER_WEATHER_WARNING) {
      int tempC = round100(kelvin100toTU(wxdata_temp100x, config_tempunit));
      snprintf(wxTemp_text, sizeof(wxTemp_text), "%d?",  tempC );
      //if (strlen(wxTemp_text)>0) wxTemp_text[strlen(wxTemp_text)-1]='i'; // don't work because UTF8
    };
    
    if ((BIO_ARRAY_SIZE==0) && (layers_mode==BIO_mode)) SetLayerMode(ZOD_mode);   
    if (firstrun ==1) SetLayerMode(ZOD_mode);      // always begin with ZOD_mode
    
    #ifdef WX_ONLY
      SetLayerMode(WX_mode);      // DEBUG
    #endif
    
    #ifdef LOG_LEVEL_I
      APP_LOG(APP_LOG_LEVEL_INFO, "layers_duration %d",layers_duration); 
    #endif
    
    display_layers_mode(layers_mode);        
  };


  // Check only every hour
  if  (t->tm_min == 0) {
    switch (config_vibrate) {
      case vibrate_count:
      case vibrate_NONE: break;
      case vibrate_ALWAYS:
        vibes_double_pulse();
        break;
      case vibrate_12AM8PM:
        if (t->tm_hour == 12 || t->tm_hour == 20) vibes_double_pulse();
        break;
      case vibrate_8AMTO8PM:
        if (t->tm_hour >= 8 && t->tm_hour <= 20) vibes_double_pulse();
        break;        
    }
  };
  
  // Update weather
  if ( (firstrun == 0) && (t->tm_sec == 0) &&
     (difftime(time(NULL), wxdata_timeOfLastResponse)>
        (t->tm_hour >= 6 && t->tm_hour <= 22 ? SECONDS_BETWEEN_WEATHER_UPDATES_DAY : SECONDS_BETWEEN_WEATHER_UPDATES_NIGHT) 
     ) ) {
    //vibes_short_pulse();
    request_weather();              // dont request at firstrun because JS not ready    
  };

  #ifdef LOG_LEVEL_I2 
  APP_LOG(APP_LOG_LEVEL_INFO, "wxINFOtext=%s", wxInfo_text);
  #endif  

  
  // Updated every day or firstrun
  if (t->tm_hour == 0 || firstrun == 1) {

    static char tmp_date_text[] = "XXX 00.12.1223";
    if (config_language == language_FR) {
      strncpy(date_text, day_names_fr[t->tm_wday], 3);		// le joursem sur 2 char
      date_text[3]='\0';
      strftime(tmp_date_text, sizeof(tmp_date_text), " %d/%m/%Y", t); 
      strcat(date_text, tmp_date_text);
    }
    else {
      strftime(tmp_date_text, sizeof(tmp_date_text), "%a", t);
      strncpy(date_text, tmp_date_text, 3);
      date_text[3]='\0';
      strftime(tmp_date_text, sizeof(tmp_date_text), " %m.%d.%Y", t); 
      strcat(date_text, tmp_date_text);
    };    
    text_layer_set_text(text_date_layer, date_text);

    // WEEK NUM
    if (config_language == language_FR) {
      strftime(week_text, sizeof(week_text), "S%V", t);
    }
    else {
      strftime(week_text, sizeof(week_text), "W%U", t);
    };
    text_layer_set_text(text_week_layer, week_text);

    // ZODIAC 
    int m = get_zodiac_int(t);
    strcpy(zodiac_sign, z_sign[m]);
    text_layer_set_text(text_zodSIGN_layer, zodiac_sign);
  
    if (config_language == language_FR) {
      strcpy(zodiac_text, zodiac_fr[m]); 
    } 
    else {
      strcpy(zodiac_text, zodiac_us[m]);
    }; 
    text_layer_set_text(text_zodiac_layer, zodiac_text);


    // MOON
    moon_sign[0] = get_moon_phase_char(t);    
    text_layer_set_text(text_zodMOON_layer, moon_sign);
        
    
    // BIO    
    if (BIO_ARRAY_SIZE>0) {
      strncpy(bio_text, BioLookup[0].txt, sizeof(bio_text));
    
      struct tm bio_day;
      bio_day.tm_mday = BioLookup[0].d;  
      bio_day.tm_mon = BioLookup[0].m-1;
      bio_day.tm_year = BioLookup[0].y;
            
      bio_deltaJ = tm2jd(t)-tm2jd(&bio_day);
      
#ifdef LOG_LEVEL_I2
      APP_LOG(APP_LOG_LEVEL_INFO, "jbio %i",  tm2jd(&bio_day)); 
      APP_LOG(APP_LOG_LEVEL_INFO, "today %i",  tm2jd(t));       
      APP_LOG(APP_LOG_LEVEL_INFO, "JOUR %i",  BioLookup[0].d); 
      APP_LOG(APP_LOG_LEVEL_INFO, "MOIS %i",  BioLookup[0].m);       
      APP_LOG(APP_LOG_LEVEL_INFO, "ANNEE %i",  BioLookup[0].y);             
      APP_LOG(APP_LOG_LEVEL_INFO, "BIO DELTA %i", bio_deltaJ); 
#endif
      
      text_layer_set_text(text_bioNAME_layer, bio_text);
   
      set_biolayer(text_bioPHY_layer, 'P', bioPHY_text, get_biorythm(bio_deltaJ, 23), PHY_COLOR);            
      set_biolayer(text_bioEMO_layer, 'E', bioEMO_text, get_biorythm(bio_deltaJ, 28), EMO_COLOR);
      set_biolayer(text_bioINT_layer, 'I', bioINT_text, get_biorythm(bio_deltaJ, 33), INT_COLOR);              
    };
    
   
    // WX    
    text_layer_set_text(text_wxTEMP_layer, wxTemp_text);    
    text_layer_set_text(text_wxCITY_layer, wxCity_text);   
    text_layer_set_text(text_wxINFO_layer, wxInfo_text);   
#ifdef PBL_PLATFORM_BASALT
    text_layer_set_text(text_wxICON_layer, wxIcon_text);       //\uf055
#endif

  // AGENDA
    text_layer_set_text_color(text_msg_layer, GColorWhite);
    text_layer_set_background_color(text_msg_layer, GColorClear); 
    text_layer_set_text_color(text_msg2_layer, GColorWhite);
    text_layer_set_background_color(text_msg2_layer, GColorClear); 
    text_layer_set_text_color(text_msg3_layer, GColorWhite);
    text_layer_set_background_color(text_msg3_layer, GColorClear); 

    TextLayer *cur_agenda_layer;
    char      *cur_msg;

    int i = 0;
    char buf[20];
    char NoceTmp[32];
    struct tm aTm;
//    PblTm today;


    int jtoday    = tm2jd(t);

    // curday tested   
    struct tm curdayTm   = *t;
    int jcurdayTm = jtoday;

    int const NB_TEXTMSG = 3;
    int afound  = 0;
    global_jdiff = 99;      // 
    while (afound<NB_TEXTMSG) {

      int jdiff = jcurdayTm-jtoday;

      for (i = 0; i<AGENDA_ARRAY_SIZE; i++) {
        
        aTm.tm_mday = AgendaLookup[i].d;  
        if (aTm.tm_mday == 0) { aTm.tm_mday = curdayTm.tm_mday; };
        if (aTm.tm_mday != curdayTm.tm_mday) continue;

        aTm.tm_mon  = (AgendaLookup[i].m-1);  //janvier = 0
				if (aTm.tm_mon < 0)   { aTm.tm_mon  = curdayTm.tm_mon; };
        if (aTm.tm_mon != curdayTm.tm_mon) continue;

			  aTm.tm_year = curdayTm.tm_year;
				aTm.tm_wday = char2wday(AgendaLookup[i].wd[0], curdayTm.tm_wday);
			  if (aTm.tm_wday != curdayTm.tm_wday) continue; 

        if (isInWeek(&aTm, AgendaLookup[i].wd[1]) == false) continue;

   						
        // we found an entry curdayTm is aTm
	      if ((AgendaLookup[i].nbj == 0) || (jdiff<AgendaLookup[i].nbj)) {

          switch (afound) {
            case 0: cur_agenda_layer = text_msg_layer;  cur_msg = msg_text; break;
            case 1: cur_agenda_layer = text_msg2_layer; cur_msg = msg2_text; break;
            default: cur_agenda_layer = text_msg3_layer; cur_msg = msg3_text; 
          };

          // couleur de fond
          #ifdef PBL_COLOR
          if (afound==0) {
            global_jdiff = jdiff;
            if (jdiff < THRESHOLD_DAY1) {
              //window_set_background_color(s_main_window, BACK_1DAY_COLOR);
              text_layer_set_background_color(ag_back_layer, BACK_1DAY_COLOR);
            }
            else if (jdiff < THRESHOLD_DAY7) {
              //window_set_background_color(s_main_window, BACK_7DAY_COLOR); 
              text_layer_set_background_color(ag_back_layer, BACK_7DAY_COLOR);  
            }            
            else {
              //window_set_background_color(s_main_window, BACK_ALLDAY_COLOR);
              text_layer_set_background_color(ag_back_layer, BACK_ALLDAY_COLOR); 
            };
          }
          #endif
          
          // Bold if < 8 jour
          if (jdiff < 8) { 
            if (afound<2) {
              text_layer_set_font(cur_agenda_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD)); 
            }
            else {
              text_layer_set_font(cur_agenda_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD)); 
            };
          }
          else { 
            if (afound<2) {
              text_layer_set_font(cur_agenda_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18)); 
            }
            else {
              text_layer_set_font(cur_agenda_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14)); 
            };
          }

 	        if (jdiff == 0) { 
            strcpy(buf, "J: "); buf[0]=day_char[config_language];						// localisation
            text_layer_set_text_color(cur_agenda_layer, GColorBlack);
            text_layer_set_background_color(cur_agenda_layer, GColorWhite);
          } 
          else if (jdiff < 70) {
            snprintf(buf,10,"J+%d: ", jdiff); buf[0]=day_char[config_language];
          }
          else if (jdiff < 370) { 
            snprintf(buf,10,"M+%d: ",  + (aTm.tm_year-t->tm_year)*12+aTm.tm_mon-t->tm_mon); 
          }
          else { 
           snprintf(buf,10,"A+%d: ", aTm.tm_year-t->tm_year); buf[0]=year_char[config_language];
          };
          strcpy(cur_msg, buf);

          //
          int year_delta = aTm.tm_year-AgendaLookup[i].p1;
          switch(AgendaLookup[i].param[0]) {
            case 'A':
            case 'Y':
              snprintf(buf, 20, AgendaLookup[i].txt, year_delta);  break;
            case 'R':
              if (year_delta<0 || year_delta >= NOCE_ARRAY_SIZE) year_delta=0;
              get_noce(config_language, year_delta, NoceTmp);
              snprintf(buf, 20, AgendaLookup[i].txt, NoceTmp);
              /*switch(config_language) {
                case language_FR:snprintf(buf, 20, AgendaLookup[i].txt, NoceLookup_fr[year_delta]); break;
                default         :snprintf(buf, 20, AgendaLookup[i].txt, NoceLookup_us[year_delta]); 
              }*/
              break;
            case 'V':
                snprintf(buf, 20, AgendaLookup[i].txt, AGwVersion);
              break;
            default: 
              strcpy(buf, AgendaLookup[i].txt);
          };         
          strcat(cur_msg, buf);
     	
          afound++;
          if (afound==NB_TEXTMSG) break;
        };
			}; // for agenda

			if ((jdiff>365) && (afound==0)) break;

      jcurdayTm++;	//check next julian day
      TmAddDays(&curdayTm, 1);
	
		  
    }; // while 3 lines

    if (afound < 1) { strcpy(msg_text,  " -=- "); };
    if (afound < 2) { strcpy(msg2_text, " -=- "); };
    if (afound < 3) { strcpy(msg3_text, " -=- "); };

    text_layer_set_text(text_msg_layer, msg_text);
    text_layer_set_text(text_msg2_layer, msg2_text);
    text_layer_set_text(text_msg3_layer, msg3_text);      

  }; // day update


  firstrun = 0;
}


static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  text_layers_update(tick_time);
}


static void handle_bluetooth(bool connected) {
  myBTState = connected;
  APP_LOG(APP_LOG_LEVEL_INFO, "Bluetooth callback. State=%i", connected); 
  layer_mark_dirty(line_layer);
}

static void handle_battery(BatteryChargeState charge_state) {
  myBatState = charge_state;
  APP_LOG(APP_LOG_LEVEL_INFO, "Battery callback. Charge=%i", charge_state.charge_percent);   
  layer_mark_dirty(line_layer);
}

static void set_Initial_Layers() {
  #ifdef LOG_LEVEL_I
  APP_LOG(APP_LOG_LEVEL_INFO, "Setting Initial Layers");   
  #endif
  
  time_t t = time(0);
  struct tm *pebble_time = localtime(&t);
    
  myBTState = bluetooth_connection_service_peek();
  myBatState = battery_state_service_peek();
  firstrun = 1;
  text_layers_update(pebble_time);           // this makes line layer dirty
}




void parse_AGLINES_fromPersist(void){
  
  static char agline[255+1];
 
  int nbagline = persist_read_int(KEY_AG_NBLINE);
  #ifdef LOG_LEVEL_I
  APP_LOG(APP_LOG_LEVEL_INFO, "PARSING AG nblines=%i", nbagline);
  #endif
  
  if (AgendaLookup != NULL) {
    AGENDA_ARRAY_SIZE = 0;
    free(AgendaLookup);
    AgendaLookup = NULL;
  };
  
  AgendaLookup = (agenda*) malloc(sizeof(agenda) * nbagline);
  if (AgendaLookup == NULL) {
     AGENDA_ARRAY_SIZE = 0;
     APP_LOG(APP_LOG_LEVEL_INFO, "AGmalloc failed");
    return;
  }
  else AGENDA_ARRAY_SIZE = nbagline;
  
  int nbaglineloaded = 0;
  for (int i=0; i<nbagline; i++) {        
    if (persist_exists(KEY_AG_LINES+i)) {
      persist_read_string(KEY_AG_LINES+i, agline, sizeof(agline));
      #ifdef LOG_LEVEL_I2
      APP_LOG(APP_LOG_LEVEL_INFO, "PARSING AG line index=%i, LINE=%s", i, agline);
      #endif
      if ((agline[0]=='a') && (agline[1]='=')) {
        //agline
        
        AgendaLookup[i].m = AGwLIN_get_integer(agline, 'm');
        #ifdef LOG_LEVEL_I2 
          APP_LOG(APP_LOG_LEVEL_INFO, "m=%i", AgendaLookup[i].m); 
        #endif
          
        AgendaLookup[i].d = AGwLIN_get_integer(agline, 'd');
        #ifdef LOG_LEVEL_I2         
          APP_LOG(APP_LOG_LEVEL_INFO, "d=%i", AgendaLookup[i].d);
        #endif
                
        AGwLIN_get_string(agline, 'w', AgendaLookup[i].wd, sizeof(AgendaLookup[i].wd));
        #ifdef LOG_LEVEL_I2         
          APP_LOG(APP_LOG_LEVEL_INFO, "w='%s'", AgendaLookup[i].wd);
        #endif
        
        AgendaLookup[i].nbj = AGwLIN_get_integer(agline, 'n');
        #ifdef LOG_LEVEL_I2         
          APP_LOG(APP_LOG_LEVEL_INFO, "n=%i", AgendaLookup[i].nbj);
        #endif
        
        AGwLIN_get_string(agline, 't', AgendaLookup[i].txt, sizeof(AgendaLookup[i].txt));
        #ifdef LOG_LEVEL_I2         
          APP_LOG(APP_LOG_LEVEL_INFO, "t='%s'", AgendaLookup[i].txt);                        
        #endif        
          
        AGwLIN_get_string(agline, 'p', AgendaLookup[i].param, sizeof( AgendaLookup[i].param));
        #ifdef LOG_LEVEL_I2 
          APP_LOG(APP_LOG_LEVEL_INFO, "p='%s'",  AgendaLookup[i].param);        
        #endif
        
        AgendaLookup[i].p1 = AGwLIN_get_integer(agline, 'q');
        #ifdef LOG_LEVEL_I2                 
          APP_LOG(APP_LOG_LEVEL_INFO, "q=%i", AgendaLookup[i].p1);
        #endif
        nbaglineloaded++; 
      };
    };
  };
  if (nbaglineloaded<AGENDA_ARRAY_SIZE) {
    #ifdef LOG_LEVEL_I
    APP_LOG(APP_LOG_LEVEL_ERROR, "AG NbLine mismatch. Reload from web. Reducing from %i to %i", AGENDA_ARRAY_SIZE, nbaglineloaded);
    AGENDA_ARRAY_SIZE=nbaglineloaded;
    #endif
  }
  #ifdef LOG_LEVEL_I
  APP_LOG(APP_LOG_LEVEL_INFO, "End of AG PARSING");
  #endif
};


void parse_BIOLINES_fromPersist(void){
  
  static char bioline[255+1];
 
  int nbbioline = persist_read_int(KEY_BIO_NBLINE);
  #ifdef LOG_LEVEL_I
  APP_LOG(APP_LOG_LEVEL_INFO, "PARSING BIO nblines=%i", nbbioline);
  #endif
  
  if (BioLookup != NULL) {
    BIO_ARRAY_SIZE = 0;
    free(BioLookup);
    BioLookup = NULL;
  };
  
  BioLookup = (bio*) malloc(sizeof(bio) * nbbioline);
  if (BioLookup == NULL) {
     BIO_ARRAY_SIZE = 0;
     APP_LOG(APP_LOG_LEVEL_INFO, "BIOmalloc failed");
    return;
  }
  else BIO_ARRAY_SIZE = nbbioline;
  
  int nbbiolineloaded = 0;
  for (int i=0; i<nbbioline; i++) {        
    if (persist_exists(KEY_BIO_LINES+i)) {
      persist_read_string(KEY_BIO_LINES+i, bioline, sizeof(bioline));
      #ifdef LOG_LEVEL_I2
      APP_LOG(APP_LOG_LEVEL_INFO, "PARSING BIO line index=%i, LINE=%s", i, bioline);
      #endif
      if ((bioline[0]=='b') && (bioline[1]='=')) {
        //agline
        
        BioLookup[i].m = AGwLIN_get_integer(bioline, 'm');
        #ifdef LOG_LEVEL_I2 
          APP_LOG(APP_LOG_LEVEL_INFO, "m=%i", BioLookup[i].m); 
        #endif
          
        BioLookup[i].d = AGwLIN_get_integer(bioline, 'd');
        #ifdef LOG_LEVEL_I2         
          APP_LOG(APP_LOG_LEVEL_INFO, "d=%i", BioLookup[i].d);
        #endif
                        
        BioLookup[i].y = AGwLIN_get_integer(bioline, 'y');
        #ifdef LOG_LEVEL_I2         
          APP_LOG(APP_LOG_LEVEL_INFO, "y=%i", BioLookup[i].y);
        #endif
        
        AGwLIN_get_string(bioline, 't', BioLookup[i].txt, sizeof(BioLookup[i].txt));
        #ifdef LOG_LEVEL_I2         
          APP_LOG(APP_LOG_LEVEL_INFO, "t='%s'", BioLookup[i].txt);                        
        #endif        
                  
        nbbiolineloaded++; 
      };
    };
  };
  if (nbbiolineloaded<BIO_ARRAY_SIZE) {
    #ifdef LOG_LEVEL_I
    APP_LOG(APP_LOG_LEVEL_ERROR, "Bio NbLine mismatch. Reload from web. Reducing from %i to %i", BIO_ARRAY_SIZE, nbbiolineloaded);
    BIO_ARRAY_SIZE=nbbiolineloaded;
    #endif
  }
  #ifdef LOG_LEVEL_I
  APP_LOG(APP_LOG_LEVEL_INFO, "End of BIO PARSING");
  #endif
};



void store_LINE(int* agindex, int* bioindex, char* line) {
    
  if ((line[0]=='a') && (line[1]=='=')) {
    #ifdef LOG_LEVEL_I2 
      APP_LOG(APP_LOG_LEVEL_INFO, "STORING agenda line index=%i, LINE=%s", *agindex, line);
    #endif
    persist_write_string(KEY_AG_LINES+*agindex, line);
    (*agindex)++;
  }
  else if ((line[0]=='b') && (line[1]=='=')) {
    #ifdef LOG_LEVEL_I2 
      APP_LOG(APP_LOG_LEVEL_INFO, "STORING bio line index=%i, LINE=%s", *bioindex, line);
    #endif
    persist_write_string(KEY_BIO_LINES+*bioindex, line);
    (*bioindex)++;
  }
  else {
    #ifdef LOG_LEVEL_I
    APP_LOG(APP_LOG_LEVEL_INFO, "SKIPPING unknown line LINE=%s", line);
    #endif
  }
  
}


void store_AGwFILE_toPersist(NetDownload *download) {
  #ifdef LOG_LEVEL_I
  APP_LOG(APP_LOG_LEVEL_INFO, "STORING FILE length=%lu", download->length);
  #endif
  
  int aglineindex = 0;
  int biolineindex = 0;  
  
  uint32_t i = 0;
  uint32_t ii = 0;
  static char buffer[256];
  memset(buffer, 0, sizeof(buffer));
  
  while (i<download->length) {    
    if (download->data[i]==';') {
      buffer[ii]='\0';
      #ifdef LOG_LEVEL_I
        APP_LOG(APP_LOG_LEVEL_INFO, "STORING i=%lu, Line=%s", i, buffer);
      #endif
      store_LINE(&aglineindex, &biolineindex, buffer);
      ii = 0;
      buffer[0]='\0';
    }    
    else {
      if ((download->data[i]>=32) && (ii<sizeof(buffer))) {
        buffer[ii]=download->data[i];
        ii++;
      };
    }
    i++; 
  }
  #ifdef LOG_LEVEL_I
  APP_LOG(APP_LOG_LEVEL_INFO, "STORING KEY_AG_NBLINE=%i", aglineindex);    
  #endif
  persist_write_int(KEY_AG_NBLINE, aglineindex);
  
  #ifdef LOG_LEVEL_I
  APP_LOG(APP_LOG_LEVEL_INFO, "STORING KEY_BIO_NBLINE=%i", biolineindex);    
  #endif
  persist_write_int(KEY_BIO_NBLINE, biolineindex);
  
}


//--------------------------------------------------------------------------
// Download handler...
//--------------------------------------------------------------------------

void download_complete_handler(NetDownload *download) {
  #ifdef LOG_LEVEL_I
  APP_LOG(APP_LOG_LEVEL_INFO, "Loaded data %lu bytes", (unsigned long) download->length);
  APP_LOG(APP_LOG_LEVEL_INFO, "Heap free is %lu bytes", (unsigned long) heap_bytes_free());
  #endif
  
  store_AGwFILE_toPersist(download);
  parse_AGLINES_fromPersist();
  parse_BIOLINES_fromPersist();
  
  free(download->data);
    
  download->data = NULL;
  netdownload_destroy(download);
  
  #ifdef LOG_LEVEL_I
  APP_LOG(APP_LOG_LEVEL_INFO, "Refreshing display");
  #endif
  set_Initial_Layers(); // refresh all !
  
}



//--------------------------------------------------------------------------
// PHONE Configuration SYNC
//--------------------------------------------------------------------------

//static void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context) {
//static void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, void *context) {
static void sync_changed_handler(DictionaryIterator *iter, void *context) {
 
      
  bool needrefresh = false;
  bool needwxrefresh = false;  
  bool needdownload = false;
  Tuple *new_tuple = dict_read_first(iter);
  // Update UI here
  // We know these values are uint8 format
  
  
  while (new_tuple) {
    
    int uvalue = new_tuple->value->uint8;
    #ifdef LOG_LEVEL_I  
    switch (new_tuple->type) {
      case TUPLE_CSTRING:
        APP_LOG(APP_LOG_LEVEL_INFO, "Pebble Phone Sync ask change for key=%u, value=%s", (int)new_tuple->key, new_tuple->value->cstring);    
      break;
      default:
        APP_LOG(APP_LOG_LEVEL_INFO, "Pebble Phone Sync ask change for key=%u, value=%li", (int)new_tuple->key,  new_tuple->value->int32);  
    };
    #endif
     

    switch (new_tuple->key) {
      case KEY_LANGUAGE:     
        if ((uvalue >= 0) && (uvalue < language_count) && (config_language != uvalue)) {
          #ifdef LOG_LEVEL_I2 
          APP_LOG(APP_LOG_LEVEL_INFO, "Pebble Setting Language. New Value=%u", uvalue);  
          #endif
          config_language = uvalue;
          persist_write_int(KEY_LANGUAGE, config_language); 
          needrefresh = true;
          
          //  relocate inverter layer
          //GRect rect = layer_get_frame(inverter_layer_get_layer(inverter_layer));
          //rect.origin.x = (screen == screen_black) ? 144 : 0;
          //layer_set_frame(inverter_layer_get_layer(inverter_layer), rect);
        }
        break;
      case KEY_VIBRATE:     
        if ((uvalue >= 0) && (uvalue < vibrate_count) && (config_vibrate != uvalue)) {
          #ifdef LOG_LEVEL_I2
          APP_LOG(APP_LOG_LEVEL_INFO, "Pebble Setting Vibrate. New value=%u", uvalue);
          #endif
          config_vibrate = uvalue;
          persist_write_int(KEY_VIBRATE, config_vibrate);         
          //needrefresh = true;
        }
        break;
      case KEY_AGURL:     
        if (strcmp(new_tuple->value->cstring, config_agurl)!=0)   {
          #ifdef LOG_LEVEL_I2
          APP_LOG(APP_LOG_LEVEL_INFO, "Pebble Setting AgURL. New value=%s", new_tuple->value->cstring);
          #endif
          strcpy(config_agurl, new_tuple->value->cstring);
          persist_write_string(KEY_AGURL, config_agurl);                            
        }         
        needdownload = true;        
        break;
      case KEY_TEMPUNIT:     
        if ((uvalue >= 0) && (uvalue < tempunit_count) && (config_tempunit != uvalue)) {
          #ifdef LOG_LEVEL_I2
          APP_LOG(APP_LOG_LEVEL_INFO, "Pebble Setting TempUnit. New value=%u", uvalue);
          #endif
          config_tempunit = uvalue;
          persist_write_int(KEY_TEMPUNIT, config_tempunit);         
          //needrefresh = true;
        }
        break;
      case KEY_WX_TEMP100X:
        #ifdef LOG_LEVEL_I2
        APP_LOG(APP_LOG_LEVEL_INFO, "WX temp. New=%li", new_tuple->value->int32);
        #endif
        wxdata_temp100x = new_tuple->value->int32; 
        wxdata_timeOfLastResponse = time(NULL);
        needwxrefresh = true;     
        break;
      case KEY_WX_CITYNAME:
        #ifdef LOG_LEVEL_I2
        APP_LOG(APP_LOG_LEVEL_INFO, "WX CityName. New=%s", new_tuple->value->cstring);
        #endif
        strncpy(wxdata_city, new_tuple->value->cstring, sizeof(wxdata_city));
        //needwxrefresh = true;
        break;
      case KEY_WX_HUMIDITY:
        #ifdef LOG_LEVEL_I2
        APP_LOG(APP_LOG_LEVEL_INFO, "WX Humidity. New=%li", new_tuple->value->int32);
        #endif
        wxdata_humidity = new_tuple->value->int32;
        //needwxrefresh = true;
        break;
      case KEY_WX_CLOUDS:
        #ifdef LOG_LEVEL_I2
        APP_LOG(APP_LOG_LEVEL_INFO, "WX Clouds. New=%li", new_tuple->value->int32);
        #endif
        wxdata_clouds = new_tuple->value->int32;
        //needwxrefresh = true;
        break;
      case KEY_WX_CONDITION:
        #ifdef LOG_LEVEL_I2
        APP_LOG(APP_LOG_LEVEL_INFO, "WX Condition. New=%li", new_tuple->value->int32);
        #endif
        wxdata_condition = new_tuple->value->int32;
        //needwxrefresh = true;
        break;
      case KEY_WX_ICON:
        #ifdef LOG_LEVEL_I2
        APP_LOG(APP_LOG_LEVEL_INFO, "WX Icon. New=%s", new_tuple->value->cstring);
        #endif
        strncpy(wxdata_icon, new_tuple->value->cstring, sizeof(wxdata_icon));
        //needwxrefresh = true;
        break;
      case KEY_WX_DAY1_TIME:
        #ifdef LOG_LEVEL_I2
        APP_LOG(APP_LOG_LEVEL_INFO, "WX D1 time. New=%li", new_tuple->value->int32);
        #endif
        wxdata_day1time = new_tuple->value->int32;         
        //needwxrefresh = true;
        break;      
      case KEY_WX_DAY1TEMPMIN:
        #ifdef LOG_LEVEL_I2
        APP_LOG(APP_LOG_LEVEL_INFO, "WX D1 temp min. New=%li", new_tuple->value->int32);
        #endif
        wxdata_day1tempmin = new_tuple->value->int32;         
        //needwxrefresh = true;
        break;
      case KEY_WX_DAY1TEMPMAX:
        #ifdef LOG_LEVEL_I2
        APP_LOG(APP_LOG_LEVEL_INFO, "WX D1 temp max. New=%li", new_tuple->value->int32);
        #endif
        wxdata_day1tempmax = new_tuple->value->int32;         
        //needwxrefresh = true;
        break;
      case KEY_WX_DAY2_TIME:
        #ifdef LOG_LEVEL_I2
        APP_LOG(APP_LOG_LEVEL_INFO, "WX D2 time. New=%li", new_tuple->value->int32);
        #endif
        wxdata_day2time = new_tuple->value->int32;         
        //needwxrefresh = true;
        break;            
      case KEY_WX_DAY2TEMPMIN:
        #ifdef LOG_LEVEL_I2
        APP_LOG(APP_LOG_LEVEL_INFO, "WX D2 temp min. New=%li", new_tuple->value->int32);
        #endif
        wxdata_day2tempmin = new_tuple->value->int32;         
        //needwxrefresh = true;
        break;
      case KEY_WX_DAY2TEMPMAX:
        #ifdef LOG_LEVEL_I2
        APP_LOG(APP_LOG_LEVEL_INFO, "WX D2 temp max. New=%li", new_tuple->value->int32);
        #endif
        wxdata_day2tempmax = new_tuple->value->int32;         
        //needwxrefresh = true;
        break;
      case KEY_WX_H0_CONDITIONS:
        #ifdef LOG_LEVEL_I2
        APP_LOG(APP_LOG_LEVEL_INFO, "WX H0 temp max. New=%li", new_tuple->value->int32);
        #endif
        wxdata_h0conditions = new_tuple->value->int32;         
        //needwxrefresh = true;
        break;   
      case KEY_WX_H3_CONDITIONS:
        #ifdef LOG_LEVEL_I2
        APP_LOG(APP_LOG_LEVEL_INFO, "WX H3 temp max. New=%li", new_tuple->value->int32);
        #endif
        wxdata_h3conditions = new_tuple->value->int32;         
        //needwxrefresh = true;
        break; 
      case KEY_WX_H6_CONDITIONS:
        #ifdef LOG_LEVEL_I2
        APP_LOG(APP_LOG_LEVEL_INFO, "WX H6 temp max. New=%li", new_tuple->value->int32);
        #endif
        wxdata_h6conditions = new_tuple->value->int32;         
        //needwxrefresh = true;
        break;       
      case KEY_WX_H9_CONDITIONS:
        #ifdef LOG_LEVEL_I2
        APP_LOG(APP_LOG_LEVEL_INFO, "WX H9 temp max. New=%li", new_tuple->value->int32);
        #endif
        wxdata_h9conditions = new_tuple->value->int32;         
        //needwxrefresh = true;
        break;     
      case KEY_WX_H12_CONDITIONS:
        #ifdef LOG_LEVEL_I2
        APP_LOG(APP_LOG_LEVEL_INFO, "WX H12 temp max. New=%li", new_tuple->value->int32);
        #endif
        wxdata_h12conditions = new_tuple->value->int32;         
        //needwxrefresh = true;
        break;           
    };
    new_tuple = dict_read_next(iter);
  };
  
  if (needdownload==true) {
    #ifdef LOG_LEVEL_I
      APP_LOG(APP_LOG_LEVEL_INFO, "Requesting netdownload");
    #endif
    netdownload_request(config_agurl);
  };
  
  if (needwxrefresh==true) {
    update_weathertext();
    //vibes_double_pulse();   // Debug
  };
  if (needrefresh==true) set_Initial_Layers();  
}


static void main_window_load(Window *window) {
  
#ifdef PBL_COLOR
  #ifdef LOG_LEVEL_I
  APP_LOG(APP_LOG_LEVEL_INFO, "Pebble is COLOR"); 
  #endif  
#else
  #ifdef LOG_LEVEL_I
  APP_LOG(APP_LOG_LEVEL_INFO, "Pebble is B&W"); 
  #endif
  
#endif
            
  window_set_background_color(window, GColorBlack);  
  Layer *window_layer = window_get_root_layer(window);


 //Initialise screen = 144 x 168
 // GRect(X, Y, W, H)
 
#define Z1_minY  0 
#define Z2_minY 20
#define Z3_minY 70
#define Z4_minY 107
  
 //---------------------------------------------------------------------------
 // ZONE 1
       
  // Jour Date
  text_date_layer = text_layer_create_ext(window_layer, GRect(0+1, 0, 110, 19), GColorWhite, GColorClear,
                                          fonts_get_system_font(FONT_KEY_GOTHIC_18), GTextAlignmentLeft);   
  // Week Number ISO8601
  text_week_layer = text_layer_create_ext(window_layer, GRect(118, 0 , 144-118, 19), GColorWhite, GColorClear,
                                          fonts_get_system_font(FONT_KEY_GOTHIC_18), GTextAlignmentRight);  

  //---------------------------------------------------------------------------
 // ZONE 2  
  // Heure
  text_time_layer = text_layer_create_ext(window_layer, GRect(0, 20, 144, 45), GColorWhite, GColorClear,
                                          fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD),GTextAlignmentCenter);   

  //---------------------------------------------------------------------------
  // ZONE 3
  //

  // Zodiac sign
  text_zodSIGN_layer = text_layer_create_ext(window_layer, GRect(0, Z3_minY, 40, 40), GColorWhite, GColorClear,
                                             fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ZODIAC_28)), GTextAlignmentCenter);  

  // Zodiac text
  text_zodiac_layer = text_layer_create_ext(window_layer, GRect(38, Z3_minY+2, 144-40-42, 35), GColorWhite, GColorClear,
                                            fonts_get_system_font(FONT_KEY_GOTHIC_24), GTextAlignmentLeft);    

  // Moon Phase
  text_zodMOON_layer = text_layer_create_ext(window_layer, GRect(144-40, Z3_minY+5, 40, 35), GColorWhite, GColorClear,
                                             fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MOONPHASE_28)), GTextAlignmentCenter);  

  #define bioW 33
  // Biorythm Name
  text_bioNAME_layer = text_layer_create_ext(window_layer,GRect(0, Z3_minY, bioW*3, 20), GColorWhite, GColorClear,
                                         fonts_get_system_font(FONT_KEY_GOTHIC_18), GTextAlignmentCenter);  

  text_bioPHY_layer = text_layer_create_ext(window_layer, GRect(0, Z3_minY+20, bioW, 17), PHY_COLOR, GColorClear,
                                            fonts_get_system_font(FONT_KEY_GOTHIC_14), GTextAlignmentCenter);     
      
  text_bioEMO_layer = text_layer_create_ext(window_layer, GRect(0+bioW, Z3_minY+20, bioW, 17), EMO_COLOR, GColorClear,
                                            fonts_get_system_font(FONT_KEY_GOTHIC_14), GTextAlignmentCenter);  
  
  text_bioINT_layer = text_layer_create_ext(window_layer, GRect(0+bioW+bioW, Z3_minY+20, bioW, 17), INT_COLOR, GColorClear,
                                            fonts_get_system_font(FONT_KEY_GOTHIC_14), GTextAlignmentCenter);   
  
  biograph_layer = layer_create(GRect(100,Z3_minY+2,44,35));
  layer_set_update_proc(biograph_layer, biograph_layer_update_callback);
  layer_add_child(window_layer, biograph_layer);
  
  // Weather         
#ifdef PBL_PLATFORM_BASALT
  text_wxICON_layer = text_layer_create_ext(window_layer, GRect(0, Z3_minY+6, wxI_w, 23), GColorWhite, GColorClear,
                                            fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_WEATHER_18)), GTextAlignmentCenter);  
#endif
     
  graph_wxRAIN_layer = layer_create(GRect(0+1,Z3_minY+6+23,44,8));
  layer_set_update_proc(graph_wxRAIN_layer, graph_wxRAIN_layer_update_callback);
  layer_add_child(window_layer, graph_wxRAIN_layer);

#ifdef PBL_PLATFORM_BASALT
  text_wxTEMP_layer = text_layer_create_ext(window_layer, GRect(0+wxI_w, Z3_minY, wxT_w, 38), GColorWhite, GColorClear,
                                            fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD), GTextAlignmentCenter);
#else
  text_wxTEMP_layer = text_layer_create_ext(window_layer, GRect(0,       Z3_minY-2, wxT_w, 38), GColorWhite, GColorClear,
                                            fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD), GTextAlignmentCenter);
#endif
  
#ifdef PBL_PLATFORM_BASALT  
  text_wxCITY_layer = text_layer_create_ext(window_layer, GRect(0+wxT_w+wxI_w, Z3_minY, 144-wxT_w-wxI_w, 20), GColorWhite, GColorClear,
                                            fonts_get_system_font(FONT_KEY_GOTHIC_18), GTextAlignmentRight);
    
  text_wxINFO_layer = text_layer_create_ext(window_layer, GRect(0+wxT_w+wxI_w-10, Z3_minY+20, 144-wxT_w-wxI_w+10, 20), GColorWhite, GColorClear,
                                            fonts_get_system_font(FONT_KEY_GOTHIC_14), GTextAlignmentRight);
#else    
  text_wxCITY_layer = text_layer_create_ext(window_layer, GRect(0+wxT_w, Z3_minY,    144-wxT_w, 20), GColorWhite, GColorClear,
                                            fonts_get_system_font(FONT_KEY_GOTHIC_18), GTextAlignmentCenter);
    
  text_wxINFO_layer = text_layer_create_ext(window_layer, GRect(0+wxT_w, Z3_minY+20, 144-wxT_w, 20), GColorWhite, GColorClear,
                                            fonts_get_system_font(FONT_KEY_GOTHIC_14), GTextAlignmentCenter);
#endif
  
  //---------------------------------------------------------------------------
  // ZONE 4
  //
  
  ag_back_layer = text_layer_create(GRect(0,Z4_minY+1,144,168-108));
  text_layer_set_background_color(ag_back_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(ag_back_layer));
  
  
  // Msg
  text_msg_layer = text_layer_create_ext(window_layer, GRect(0, Z4_minY, 144, 22), GColorWhite, GColorClear,
                                         fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GTextAlignmentCenter);
  
  text_msg2_layer = text_layer_create_ext(window_layer, GRect(0, Z4_minY+20, 144, 21), GColorWhite, GColorClear,
                                          fonts_get_system_font(FONT_KEY_GOTHIC_18), GTextAlignmentCenter);
  
  text_msg3_layer = text_layer_create_ext(window_layer, GRect(0, Z4_minY+20+20, 144, 17), GColorWhite, GColorClear,
                                      fonts_get_system_font(FONT_KEY_GOTHIC_14), GTextAlignmentCenter);
  
  line_layer = layer_create(layer_get_bounds(window_layer));
  layer_set_update_proc(line_layer, line_layer_update_callback);
  layer_add_child(window_layer, line_layer);


  // Set initial configuration
  SetLayerMode(ZOD_mode);
  
  if (persist_exists(KEY_LANGUAGE)) { 
    config_language = persist_read_int(KEY_LANGUAGE);    
    #ifdef LOG_LEVEL_I2
    APP_LOG(APP_LOG_LEVEL_INFO, "Setting KEY_LANGUAGE=%u from STORAGE", config_language);  
    #endif      
  } 
  else { 
    config_language = language_FR; 
    #ifdef LOG_LEVEL_I2    
    APP_LOG(APP_LOG_LEVEL_INFO, "No KEY_LANGUAGE. Using default %u", config_language);  
    #endif
  };
  
  if (persist_exists(KEY_VIBRATE)) { 
    config_vibrate = persist_read_int(KEY_VIBRATE);
    #ifdef LOG_LEVEL_I2    
    APP_LOG(APP_LOG_LEVEL_INFO, "Setting KEY_VIBRATE=%u from STORAGE", config_vibrate);         
    #endif
  }
  else { 
    config_vibrate  = vibrate_12AM8PM;
    #ifdef LOG_LEVEL_I2    
    APP_LOG(APP_LOG_LEVEL_INFO, "No KEY_VIBRATE. Using default %u", config_vibrate);               
    #endif
  };
  
  if (persist_exists(KEY_TEMPUNIT)) { 
    config_tempunit = persist_read_int(KEY_TEMPUNIT);
    #ifdef LOG_LEVEL_I2    
    APP_LOG(APP_LOG_LEVEL_INFO, "Setting KEY_TEMPUNIT=%u from STORAGE", config_tempunit);         
    #endif
  }
  else { 
    config_tempunit  = tempunit_CELSIUS;
    #ifdef LOG_LEVEL_I2    
    APP_LOG(APP_LOG_LEVEL_INFO, "No KEY_TEMPUNIT. Using default %u", config_tempunit);               
    #endif
  };
  
  if (persist_exists(KEY_AGURL)) { 
    persist_read_string(KEY_AGURL, config_agurl, sizeof(config_agurl));
    #ifdef LOG_LEVEL_I2    
    APP_LOG(APP_LOG_LEVEL_INFO, "Setting KEY_AGURL=%s from STORAGE", config_agurl);         
    #endif
  }
  else { 
    strcpy(config_agurl, "");
    #ifdef LOG_LEVEL_I2    
    APP_LOG(APP_LOG_LEVEL_INFO, "No KEY_AGURL. Using default %s", config_agurl);         
    #endif
  };
  

  if (persist_exists(KEY_AG_NBLINE)) {
    #ifdef LOG_LEVEL_I2    
    APP_LOG(APP_LOG_LEVEL_INFO, "AG_NBLINES Exists... trying to parse"); 
    #endif
    parse_AGLINES_fromPersist(); 
  };    
  if (persist_exists(KEY_BIO_NBLINE)) {
    #ifdef LOG_LEVEL_I2    
    APP_LOG(APP_LOG_LEVEL_INFO, "BIO_NBLINES Exists... trying to parse"); 
    #endif
    parse_BIOLINES_fromPersist(); 
  };    
    //Set initial time so display isn't blank
  set_Initial_Layers();
}



static void main_window_unload(Window *window) {
  #ifdef LOG_LEVEL_I    
  APP_LOG(APP_LOG_LEVEL_INFO, "Unsubscribing services");
  #endif  
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  
  //layer
  #ifdef LOG_LEVEL_I
  APP_LOG(APP_LOG_LEVEL_INFO, "Destroying layers");  
  #endif  
  layer_destroy(line_layer);  
  layer_destroy(biograph_layer) ;
  layer_destroy(graph_wxRAIN_layer);
  
  text_layer_destroy(text_msg3_layer);
  text_layer_destroy(text_msg2_layer);
  text_layer_destroy(text_msg_layer);
  
  text_layer_destroy(ag_back_layer);
    
  text_layer_destroy(text_wxINFO_layer);
#ifdef PBL_PLATFORM_BASALT
  text_layer_destroy(text_wxICON_layer);
#endif
  text_layer_destroy(text_wxCITY_layer);
  text_layer_destroy(text_wxTEMP_layer);
  
  text_layer_destroy(text_bioNAME_layer);
  text_layer_destroy(text_bioINT_layer);
  text_layer_destroy(text_bioPHY_layer);  
  text_layer_destroy(text_bioEMO_layer);    
  
  text_layer_destroy(text_zodMOON_layer);
  text_layer_destroy(text_zodiac_layer);
  text_layer_destroy(text_zodSIGN_layer);
  
  text_layer_destroy(text_time_layer);
  
  text_layer_destroy(text_week_layer);
  text_layer_destroy(text_date_layer);
  
  #ifdef LOG_LEVEL_I    
  APP_LOG(APP_LOG_LEVEL_INFO, "Destroying AgendaLookup");
  #endif
  AgendaLookup_free(); 
  BioLookup_free();
}

//--------------------------------------------------------------------------
static void handle_init() {
//--------------------------------------------------------------------------
  #ifdef LOG_LEVEL_I 
  APP_LOG(APP_LOG_LEVEL_INFO, "AGwatch Starting %s", AGwVersion);     
  #endif
  // Need to initialize this first to make sure it is there when
  // the window_load function is called by window_stack_push.
  
  netdownload_initialize(download_complete_handler, sync_changed_handler);
  
  // Create main window
  s_main_window = window_create();
  
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  window_stack_push(s_main_window, true /* Animated */);
  
  #ifdef LOG_LEVEL_I 
  APP_LOG(APP_LOG_LEVEL_INFO, "Subscribing services");  
  #endif
  battery_state_service_subscribe(handle_battery);
  bluetooth_connection_service_subscribe(handle_bluetooth);
  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick); 
}
  

//--------------------------------------------------------------------------  
static void handle_deinit() {
//--------------------------------------------------------------------------  
  
  #ifdef LOG_LEVEL_I 
  APP_LOG(APP_LOG_LEVEL_INFO, "Destroying netdownload");    
  #endif
  netdownload_deinitialize(); // call this to avoid 20B memory leak

  window_destroy(s_main_window); 
  #ifdef LOG_LEVEL_I 
  APP_LOG(APP_LOG_LEVEL_INFO, "AGwatch End");
  #endif
}


int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
