#include <gui/editarecualizador_screen/EditarEcualizadorView.hpp>


EditarEcualizadorView::EditarEcualizadorView()
{

}

void EditarEcualizadorView::setupScreen()
{
	get_user_data(&buff_data);

    EditarEcualizadorViewBase::setupScreen();
    UpdateSlides();
}

void EditarEcualizadorView::tearDownScreen()
{
	set_user_data(buff_data);
	if(buff_data.general_config.equalization_type == EQ_PROF_PERS )
		xSemaphoreGive(semCalcCoefs);
    EditarEcualizadorViewBase::tearDownScreen();
}



void EditarEcualizadorView::GFXSliderEcu1(int value){
	if(buff_data.general_config.equalization_type == EQ_PROF_PERS ){
		buff_data.general_equalizer_pers._20hz = value;
	}
	else{
		this->Slider1.setValue(buff_data.general_equalizer._20hz);
		this->invalidate();
	}
}
void EditarEcualizadorView::GFXSliderEcu2(int value){
	if(buff_data.general_config.equalization_type == EQ_PROF_PERS ){
		buff_data.general_equalizer_pers._200hz = value;
	}
	else{
		this->Slider2.setValue(buff_data.general_equalizer._200hz);
		this->invalidate();
	}
}

void EditarEcualizadorView::GFXSliderEcu3(int value){
	if(buff_data.general_config.equalization_type == EQ_PROF_PERS ){
		buff_data.general_equalizer_pers._500hz = value;
	}
	else{
		this->Slider3.setValue(buff_data.general_equalizer._500hz);
		this->invalidate();
	}
}
void EditarEcualizadorView::GFXSliderEcu4(int value){
	if(buff_data.general_config.equalization_type == EQ_PROF_PERS ){
		buff_data.general_equalizer_pers._1000hz = value;
	}
	else{
		this->Slider4.setValue(buff_data.general_equalizer._1000hz);
		this->invalidate();
	}
}
void EditarEcualizadorView::GFXSliderEcu5(int value){
	if(buff_data.general_config.equalization_type == EQ_PROF_PERS ){
		buff_data.general_equalizer_pers._4000hz = value;
	}
	else{
		this->Slider5.setValue(buff_data.general_equalizer._4000hz);
		this->invalidate();
	}
}
void EditarEcualizadorView::GFXSliderEcu6(int value){
	if(buff_data.general_config.equalization_type == EQ_PROF_PERS ){
		buff_data.general_equalizer_pers._8000hz = value;
	}
	else{
		this->Slider6.setValue(buff_data.general_equalizer._8000hz);
		this->invalidate();
	}
}

void EditarEcualizadorView::GFXSliderEcu7(int value){
	if(buff_data.general_config.equalization_type == EQ_PROF_PERS ){
		buff_data.general_equalizer_pers._16000hz = value;
	}
	else{
		this->Slider7.setValue(buff_data.general_equalizer._16000hz);
		this->invalidate();
	}
}

void EditarEcualizadorView::UpdateSlides(){
	if(buff_data.general_config.equalization_type == EQ_PROF_PERS ){
		this->Slider1.setValue(buff_data.general_equalizer_pers._20hz);
		this->Slider2.setValue(buff_data.general_equalizer_pers._200hz);
		this->Slider3.setValue(buff_data.general_equalizer_pers._500hz);
		this->Slider4.setValue(buff_data.general_equalizer_pers._1000hz);
		this->Slider5.setValue(buff_data.general_equalizer_pers._4000hz);
		this->Slider6.setValue(buff_data.general_equalizer_pers._8000hz);
		this->Slider7.setValue(buff_data.general_equalizer_pers._16000hz);
	}
	else{
		this->Slider1.setValue(buff_data.general_equalizer._20hz);
		this->Slider2.setValue(buff_data.general_equalizer._200hz);
		this->Slider3.setValue(buff_data.general_equalizer._500hz);
		this->Slider4.setValue(buff_data.general_equalizer._1000hz);
		this->Slider5.setValue(buff_data.general_equalizer._4000hz);
		this->Slider6.setValue(buff_data.general_equalizer._8000hz);
		this->Slider7.setValue(buff_data.general_equalizer._16000hz);
	}
}
