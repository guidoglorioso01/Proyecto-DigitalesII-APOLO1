#include <gui/generalmenu_screen/GeneralMenuView.hpp>
//extern "C" {
//    #include "user_info.h"
//}

extern SemaphoreHandle_t semCalcCoefs;



GeneralMenuView::GeneralMenuView()
{

}

void GeneralMenuView::setupScreen()
{
    GeneralMenuViewBase::setupScreen();
    // Cargo informacion usuario
    get_user_data(&buff_data);
    UpdateTypeSystem();
    UpdateEcProfile();
    UpdateLoudnessState();
    UpdateMaxVolume();
}

void GeneralMenuView::tearDownScreen()
{
	set_user_data(buff_data);
	xSemaphoreGive(semCalcCoefs);
    GeneralMenuViewBase::tearDownScreen();
}
void GeneralMenuView::GFXCambioEcualizador(){
	switch(buff_data.general_config.equalization_type){
		default:
		case EQ_PROF_PLANO:
			buff_data.general_config.equalization_type = EQ_PROF_POP;
			break;
		case EQ_PROF_POP:
			buff_data.general_config.equalization_type = EQ_PROF_ROCK;
			break;
		case EQ_PROF_ROCK:
			buff_data.general_config.equalization_type = EQ_PROF_ELECTRO;
			break;
		case EQ_PROF_ELECTRO:
			buff_data.general_config.equalization_type = EQ_PROF_VOCAL;
			break;
		case EQ_PROF_VOCAL:
			buff_data.general_config.equalization_type = EQ_PROF_CINE;
			break;
		case EQ_PROF_CINE:
			buff_data.general_config.equalization_type = EQ_PROF_PERS;
			break;
		case EQ_PROF_PERS:
			buff_data.general_config.equalization_type = EQ_PROF_PLANO;
			break;
	}
	UpdateEcProfile();
}
void GeneralMenuView::GFXTipoSistemaChange(){

	switch(buff_data.general_config.system_type){
		default:
		case SYSTEM_2_0:
			buff_data.general_config.system_type = SYSTEM_2_1;
			break;
		case SYSTEM_2_1:
			buff_data.general_config.system_type = SYSTEM_3_0;
			break;
		case SYSTEM_3_0:
			buff_data.general_config.system_type = SYSTEM_3_1;
			break;
		case SYSTEM_3_1:
			buff_data.general_config.system_type = SYSTEM_4_0;
			break;
		case SYSTEM_4_0:
			buff_data.general_config.system_type = SYSTEM_PERS;
			break;
		case SYSTEM_PERS:
			buff_data.general_config.system_type = SYSTEM_2_0;
			break;
	}
	UpdateTypeSystem();
}

void GeneralMenuView::GFXChangeLoudnessState(){
	buff_data.general_config.loudness_state = !	buff_data.general_config.loudness_state;
	UpdateLoudnessState();
}

void GeneralMenuView::GFXVolumenMaximoChange(int value){
	buff_data.general_config.max_volume= (uint8_t) value;
}


