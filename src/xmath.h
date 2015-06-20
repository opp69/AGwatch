
static const char *zodiac_fr[] = {"Capricorne","Verseau","Poissons","Bélier","Taureau","Gémeaux","Cancer","Lion","Vierge",
                               "Balance","Scorpion","Sagittaire"};
static const char *zodiac_us[] = {"Capricorn","Aquarius","Pisces","Aries","Taurus","Gemini","Cancer","Leo","Virgo",
                               "Libra","Scorpio","Sagittarius"};

static const char *z_sign[] = {"J","K","L","A","B","C","D","E","F","G","H","I"}; // Zodiac sign for Zodiac fonts starting in January
static const int z_day[] = {20,20,21,20,21,21,23,24,23,23,22,22}; // Day of Zodiac change


#define NOCE_ARRAY_SIZE 70
static const char *NoceLookup_fr[NOCE_ARRAY_SIZE] =
{
  "néant", // 0,
  "coton", // pour le premier anniversaire de mariage,
  "cuir", //  pour 2 ans de mariage,
  "froment", //  pour 3 ans de mariage,
  "cire", //  pour 4 ans de mariage,
  "bois", //  pour 5 ans de mariage,
  "chypre", //  pour 6 ans de mariage,
  "laine", //  pour 7 ans de mariage,
  "coquelicot", //  pour 8 ans de mariage,
  "faïence", //  pour 9 ans de mariage,
  "étain", //  pour 10 ans de mariage,
  "corail", //  pour 11 ans de mariage,
  "soie", //  pour 12 ans de mariage,
  "muguet", //  pour 13 ans de mariage,
  "plomb", //  pour 14 ans de mariage,
  "cristal", //  pour 15 ans de mariage,
  "saphir", //  pour 16 ans de mariage,
  "rose", //  pour 17 ans de mariage,
  "turquoise", //  pour 18 ans de mariage,
  "cretonne", //  pour 19 ans de mariage,
  "porcelaine", //  pour 20 ans de mariage,
  "opale", //  pour 21 ans de mariage,
  "bronze", //  pour 22 ans de mariage,
  "béryl", //  pour 23 ans de mariage,
  "satin", //  pour 24 ans de mariage,
  "argent", //  pour 25 ans de mariage,
  "jade", //  pour 26 ans de mariage,
  "acajou", //  pour 27 ans de mariage,
  "nickel", //  pour 28 ans de mariage,
  "velours", //  pour 29 ans de mariage,
  "perle", //  pour 30 ans de mariage,
  "basane", //  pour 31 ans de mariage,
  "cuivre", //  pour 32 ans de mariage,
  "porphyre", // pour 33 ans de mariage,
  "ambre", // pour 34 ans de mariage,
  "rubis", // pour 35 ans de mariage,
  "mousseline", // pour 36 ans de mariage,
  "papier", // pour 37 ans de mariage,
  "mercure", // pour 38 ans de mariage,
  "crêpe", // pour 39 ans de mariage,
  "émeraude", // pour 40 ans de mariage.
  "fer", //41 ans de mariage : 
  "nacre", //42 ans de mariage
  "flanelle", //43 ans de mariage
  "topaze", //44 ans de mariage
  "platine", //45 ans de mariage
  "lavande", //46 ans de mariage
  "cachemire", //47 ans de mariage
  "améthyste", //48 ans de mariage
  "cèdre", //49 ans de mariage
  "or", //50 ans de mariage
  "camélia", //51 ans de mariage
  "tourmaline", //52 ans de mariage
  "merisier", //53 ans de mariage
  "zibeline", //54 ans de mariage
  "d’orchidée", //55 ans de mariage
  "lapis-lazuli", //56 ans de mariage
  "azalée", //57 ans de mariage
  "érable", //58 ans de mariage
  "vison", //59 ans de mariage
  "diamant", //60 ans de mariage
  "platane", //61 ans de mariage
  "ivoire", //62 ans de mariage
  "lilas", //63 ans de mariage
  "astrakan," //64 ans de mariage
  "palissandre", //65 ans de mariage
  "jasmin", //66 ans de mariage
  "chinchilla", //67 ans de mariage
  "granite", //68 ans de mariage
  "mélèze", //69 ans de mariage
  "platine" //70 ans de mariage
};

