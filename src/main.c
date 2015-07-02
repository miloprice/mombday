/* 	Earth Daylight Time, a Pebble watchface by Milo Price
	some portions adapted from the "bitmap image" template on CloudPebble
	and the Pebble SDK Tutorial found at http://ninedof.wordpress.com/pebble-sdk-tutorial/
*/
#include "pebble.h"

static Window *window;

static BitmapLayer *image_layer;
static BitmapLayer *h_layer;
static BitmapLayer *hl_layer;

static TextLayer *text_layer;
static TextLayer *text_layer_shadow1;
static TextLayer *text_layer_shadow2;
static TextLayer *text_layer_shadow3;
static TextLayer *text_layer_shadow4;

static TextLayer *text_layer_weekday;
static TextLayer *text_layer_monthday;
static TextLayer *text_layer_monthname;

static GBitmap *moon;
static GBitmap *himage;
static GBitmap *hlimage;

enum SettingsKey{
	SETTING_DATE_KEY = 0x0,		// TUPLE_CSTRING
	SETTING_GMT_KEY = 0x1,		// TUPLE_CSTRING
};

static AppSync async;
static uint8_t sync_buffer[32];

const char *showdate = "Y";
const char *gmtmod = "000";
int ggmod = 0;	//global representation of gmt modifier

//InverterLayer *inv_layer;	//Inverter layer

char buffer[] = "00:00";

char weekday[] = "Sun";
char monthday[] = "14";
char monthname[] = "SEPTEMBER";

int hasanim;
int lastx1 = 144;
int lastx2 = 144;
int nighthr = 0;
int lasthr = 22;

//static uint8_t syncbuffer[256];

void on_animation_stopped(Animation *anim, bool finished, void *context)
{
	// Free memory used by Animation
	property_animation_destroy((PropertyAnimation*) anim);
}

void animate_layer(Layer *layer, GRect *start, GRect *finish, int duration, int delay)
{
    //Declare animation
    PropertyAnimation *anim = property_animation_create_layer_frame(layer, start, finish);
 
    //Set characteristics
    animation_set_duration((Animation*) anim, duration);
    animation_set_delay((Animation*) anim, delay);
 
    //Set stopped handler to free memory
    AnimationHandlers handlers = {
        //The reference to the stopped handler is the only one in the array
        .stopped = (AnimationStoppedHandler) on_animation_stopped
    };
    animation_set_handlers((Animation*) anim, handlers, NULL);
 
    //Start animation!
    animation_schedule((Animation*) anim);
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed){
	if (clock_is_24h_style()){
		strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
	} else {
		strftime(buffer, sizeof("00:00"), "%l:%M", tick_time);
	}
	
	strftime(weekday, sizeof("Sun"), "%a", tick_time);
	strftime(monthday, sizeof("14"), "%e", tick_time);
	strftime(monthname, sizeof("Sep"), "%b", tick_time);
	
	text_layer_set_text(text_layer_shadow1, buffer);
	text_layer_set_text(text_layer_shadow2, buffer);
	text_layer_set_text(text_layer_shadow3, buffer);
	text_layer_set_text(text_layer_shadow4, buffer);
	text_layer_set_text(text_layer, buffer);
	
	if (strcmp(showdate,"Y") == 0){
		text_layer_set_text(text_layer_weekday, weekday);
		text_layer_set_text(text_layer_monthday, monthday);
		text_layer_set_text(text_layer_monthname, monthname);
	}
	
	int hours = tick_time->tm_hour;
	int mins = tick_time->tm_min;
	int secs = tick_time->tm_sec;
	int hour1 = (secs / 2) % 24; //Used for debugging (allows time to pass quickly)
    if (hours > 6 && hours < 20) {
        // Daytime, loosely defined.
        //layer_set_hidden(inverter_layer_get_layer(inv_layer), true);
	    window_set_background_color	(window, GColorWhite);
	    text_layer_set_text_color(text_layer, GColorBlack);
	    text_layer_set_text_color(text_layer_shadow1, GColorWhite);
	    text_layer_set_text_color(text_layer_shadow2, GColorWhite);
	    text_layer_set_text_color(text_layer_shadow3, GColorWhite);
	    text_layer_set_text_color(text_layer_shadow4, GColorWhite);
    } else {
        // Nighttime, loosely defined.
        //layer_set_hidden(inverter_layer_get_layer(inv_layer), false);
	    window_set_background_color	(window, GColorBlack);
	    text_layer_set_text_color(text_layer, GColorWhite);
	    text_layer_set_text_color(text_layer_shadow1, GColorBlack);
	    text_layer_set_text_color(text_layer_shadow2, GColorBlack);
	    text_layer_set_text_color(text_layer_shadow3, GColorBlack);
	    text_layer_set_text_color(text_layer_shadow4, GColorBlack);
    }
}

static void app_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void* context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "app error %d", app_message_error);
}

 static void settings_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