void GeneralMenuView::UpdateTypeSystem(){

	switch(buff_data.general_config.system_type){
		default:
		case SYSTEM_2_0:
			this->ButtSistema.setLabelText(touchgfx::TypedText(T_DOS_CERO_CONFIGRAP));
			buff_data.audio_output[0].state = ON;
			buff_data.audio_output[1].state = ON;
			buff_data.audio_output[2].state = OFF;
			buff_data.audio_output[3].state = OFF;
			buff_data.audio_output[0].channel_audio = LEFT_CHANNEL_OUTPUT;
			buff_data.audio_output[1].channel_audio = RIGHT_CHANNEL_OUTPUT;
			buff_data.audio_output[2].channel_audio = LEFT_CHANNEL_OUTPUT;
			buff_data.audio_output[3].channel_audio = RIGHT_CHANNEL_OUTPUT;
			buff_data.audio_output[0].type_equalizer = TYPE_FLAT;
			buff_data.audio_output[1].type_equalizer = TYPE_FLAT;
			buff_data.audio_output[2].type_equalizer = TYPE_FLAT;
			buff_data.audio_output[3].type_equalizer = TYPE_FLAT;
			break;
		case SYSTEM_2_1:
			this->ButtSistema.setLabelText(touchgfx::TypedText(T_DOS_UNO_CONFIGRAP));
			buff_data.audio_output[0].state = ON;
			buff_data.audio_output[1].state = ON;
			buff_data.audio_output[2].state = ON;
			buff_data.audio_output[3].state = OFF;
			buff_data.audio_output[0].channel_audio = LEFT_CHANNEL_OUTPUT;
			buff_data.audio_output[1].channel_audio = RIGHT_CHANNEL_OUTPUT;
			buff_data.audio_output[2].channel_audio = LEFT_CHANNEL_OUTPUT; // Hay que implentar el MONO
			buff_data.audio_output[3].channel_audio = RIGHT_CHANNEL_OUTPUT;
			buff_data.audio_output[0].type_equalizer = TYPE_HP1;
			buff_data.audio_output[1].type_equalizer = TYPE_HP1;
			buff_data.audio_output[2].type_equalizer = TYPE_LP1;
			buff_data.audio_output[3].type_equalizer = TYPE_FLAT;
		break;
		case SYSTEM_3_0:
			this->ButtSistema.setLabelText(touchgfx::TypedText(T_TRES_CERO_CONFIGRAP));
			buff_data.audio_output[0].state = ON;
			buff_data.audio_output[1].state = ON;
			buff_data.audio_output[2].state = ON;
			buff_data.audio_output[3].state = OFF;
			buff_data.audio_output[0].channel_audio = LEFT_CHANNEL_OUTPUT;
			buff_data.audio_output[1].channel_audio = RIGHT_CHANNEL_OUTPUT;
			buff_data.audio_output[2].channel_audio = LEFT_CHANNEL_OUTPUT; // Hay que implentar el MONO
			buff_data.audio_output[3].channel_audio = RIGHT_CHANNEL_OUTPUT;
			buff_data.audio_output[0].type_equalizer = TYPE_FLAT;
			buff_data.audio_output[1].type_equalizer = TYPE_FLAT;
			buff_data.audio_output[2].type_equalizer = TYPE_HP1;
			buff_data.audio_output[3].type_equalizer = TYPE_FLAT;
			break;
		case SYSTEM_3_1:
			this->ButtSistema.setLabelText(touchgfx::TypedText(T_TRES_UNO_CONFIGRAP));
			buff_data.audio_output[0].state = ON;
			buff_data.audio_output[1].state = ON;
			buff_data.audio_output[2].state = ON;
			buff_data.audio_output[3].state = ON;
			buff_data.audio_output[0].channel_audio = LEFT_CHANNEL_OUTPUT;
			buff_data.audio_output[1].channel_audio = RIGHT_CHANNEL_OUTPUT;
			buff_data.audio_output[2].channel_audio = LEFT_CHANNEL_OUTPUT; // Hay que implentar el MONO
			buff_data.audio_output[3].channel_audio = RIGHT_CHANNEL_OUTPUT;
			buff_data.audio_output[0].type_equalizer = TYPE_HP1;
			buff_data.audio_output[1].type_equalizer = TYPE_HP1;
			buff_data.audio_output[2].type_equalizer = TYPE_HP1;
			buff_data.audio_output[3].type_equalizer = TYPE_LP1;
			break;
		case SYSTEM_4_0:
			this->ButtSistema.setLabelText(touchgfx::TypedText(T_CUATRO_CERO_CONFIGRAP));
			buff_data.audio_output[0].state = ON;
			buff_data.audio_output[1].state = ON;
			buff_data.audio_output[2].state = ON;
			buff_data.audio_output[3].state = ON;
			buff_data.audio_output[0].channel_audio = LEFT_CHANNEL_OUTPUT;
			buff_data.audio_output[1].channel_audio = RIGHT_CHANNEL_OUTPUT;
			buff_data.audio_output[2].channel_audio = LEFT_CHANNEL_OUTPUT;
			buff_data.audio_output[3].channel_audio = RIGHT_CHANNEL_OUTPUT;
			buff_data.audio_output[0].type_equalizer = TYPE_HP1;
			buff_data.audio_output[1].type_equalizer = TYPE_HP1;
			buff_data.audio_output[2].type_equalizer = TYPE_HP1;
			buff_data.audio_output[3].type_equalizer = TYPE_HP1;
			break;
		case SYSTEM_PERS:
			this->ButtSistema.setLabelText(touchgfx::TypedText(T_PERS_SYSTEM_CONFIGRAP));
			buff_data.audio_output[0].state = buff_data.audio_output_pers[0].state;
			buff_data.audio_output[1].state = buff_data.audio_output_pers[1].state;
			buff_data.audio_output[2].state = buff_data.audio_output_pers[2].state;
			buff_data.audio_output[3].state = buff_data.audio_output_pers[3].state;
			buff_data.audio_output[0].channel_audio = buff_data.audio_output_pers[0].channel_audio ;
			buff_data.audio_output[1].channel_audio = buff_data.audio_output_pers[0].channel_audio;
			buff_data.audio_output[2].channel_audio = buff_data.audio_output_pers[0].channel_audio;
			buff_data.audio_output[3].channel_audio = buff_data.audio_output_pers[0].channel_audio;
			buff_data.audio_output[0].type_equalizer = buff_data.audio_output_pers[0].type_equalizer;
			buff_data.audio_output[1].type_equalizer = buff_data.audio_output_pers[0].type_equalizer;
			buff_data.audio_output[2].type_equalizer = buff_data.audio_output_pers[0].type_equalizer;
			buff_data.audio_output[3].type_equalizer = buff_data.audio_output_pers[0].type_equalizer;
			break;
	}

}

