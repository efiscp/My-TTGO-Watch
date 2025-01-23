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

#define DOTMATRIX_TEXT_POS_X		30
#define DOTMATRIX_TEXT_POS_Y		10
#define SMALL_DIGIT_TEXT_POS_X		150
#define SMALL_DIGIT_TEXT_POS_Y		10
#define LARGE_DIGIT_TEXT_POS_X		30
#define LARGE_DIGIT_TEXT_POS_Y		35

extern lv_font_t dotmatrix_24px;
extern lv_font_t sevenSegment_24px;
extern lv_font_t sevenSegment_48px;

static bool casio676tile_init = false;
static uint32_t casio676tile_num;
static lv_obj_t* tileref = NULL;
static lv_obj_t* dotMatrixLabel;
static lv_obj_t* smallDigitLabel;
static lv_obj_t* largeDigitLabel;

static lv_task_t* casio676Task;

static Casio676* casio;

static void casio676task(lv_task_t* task);
static void casioActivateCb(lv_obj_t * obj, lv_event_t event);

LV_IMG_DECLARE(calc_app_64px);
LV_IMG_DECLARE(dbc610_keypad);
#define KEYPAD_HEIGHT 140		//dbc610_keypad.header.h cannot be used

struct ButtonPos{

	static constexpr uint8_t width = SCREEN_WIDTH;
	static constexpr uint8_t height = SCREEN_HEIGHT - KEYPAD_HEIGHT;
	static constexpr uint8_t startPosX = 0;
	static constexpr uint8_t startPosY = 0;
	static constexpr uint8_t rowCount = 2;
	static constexpr uint8_t colCount = 2;
};

struct KeyPos{

	static constexpr uint8_t width = SCREEN_WIDTH;
	static constexpr uint8_t height = KEYPAD_HEIGHT;
	static constexpr uint8_t startPosX = 0;
	static constexpr uint8_t startPosY = SCREEN_HEIGHT - height;
	static constexpr uint8_t rowCount = 4;
	static constexpr uint8_t colCount = 4;
};

template<typename T>
struct PosCalc{

	static constexpr uint8_t calcPosX(uint8_t col){
		return T::startPosX + col * width;
	}

	static constexpr uint8_t calcPosY(uint8_t row){
		return T::startPosY + row * height;
	}

	static constexpr uint8_t width = T::width / T::colCount;
	static constexpr uint8_t height = T::height / T::rowCount;
};

struct StylableKey{
	virtual void setStyle(lv_style_t* style) = 0;
	virtual ~StylableKey(void){}
};

template<typename T, uint8_t R, uint8_t C, CasioEvent_t E>
struct Key: public StylableKey{

	Key(void){
		//register
		buttonRef = lv_btn_create(tileref, NULL);
		lv_obj_set_pos(buttonRef, posX, posY);
		lv_obj_set_size(buttonRef, T::width, T::height);

		lv_obj_set_event_cb(buttonRef, &event_cb);

	}
	void setStyle(lv_style_t* style){
		lv_obj_add_style(buttonRef, 0, style);
	}

	static void event_cb(lv_obj_t * obj, lv_event_t event){
		if(LV_EVENT_PRESSED == event){
			log_d("KEY event %d", E);
			casio->processEvent(E);
		}
	}

private:
	static constexpr uint8_t posX = T::calcPosX(C);
	static constexpr uint8_t posY = T::calcPosY(R);

	lv_obj_t* buttonRef;
};

