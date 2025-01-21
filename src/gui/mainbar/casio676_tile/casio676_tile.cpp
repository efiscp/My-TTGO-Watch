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
#include "gui/statusbar.h"
#include "app/casio676/casio676.hpp"
#include "gui/app.h"

#define SCREEN_WIDTH				240
#define SCREEN_HEIGHT				240

#define KEYPAD_WIDTH				SCREEN_WIDTH
#define KEYPAD_HEIGHT				140

#define KEYPAD_POS_X				0
#define KEYPAD_POS_Y				(SCREEN_HEIGHT - KEYPAD_HEIGHT)

#define DOTMATRIX_TEXT_POS_X		30
#define DOTMATRIX_TEXT_POS_Y		10
#define SMALL_DIGIT_TEXT_POS_X		150
#define SMALL_DIGIT_TEXT_POS_Y		10
#define LARGE_DIGIT_TEXT_POS_X		30
#define LARGE_DIGIT_TEXT_POS_Y		35


#define BUTTON_WIDTH				(SCREEN_WIDTH / 2)
#define BUTTON_HEIGHT				(KEYPAD_POS_Y / 2)
#define BUTTON_B_POS_X				0
#define BUTTON_B_POS_Y				0
#define BUTTON_C_POS_X				0
#define BUTTON_C_POS_Y				(BUTTON_B_POS_Y + BUTTON_HEIGHT)
#define BUTTON_L_POS_X				(BUTTON_B_POS_X + BUTTON_WIDTH)
#define BUTTON_L_POS_Y				0
#define BUTTON_A_POS_X				(BUTTON_C_POS_X + BUTTON_WIDTH)
#define BUTTON_A_POS_Y				(BUTTON_L_POS_Y + BUTTON_HEIGHT)

#define KEYPAD_KEY_WIDTH			(KEYPAD_WIDTH / 4)
#define KEYPAD_KEY_HEIGHT			(KEYPAD_HEIGHT / 4)

#define KEYPAD_KEY_ROW_0			KEYPAD_POS_Y
#define KEYPAD_KEY_ROW_1			(KEYPAD_KEY_ROW_0 + KEYPAD_KEY_HEIGHT)
#define KEYPAD_KEY_ROW_2			(KEYPAD_KEY_ROW_1 + KEYPAD_KEY_HEIGHT)
#define KEYPAD_KEY_ROW_3			(KEYPAD_KEY_ROW_2 + KEYPAD_KEY_HEIGHT)

#define KEYPAD_KEY_COL_0			KEYPAD_POS_X
#define KEYPAD_KEY_COL_1			KEYPAD_KEY_COL_0 + KEYPAD_KEY_WIDTH
#define KEYPAD_KEY_COL_2			KEYPAD_KEY_COL_1 + KEYPAD_KEY_WIDTH
#define KEYPAD_KEY_COL_3			KEYPAD_KEY_COL_2 + KEYPAD_KEY_WIDTH


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

static lv_obj_t* keypad = NULL;

static lv_task_t* casio676Task;

static Casio676* casio;

static void casio676task(lv_task_t* task);
static void casioActivateCb(lv_obj_t * obj, lv_event_t event);

GEN_BUTTON(buttonA, BUTTON_A_PRESSED)
GEN_BUTTON(buttonB, BUTTON_B_PRESSED)
GEN_BUTTON(buttonC, BUTTON_C_PRESSED)
GEN_BUTTON(buttonL, BUTTON_L_PRESSED)

GEN_BUTTON(keyRevDivMc, KEY_DIV_MC_REV_PRESSED)
GEN_BUTTON(keyABMulMr, KEY_A_B_MUL_MR_PRESSED)
GEN_BUTTON(keyCDMinMmin, KEY_C_D_MIN_MMIN_PRESSED)
GEN_BUTTON(keyFwdPlusMplus, KEY_FWD_PLUS_MPLUS_PRESSED)

GEN_BUTTON(keyEF7, KEY_E_F_7_PRESSED)
GEN_BUTTON(keyGH8, KEY_G_H_8_PRESSED)
GEN_BUTTON(keyIJ9, KEY_I_J_9_PRESSED)
GEN_BUTTON(keyKL0, KEY_K_L_0_PRESSED)

