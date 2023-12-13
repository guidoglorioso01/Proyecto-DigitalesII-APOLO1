#include <gui/canalesmenu_screen/CanalesMenuView.hpp>

CanalesMenuView::CanalesMenuView()
{

}

void CanalesMenuView::setupScreen()
{
	get_user_data(&buff_data);
    CanalesMenuViewBase::setupScreen();
    this->channelInUse = buff_data.channelInUse;

	UpdateStateChannel();
	UpdateVolumeChannel();
	UpdateChannelOutput();
	UpdateChannelSelec();
}

void CanalesMenuView::tearDownScreen()
{
	set_user_data(buff_data);
    CanalesMenuViewBase::tearDownScreen();
}

void CanalesMenuView::GFXCanalSelec()
{

	switch(this->channelInUse){
		default:
		case CHANNEL_0:
			this->channelInUse = CHANNEL_1;
			break;
		case CHANNEL_1:
			this->channelInUse = CHANNEL_2;
			break;
		case CHANNEL_2:
			this->channelInUse = CHANNEL_3;
			break;
		case CHANNEL_3:
			this->channelInUse = CHANNEL_0;
			break;
	}
	UpdateChannelSelec();
	UpdateStateChannel();
	UpdateVolumeChannel();
	UpdateChannelOutput();
}

void CanalesMenuView::UpdateChannelSelec(){
	switch(this->channelInUse){
		default:
		case CHANNEL_0:
			this->ButtCanalSalidaSelec.setLabelText(touchgfx::TypedText(T_CANAL0_CONFIGCANAL));
			break;
		case CHANNEL_1:
			this->ButtCanalSalidaSelec.setLabelText(touchgfx::TypedText(T_CANAL1_CONFIGCANAL));
		break;
		case CHANNEL_2:
			this->ButtCanalSalidaSelec.setLabelText(touchgfx::TypedText(T_CANAL2_CONFIGCANAL));
		break;
		case CHANNEL_3:
			this->ButtCanalSalidaSelec.setLabelText(touchgfx::TypedText(T_CANAL3_CONFIGCANAL));
		break;
	}
	buff_data.channelInUse = this->channelInUse;
}
void CanalesMenuView::GFXEstadoCanal(void){
	if(buff_data.general_config.system_type == SYSTEM_PERS){
		buff_data.audio_output_pers[this->channelInUse].state = !buff_data.audio_output_pers[this->channelInUse].state;
		buff_data.audio_output[this->channelInUse].state =buff_data.audio_output_pers[this->channelInUse].state;
	}
	UpdateStateChannel();

}
void CanalesMenuView::UpdateStateChannel(){
	if(buff_data.audio_output[this->channelInUse].state == ON)
		this->TogEstadoCanal.forceState(ON);
	else
		this->TogEstadoCanal.forceState(OFF);
	this->TogEstadoCanal.invalidate();
}

void CanalesMenuView::GFXVolumenCanalSelec(int value){
	buff_data.audio_output[this->channelInUse].channel_volume = (uint8_t)value;
}


void CanalesMenuView::UpdateVolumeChannel(){
	this->sliderVolume.setValue(buff_data.audio_output[this->channelInUse].channel_volume);
}

void CanalesMenuView::GFXCanalAudio(){
	if(buff_data.general_config.system_type == SYSTEM_PERS){
		if(buff_data.audio_output_pers[this->channelInUse].channel_audio == RIGHT_CHANNEL_OUTPUT)
			buff_data.audio_output_pers[this->channelInUse].channel_audio =LEFT_CHANNEL_OUTPUT;
		else
			buff_data.audio_output_pers[this->channelInUse].channel_audio =RIGHT_CHANNEL_OUTPUT;
		buff_data.audio_output[this->channelInUse].channel_audio=buff_data.audio_output_pers[this->channelInUse].channel_audio;
	}
	UpdateChannelOutput();
}


void CanalesMenuView::UpdateChannelOutput(){
	if(buff_data.audio_output[this->channelInUse].channel_audio == RIGHT_CHANNEL_OUTPUT)
		this->ButtCanalAudio.setLabelText(touchgfx::TypedText(T_DERECHO_CONFIGCANAL));
	else
		this->ButtCanalAudio.setLabelText(touchgfx::TypedText(T_IZQUIERDO_CONFIGCANAL));
	this->ButtCanalAudio.invalidate();
}

