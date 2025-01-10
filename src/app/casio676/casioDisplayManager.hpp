#ifndef SRC_APP_CASIO676_CASIODISPLAYMANAGER_HPP_
#define SRC_APP_CASIO676_CASIODISPLAYMANAGER_HPP_

#include "lvgl.h"

class Casio676_DisplayManager{
public:
	Casio676_DisplayManager(lv_obj_t* dotMatrix, lv_obj_t* smallDigit, lv_obj_t* largeDigit):
		dotMatrix(dotMatrix), smallDigit(smallDigit), largeDigit(largeDigit){}

	void setDotMatrixText(const std::string& text){
		lv_label_set_text(dotMatrix, text.c_str());
	}

	void setDotMatrixText(const char* text){
		lv_label_set_text(dotMatrix, text);
	}

	void setSmallDigitText(const std::string& text){
		lv_label_set_text(smallDigit, text.c_str());
	}

	void setSmallDigitText(const char* text){
		lv_label_set_text(smallDigit, text);
	}

	void setLargeDigitText(const std::string& text){
		lv_label_set_text(largeDigit, text.c_str());
	}

	void setLargeDigitText(const char* text){
		lv_label_set_text(largeDigit, text);
	}

private:
		lv_obj_t* dotMatrix;
		lv_obj_t* smallDigit;
		lv_obj_t* largeDigit;
};



#endif /* SRC_APP_CASIO676_CASIODISPLAYMANAGER_HPP_ */
