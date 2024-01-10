#include <gui/mainscreen_screen/MainScreenView.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include "I2CDriver.h"

MainScreenView::MainScreenView()
{
	state_volume_butt = false;
}

void MainScreenView::setupScreen()
{
	get_user_data(&buff_data);
    state_volume_butt = false;
    backgroundVolume.setVisible(false);
    volumeSlider.setVisible(false);
    give_sem_save_data();

    MainScreenViewBase::setupScreen();
}

void MainScreenView::tearDownScreen()
{

    MainScreenViewBase::tearDownScreen();
}

// Button next press
void MainScreenView::function6() {
//	static uint8_t prueba = 0;
//	prueba = !prueba;
//	if(prueba){
//		this->
//	}
	send_cmd_esp(NEXT_SONG_CMD);
}

// Button pause press
void MainScreenView::function9() {
	static bool flag = true;
	TypedText textoStop = T_STOP_LABBEL;
	TypedText textoPlay = T_PAUSE_PLAY;
	send_cmd_esp(STOP_SONG_CMD); // -> La logica de poner pause/play se resuelve en el esp
	if(flag)
		this->ButtPause.setLabelText(textoPlay);
	else
		this->ButtPause.setLabelText(textoStop);
	flag = !flag;


}

// Button previous press
void MainScreenView::function12() {
	send_cmd_esp(PREVIOUS_SONG_CMD);
}

// Button volume press
void MainScreenView::function3()
{

	if(state_volume_butt == false){
		// boton presionado para mostrar slider
	    backgroundVolume.setVisible(true);
	    volumeSlider.setVisible(true);
	    volumeSlider.setValue(buff_data.main_volume);
	}
	else{
		// boton presionado para ocultar slider
	    backgroundVolume.setVisible(false);
	    volumeSlider.setVisible(false);
	}
	this->volumeSlider.invalidate();
	this->backgroundVolume.invalidate();
	state_volume_butt = !state_volume_butt;

}

// Volume slider changed
void MainScreenView::volumeConfirmed(int value)
{
	get_user_data(&buff_data);
	buff_data.main_volume = value;
	set_user_data(buff_data);
	this->function3();
}

void MainScreenView::update_progess_music(unsigned char valor){

	this->ProgressMusic.setValue(valor);
}