GEN_BUTTON(keyMN4, KEY_M_N_4_PRESSED)
GEN_BUTTON(keyOP5, KEY_O_P_5_PRESSED)
GEN_BUTTON(keyQR6, KEY_Q_R_6_PRESSED)
GEN_BUTTON(keySTDot, KEY_S_T_DOT_PRESSED)

GEN_BUTTON(keyUV1, KEY_U_V_1_PRESSED)
GEN_BUTTON(keyWX2, KEY_W_X_2_PRESSED)
GEN_BUTTON(keyYZ3, KEY_Y_Z_3_PRESSED)
GEN_BUTTON(keySpaceEq, KEY_SPACE_EQ_PRESSED)

LV_IMG_DECLARE(calc_app_64px);
LV_IMG_DECLARE(dbc610_keypad);

typedef struct{
	lv_obj_t* buttonRef;
	uint8_t posX;
	uint8_t posY;
	void (*eventCbPtr)(lv_obj_t * obj, lv_event_t event);
}ButtonParams_t;

static const ButtonParams_t buttonParams[] = {
		{buttonA, BUTTON_A_POS_X, BUTTON_A_POS_Y, &buttonA_event_cb},
		{buttonB, BUTTON_B_POS_X, BUTTON_B_POS_Y, &buttonB_event_cb},
		{buttonL, BUTTON_L_POS_X, BUTTON_L_POS_Y, &buttonL_event_cb},
		{buttonC, BUTTON_C_POS_X, BUTTON_C_POS_Y, &buttonC_event_cb},

		{keyRevDivMc, KEYPAD_KEY_COL_0, KEYPAD_KEY_ROW_0, &keyRevDivMc_event_cb},
		{keyABMulMr, KEYPAD_KEY_COL_1, KEYPAD_KEY_ROW_0, &keyABMulMr_event_cb},
		{keyCDMinMmin, KEYPAD_KEY_COL_2, KEYPAD_KEY_ROW_0, &keyCDMinMmin_event_cb},
		{keyFwdPlusMplus, KEYPAD_KEY_COL_3, KEYPAD_KEY_ROW_0, &keyFwdPlusMplus_event_cb},

		{keyEF7, KEYPAD_KEY_COL_0, KEYPAD_KEY_ROW_1, &keyEF7_event_cb},
		{keyGH8, KEYPAD_KEY_COL_1, KEYPAD_KEY_ROW_1, &keyGH8_event_cb},
		{keyIJ9, KEYPAD_KEY_COL_2, KEYPAD_KEY_ROW_1, &keyIJ9_event_cb},
		{keyKL0, KEYPAD_KEY_COL_3, KEYPAD_KEY_ROW_1, &keyKL0_event_cb},

		{keyMN4, KEYPAD_KEY_COL_0, KEYPAD_KEY_ROW_2, &keyMN4_event_cb},
		{keyOP5, KEYPAD_KEY_COL_1, KEYPAD_KEY_ROW_2, &keyOP5_event_cb},
		{keyQR6, KEYPAD_KEY_COL_2, KEYPAD_KEY_ROW_2, &keyQR6_event_cb},
		{keySTDot, KEYPAD_KEY_COL_3, KEYPAD_KEY_ROW_2, &keySTDot_event_cb},

		{keyUV1, KEYPAD_KEY_COL_0, KEYPAD_KEY_ROW_3, &keyUV1_event_cb},
		{keyWX2, KEYPAD_KEY_COL_1, KEYPAD_KEY_ROW_3, &keyWX2_event_cb},
		{keyYZ3, KEYPAD_KEY_COL_2, KEYPAD_KEY_ROW_3, &keyYZ3_event_cb},
		{keySpaceEq, KEYPAD_KEY_COL_3, KEYPAD_KEY_ROW_3, &keySpaceEq_event_cb},
};

void casio676_tile_setup(void);
static int registed = app_autocall_function(&casio676_tile_setup, 1);

