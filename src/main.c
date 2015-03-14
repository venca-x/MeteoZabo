#include <pebble.h>

//http://ninedof.wordpress.com/2014/02/02/pebble-sdk-2-0-tutorial-6-appmessage-for-pebblekit-js/

#define KEY_TEMPERATURE_OUT 0
#define KEY_TEMPERATURE_IN 1
#define KEY_LAST_UPDATE 2
#define KEY_RAIN_5M 3
#define KEY_RAIN_1H 4
#define KEY_RAIN_24H 5
#define KEY_PRESSURE 6
  
Window* g_window;
TextLayer *g_text_layer_time;//vrstva pro cas
TextLayer *g_text_layer_temperature;//teplota
TextLayer *g_text_layer_last_update;//posledni aktualizace
TextLayer *g_text_layer_rain;//srazky
TextLayer *g_text_layer_pressure;//tlak

//GBitmap *future_bitmap;
BitmapLayer *g_bitmap_layer_weather;//obrazek pocasi

char buffer[] = "00:00";

void window_load(Window *window)
{
  //We will add the creation of the Window's elements here soon!
  
  // Get the root layer
  Layer *window_layer = window_get_root_layer(window);  
  GRect bounds = layer_get_bounds(window_layer);//widnows size
  
  //hours
  g_text_layer_time = text_layer_create(GRect(0, 20, 144, 55));
  text_layer_set_background_color(g_text_layer_time, GColorBlack);
  text_layer_set_text_color(g_text_layer_time, GColorWhite);
  text_layer_set_font(g_text_layer_time, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));   
  text_layer_set_text_alignment(g_text_layer_time, GTextAlignmentCenter);  
  text_layer_set_text(g_text_layer_time, "??:??");   
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(g_text_layer_time));   

  //temperature layer
  g_text_layer_temperature = text_layer_create(GRect(5, 70, 144, 20));
  text_layer_set_background_color(g_text_layer_temperature, GColorClear);
  text_layer_set_text_color(g_text_layer_temperature, GColorBlack);
  text_layer_set_text_alignment(g_text_layer_temperature, GTextAlignmentLeft);
  text_layer_set_font(g_text_layer_temperature, fonts_get_system_font(FONT_KEY_GOTHIC_18)); 
  text_layer_set_text(g_text_layer_temperature, "Temp: ?? / ??");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(g_text_layer_temperature));  
  
  //rain layer
  g_text_layer_rain = text_layer_create(GRect(5, 90, 144, 20));
  text_layer_set_background_color(g_text_layer_rain, GColorClear);
  text_layer_set_text_color(g_text_layer_rain, GColorBlack);
  text_layer_set_text_alignment(g_text_layer_rain, GTextAlignmentLeft);
  text_layer_set_font(g_text_layer_rain, fonts_get_system_font(FONT_KEY_GOTHIC_18)); 
  text_layer_set_text(g_text_layer_rain, "Rain: ?? / ?? ?? / ??");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(g_text_layer_rain));  
  
  //pressure layer
  g_text_layer_pressure = text_layer_create(GRect(5, 110, 144, 20));
  text_layer_set_background_color(g_text_layer_pressure, GColorClear);
  text_layer_set_text_color(g_text_layer_pressure, GColorBlack);
  text_layer_set_text_alignment(g_text_layer_pressure, GTextAlignmentLeft);
  text_layer_set_font(g_text_layer_pressure, fonts_get_system_font(FONT_KEY_GOTHIC_18)); 
  text_layer_set_text(g_text_layer_pressure, "Pressure: ??");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(g_text_layer_pressure));  
  
  //last update layer
  g_text_layer_last_update = text_layer_create(GRect(5, 130, 144, 20));
  text_layer_set_background_color(g_text_layer_last_update, GColorClear);
  text_layer_set_text_color(g_text_layer_last_update, GColorBlack);
  text_layer_set_text_alignment(g_text_layer_last_update, GTextAlignmentLeft);
  text_layer_set_font(g_text_layer_last_update, fonts_get_system_font(FONT_KEY_GOTHIC_14)); 
  text_layer_set_text(g_text_layer_last_update, "Last update:");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(g_text_layer_last_update));  
  
  //image wheather
  //g_bitmap_layer_weather = bitmap_layer_create(GRect(50, 50, bounds.size.w, bounds.size.h));//144×168
  g_bitmap_layer_weather = bitmap_layer_create(GRect((bounds.size.w-25), (bounds.size.h-25), 25, 25));
  bitmap_layer_set_bitmap(g_bitmap_layer_weather, gbitmap_create_with_resource(RESOURCE_ID_WEATHER_SUNNY_BLACK));  
  bitmap_layer_set_compositing_mode(g_bitmap_layer_weather, GCompOpClear);
  
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(g_bitmap_layer_weather));
}

