#include "pebble.h"
	
Window *window;
TextLayer *text_date_layer;
TextLayer *text_time_layer;
TextLayer *text_fail_layer;
Layer *line_layer;

// The different watch->app messages we can send
static const int MACRODROID_WATCH_TO_APP_BUTTON_PRESS = 1;
static const int MACRODROID_WATCH_TO_APP_BATTERY_LEVEL = 2;

// Identifiers for the different button types on the pebble watch
static const int MACRODROID_BUTTON_ID_UP = 1;
static const int MACRODROID_BUTTON_ID_MIDDLE = 2;
static const int MACRODROID_BUTTON_ID_DOWN = 3;
	
// Identifiers for the different press types on the pebble watch
static const int MACRODROID_BUTTON_PRESS_SINGLE = 100;
static const int MACRODROID_BUTTON_PRESS_LONG = 200;
static const int MACRODROID_BUTTON_PRESS_MULTI = 300;

// identifies a battery level message
static const int MACRODROID_BATTERY_LEVEL_MESSAGE_ID = 1000;

// Identifiers for commands from Macrodroid
static const int MACRODROID_COMMAND_KEY = 999;

static const int MACRODROID_COMMAND_VIBRATE = 3;
static const int MACRODROID_COMMAND_LIGHT_ON = 4;

void hide_info_handler(void* context) {
	text_layer_set_text(text_fail_layer, "");
}

////////////////////////////
// MESSAGE SENDING CALLBACKS
////////////////////////////

 void out_sent_handler(DictionaryIterator *sent, void *context) {
   // outgoing message was delivered
 }


 void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
   // outgoing message failed
   text_layer_set_text(text_fail_layer, "<Failed to send>");
	 
   AppTimer * timer = app_timer_register(2500, (AppTimerCallback) hide_info_handler, NULL);
 }


void in_received_handler(DictionaryIterator *received, void *context) {
	// Check for fields you expect to receive
    Tuple * command_tuple = dict_find(received, MACRODROID_COMMAND_KEY);

    // Act on the found fields received
    if (command_tuple) {
		
		int command = command_tuple->value->int32;
		
		if (command == MACRODROID_COMMAND_VIBRATE) {
			vibes_long_pulse();	
		} else if (command == MACRODROID_COMMAND_LIGHT_ON) {
			light_enable_interaction();
		}
     }
 }


 void in_dropped_handler(AppMessageResult reason, void *context) {
   // incoming message dropped
 }

void send_button_press(int macrodroidButtonId) {
	DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
	  
    Tuplet value = TupletInteger(MACRODROID_WATCH_TO_APP_BUTTON_PRESS, macrodroidButtonId);
    dict_write_tuplet(iter, &value);
	
    app_message_outbox_send();
}

//////////////////////
// Click Handler Stuff
//////////////////////

void down_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  // ... called on single click ...
  // Window *window = (Window *)context; // This context defaults to the window, but may be changed with \ref window_set_click_context.
	
	ButtonId id = click_recognizer_get_button_id(recognizer);
	
	switch (id) {
    case BUTTON_ID_UP:
		send_button_press(MACRODROID_BUTTON_PRESS_SINGLE + MACRODROID_BUTTON_ID_UP);
        break;

    case BUTTON_ID_SELECT:
		send_button_press(MACRODROID_BUTTON_PRESS_SINGLE + MACRODROID_BUTTON_ID_MIDDLE);
        break;

    case BUTTON_ID_DOWN:
		send_button_press(MACRODROID_BUTTON_PRESS_SINGLE + MACRODROID_BUTTON_ID_DOWN);
        break;
		
	default:
		break;
  }
	
	/*DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
	  
    Tuplet value = TupletInteger(1, 2001);
    dict_write_tuplet(iter, &value);
	
    app_message_outbox_send();*/
}

void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
	ButtonId id = click_recognizer_get_button_id(recognizer);
	
	switch (id) {
    case BUTTON_ID_UP:
		send_button_press(MACRODROID_BUTTON_PRESS_LONG + MACRODROID_BUTTON_ID_UP);
        break;

    case BUTTON_ID_SELECT:
		send_button_press(MACRODROID_BUTTON_PRESS_LONG + MACRODROID_BUTTON_ID_MIDDLE);
        break;

    case BUTTON_ID_DOWN:
		send_button_press(MACRODROID_BUTTON_PRESS_LONG + MACRODROID_BUTTON_ID_DOWN);
        break;
		
	default:
		break;
	}
}

void select_long_click_release_handler(ClickRecognizerRef recognizer, void *context) {

}