// 	switch (key) {
// 		case SETTING_DATE_KEY:
// 	  	if (strcmp(new_tuple->value->cstring,"N") == 0 && strcmp(showdate, "Y") == 0){
// 			text_layer_destroy(text_layer_weekday);
// 			text_layer_destroy(text_layer_monthday);
// 			text_layer_destroy(text_layer_monthname);
// 			showdate = "N";
// 		} else if (strcmp(new_tuple->value->cstring,"Y") == 0 && strcmp(showdate, "N") == 0){
			
// 			ResHandle date_font = resource_get_handle(RESOURCE_ID_FONT_NUNITO_BOLD_15);
// 				// Day of week
// 			text_layer_weekday = text_layer_create(GRect(0, 28, 30, 50));
// 			text_layer_set_background_color(text_layer_weekday, GColorClear);
// 			text_layer_set_text_color(text_layer_weekday, GColorWhite);
// 			text_layer_set_text_alignment(text_layer_weekday, GTextAlignmentLeft);
// 			text_layer_set_font(text_layer_weekday, fonts_get_system_font(FONT_KEY_GOTHIC_14));
			
// 			layer_add_child(window_get_root_layer(window), (Layer*) text_layer_weekday);
			
// 				// Name of month
// 			text_layer_monthname = text_layer_create(GRect(0, 0, 50, 20));
// 			text_layer_set_background_color(text_layer_monthname, GColorClear);
// 			text_layer_set_text_color(text_layer_monthname, GColorWhite);
// 			text_layer_set_text_alignment(text_layer_monthname, GTextAlignmentLeft);
// 			text_layer_set_font(text_layer_monthname, fonts_get_system_font(FONT_KEY_GOTHIC_14));
			
// 			layer_add_child(window_get_root_layer(window), (Layer*) text_layer_monthname);
			
			
// 				// Day of month
// 			text_layer_monthday = text_layer_create(GRect(0, 14, 30, 29));
// 			text_layer_set_background_color(text_layer_monthday, GColorClear);
// 			text_layer_set_text_color(text_layer_monthday, GColorWhite);
// 			text_layer_set_text_alignment(text_layer_monthday, GTextAlignmentLeft);
// 			text_layer_set_font(text_layer_monthday, fonts_load_custom_font(date_font));
			
// 			layer_add_child(window_get_root_layer(window), (Layer*) text_layer_monthday);
			
// 			showdate = "Y";
// 		}
//         break;
//     case SETTING_GMT_KEY:
		
//       gmtmod = new_tuple->value->cstring;
// 		adj_hour();
// 	  hasanim = 0;
//       break;
   }
// }

