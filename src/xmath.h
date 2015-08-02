
static const char *zodiac_fr[] = {"Capricorne","Verseau","Poissons","Bélier","Taureau","Gémeaux","Cancer","Lion","Vierge",
                               "Balance","Scorpion","Sagittaire"};
static const char *zodiac_us[] = {"Capricorn","Aquarius","Pisces","Aries","Taurus","Gemini","Cancer","Leo","Virgo",
                               "Libra","Scorpio","Sagittarius"};

static const char *z_sign[] = {"J","K","L","A","B","C","D","E","F","G","H","I"}; // Zodiac sign for Zodiac fonts starting in January
static const int z_day[] = {20,20,21,20,21,21,23,24,23,23,22,22}; // Day of Zodiac change


#define NOCE_ARRAY_SIZE 70

/* 75 ans de mariage : les noces d’albâtre,
80 ans de mariage : les noces de chêne,
85 ans de mariage : les noces d’uranium,
90 ans de mariage : les noces de granite,
100 ans de mariage : les noces d’eau.
*/


void get_noce(enum SettingLanguage _lng, int _year, char *_buf) {
  ResHandle rh;
  switch (_lng) {
    case language_FR:
      rh = resource_get_handle(RESOURCE_ID_NOCES_FR);
      break;
    default:
      rh = resource_get_handle(RESOURCE_ID_NOCES_US);    
  }
  resource_load_byte_range(rh, _year*32, (uint8_t*) _buf, 32-2);
}



static const int nbDaysM[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };


int floor100(int _n) {
  if (_n>=0) return _n /100;      // 101 --> 1
  else       return (_n+99)/100;  // -101 --> -2
}

int ceil100(int _n) {
  if (_n>=0) return (_n+99) /100;  // 101 --> 2
  else       return _n/100;  // -101 --> -1
}

int round100(int _n) {
  if (_n>=0) return (_n+50) /100;  // 101 --> 1
  else       return (_n-50)/100;  // -101 --> -1
}


int kelvin100toTU(int _t, enum SettingTempUnit _unit) { 
  
  //APP_LOG(APP_LOG_LEVEL_INFO, "kelvin100toTU", uvalue);
  
  switch (_unit) {
    case tempunit_CELSIUS:
      return (_t - 27315);
      break;
    default:
      return ((_t - 27315)*1.8+3200);
  }
}



int get_biorythm(int _bio_deltaJ, int _period)
{
  int32_t angle = (_bio_deltaJ % _period) *TRIG_MAX_ANGLE/_period;
  int bval = sin_lookup(angle) * 100 / TRIG_MAX_RATIO;
  //if (bval>99) bval=99;
  //if (bval<-99) bval=-99;
  return bval;
}




//return the zodiac
int get_zodiac_int(struct tm *t)
{
  int month = t->tm_mon;
  int day   = t->tm_mday;

  int m = (day < z_day[month]) ? month : month + 1;
  if (m == 12) m = 0;
  return m;
}


bool isLeapYear(const int _Y) {
  int Y = _Y;
  if (Y<1000) Y=Y+1900;
  return ( ((Y%4 == 0) && (Y%100 != 0)) || (Y%400 == 0) );
}

int nbDaysInMonth(const int _M, const int _Y) {
  return nbDaysM[_M]+ (_M == 1)*isLeapYear(_Y);   // FEB = 1
}

//return julian day number for time
// http://aa.usno.navy.mil/faq/docs/JD_Formula.php
int tm2jd(struct tm *t)
{
    int y,m;
    y = t->tm_year + 1900;
    m = t->tm_mon + 1;
    return t->tm_mday-32075+1461*(y+4800+(m-14)/12)/4+367*(m-2-(m-14)/12*12)/12-3*((y+4900+(m-14)/12)/100)/4;
}


static void TmAddDays(struct tm *_t, int _numDays) {
  int i;
  for (i=0; i<_numDays; i++) {
    if (_t->tm_mday == nbDaysInMonth(_t->tm_mon, _t->tm_year)) {
     if (_t->tm_mon == 11) 
       { _t->tm_year++; _t->tm_mon = 0; _t->tm_mday = 1; } 
       else { _t->tm_mon++; _t->tm_mday = 1; }    
    } 
    else 
    { _t->tm_mday++; }
    _t->tm_wday = (_t->tm_wday+1) % 7;
  };
};


int moon_phase(int jdn)
{
    double jd;
    jd = jdn-2451550.1;
    jd /= 29.530588853;
    jd -= (int)jd;
    return (int)(jd*27 + 0.5); /* scale fraction from 0-27 and round by adding 0.5 */
}  


char get_moon_phase_char(struct tm *t)
{    
    // moon
	//static char MoonPhaseCharLookup[] = "1abcdefghijklm0nopqrstuvwxyz";
	static char MoonPhaseCharLookup[] =   "1NOPQRSTUVWXYZ0ABCDEFGHIJKLM";
	  	
  int moonphase_number = moon_phase(tm2jd(t));
    
	// correct for southern hemisphere	
  if ((moonphase_number > 0)  && (LATITUDE < 0))
    moonphase_number = 28 - moonphase_number;
    
	if ((moonphase_number<0) && (moonphase_number>27)) {
	  return '\0';
	}
	else {
	  return MoonPhaseCharLookup[moonphase_number];	
    }
}


int char2wday(char _c, int _wd)
{
  switch(_c) {
    case 'm':
    case 'L': return 1; break;
    case 'u':
    case 'A': return 2; break;
    case 'w':     
    case 'E': return 3; break;
    case 't':
    case 'J': return 4; break;
    case 'f':    
    case 'V': return 5; break;
    case 'a':        
    case 'S': return 6; break;
    case 's':            
    case 'D': return 0; break;
    default: return _wd; break;
  }
}


bool isInWeek(struct tm *_t, char _Week)
{
  switch (_Week) {
    case '1': if (((_t->tm_mday-1) / 7) == 0) { return true;} else {return false;}; break;
    case '2': if (((_t->tm_mday-1) / 7) == 1) { return true;} else {return false;}; break;
    case '3': if (((_t->tm_mday-1) / 7) == 2) { return true;} else {return false;}; break;
    case '4': if (((_t->tm_mday-1) / 7) == 3) { return true;} else {return false;}; break;
    case '5': if (((_t->tm_mday-1) / 7) == 4) { return true;} else {return false;}; break;
    case '9': if ((_t->tm_mday+7) > nbDaysInMonth(_t->tm_mon, _t->tm_year)) { return true;} else {return false;}; break;
    default: return true;
  }; 
}