static void displaySetup(void){
	dotMatrixLabel = lv_label_create(tileref, NULL);
	smallDigitLabel = lv_label_create(tileref, NULL);
	largeDigitLabel = lv_label_create(tileref, NULL);

	lv_obj_set_style_local_text_font(dotMatrixLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &dotmatrix_24px);
	lv_obj_set_style_local_text_font(smallDigitLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &sevenSegment_24px);
	lv_obj_set_style_local_text_font(largeDigitLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &sevenSegment_48px);

	lv_obj_set_pos(dotMatrixLabel, DOTMATRIX_TEXT_POS_X, DOTMATRIX_TEXT_POS_Y);
	lv_obj_set_pos(smallDigitLabel, SMALL_DIGIT_TEXT_POS_X, SMALL_DIGIT_TEXT_POS_Y);
	lv_obj_set_pos(largeDigitLabel, LARGE_DIGIT_TEXT_POS_X, LARGE_DIGIT_TEXT_POS_Y);

	lv_obj_set_style_local_text_color(dotMatrixLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_obj_set_style_local_text_color(smallDigitLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_obj_set_style_local_text_color(largeDigitLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);

	//lv_obj_set_style_local_bg_color(tileref, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xb0, 0xb0, 0xb0));
	lv_obj_set_style_local_bg_color(tileref, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xc2, 0xd3, 0xcb));
	lv_obj_set_style_local_bg_opa(tileref, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);

}

//#define CASIO_BUTTON_DEBUG
static void buttonStyleSetup(void){
	lv_style_init(&buttonStyle);
	lv_style_set_radius(&buttonStyle, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_opa(&buttonStyle, LV_STATE_DEFAULT, LV_OPA_TRANSP);

#ifndef CASIO_BUTTON_DEBUG
	lv_style_set_border_width(&buttonStyle, LV_STATE_DEFAULT, 0);
	lv_style_set_border_opa(&buttonStyle, LV_STATE_DEFAULT | LV_STATE_FOCUSED, LV_OPA_TRANSP);
	lv_style_set_outline_opa(&buttonStyle, LV_STATE_DEFAULT | LV_STATE_FOCUSED, LV_OPA_TRANSP);
#endif
}

static void buttonSetup(lv_obj_t* buttonRef, lv_coord_t x, lv_coord_t y, lv_style_t* const style, void (*cb)(lv_obj_t*, lv_event_t)){
	buttonRef = lv_btn_create(tileref, NULL);
    lv_obj_set_pos(buttonRef, x, y);
    lv_obj_set_size(buttonRef, BUTTON_WIDTH, BUTTON_HEIGHT);
	lv_obj_add_style(buttonRef, 0, style);

	lv_obj_set_event_cb(buttonRef, cb);
}

void casio676_tile_setup(void){

	if( !registed )
		return;

	casio676tile_num = mainbar_add_app_tile(1, 1, "CASIO");

	tileref = mainbar_get_tile_obj(casio676tile_num);
	app_register("CASIO", &calc_app_64px, &casioActivateCb);
	//mainbar_add_tile_button_cb(casio676tile_num, &casioActivateCb);

	//gui setup
	displaySetup();
	buttonStyleSetup();

	for(uint8_t i = 0; i < sizeof(buttonParams)/sizeof(buttonParams[0]); ++i)
		buttonSetup(buttonParams[i].buttonRef, buttonParams[i].posX, buttonParams[i].posY, &buttonStyle, buttonParams[i].eventCbPtr);

	keypad = lv_img_create(tileref, NULL);
	lv_img_set_src(keypad, &dbc610_keypad);
	lv_obj_set_pos(keypad, KEYPAD_POS_X, KEYPAD_POS_Y);

    //Casio676 core setup
    casio = new Casio676(dotMatrixLabel, smallDigitLabel, largeDigitLabel);
    casio676Task = lv_task_create(casio676task, 100u, LV_TASK_PRIO_MID, NULL);

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

static void casioActivateCb(lv_obj_t * obj, lv_event_t event){
	if(LV_EVENT_CLICKED == event){
		log_d("Casio activate CB");
		mainbar_jump_to_tilenumber(casio676tile_num, LV_ANIM_OFF, true);
	}
}





