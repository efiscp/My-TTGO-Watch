#include "keypad.hpp"

template<const char... C>
const char KeypadKey<C...>::get(void){
	const char c = chars[index];
	index = (index + 1) % sizeof...(C);
	return c;
}

template<const char... C>
void KeypadKey<C...>::reset(void){
	index = 0;
}

bool Keypad::get(CasioEvent_t e, char* key){
	if(keymap.find(e) != keymap.end()){
		*key = keymap[e]->get();
		return true;
	}

	return false;
}

void Keypad::reset(void){
	for(auto itr = keymap.begin(); itr != keymap.end(); ++itr)
		itr->second->reset();
}

NumericKeypad::NumericKeypad(void){
	keymap.insert(std::pair<CasioEvent_t, KeypadKeyInterface*>(KEY_K_L_0_PRESSED, new KeypadKey<'0'>));
	keymap.insert(std::pair<CasioEvent_t, KeypadKeyInterface*>(KEY_U_V_1_PRESSED, new KeypadKey<'1'>));
	keymap.insert(std::pair<CasioEvent_t, KeypadKeyInterface*>(KEY_W_X_2_PRESSED, new KeypadKey<'2'>));
	keymap.insert(std::pair<CasioEvent_t, KeypadKeyInterface*>(KEY_Y_Z_3_PRESSED, new KeypadKey<'3'>));
	keymap.insert(std::pair<CasioEvent_t, KeypadKeyInterface*>(KEY_M_N_4_PRESSED, new KeypadKey<'4'>));
	keymap.insert(std::pair<CasioEvent_t, KeypadKeyInterface*>(KEY_O_P_5_PRESSED, new KeypadKey<'5'>));
	keymap.insert(std::pair<CasioEvent_t, KeypadKeyInterface*>(KEY_Q_R_6_PRESSED, new KeypadKey<'6'>));
	keymap.insert(std::pair<CasioEvent_t, KeypadKeyInterface*>(KEY_E_F_7_PRESSED, new KeypadKey<'7'>));
	keymap.insert(std::pair<CasioEvent_t, KeypadKeyInterface*>(KEY_G_H_8_PRESSED, new KeypadKey<'8'>));
	keymap.insert(std::pair<CasioEvent_t, KeypadKeyInterface*>(KEY_I_J_9_PRESSED, new KeypadKey<'9'>));
}

AlphanumericKeypad::AlphanumericKeypad(void){
	keymap.insert(std::pair<CasioEvent_t, KeypadKeyInterface*>(KEY_A_B_MUL_MR_PRESSED, new KeypadKey<'A','B', ':'>));
	keymap.insert(std::pair<CasioEvent_t, KeypadKeyInterface*>(KEY_C_D_MIN_MMIN_PRESSED, new KeypadKey<'C', 'D'>));
	keymap.insert(std::pair<CasioEvent_t, KeypadKeyInterface*>(KEY_E_F_7_PRESSED, new KeypadKey<'E', 'F', '7'>));
	keymap.insert(std::pair<CasioEvent_t, KeypadKeyInterface*>(KEY_G_H_8_PRESSED, new KeypadKey<'G', 'H', '8'>));
	keymap.insert(std::pair<CasioEvent_t, KeypadKeyInterface*>(KEY_I_J_9_PRESSED, new KeypadKey<'I', 'J', '9'>));
	keymap.insert(std::pair<CasioEvent_t, KeypadKeyInterface*>(KEY_K_L_0_PRESSED, new KeypadKey<'K', 'L', '0'>));
	keymap.insert(std::pair<CasioEvent_t, KeypadKeyInterface*>(KEY_M_N_4_PRESSED, new KeypadKey<'M', 'N', '4'>));
	keymap.insert(std::pair<CasioEvent_t, KeypadKeyInterface*>(KEY_O_P_5_PRESSED, new KeypadKey<'O', 'P', '5'>));
	keymap.insert(std::pair<CasioEvent_t, KeypadKeyInterface*>(KEY_Q_R_6_PRESSED, new KeypadKey<'Q', 'R', '6'>));
	keymap.insert(std::pair<CasioEvent_t, KeypadKeyInterface*>(KEY_S_T_DOT_PRESSED, new KeypadKey<'S', 'T', '.'>));
	keymap.insert(std::pair<CasioEvent_t, KeypadKeyInterface*>(KEY_U_V_1_PRESSED, new KeypadKey<'U', 'V', '1'>));
	keymap.insert(std::pair<CasioEvent_t, KeypadKeyInterface*>(KEY_W_X_2_PRESSED, new KeypadKey<'W', 'X', '2'>));
	keymap.insert(std::pair<CasioEvent_t, KeypadKeyInterface*>(KEY_Y_Z_3_PRESSED, new KeypadKey<'X', 'Y', '3'>));
	keymap.insert(std::pair<CasioEvent_t, KeypadKeyInterface*>(KEY_SPACE_EQ_PRESSED, new KeypadKey<' ', '='>));
}

template<const char... C>
constexpr std::array<const char, sizeof...(C)> KeypadKey<C...>::chars;
