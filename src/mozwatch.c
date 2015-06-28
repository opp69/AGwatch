#include <pebble.h>

//#define LOG_LEVEL_I  
//#define LOG_LEVEL_I2
  
  
 /*#define TupletStaticCString(_key, _cstring, length) \
  ((const Tuplet) { .type = TUPLE_CSTRING, .key = _key, .cstring = { .data = _cstring, .length = length + 1 }})

#define MyTupletCString(_key, _cstring) \
  ((const Tuplet) { .type = TUPLE_CSTRING, .key = _key, .cstring = { .data = _cstring, .length = strlen(_cstring) + 1 }})
*/
  
/*================================================================*/
#define AGwVersion  "v4.3"
/*================================================================*/
  
  
#include "xconfig.h"
#include "netdownload.h"

Window *s_main_window;

int firstrun = 1; // Variable to check for first run. 1 - YES. 0 - NO.

TextLayer *text_date_layer;     // zone 1
TextLayer *text_week_layer;

TextLayer *text_time_layer;			// zone 2 de texte où l'heure sera affichée

TextLayer *text_z_layer;        // zone 3
TextLayer *text_moon_layer;
TextLayer *text_zodiac_layer;

TextLayer *text_bioname_layer;
TextLayer *text_bioPHY_layer;
TextLayer *text_bioEMO_layer;
TextLayer *text_bioINT_layer;

TextLayer *text_msg_layer;      // zone 4
TextLayer *text_msg2_layer;
TextLayer *text_msg3_layer;

Layer *line_layer;
Layer *biograph_layer;

#ifdef PBL_COLOR
  #define PHY_COLOR GColorMelon  
  #define EMO_COLOR GColorCeleste  
  #define INT_COLOR GColorMintGreen      
  #define BACK_ALLDAY_COLOR GColorBlack
  #define BACK_7DAY_COLOR GColorOxfordBlue
  #define BACK_1DAY_COLOR GColorBulgarianRose
#else
  #define PHY_COLOR GColorWhite  
  #define INT_COLOR GColorWhite    
  #define EMO_COLOR GColorWhite      
  #define BACK_ALLDAY_COLOR GColorBlack
  #define BACK_7DAY_COLOR GColorBlack
  #define BACK_1DAY_COLOR GColorBlack  
#endif


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

enum KEY_Settings { KEY_LANGUAGE = 1, KEY_VIBRATE, KEY_AGURL };
static enum SettingLanguage { language_FR = 0, language_US, language_count } config_language;
static enum SettingVibrate  { vibrate_NONE = 0, vibrate_12AM8PM, vibrate_8AMTO8PM, vibrate_ALWAYS, vibrate_count } config_vibrate;
static char config_agurl[128];

static enum LayersMode { ZOD_mode = 1, BIO_mode} layers_mode;

int bio_deltaJ = 0;

// PERSISTANT KEY TO STORE THE AGLINES
#define KEY_AG_NBLINE   999
#define KEY_AG_LINES    KEY_AG_NBLINE+1

#define KEY_BIO_NBLINE   1999
#define KEY_BIO_LINES    KEY_BIO_NBLINE+1

  

//static enum SettingTimeFormat { timeformat_12 = 0, timeformat_24, timeformat_count } config_timeformat;
//static AppSync s_sync;
//static uint8_t s_sync_buffer[256];

static BatteryChargeState myBatState;
static bool myBTState;    // unknown


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
  
 //Initialise screen = 144 x 168
