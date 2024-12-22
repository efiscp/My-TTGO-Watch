#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#else
    #include <Arduino.h>
    #ifdef M5PAPER
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
    #endif
#endif

#include "config.h"
#include "gui/mainbar/mainbar.h"
#include "gui/widget_styles.h"
#include "app/casio676/casio676.hpp"

#define GEN_BUTTON(NAME, EVENT)										\
	static lv_obj_t* NAME;											\
	static void NAME##_event_cb(lv_obj_t * obj, lv_event_t event){	\
		if(LV_EVENT_CLICKED == event){								\
			casio->processEvent(EVENT);								\
			log_d(#NAME " clicked");								\
		}															\
	}																\

extern lv_font_t dotmatrix_24px;
extern lv_font_t sevenSegment_24px;
extern lv_font_t sevenSegment_48px;

static bool casio676tile_init = false;
static uint32_t casio676tile_num;
static lv_obj_t* tileref = NULL;
static lv_obj_t* dotMatrixLabel;
static lv_obj_t* smallDigitLabel;
static lv_obj_t* largeDigitLabel;
static lv_style_t buttonStyle;

static lv_task_t* casio676Task;

static Casio676* casio;

static void casio676task(lv_task_t* task);

GEN_BUTTON(buttonA, BUTTON_A_PRESSED)
GEN_BUTTON(buttonB, BUTTON_B_PRESSED)
GEN_BUTTON(buttonC, BUTTON_C_PRESSED)
GEN_BUTTON(buttonL, BUTTON_L_PRESSED)

static void displaySetup(void){
	dotMatrixLabel = lv_label_create(tileref, NULL);
	smallDigitLabel = lv_label_create(tileref, NULL);
	largeDigitLabel = lv_label_create(tileref, NULL);

	lv_obj_set_style_local_text_font(dotMatrixLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &dotmatrix_24px);
	lv_obj_set_style_local_text_font(smallDigitLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &sevenSegment_24px);
	lv_obj_set_style_local_text_font(largeDigitLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &sevenSegment_48px);

	lv_obj_set_pos(dotMatrixLabel, 30, 40);
	lv_obj_set_pos(smallDigitLabel, 150, 40);
	lv_obj_set_pos(largeDigitLabel, 30, 75);
}

static void buttonStyleSetup(void){
	lv_style_init(&buttonStyle);
	lv_style_set_radius(&buttonStyle, LV_STATE_DEFAULT, 0);
	lv_style_set_border_width(&buttonStyle, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_opa(&buttonStyle, LV_STATE_DEFAULT, LV_OPA_TRANSP);
	lv_style_set_border_opa(&buttonStyle, LV_STATE_DEFAULT | LV_STATE_FOCUSED, LV_OPA_TRANSP);
	lv_style_set_outline_opa(&buttonStyle, LV_STATE_DEFAULT | LV_STATE_FOCUSED, LV_OPA_TRANSP);
}

static void buttonSetup(lv_obj_t* buttonRef, lv_coord_t x, lv_coord_t y, lv_style_t* const style, void (*cb)(lv_obj_t*, lv_event_t)){
	buttonRef = lv_btn_create(tileref, NULL);
    lv_obj_set_pos(buttonRef, x, y);
    lv_obj_set_size(buttonRef, 120, 40);
	lv_obj_add_style(buttonRef, 0, style);

	lv_obj_set_event_cb(buttonRef, cb);
}

void casio676_tile_setup(void){
	if(casio676tile_init){
		log_e("casio676tile already inited");
		return;
	}

	casio676tile_num = mainbar_add_tile( 4, 1, "casio676 tile", ws_get_mainbar_style() );
	tileref = mainbar_get_tile_obj(casio676tile_num);

	//gui setup
	displaySetup();
	buttonStyleSetup();
	buttonSetup(buttonA, 0, 30, &buttonStyle, buttonA_event_cb);
	buttonSetup(buttonC, 0, 70, &buttonStyle, buttonC_event_cb);
	buttonSetup(buttonL, 120, 30, &buttonStyle, buttonL_event_cb);
	buttonSetup(buttonB, 120, 70, &buttonStyle, buttonB_event_cb);

    //Casio676 core setup
    casio = new Casio676(dotMatrixLabel, smallDigitLabel, largeDigitLabel);
    casio676Task = lv_task_create(casio676task, 500u, LV_TASK_PRIO_MID, NULL);

	casio676tile_init = true;
}

static void casio676task(lv_task_t* task){
	casio->periodic();
}

uint32_t casio676_tile_get_tile_num(void){
	if(!casio676tile_init){
		log_e("casio676tile not inited");
		while(true);
	}

	return casio676tile_num;
}