struct KeyPad{
	KeyPad(void){
		//initialize button style
		lv_style_init(&buttonStyle);
		lv_style_set_radius(&buttonStyle, LV_STATE_DEFAULT, 0);
		lv_style_set_bg_opa(&buttonStyle, LV_STATE_DEFAULT, LV_OPA_TRANSP);

	#ifndef CASIO_BUTTON_DEBUG
		lv_style_set_border_width(&buttonStyle, LV_STATE_DEFAULT, 0);
		lv_style_set_border_opa(&buttonStyle, LV_STATE_DEFAULT | LV_STATE_FOCUSED, LV_OPA_TRANSP);
		lv_style_set_outline_opa(&buttonStyle, LV_STATE_DEFAULT | LV_STATE_FOCUSED, LV_OPA_TRANSP);
	#endif

		//add buttons/keys
		keyList.push_back(new Key<PosCalc<KeyPos>, 0, 0, KEY_DIV_MC_REV_PRESSED>);
		keyList.push_back(new Key<PosCalc<KeyPos>, 0, 1, KEY_A_B_MUL_MR_PRESSED>);
		keyList.push_back(new Key<PosCalc<KeyPos>, 0, 2, KEY_C_D_MIN_MMIN_PRESSED>);
		keyList.push_back(new Key<PosCalc<KeyPos>, 0, 3, KEY_FWD_PLUS_MPLUS_PRESSED>);

		keyList.push_back(new Key<PosCalc<KeyPos>, 1, 0, KEY_E_F_7_PRESSED>);
		keyList.push_back(new Key<PosCalc<KeyPos>, 1, 1, KEY_G_H_8_PRESSED>);
		keyList.push_back(new Key<PosCalc<KeyPos>, 1, 2, KEY_I_J_9_PRESSED>);
		keyList.push_back(new Key<PosCalc<KeyPos>, 1, 3, KEY_K_L_0_PRESSED>);

		keyList.push_back(new Key<PosCalc<KeyPos>, 2, 0, KEY_M_N_4_PRESSED>);
		keyList.push_back(new Key<PosCalc<KeyPos>, 2, 1, KEY_O_P_5_PRESSED>);
		keyList.push_back(new Key<PosCalc<KeyPos>, 2, 2, KEY_Q_R_6_PRESSED>);
		keyList.push_back(new Key<PosCalc<KeyPos>, 2, 3, KEY_S_T_DOT_PRESSED>);

		keyList.push_back(new Key<PosCalc<KeyPos>, 3, 0, KEY_U_V_1_PRESSED>);
		keyList.push_back(new Key<PosCalc<KeyPos>, 3, 1, KEY_W_X_2_PRESSED>);
		keyList.push_back(new Key<PosCalc<KeyPos>, 3, 2, KEY_Y_Z_3_PRESSED>);
		keyList.push_back(new Key<PosCalc<KeyPos>, 3, 3, KEY_SPACE_EQ_PRESSED>);

		keyList.push_back(new Key<PosCalc<ButtonPos>, 0, 0, BUTTON_B_PRESSED>);
		keyList.push_back(new Key<PosCalc<ButtonPos>, 0, 1, BUTTON_L_PRESSED>);
		keyList.push_back(new Key<PosCalc<ButtonPos>, 1, 0, BUTTON_C_PRESSED>);
		keyList.push_back(new Key<PosCalc<ButtonPos>, 1, 1, BUTTON_A_PRESSED>);

		//set key/button style
		for(auto itr = keyList.begin(); itr != keyList.end(); ++itr)
			(*itr)->setStyle(&buttonStyle);

		//add keypad image
		keypad = lv_img_create(tileref, NULL);
		lv_img_set_src(keypad, &dbc610_keypad);
		lv_obj_set_pos(keypad, KeyPos::startPosX, KeyPos::startPosY);
	}

	~KeyPad(void){
		for(auto itr = keyList.begin(); itr != keyList.end(); ++itr)
			delete *itr;
	}

private:
	std::list<StylableKey*> keyList;
	lv_style_t buttonStyle;
	lv_obj_t* keypad = NULL;

};
static KeyPad* k;

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

	k = new KeyPad;

}

void casio676_tile_setup(void){

	if( !registed )
		return;

	casio676tile_num = mainbar_add_app_tile(1, 1, "CASIO");

	tileref = mainbar_get_tile_obj(casio676tile_num);
	app_register("CASIO", &calc_app_64px, &casioActivateCb);

	//gui setup
	displaySetup();

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