void line_layer_update_callback(Layer *me, GContext* ctx) {

  #ifdef LOG_LEVEL_I
  APP_LOG(APP_LOG_LEVEL_INFO, "Line Layer update");
  #endif
  
#ifdef PBL_COLOR
  graphics_context_set_stroke_color(ctx, GColorChromeYellow);
#else
  graphics_context_set_stroke_color(ctx, GColorWhite);
#endif
  

 graphics_draw_line(ctx, GPoint(6, 22), GPoint(144-6, 22));
 graphics_draw_line(ctx, GPoint(6, 107), GPoint(144-6, 107));  
//  graphics_draw_line(ctx, GPoint(6, 59), GPoint(132, 59));

#ifdef PBL_COLOR
  graphics_context_set_stroke_color(ctx, GColorRajah);
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
  
  //_mode = BIO_mode;
  if (_mode == BIO_mode) {
    layer_set_hidden(text_layer_get_layer(text_zodiac_layer),   true);
    layer_set_hidden(text_layer_get_layer(text_z_layer),   true);    
    layer_set_hidden(text_layer_get_layer(text_moon_layer),   true);        
    layer_set_hidden(text_layer_get_layer(text_bioname_layer), false); 
    layer_set_hidden(text_layer_get_layer(text_bioINT_layer), false);     
    layer_set_hidden(text_layer_get_layer(text_bioEMO_layer), false);         
    layer_set_hidden(text_layer_get_layer(text_bioPHY_layer), false);           
    layer_set_hidden(biograph_layer, false);               
  }
  else if (_mode == ZOD_mode) {
    layer_set_hidden(text_layer_get_layer(text_zodiac_layer),   false);
    layer_set_hidden(text_layer_get_layer(text_z_layer),   false);    
    layer_set_hidden(text_layer_get_layer(text_moon_layer),   false);        
    layer_set_hidden(text_layer_get_layer(text_bioname_layer), true);
    layer_set_hidden(text_layer_get_layer(text_bioINT_layer), true);     
    layer_set_hidden(text_layer_get_layer(text_bioEMO_layer), true);         
    layer_set_hidden(text_layer_get_layer(text_bioPHY_layer), true);     
    layer_set_hidden(biograph_layer, true);                 
  };  
  
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

    //Set the TextLayer text
    text_layer_set_text(text_time_layer, time_text); 
    
    if (BIO_ARRAY_SIZE>0) {
      if ((t->tm_min % 2)==0) display_layers_mode(BIO_mode); else display_layers_mode(ZOD_mode);
    };
    
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
        if (t->tm_hour >= 8 || t->tm_hour <= 20) vibes_double_pulse();
        break;        
    }
  };
  

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
    text_layer_set_text(text_z_layer, zodiac_sign);
  
    if (config_language == language_FR) {
      strcpy(zodiac_text, zodiac_fr[m]); 
    } 
    else {
      strcpy(zodiac_text, zodiac_us[m]);
    }; 
    text_layer_set_text(text_zodiac_layer, zodiac_text);


    // MOON
    moon_sign[0] = get_moon_phase_char(t);    
    text_layer_set_text(text_moon_layer, moon_sign);
    
    // COMPUTE JULIAN DAY  for TODAY
    
    
    // BIO
    
    if (BIO_ARRAY_SIZE>0) {
      strncpy(bio_text, BioLookup[0].txt, sizeof(bio_text));
    
      struct tm bio_day;
      bio_day.tm_mday = BioLookup[0].d;  
      bio_day.tm_mon = BioLookup[0].m-1;
      bio_day.tm_year = BioLookup[0].y;
            
      bio_deltaJ = tm2jd(t)-tm2jd(&bio_day);
      
#ifdef LOG_LEVEL_I
      APP_LOG(APP_LOG_LEVEL_INFO, "jbio %i",  tm2jd(&bio_day)); 
      APP_LOG(APP_LOG_LEVEL_INFO, "today %i",  tm2jd(t));       
      APP_LOG(APP_LOG_LEVEL_INFO, "JOUR %i",  BioLookup[0].d); 
      APP_LOG(APP_LOG_LEVEL_INFO, "MOIS %i",  BioLookup[0].m);       
      APP_LOG(APP_LOG_LEVEL_INFO, "ANNEE %i",  BioLookup[0].y);             
      APP_LOG(APP_LOG_LEVEL_INFO, "BIO DELTA %i", bio_deltaJ); 
#endif
      
      text_layer_set_text(text_bioname_layer, bio_text);
   
      set_biolayer(text_bioPHY_layer, 'P', bioPHY_text, get_biorythm(bio_deltaJ, 23), PHY_COLOR);            
      set_biolayer(text_bioEMO_layer, 'E', bioEMO_text, get_biorythm(bio_deltaJ, 28), EMO_COLOR);
      set_biolayer(text_bioINT_layer, 'I', bioINT_text, get_biorythm(bio_deltaJ, 33), INT_COLOR);              
    };
    
   
    
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
    struct tm aTm;
