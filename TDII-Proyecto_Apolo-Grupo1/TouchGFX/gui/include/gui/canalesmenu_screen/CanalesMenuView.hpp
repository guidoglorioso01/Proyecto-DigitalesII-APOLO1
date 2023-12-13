#ifndef CANALESMENUVIEW_HPP
#define CANALESMENUVIEW_HPP

#include <gui_generated/canalesmenu_screen/CanalesMenuViewBase.hpp>
#include <gui/canalesmenu_screen/CanalesMenuPresenter.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include "user_info.h"



class CanalesMenuView : public CanalesMenuViewBase
{
public:
    CanalesMenuView();
    virtual ~CanalesMenuView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void GFXCanalSelec();
    void GFXEstadoCanal();
	void GFXVolumenCanalSelec(int value);
	void GFXCanalAudio();

	void UpdateChannelSelec();
	void UpdateStateChannel();
	void UpdateVolumeChannel();
	void UpdateChannelOutput();

protected:
	uint8_t channelInUse;
	void config_eq(uint8_t *values,Equalizer *position);
	UserData buff_data;
};

#endif // CANALESMENUVIEW_HPP
