//NOTE: longitude is positive for East and negative for West
// Paris LAT 48.85, LON -2.33

#define LATITUDE    48.85
//#define LONGITUDE   -2.33
//#define TIMEZONE 	0

//NOTE: Change false to true if you want to enable the vibe function at 8 and 20
//#define HOUR_8_20_VIBRATION   true
//#define FRENCH 			          true  --> by JS COnfig now
#define BATT_PERCENT          20

  
#define rW 7
#define rC 3
  
void draw_rainsquare(GContext* _ctx, int _h, int _size, GColor _stroke, GColor _fill) {
  graphics_context_set_stroke_color(_ctx, _stroke);
  graphics_context_set_fill_color(_ctx, _fill); 
  switch (_size) {
    case 3:
      graphics_fill_rect(_ctx, GRect(rW * _h + 2 ,  2, 3, 3), 0, GCornerNone);  
      break;
    case 5:
      graphics_fill_rect(_ctx, GRect(rW * _h + 1 ,  1, 5, 5), 0, GCornerNone);    
      break;
    case 7:
      graphics_fill_rect(_ctx, GRect(rW * _h     ,  0, 7, 7), 0, GCornerNone);
      break;
    case 1:    
    default:
      graphics_fill_rect(_ctx, GRect(rW * _h + 3 ,  3, 1, 1), 0, GCornerNone);    
  }    
}
  
void draw_rain(GContext* _ctx, int _h, int _cond) {
  switch (_cond) {
    case 200:
    case 230:
    draw_rainsquare(_ctx, _h, 3, THUNDER_COLOR, RAIN_COLOR);
    break;
    case 201:
    case 231:
    draw_rainsquare(_ctx, _h, 5, THUNDER_COLOR, RAIN_COLOR);
    break;
    case 202:
    case 232:
    draw_rainsquare(_ctx, _h, 7, THUNDER_COLOR, RAIN_COLOR);
    break;
    case 210:
    draw_rainsquare(_ctx, _h, 3, THUNDER_COLOR, THUNDER_COLOR);    
    break;
    case 211:
    draw_rainsquare(_ctx, _h, 5, THUNDER_COLOR, THUNDER_COLOR);    
    break;
    case 212:
    case 213:     
    draw_rainsquare(_ctx, _h, 7, THUNDER_COLOR, THUNDER_COLOR);    
    break; 
    case 500: // light rain
    case 520: // shower rain    
    draw_rainsquare(_ctx, _h, 3, RAIN_COLOR, RAIN_COLOR);            
    break;
    case 501: // moderate rain
    case 521: // moderate shower rain 
    draw_rainsquare(_ctx, _h, 5, RAIN_COLOR, RAIN_COLOR);  
    break;
    case 502: // heavy
    case 503:      
    case 504:            
    case 522:
    case 531:
    draw_rainsquare(_ctx, _h, 7, RAIN_COLOR, RAIN_COLOR);  
    break;

    case 600:   //SNOW
    draw_rainsquare(_ctx, _h, 3, SNOW_COLOR, SNOW_COLOR);  
    break;
    case 601:
    draw_rainsquare(_ctx, _h, 5, SNOW_COLOR, SNOW_COLOR);
    break;
    case 602:
    draw_rainsquare(_ctx, _h, 7, SNOW_COLOR, SNOW_COLOR);
    break;      
    case 615:
    case 620:
    draw_rainsquare(_ctx, _h, 3, SNOW_COLOR, RAIN_COLOR);
    break;
    case 616:
    case 621:
    draw_rainsquare(_ctx, _h, 5, SNOW_COLOR, RAIN_COLOR);
    break;
    case 622:
    draw_rainsquare(_ctx, _h, 7, SNOW_COLOR, RAIN_COLOR);
    break;      
    default:
    draw_rainsquare(_ctx, _h, 1, GColorWhite, GColorWhite);
  };
};



TextLayer *text_layer_create_ext(Layer *_wnd, GRect _frame, GColor _tcolor, GColor _bcolor, GFont _font, GTextAlignment _align) {
  
  TextLayer *layer;
  layer = text_layer_create(_frame);
  text_layer_set_text_color      (layer, _tcolor);
  text_layer_set_background_color(layer, _bcolor);
  text_layer_set_font(layer, _font);
  text_layer_set_text_alignment(layer, _align);
  layer_add_child(_wnd, text_layer_get_layer(layer));
  return layer;
}  