void select_multi_click_handler(ClickRecognizerRef recognizer, void *context) {
	ButtonId id = click_recognizer_get_button_id(recognizer);
	
	switch (id) {
    case BUTTON_ID_UP:
		send_button_press(MACRODROID_BUTTON_PRESS_MULTI + MACRODROID_BUTTON_ID_UP);
        break;

    case BUTTON_ID_SELECT:
		send_button_press(MACRODROID_BUTTON_PRESS_MULTI + MACRODROID_BUTTON_ID_MIDDLE);
        break;

    case BUTTON_ID_DOWN:
		send_button_press(MACRODROID_BUTTON_PRESS_MULTI + MACRODROID_BUTTON_ID_DOWN);
        break;
		
	default:
		break;
	}
}

/////////////////////////////
// BATTERY LEVEL HANDLER
/////////////////////////////
void battery_level_change(BatteryChargeState charge) {
	DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
	  
    Tuplet value = TupletInteger(MACRODROID_WATCH_TO_APP_BATTERY_LEVEL, (int) charge.charge_percent);
    dict_write_tuplet(iter, &value);
	
    app_message_outbox_send();
}


void config_provider(Window *window) {
    // single click / repeat-on-hold config:
    window_single_click_subscribe(BUTTON_ID_UP, down_single_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_SELECT, down_single_click_handler);
  //window_single_repeating_click_subscribe(BUTTON_ID_SELECT, 1000, select_single_click_handler);
  // multi click config:
  //window_multi_click_subscribe(BUTTON_ID_SELECT, 2, 10, 0, true, select_multi_click_handler);
  // long click config:

    window_long_click_subscribe(BUTTON_ID_UP, 700, select_long_click_handler, select_long_click_release_handler);
	window_long_click_subscribe(BUTTON_ID_DOWN, 700, select_long_click_handler, select_long_click_release_handler);
	window_long_click_subscribe(BUTTON_ID_SELECT, 700, select_long_click_handler, select_long_click_release_handler);
	
	// multi click config:
    window_multi_click_subscribe(BUTTON_ID_UP, 2, 10, 0, true, select_multi_click_handler);
	window_multi_click_subscribe(BUTTON_ID_DOWN, 2, 10, 0, true, select_multi_click_handler);
	window_multi_click_subscribe(BUTTON_ID_SELECT, 2, 10, 0, true, select_multi_click_handler);
}


void line_layer_update_callback(Layer *layer, GContext* ctx) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}

void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  // Need to be static because they're used by the system later.
  static char time_text[] = "00:00";
  static char date_text[] = "Xxxxxxxxx 00";

  char *time_format;


  // TODO: Only update the date when it's changed.
  strftime(date_text, sizeof(date_text), "%B %e", tick_time);
  text_layer_set_text(text_date_layer, date_text);


  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

  strftime(time_text, sizeof(time_text), time_format, tick_time);

  // Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }

  text_layer_set_text(text_time_layer, time_text);
}

void handle_deinit(void) {
  tick_timer_service_unsubscribe();
}

void handle_init(void) {
  window = window_create();
  window_set_fullscreen(window, true);
  window_stack_push(window, true /* Animated */);
  window_set_background_color(window, GColorBlack);
	
  Layer *window_layer = window_get_root_layer(window);

  text_date_layer = text_layer_create(GRect(8, 68, 144-8, 168-68));
  text_layer_set_text_color(text_date_layer, GColorWhite);
  text_layer_set_background_color(text_date_layer, GColorClear);
  text_layer_set_font(text_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  layer_add_child(window_layer, text_layer_get_layer(text_date_layer));

  text_time_layer = text_layer_create(GRect(7, 92, 144-7, 168-92));
  text_layer_set_text_color(text_time_layer, GColorWhite);
  text_layer_set_background_color(text_time_layer, GColorClear);
  text_layer_set_font(text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_49)));
  layer_add_child(window_layer, text_layer_get_layer(text_time_layer));

  text_fail_layer = text_layer_create(GRect(30, 150, 144-30, 168-150));
  text_layer_set_text_color(text_fail_layer, GColorWhite);
  text_layer_set_background_color(text_fail_layer, GColorClear);
  //text_layer_set_font(text_fail_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21)));
  text_layer_set_font(text_fail_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  layer_add_child(window_layer, text_layer_get_layer(text_fail_layer));
	
  GRect line_frame = GRect(8, 97, 139, 2);
  line_layer = layer_create(line_frame);
  layer_set_update_proc(line_layer, line_layer_update_callback);
  layer_add_child(window_layer, line_layer);

  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
  // TODO: Update display here to avoid blank display on launch?
}


int main(void) {
    handle_init();

	app_message_register_inbox_received(in_received_handler);
 	app_message_register_inbox_dropped(in_dropped_handler);
    app_message_register_outbox_sent(out_sent_handler);
    app_message_register_outbox_failed(out_failed_handler);

    const uint32_t inbound_size = 256;
    const uint32_t outbound_size = 256;
    app_message_open(inbound_size, outbound_size);
	
	window_set_click_config_provider(window, (ClickConfigProvider) config_provider);
	
	// Listen for battery updates
	battery_state_service_subscribe(battery_level_change);	
	
    app_event_loop();
  
    handle_deinit();
}