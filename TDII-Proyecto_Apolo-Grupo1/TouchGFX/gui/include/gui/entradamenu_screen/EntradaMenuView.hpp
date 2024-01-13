#ifndef ENTRADAMENUVIEW_HPP
#define ENTRADAMENUVIEW_HPP

#include <Control_DatosUsuario.h>
#include <gui_generated/entradamenu_screen/EntradaMenuViewBase.hpp>
#include <gui/entradamenu_screen/EntradaMenuPresenter.hpp>
//#include <string>
#include <texts/TextKeysAndLanguages.hpp>



class EntradaMenuView : public EntradaMenuViewBase
{
public:
    EntradaMenuView();
    virtual ~EntradaMenuView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    void GFXBalanceChange(int value);
    void GFXInputAudio();
    void GFXCanalSelecIN();
    void UpdateInputAudio();
    void UpdateInputChannel();
    void UpdateBalanceInput();
protected:

private:
	UserData buff_data;
};

#endif // ENTRADAMENUVIEW_HPP
