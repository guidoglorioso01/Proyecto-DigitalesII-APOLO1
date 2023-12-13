#include <gui/entradamenu_screen/EntradaMenuView.hpp>

EntradaMenuView::EntradaMenuView()
{

}

void EntradaMenuView::setupScreen()
{
	get_user_data(&buff_data);
    EntradaMenuViewBase::setupScreen();
    UpdateBalanceInput();
    UpdateInputAudio();
    UpdateInputChannel();
}

void EntradaMenuView::tearDownScreen()
{
	set_user_data(buff_data);
    EntradaMenuViewBase::tearDownScreen();
}

void EntradaMenuView::GFXInputAudio(){

	if(buff_data.audio_input.type_in == AUX)
		buff_data.audio_input.type_in = BLUT;
	else
		buff_data.audio_input.type_in = AUX;
	UpdateInputAudio();
}
void EntradaMenuView::GFXCanalSelecIN(){

	switch(buff_data.audio_input.channel){
		case LEFT_CHANNEL_INPUT:
			buff_data.audio_input.channel = RIGHT_CHANNEL_INPUT;
		break;
		case RIGHT_CHANNEL_INPUT:
			buff_data.audio_input.channel = STEREO_CHANNEL_INPUT;
		break;
		default:
		case STEREO_CHANNEL_INPUT:
			buff_data.audio_input.channel = LEFT_CHANNEL_INPUT;
		break;
	}
	UpdateInputChannel();
}

void EntradaMenuView::GFXBalanceChange(int value){
	buff_data.audio_input.channel_balance = value;
}
void EntradaMenuView::UpdateInputAudio(){
	if(buff_data.audio_input.type_in == AUX)
		this->ButtEntradaAudio.setLabelText(touchgfx::TypedText(T_AUX_INAUDIO));
	else
		this->ButtEntradaAudio.setLabelText(touchgfx::TypedText(T_BLUETOOTH_INAUDIO));
}

void EntradaMenuView::UpdateInputChannel(){

	switch(buff_data.audio_input.channel){
		case LEFT_CHANNEL_INPUT:
			this->ButtCanalAudioIN.setLabelText(touchgfx::TypedText(T_DERECHO_INAUDIO));
			break;
		case RIGHT_CHANNEL_INPUT:
			this->ButtCanalAudioIN.setLabelText(touchgfx::TypedText(T_IZQUIERDO_INAUDIO));
			break;
		default:
		case STEREO_CHANNEL_INPUT:
			this->ButtCanalAudioIN.setLabelText(touchgfx::TypedText(T_ESTEREO_INAUDIO));
			break;
	}
}
void EntradaMenuView::UpdateBalanceInput(){
	this->sliderBalance.setValue(buff_data.audio_input.channel_balance );

}
