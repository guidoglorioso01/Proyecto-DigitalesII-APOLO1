#include <gui/mainscreen_screen/MainScreenView.hpp>


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
void MainScreenView::function6(){

}

// Button pause press
void MainScreenView::function9(){

}

// Button before press
void MainScreenView::function12()
{

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
