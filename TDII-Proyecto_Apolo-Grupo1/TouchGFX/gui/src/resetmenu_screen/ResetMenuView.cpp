#include <gui/resetmenu_screen/ResetMenuView.hpp>
#include "Control_DatosUsuario.h"

ResetMenuView::ResetMenuView()
{

}

void ResetMenuView::setupScreen()
{
    ResetMenuViewBase::setupScreen();
}

void ResetMenuView::tearDownScreen()
{
    ResetMenuViewBase::tearDownScreen();
}


void ResetMenuView::ButtYesPress()
{
	reset_userdata();
}
