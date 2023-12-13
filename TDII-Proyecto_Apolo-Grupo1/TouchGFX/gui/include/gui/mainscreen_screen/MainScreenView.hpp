#ifndef MAINSCREENVIEW_HPP
#define MAINSCREENVIEW_HPP

#include <gui_generated/mainscreen_screen/MainScreenViewBase.hpp>
#include <gui/mainscreen_screen/MainScreenPresenter.hpp>
#include "user_info.h"
#include "I2CDriver.h"



class MainScreenView : public MainScreenViewBase
{
public:
    MainScreenView();
    virtual ~MainScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void function6();
    virtual void function9();
    virtual void function12();
    virtual void function3();
    virtual void volumeConfirmed(int value);

private:
	bool state_volume_butt;
	UserData buff_data;

protected:

};

#endif // MAINSCREENVIEW_HPP