//    PblTm today;


    int jtoday    = tm2jd(t);

    // curday tested   
    struct tm curdayTm   = *t;
    int jcurdayTm = jtoday;

    int const NB_TEXTMSG = 3;
    int afound  = 0;
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
            if (jdiff==0) {
              window_set_background_color(s_main_window, BACK_1DAY_COLOR);
            }
            else if (jdiff < 8) {
              window_set_background_color(s_main_window, BACK_7DAY_COLOR); 
            }            
            else {
              window_set_background_color(s_main_window, BACK_ALLDAY_COLOR); 
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
              switch(config_language) {
                case language_FR:snprintf(buf, 20, AgendaLookup[i].txt, NoceLookup_fr[year_delta]); break;
                default         :snprintf(buf, 20, AgendaLookup[i].txt, NoceLookup_us[year_delta]); 
              }
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

    if (afound < 1) { strcpy(msg_text, " -=- "); };
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
  APP_LOG(APP_LOG_LEVEL_INFO, "STORING FILE");
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
static void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, void *context) {

  // Update UI here
  // We know these values are uint8 format
    
  int uvalue = new_tuple->value->uint8;
  #ifdef LOG_LEVEL_I  
  switch (new_tuple->type) {
    case TUPLE_CSTRING:
      APP_LOG(APP_LOG_LEVEL_INFO, "Pebble Phone Sync ask change for key=%u, value=%s", (int)key, new_tuple->value->cstring);    
    break;
    default:
      APP_LOG(APP_LOG_LEVEL_INFO, "Pebble Phone Sync ask change for key=%u, value=%u", (int)key, uvalue);  
  };
  #endif
   
  
  bool needrefresh = false;
  switch (key) {
    case KEY_LANGUAGE:     
      if ((uvalue >= 0) && (uvalue < language_count) && (config_language != uvalue)) {
        #ifdef LOG_LEVEL_I 
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
        #ifdef LOG_LEVEL_I
        APP_LOG(APP_LOG_LEVEL_INFO, "Pebble Setting Vibrate. New value=%u", uvalue);
        #endif
        config_vibrate = uvalue;
        persist_write_int(KEY_VIBRATE, config_vibrate);         
        //needrefresh = true;
      }
      break;
    case KEY_AGURL:     
      if (strcmp(new_tuple->value->cstring, config_agurl)!=0)   {
        #ifdef LOG_LEVEL_I
        APP_LOG(APP_LOG_LEVEL_INFO, "Pebble Setting AgURL. New value=%s", new_tuple->value->cstring);
        #endif
        strcpy(config_agurl, new_tuple->value->cstring);
        persist_write_string(KEY_AGURL, config_agurl);                            
      }         
      netdownload_request(config_agurl);
      break; 
  };
  if (needrefresh==true) set_Initial_Layers();
}


