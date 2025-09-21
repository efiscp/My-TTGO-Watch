#ifndef SRC_APP_CASIO676_KEYPAD_KEYPAD_HPP_
#define SRC_APP_CASIO676_KEYPAD_KEYPAD_HPP_

#include <array>
#include <map>

enum CasioEvent_t {
	BUTTON_B_PRESSED,		//top left button
	BUTTON_C_PRESSED,		//bottom left button
	BUTTON_L_PRESSED,		//top right button
	BUTTON_A_PRESSED,		//bottom right button

	KEY_DIV_MC_REV_PRESSED,
	KEY_A_B_MUL_MR_PRESSED,
	KEY_C_D_MIN_MMIN_PRESSED,
	KEY_FWD_PLUS_MPLUS_PRESSED,

	KEY_E_F_7_PRESSED,
	KEY_G_H_8_PRESSED,
	KEY_I_J_9_PRESSED,
	KEY_K_L_0_PRESSED,

	KEY_M_N_4_PRESSED,
	KEY_O_P_5_PRESSED,
	KEY_Q_R_6_PRESSED,
	KEY_S_T_DOT_PRESSED,

	KEY_U_V_1_PRESSED,
	KEY_W_X_2_PRESSED,
	KEY_Y_Z_3_PRESSED,
	KEY_SPACE_EQ_PRESSED,
};

struct KeypadKeyInterface{
	virtual ~KeypadKeyInterface(void){}
	virtual const char get(void) = 0;
	virtual void reset(void) = 0;
};

template<const char... C>
struct KeypadKey:public KeypadKeyInterface{
	const char get(void);
	void reset(void);

private:
	static constexpr std::array<const char, sizeof...(C)> chars = {C...};
	size_t index = 0;

};

struct Keypad{
	bool get(CasioEvent_t e, char* key);
	void reset(void);

protected:
	std::map<CasioEvent_t, KeypadKeyInterface*> keymap;
};

struct NumericKeypad:public Keypad{
	NumericKeypad(void);
};

struct AlphanumericKeypad:public Keypad{
	AlphanumericKeypad(void);
};

#endif /* SRC_APP_CASIO676_KEYPAD_KEYPAD_HPP_ */