/* 75 ans de mariage : les noces d’albâtre,
80 ans de mariage : les noces de chêne,
85 ans de mariage : les noces d’uranium,
90 ans de mariage : les noces de granite,
100 ans de mariage : les noces d’eau.
*/

static const char *NoceLookup_us[NOCE_ARRAY_SIZE] =
{
  "none", // 0,
  "paper", // pour le premier anniversaire de mariage,
  "cotton", //  pour 2 ans de mariage,
  "leather", //  pour 3 ans de mariage,
  "silk", //  pour 4 ans de mariage,
  "wood", //  pour 5 ans de mariage,
  "iron", //  pour 6 ans de mariage,
  "wool", //  pour 7 ans de mariage,
  "bronze", //  pour 8 ans de mariage,
  "pottery", //  pour 9 ans de mariage,
  "tin", //  pour 10 ans de mariage,
  "steel", //  pour 11 ans de mariage,
  "home decor", //  pour 12 ans de mariage,
  "lace", //  pour 13 ans de mariage,
  "ivory", //  pour 14 ans de mariage,
  "crystal", //  pour 15 ans de mariage,
  "16th", //  pour 16 ans de mariage,
  "17th", //  pour 17 ans de mariage,
  "18th", //  pour 18 ans de mariage,
  "19th", //  pour 19 ans de mariage,
  "china", //  pour 20 ans de mariage,
  "21th", //  pour 21 ans de mariage,
  "22th", //  pour 22 ans de mariage,
  "23th", //  pour 23 ans de mariage,
  "opal", //  pour 24 ans de mariage,
  "silver", //  pour 25 ans de mariage,
  "26th", //  pour 26 ans de mariage,
  "27th", //  pour 27 ans de mariage,
  "28th", //  pour 28 ans de mariage,
  "29th", //  pour 29 ans de mariage,
  "pearl", //  pour 30 ans de mariage,
  "31th", //  pour 31 ans de mariage,
  "32th", //  pour 32 ans de mariage,
  "33th", // pour 33 ans de mariage,
  "34th", // pour 34 ans de mariage,
  "coral", // pour 35 ans de mariage,
  "36th", // pour 36 ans de mariage,
  "37th", // pour 37 ans de mariage,
  "38th", // pour 38 ans de mariage,
  "39th", // pour 39 ans de mariage,
  "ruby", // pour 40 ans de mariage.
  "41th", //41 ans de mariage : 
  "42th", //42 ans de mariage
  "43th", //43 ans de mariage
  "44th", //44 ans de mariage
  "saphire", //45 ans de mariage
  "46th", //46 ans de mariage
  "47th", //47 ans de mariage
  "48th", //48 ans de mariage
  "49th", //49 ans de mariage
  "gold", //50 ans de mariage
  "51th", //51 ans de mariage
  "52th", //52 ans de mariage
  "53th", //53 ans de mariage
  "54th", //54 ans de mariage
  "emerald", //55 ans de mariage
  "56th", //56 ans de mariage
  "57th", //57 ans de mariage
  "58th", //58 ans de mariage
  "59th", //59 ans de mariage
  "diamond", //60 ans de mariage
  "61th", //61 ans de mariage
  "62th", //62 ans de mariage
  "63th", //63 ans de mariage
  "64th," //64 ans de mariage
  "65th", //65 ans de mariage
  "66th", //66 ans de mariage
  "67th", //67 ans de mariage
  "68th", //68 ans de mariage
  "69th", //69 ans de mariage
  "70th" //70 ans de mariage
};



static const int nbDaysM[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };




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




