#ifndef SRC_APP_CASIO676_CASIODISPLAYMANAGER_HPP_
#define SRC_APP_CASIO676_CASIODISPLAYMANAGER_HPP_

#include "lvgl.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#endif

class Casio676_DisplayManager{
public:
	Casio676_DisplayManager(lv_obj_t* dotMatrix, lv_obj_t* smallDigit, lv_obj_t* largeDigit):
		dotMatrix(dotMatrix), smallDigit(smallDigit), largeDigit(largeDigit){}

	void setDotMatrixText(const std::string& text){
		setDotMatrixText(text.c_str());
	}

	void setDotMatrixText(const char* text){
		dotMatrixWrapper.setText(text);
		lv_label_set_text(dotMatrix, dotMatrixWrapper.getCurrent().c_str());
	}

	void setSmallDigitText(const std::string& text){
		setSmallDigitText(text.c_str());
	}

	void setSmallDigitText(const char* text){
		smallDigitWrapper.setText(text);
		lv_label_set_text(smallDigit, smallDigitWrapper.getCurrent().c_str());
	}

	void setLargeDigitText(const std::string& text){
		setLargeDigitText(text.c_str());
	}

	void setLargeDigitText(const char* text){
		largeDigitWrapper.setText(text);
		lv_label_set_text(largeDigit, largeDigitWrapper.getCurrent().c_str());
	}

	void blinkLargeDigitText(char c, uint8_t start, uint8_t width){
		largeDigitWrapper.setBlinkText(c, start, width);
	}

	void blinkSmallDigitText(char c, uint8_t start, uint8_t width){
		smallDigitWrapper.setBlinkText(c, start, width);
	}

	void blinkDotMatrixText(char c, uint8_t start, uint8_t width){
		dotMatrixWrapper.setBlinkText(c, start, width);
	}

	void startLargeDigitBlinking(void){
		largeDigitWrapper.start();
	}

	void startSmallDigitBlinking(void){
		smallDigitWrapper.start();
	}

	void startDotMatrixBlinking(void){
		dotMatrixWrapper.start();
	}

	void stopBlinking(bool refresh){
		largeDigitWrapper.stop();
		smallDigitWrapper.stop();
		dotMatrixWrapper.stop();

		if(refresh){
			lv_label_set_text(dotMatrix, dotMatrixWrapper.getCurrent().c_str());
			lv_label_set_text(smallDigit, smallDigitWrapper.getCurrent().c_str());
			lv_label_set_text(largeDigit, largeDigitWrapper.getCurrent().c_str());
		}
	}


	void blink(void){
		if(largeDigitWrapper.isBlinking()){
			lv_label_set_text(largeDigit, largeDigitWrapper.getNext().c_str());
			//log_d("blink");
		}

		if(smallDigitWrapper.isBlinking())
			lv_label_set_text(smallDigit, smallDigitWrapper.getNext().c_str());

		if(dotMatrixWrapper.isBlinking())
			lv_label_set_text(dotMatrix, dotMatrixWrapper.getNext().c_str());
	}


private:
	struct TextWrapper{
		TextWrapper(){}

		bool isBlinking(void) const{
			return blinking;
		}

		void setText(const char* str){
			normalText = std::string(str);
			updateBlinkText();
		}

		void setBlinkText(char c, uint8_t start, uint8_t width){
			blinkChar = c;
			blinkStart = start;
			blinkWidth = width;
			updateBlinkText();
		}

		void start(void){
			normal = true;
			blinking = true;
		}

		void stop(void){
			blinking = false;
			normal = true;
		}

		const std::string& getNext(void){
			if(!blinking)
				return normalText;

			normal = !normal;
			return getCurrent();
		}

		const std::string& getCurrent(void){
			return normal ? normalText : blinkText;
		}

	private:
		void updateBlinkText(){
			blinkText = normalText;
			for(uint8_t i = blinkStart; i < blinkStart + blinkWidth; ++i)
				blinkText[i] = blinkChar;
		}

		std::string normalText;
		std::string blinkText;
		bool blinking = false;
		bool normal = true;

		char blinkChar;
		uint8_t blinkStart = 0;
		uint8_t blinkWidth = 0;
	};

		lv_obj_t* dotMatrix;
		lv_obj_t* smallDigit;
		lv_obj_t* largeDigit;

		TextWrapper largeDigitWrapper;
		TextWrapper smallDigitWrapper;
		TextWrapper dotMatrixWrapper;
};



#endif /* SRC_APP_CASIO676_CASIODISPLAYMANAGER_HPP_ */