void window_unload(Window *window)
{
  //We will safely destroy the Window's elements here!
  text_layer_destroy(g_text_layer_time);
  text_layer_destroy(g_text_layer_temperature);
  text_layer_destroy(g_text_layer_last_update); 
  text_layer_destroy(g_text_layer_rain);
  text_layer_destroy(g_text_layer_pressure);
  
  //gbitmap_destroy(future_bitmap);
  bitmap_layer_destroy(g_bitmap_layer_weather);
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "TICK");  

    //Here we will update the watchface display
    //Format the buffer string using tick_time as the time source
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
      
    //Change the TextLayer text to show the new time!
    text_layer_set_text(g_text_layer_time, buffer);    
    
    //Every five minutes
    if(tick_time->tm_min % 5 == 0)
    {
        // Begin dictionary
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);

        // Add a key-value pair
        dict_write_uint8(iter, 0, 0);

        // Send the message!
        app_message_outbox_send();      
    }  
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "inbox_received_callback");

  // Store incoming information
  static char temperature_in_buffer[32];
  static char temperature_out_buffer[32];
  static char weather_layer_buffer[32];
  
  static char last_update_layer_buffer[32];
  
  static char rain_5m_layer_buffer[32];
  static char rain_1h_layer_buffer[32];
  static char rain_24h_layer_buffer[32];
  static char rain_layer_buffer[32];
  
  static char pressure_layer_buffer[32];
  
  // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_TEMPERATURE_OUT:
      snprintf(temperature_in_buffer, sizeof(temperature_in_buffer), "%s", t->value->cstring);
      break;
    case KEY_TEMPERATURE_IN:
      snprintf(temperature_out_buffer, sizeof(temperature_out_buffer), "%s", t->value->cstring);
      break;
    case KEY_LAST_UPDATE:
      snprintf(last_update_layer_buffer, sizeof(last_update_layer_buffer), "%s", t->value->cstring);
      break;
    case KEY_RAIN_5M:
      snprintf(rain_5m_layer_buffer, sizeof(rain_5m_layer_buffer), "%s", t->value->cstring);      
      break;      
    case KEY_RAIN_1H:
      snprintf(rain_1h_layer_buffer, sizeof(rain_1h_layer_buffer), "%s", t->value->cstring);      
      break;      
    case KEY_RAIN_24H:
      snprintf(rain_24h_layer_buffer, sizeof(rain_24h_layer_buffer), "%s", t->value->cstring);      
      break;
    case KEY_PRESSURE:
      snprintf(pressure_layer_buffer, sizeof(pressure_layer_buffer), "%s hPa", t->value->cstring);      
      break;
      
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    // Look for next item
    t = dict_read_next(iterator);
  }
  
  // Assemble full string and display
  snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "Temp: %s\u00B0C / %s\u00B0C", temperature_in_buffer, temperature_out_buffer);
  text_layer_set_text(g_text_layer_temperature, weather_layer_buffer);  

  //rain    
  snprintf(rain_layer_buffer, sizeof(rain_layer_buffer), "Rain: %s / %s / %s", rain_5m_layer_buffer, rain_1h_layer_buffer, rain_24h_layer_buffer);
  text_layer_set_text(g_text_layer_rain, rain_layer_buffer);      

  //pressure
  text_layer_set_text(g_text_layer_pressure, pressure_layer_buffer);  
  
  //last update  
  text_layer_set_text(g_text_layer_last_update, last_update_layer_buffer);  
  
  //weather icon
  bitmap_layer_set_bitmap(g_bitmap_layer_weather, gbitmap_create_with_resource(RESOURCE_ID_WEATHER_CLOUDY_BLACK));
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}


void init()
{
  //Create the app elements here!
  g_window = window_create();
  window_set_window_handlers(g_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
 
  window_stack_push(g_window, true);

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());  
}
 
void deinit()
{
  //Destroy elements here to save memory!
  window_destroy(g_window);
}
 
int main(void)
{
  init();
  app_event_loop();
  deinit();
}