static void main_window_load(Window *window) {
  
#ifdef PBL_COLOR
  #ifdef LOG_LEVEL_I
  APP_LOG(APP_LOG_LEVEL_INFO, "Pebble is COLOR"); 
  #endif
  //window_set_background_color(window, GColorOxfordBlue);
  window_set_background_color(window, GColorBlack);
#else
  #ifdef LOG_LEVEL_I
  APP_LOG(APP_LOG_LEVEL_INFO, "Pebble is B&W"); 
  #endif
  window_set_background_color(window, GColorBlack);
#endif
    
  Layer *window_layer = window_get_root_layer(window);


  //Initialise screen = 144 x 168
  // GRect(X, Y, W, H)

  // Jour Date
  text_date_layer = text_layer_create(GRect(0+1, 0, 110, 19));
  text_layer_set_text_color(text_date_layer, GColorWhite);
  text_layer_set_background_color(text_date_layer, GColorClear);
  text_layer_set_font(text_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  layer_add_child(window_layer, text_layer_get_layer(text_date_layer));

  // Week Number ISO8601
  text_week_layer = text_layer_create(GRect(118, 0 , 144-118, 19));
  text_layer_set_text_color(text_week_layer, GColorWhite);
  text_layer_set_background_color(text_week_layer, GColorClear);
  text_layer_set_font(text_week_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(text_week_layer, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(text_week_layer));

  // Heure
  text_time_layer = text_layer_create(GRect(0, 20, 144, 45));
  text_layer_set_background_color(text_time_layer, GColorClear);
  text_layer_set_text_color(text_time_layer, GColorWhite);
  text_layer_set_font(text_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(text_time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_time_layer));

  //---------------------------------------------------------------------------
  // ZONE 3
  //
  
  // Zodiac sign
  text_z_layer = text_layer_create(GRect(0, 70, 40, 40));
  text_layer_set_text_color      (text_z_layer, GColorWhite);
  text_layer_set_background_color(text_z_layer, GColorClear);
  text_layer_set_font(text_z_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ZODIAC_28)));
  text_layer_set_text_alignment(text_z_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_z_layer));

  // Zodiac text
  text_zodiac_layer = text_layer_create(GRect(38, 72, 144-40-42, 35));
  text_layer_set_text_color(text_zodiac_layer, GColorWhite);
  text_layer_set_background_color(text_zodiac_layer, GColorClear);
  text_layer_set_font(text_zodiac_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24) );
  text_layer_set_text_alignment(text_zodiac_layer, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(text_zodiac_layer));

  // Moon Phase
  text_moon_layer = text_layer_create(GRect(144-40, 75, 40, 35));
  text_layer_set_text_color      (text_moon_layer, GColorWhite);
  text_layer_set_background_color(text_moon_layer, GColorClear);
  text_layer_set_font(text_moon_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MOONPHASE_28)));
  text_layer_set_text_alignment(text_moon_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_moon_layer));

  #define bioW 33
  // Biorythm Name
  text_bioname_layer = text_layer_create(GRect(0, 70, bioW*3, 20));
  text_layer_set_text_color      (text_bioname_layer, GColorWhite);
  text_layer_set_background_color(text_bioname_layer, GColorClear);
  text_layer_set_font(text_bioname_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18) );
  text_layer_set_text_alignment(text_bioname_layer, GTextAlignmentCenter);
  layer_set_hidden(text_layer_get_layer(text_bioname_layer), true);
  layer_add_child(window_layer, text_layer_get_layer(text_bioname_layer));  

  text_bioPHY_layer = text_layer_create(GRect(0, 90, bioW, 17));
  text_layer_set_text_color      (text_bioPHY_layer, PHY_COLOR);//bleu
  text_layer_set_background_color(text_bioPHY_layer, GColorClear);  
  text_layer_set_font(text_bioPHY_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14) );
  text_layer_set_text_alignment(text_bioPHY_layer, GTextAlignmentCenter);
  layer_set_hidden(text_layer_get_layer(text_bioPHY_layer), true);
  layer_add_child(window_layer, text_layer_get_layer(text_bioPHY_layer));  
   
    
  text_bioEMO_layer = text_layer_create(GRect(0+bioW, 90, bioW, 17));  
  text_layer_set_text_color      (text_bioEMO_layer, EMO_COLOR);     
  text_layer_set_background_color(text_bioEMO_layer, GColorClear);
  text_layer_set_font(text_bioEMO_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14) );
  text_layer_set_text_alignment(text_bioEMO_layer, GTextAlignmentCenter);
  layer_set_hidden(text_layer_get_layer(text_bioEMO_layer), true);
  layer_add_child(window_layer, text_layer_get_layer(text_bioEMO_layer));  
  
  text_bioINT_layer = text_layer_create(GRect(0+bioW+bioW, 90, bioW, 17));  
  text_layer_set_text_color      (text_bioINT_layer, INT_COLOR);
  text_layer_set_background_color(text_bioINT_layer, GColorClear);
  text_layer_set_font(text_bioINT_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14) );
  text_layer_set_text_alignment(text_bioINT_layer, GTextAlignmentCenter);
  layer_set_hidden(text_layer_get_layer(text_bioINT_layer), true);
  layer_add_child(window_layer, text_layer_get_layer(text_bioINT_layer));  
  
  biograph_layer = layer_create(GRect(100,72,44,35));
  layer_set_hidden(biograph_layer, true);
  layer_set_update_proc(biograph_layer, biograph_layer_update_callback);
  layer_add_child(window_layer, biograph_layer);
  
  
  
  //---------------------------------------------------------------------------
  // ZONE 4
  //

  // Msg
  text_msg_layer = text_layer_create(GRect(0, 107, 144, 22));
  text_layer_set_font(text_msg_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(text_msg_layer, GTextAlignmentCenter);
  text_layer_set_text_color(text_msg_layer, GColorWhite);
  text_layer_set_background_color(text_msg_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(text_msg_layer));

  text_msg2_layer = text_layer_create(GRect(0, 107+20, 144, 21));
  text_layer_set_font(text_msg2_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(text_msg2_layer, GTextAlignmentCenter);
  text_layer_set_text_color(text_msg2_layer, GColorWhite);
  text_layer_set_background_color(text_msg2_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(text_msg2_layer));


  text_msg3_layer = text_layer_create(GRect(0, 107+20+20, 144, 17));		//152  168
  text_layer_set_font(text_msg3_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(text_msg3_layer, GTextAlignmentCenter);
  text_layer_set_text_color(text_msg3_layer, GColorWhite);
  text_layer_set_background_color(text_msg3_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(text_msg3_layer)); 

  line_layer = layer_create(layer_get_bounds(window_layer));
  layer_set_update_proc(line_layer, line_layer_update_callback);
  layer_add_child(window_layer, line_layer);


  // Set initial configuration
  layers_mode = ZOD_mode;
  
  if (persist_exists(KEY_LANGUAGE)) { 
    config_language = persist_read_int(KEY_LANGUAGE);    
    #ifdef LOG_LEVEL_I
    APP_LOG(APP_LOG_LEVEL_INFO, "Setting KEY_LANGUAGE=%u from STORAGE", config_language);  
    #endif      
  } 
  else { 
    config_language = language_FR; 
    #ifdef LOG_LEVEL_I    
    APP_LOG(APP_LOG_LEVEL_INFO, "No KEY_LANGUAGE. Using default %u", config_language);  
    #endif
  };
  
  if (persist_exists(KEY_VIBRATE)) { 
    config_vibrate = persist_read_int(KEY_VIBRATE);
    #ifdef LOG_LEVEL_I    
    APP_LOG(APP_LOG_LEVEL_INFO, "Setting KEY_VIBRATE=%u from STORAGE", config_vibrate);         
    #endif
  }
  else { 
    config_vibrate  = vibrate_12AM8PM;
    #ifdef LOG_LEVEL_I    
    APP_LOG(APP_LOG_LEVEL_INFO, "No KEY_VIBRATE. Using default %u", config_vibrate);               
    #endif
  };
  
  if (persist_exists(KEY_AGURL)) { 
    persist_read_string(KEY_AGURL, config_agurl, sizeof(config_agurl));
    #ifdef LOG_LEVEL_I    
    APP_LOG(APP_LOG_LEVEL_INFO, "Setting KEY_AGURL=%s from STORAGE", config_agurl);         
    #endif
  }
  else { 
    strcpy(config_agurl, "");
    #ifdef LOG_LEVEL_I    
    APP_LOG(APP_LOG_LEVEL_INFO, "No KEY_AGURL. Using default %s", config_agurl);         
    #endif
  };
  

  if (persist_exists(KEY_AG_NBLINE)) {
    #ifdef LOG_LEVEL_I    
    APP_LOG(APP_LOG_LEVEL_INFO, "AG_NBLINES Exists... trying to parse"); 
    #endif
    parse_AGLINES_fromPersist(); 
  };    
  if (persist_exists(KEY_BIO_NBLINE)) {
    #ifdef LOG_LEVEL_I    
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
  text_layer_destroy(text_msg3_layer);
  text_layer_destroy(text_msg2_layer);
  text_layer_destroy(text_msg_layer);
  
  text_layer_destroy(text_bioname_layer);
  text_layer_destroy(text_bioINT_layer);
  text_layer_destroy(text_bioPHY_layer);  
  text_layer_destroy(text_bioEMO_layer);    
  
  text_layer_destroy(text_moon_layer);
  text_layer_destroy(text_zodiac_layer);
  text_layer_destroy(text_z_layer);
  
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