void GeneralMenuView::UpdateEcProfile(){

	int8_t ecualizador[7];

	switch(buff_data.general_config.equalization_type){
		default:
		case EQ_PROF_PLANO:
			this->ButtEc.setLabelText(touchgfx::TypedText(T_PLANO_CONFIGRAP));
			FLAT_PROFILE(ecualizador);
			config_eq(ecualizador,&buff_data.general_equalizer);
			break;
		case EQ_PROF_POP:
			this->ButtEc.setLabelText(touchgfx::TypedText(T_POP_CONFIGRAP));
			POP_PROFILE(ecualizador);
			config_eq(ecualizador,&buff_data.general_equalizer);
			break;
		case EQ_PROF_ROCK:
			this->ButtEc.setLabelText(touchgfx::TypedText(T_ROCK_CONFIGRAP));
			ROCK_PROFILE(ecualizador);
			config_eq(ecualizador,&buff_data.general_equalizer);
			break;
		case EQ_PROF_ELECTRO:
			this->ButtEc.setLabelText(touchgfx::TypedText(T_ELECTO_CONFIGRAP));
			ELECTRO_PROFILE(ecualizador);
			config_eq(ecualizador,&buff_data.general_equalizer);
			break;
		case EQ_PROF_VOCAL:
			this->ButtEc.setLabelText(touchgfx::TypedText(T_VOCAL_CONFIGRAP));
			VOCAL_PROFILE(ecualizador);
			config_eq(ecualizador,&buff_data.general_equalizer);
			break;
		case EQ_PROF_CINE:
			this->ButtEc.setLabelText(touchgfx::TypedText(T_CINE_CONFIGRAP));
			CINE_PROFILE(ecualizador);
			config_eq(ecualizador,&buff_data.general_equalizer);
			break;
		case EQ_PROF_PERS:
			this->ButtEc.setLabelText(touchgfx::TypedText(T_PERS_CONFIGRAP));
			buff_data.general_equalizer._20hz = buff_data.general_equalizer_pers._20hz;
			buff_data.general_equalizer._200hz = buff_data.general_equalizer_pers._200hz;
			buff_data.general_equalizer._500hz = buff_data.general_equalizer_pers._500hz;
			buff_data.general_equalizer._1000hz = buff_data.general_equalizer_pers._1000hz;
			buff_data.general_equalizer._4000hz = buff_data.general_equalizer_pers._4000hz;
			buff_data.general_equalizer._8000hz = buff_data.general_equalizer_pers._8000hz;
			buff_data.general_equalizer._16000hz = buff_data.general_equalizer_pers._16000hz;

			break;
		}
}
void GeneralMenuView::UpdateLoudnessState(){
	if(buff_data.general_config.loudness_state == ON)
		this->TogEstadoLoudness.forceState(ON);
	else
		this->TogEstadoLoudness.forceState(OFF);


}
void GeneralMenuView::UpdateMaxVolume(){
	this->sliderVolumenMaximo.setValue(buff_data.general_config.max_volume);
}