void window_load(Window *window)
{	
	hasanim = 0;
	
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);
	
	window_set_background_color	(window, GColorWhite);
	
	// This needs to be deinited on app exit which is when the event loop ends
	moon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MOON);
	himage = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CURTAINS_T_WHITE);
	hlimage = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CURTAINS_T_BLACK);
	
	// The bitmap layer holds the image for display
	image_layer = bitmap_layer_create(bounds);
	bitmap_layer_set_bitmap(image_layer, moon);
	bitmap_layer_set_alignment(image_layer, GAlignCenter);
	layer_add_child(window_layer, bitmap_layer_get_layer(image_layer));
	
    // Curtains; white display items
	h_layer = bitmap_layer_create(bounds);
	bitmap_layer_set_bitmap(h_layer, himage);
	bitmap_layer_set_alignment(h_layer, GAlignCenter);
	layer_add_child(window_layer, bitmap_layer_get_layer(h_layer));
	bitmap_layer_set_compositing_mode(h_layer, GCompOpOr);
    
    // Curtain lines; black display items
	hl_layer = bitmap_layer_create(bounds);
	bitmap_layer_set_bitmap(hl_layer, hlimage);
	bitmap_layer_set_alignment(hl_layer, GAlignCenter);
	layer_add_child(window_layer, bitmap_layer_get_layer(hl_layer));
	bitmap_layer_set_compositing_mode(hl_layer, GCompOpClear);
    
    
	
	
	//Load font
	ResHandle font_handle = resource_get_handle(RESOURCE_ID_FONT_NUNITO_48);
	ResHandle date_font = resource_get_handle(RESOURCE_ID_FONT_NUNITO_BOLD_15);

	
	//Time layer
	text_layer = text_layer_create(GRect(1, 55, 144, 168));
	text_layer_set_background_color(text_layer, GColorClear);
	text_layer_set_text_color(text_layer, GColorBlack);
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	text_layer_set_font(text_layer, fonts_load_custom_font(font_handle));
	
	text_layer_shadow1 = text_layer_create(GRect(2, 55, 144, 168));
	text_layer_set_background_color(text_layer_shadow1, GColorClear);
	text_layer_set_text_color(text_layer_shadow1, GColorWhite);
	text_layer_set_text_alignment(text_layer_shadow1, GTextAlignmentCenter);
	text_layer_set_font(text_layer_shadow1, fonts_load_custom_font(font_handle));
	
	layer_add_child(window_get_root_layer(window), (Layer*) text_layer_shadow1);
	
	text_layer_shadow2 = text_layer_create(GRect(0, 55, 144, 168));
	text_layer_set_background_color(text_layer_shadow2, GColorClear);
	text_layer_set_text_color(text_layer_shadow2, GColorWhite);
	text_layer_set_text_alignment(text_layer_shadow2, GTextAlignmentCenter);
	text_layer_set_font(text_layer_shadow2, fonts_load_custom_font(font_handle));
	
	layer_add_child(window_get_root_layer(window), (Layer*) text_layer_shadow2);
	
	text_layer_shadow3 = text_layer_create(GRect(1, 54, 144, 168));
	text_layer_set_background_color(text_layer_shadow3, GColorClear);
	text_layer_set_text_color(text_layer_shadow3, GColorWhite);
	text_layer_set_text_alignment(text_layer_shadow3, GTextAlignmentCenter);
	text_layer_set_font(text_layer_shadow3, fonts_load_custom_font(font_handle));
	
	layer_add_child(window_get_root_layer(window), (Layer*) text_layer_shadow3);
	
	text_layer_shadow4 = text_layer_create(GRect(1, 56, 144, 168));
	text_layer_set_background_color(text_layer_shadow4, GColorClear);
	text_layer_set_text_color(text_layer_shadow4, GColorWhite);
	text_layer_set_text_alignment(text_layer_shadow4, GTextAlignmentCenter);
	text_layer_set_font(text_layer_shadow4, fonts_load_custom_font(font_handle));
	
	layer_add_child(window_get_root_layer(window), (Layer*) text_layer_shadow4);
	
	layer_add_child(window_get_root_layer(window), (Layer*) text_layer);
	
	// layer
	//inv_layer = inverter_layer_create(GRect(11,8,121,154));
	//layer_add_child(window_get_root_layer(window), (Layer*) inv_layer);
	
    

	
		// Day of week
	text_layer_weekday = text_layer_create(GRect(0, 28, 30, 50));
	text_layer_set_background_color(text_layer_weekday, GColorClear);
	text_layer_set_text_color(text_layer_weekday, GColorBlack);
	text_layer_set_text_alignment(text_layer_weekday, GTextAlignmentLeft);
	text_layer_set_font(text_layer_weekday, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	
	layer_add_child(window_get_root_layer(window), (Layer*) text_layer_weekday);
	
		// Name of month
	text_layer_monthname = text_layer_create(GRect(0, 0, 50, 20));
	text_layer_set_background_color(text_layer_monthname, GColorClear);
	text_layer_set_text_color(text_layer_monthname, GColorBlack);
	text_layer_set_text_alignment(text_layer_monthname, GTextAlignmentLeft);
	text_layer_set_font(text_layer_monthname, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	
	layer_add_child(window_get_root_layer(window), (Layer*) text_layer_monthname);
	
	
		// Day of month
	text_layer_monthday = text_layer_create(GRect(0, 14, 30, 29));
	text_layer_set_background_color(text_layer_monthday, GColorClear);
	text_layer_set_text_color(text_layer_monthday, GColorBlack);
	text_layer_set_text_alignment(text_layer_monthday, GTextAlignmentLeft);
	text_layer_set_font(text_layer_monthday, fonts_load_custom_font(date_font));
	
	layer_add_child(window_get_root_layer(window), (Layer*) text_layer_monthday);
		
}

void window_unload(Window *window)
{
	text_layer_destroy(text_layer);
	text_layer_destroy(text_layer_shadow1);
	text_layer_destroy(text_layer_shadow2);
	text_layer_destroy(text_layer_shadow3);
	text_layer_destroy(text_layer_shadow4);
	text_layer_destroy(text_layer_weekday);
	text_layer_destroy(text_layer_monthday);
	text_layer_destroy(text_layer_monthname);
	
	//inverter_layer_destroy(inv_layer);
}

void handle_init(void) {
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	
	window_stack_push(window, true /* Animated */);
	
	
	
	tick_timer_service_subscribe(SECOND_UNIT, (TickHandler) tick_handler);

}



static void app_message_init(void) {
  // Reduce the sniff interval for more responsive messaging at the expense of
  // increased energy consumption by the Bluetooth module
  // The sniff interval will be restored by the system after the app has been
  // unloaded
  	//app_comm_set_sniff_interval(SNIFF_INTERVAL_REDUCED);
	// Init buffers
	const int inbound_size = 64;
	const int outbound_size = 64;
	app_message_open(inbound_size, outbound_size);
	
	Tuplet settings_init[] = {
		TupletCString(SETTING_DATE_KEY, "Y"),
		TupletCString(SETTING_GMT_KEY, "None"),
	};
	
	app_sync_init(&async, sync_buffer, sizeof(sync_buffer), settings_init, ARRAY_LENGTH(settings_init),
				  settings_changed_callback, app_error_callback, NULL);
}

int main(void) {
	
	app_message_init();
	handle_init();


  	app_event_loop();

	tick_timer_service_unsubscribe();
	
  	gbitmap_destroy(moon);
  	gbitmap_destroy(himage);
  	gbitmap_destroy(hlimage);
	
	app_sync_deinit(&async);
	
	bitmap_layer_destroy(image_layer);
	bitmap_layer_destroy(h_layer);
	window_destroy(window);
}
