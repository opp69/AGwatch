typedef struct 
{  
  int m;				// mois. 1=janvier, 12=december, 0=n'importe lequel
  int d;				// jour dans le mois. 0=n'importe lequel
  char wd[2+1];				// jour de la semaine (L=lundi...) occurence dans le mois (0=n'importe, 1=premier)
  int nbj;				// nb de jour max par rapport à la date du jour pour affichage. 0=tout le temps
  char txt[20+1];
  char param[1+1];
  int  p1;
} agenda;

typedef struct 
{  
  int m;				// mois. 1=janvier, 12=december
  int d;				// jour dans le mois
  int y;				// année
  char txt[20+1];
} bio;



int AGENDA_ARRAY_SIZE = 0;
int BIO_ARRAY_SIZE = 0;
    
// the dynamic array;
agenda *AgendaLookup = NULL;
bio *BioLookup = NULL;


void AgendaLookup_free(void) {
  if (AgendaLookup != NULL) {
    AGENDA_ARRAY_SIZE = 0;
    free(AgendaLookup);
    AgendaLookup = NULL;
  };
};


void BioLookup_free(void) {
  if (BioLookup != NULL) {
    BIO_ARRAY_SIZE = 0;
    free(BioLookup);
    BioLookup = NULL;
  };
};



int AGwLIN_get_integer(char *agline, char prefix){
  
  uint32_t i = 0;
  char res[10];
  memset(res,0, sizeof(res));
  
  while (i < strlen(agline)) {
    if (agline[i]==prefix) {      
      i++;   // skip prefix
      while ( (i<strlen(agline)) && (agline[i]!=':') ) i++;
      i++;   // skip ":"
      while ( (i<strlen(agline)) && (agline[i]!=',') ) {
        if ( (agline[i]>='0') && (agline[i]<='9') && (strlen(res)<10) )
          res[strlen(res)]=agline[i];
        i++;
      };
      break;
    }
    else if (agline[i]=='"') {
      i++;
      while ( (i<strlen(agline)) && (agline[i]!='"') ) i++;   // go to default;
      i++;
    }
    else i++;
  };  
  return atoi(res);                                    
}


char* AGwLIN_get_string(char *agline, char prefix, char *res, uint32_t reslen){
  
  uint32_t i = 0;
  res[0] = '\0';
  memset(res, 0, reslen);
  while (i < strlen(agline)) {
    if (agline[i]==prefix) {      
      i++;   // skip prefix
      while ( (i<strlen(agline)) && (agline[i]!=':') ) i++;
      i++;   // skip ":"
      while ( (i<strlen(agline)) && (agline[i]!='"') ) i++;
      i++;
      while ( (i<strlen(agline)) && (agline[i]!='"') ) {
        if ( (agline[i]>=' ') && (strlen(res)<reslen) )
          res[strlen(res)]=agline[i];
        i++;
      };
      break;
    }
    else if (agline[i]=='"') {
      i++;
      while ( (i<strlen(agline)) && (agline[i]!='"') ) i++;   // go to default;
      i++;
    }
    else i++;
  };  
  return res;                                    
}



// BIORYTHM

void set_biolayer(TextLayer * _bio_layer, char _bchar, char *_bio_text, int _bval, GColor _COLOR)
{
  snprintf(_bio_text, 6, "%c:%d", _bchar, _bval);
  if (_bval==0) {
    text_layer_set_font(_bio_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD)); 
    text_layer_set_text_color      (_bio_layer, GColorBlack);
    text_layer_set_background_color(_bio_layer, _COLOR);
  } 
  else if (abs(_bval)>95) {
    text_layer_set_text_color      (_bio_layer, _COLOR);
    text_layer_set_background_color(_bio_layer, GColorClear);      
    text_layer_set_font(_bio_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  }
  else {
    text_layer_set_text_color      (_bio_layer, _COLOR);
    text_layer_set_background_color(_bio_layer, GColorClear);      
    text_layer_set_font(_bio_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  }
  text_layer_set_text(_bio_layer, _bio_text);      
}

  
/*
static const agenda AgendaLookup[AGENDA_ARRAY_SIZE] =
{
	{1, 17, "  ", 0, "Ste Roseline",   " ",   0},
	{1, 26, "  ", 0, "Michel %da",     "A",  45},
	{2, 14, "  ", 0, "F. St Valentin", " ",   0},
	{2, 18, "  ", 0, "Jacques %da",    "A",  47},
	{2, 21, "  ", 0, "Eric %da",       "A",  63},
	{3, 30, "  ", 0, "Patrick %da",    "A",  50},
	{5, 12, "  ", 0, "Serge %da",      "A",  67},
	{5, 13, "  ", 0, "Brigitte %da",   "A",  70},
	{6,  6, "  ", 0, "Jean %da",       "A", 100},
	{6, 22, "  ", 0, "Alexandra %da",  "A",  78},
	{7,  8, "  ", 0, "Samuel %da",     "A",  69},
	{7, 12, "  ", 0, "St Olivier",     " ",   0},
	{7, 23, "  ", 0, "Ste Brigitte",   " ",   0},
	{7, 25, "  ", 0, "St Jacques",     " ",   0},
	{8,  8, "  ", 0, "Yvette %da",     "A",  47},
	{8, 11, "  ", 0, "Alexandre %da",  "A", 104},
	{8, 11, "  ", 0, "Ste Clara",      " ",   0},
	{9,  5, "  ", 0, "Clara %da",      "A",  88},
	{9, 14, "  ", 0, "Janny %da",      "A",  51},
	{9, 29, "  ", 0, "St Michel",      " ",   0},
	{9, 30, "  ", 0, "Noce: %s",       "R",  95},
	{10, 1, "  ", 0, "Bebert %da",     "A",  44},
	{10, 5, "  ", 0, "Gil %da",        "A",  75},
	{10, 7, "  ", 0, "St Serge",       " ",   0},
	{10, 8, "  ", 0, "Ste Morgane",    " ",   0},
	{10,30, "  ", 0, "Roseline %da",   "A",  46},
  {11,15, "  ", 0, "Morgane %da",    "A",  96},
	{11,22, "  ", 0, "Cédric %da",     "A",  71},
  {12, 1, "  ", 0, "Hervé %da",      "A",  67},  
	{12, 6, "  ", 0, "Gérald %da",     "A",  73},
  {12,11, "  ", 0, "Thierry %da",    "A",  65},
	{12,20, "  ", 0, "Edmond %da",     "A",  48},
	{12,21, "  ", 0, "Fernando %da",   "A",  74},


	{0, 13, "V0", 7, "Vendredi 13",         " ",   0},
	{0, 0,  "J1", 7, "Gros. Ord. ce soir",  " ",   0}

};
*/

