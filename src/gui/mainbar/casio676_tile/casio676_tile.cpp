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

extern lv_font_t dotmatrix_16px;
extern lv_font_t dotmatrix_24px;
extern lv_font_t dotmatrix_32px;

extern lv_font_t sevenSegment_12px;
extern lv_font_t sevenSegment_16px;
extern lv_font_t sevenSegment_24px;
extern lv_font_t sevenSegment_32px;
extern lv_font_t sevenSegment_48px;
extern lv_font_t sevenSegment_72px;

static bool casio676tile_init = false;
static uint32_t casio676tile_num;
static lv_obj_t* tileref = NULL;
static lv_obj_t* dotMatrixLabel;
static lv_obj_t* smallDigitLabel;
static lv_obj_t* largeDigitLabel;

static lv_task_t* casio676Task;

static Casio676* casio;



static void dotMatrixLabel_event_cb(lv_obj_t * obj, lv_event_t event);
static void casio676task(lv_task_t* task);

void casio676_tile_setup(void){
	if(casio676tile_init){
		log_e("casio676tile already inited");
		return;
	}

	casio676tile_num = mainbar_add_tile( 4, 1, "casio676 tile", ws_get_mainbar_style() );
	tileref = mainbar_get_tile_obj(casio676tile_num);

	dotMatrixLabel = lv_label_create(tileref, NULL);
	smallDigitLabel = lv_label_create(tileref, NULL);
	largeDigitLabel = lv_label_create(tileref, NULL);

	lv_obj_set_style_local_text_font(dotMatrixLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &dotmatrix_24px);
	lv_obj_set_style_local_text_font(smallDigitLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &sevenSegment_24px);
	lv_obj_set_style_local_text_font(largeDigitLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &sevenSegment_48px);

    lv_obj_set_pos(dotMatrixLabel, 30, 40);
    lv_obj_set_pos(smallDigitLabel, 150, 40);
    lv_obj_set_pos(largeDigitLabel, 30, 75);

    casio = new Casio676(dotMatrixLabel, smallDigitLabel, largeDigitLabel);

    casio676Task = lv_task_create(casio676task, 500u, LV_TASK_PRIO_MID, NULL);

    //lv_obj_set_event_cb(dotMatrixLabel, dotMatrixLabel_event_cb);
    lv_obj_set_event_cb(tileref, dotMatrixLabel_event_cb);

	casio676tile_init = true;
}

static void dotMatrixLabel_event_cb(lv_obj_t * obj, lv_event_t event){
	if(LV_EVENT_CLICKED == event){
		casio->processEvent(BUTTON_C_PRESSED);
	}
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
