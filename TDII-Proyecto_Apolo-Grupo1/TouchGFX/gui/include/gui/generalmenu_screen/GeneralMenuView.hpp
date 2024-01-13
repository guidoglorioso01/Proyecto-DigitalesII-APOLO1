#ifndef GENERALMENUVIEW_HPP
#define GENERALMENUVIEW_HPP

#include <Control_DatosUsuario.h>
#include <gui_generated/generalmenu_screen/GeneralMenuViewBase.hpp>
#include <gui/generalmenu_screen/GeneralMenuPresenter.hpp>

#include <texts/TextKeysAndLanguages.hpp>

class GeneralMenuView : public GeneralMenuViewBase
{
public:
    GeneralMenuView();
    virtual ~GeneralMenuView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    void GFXTipoSistemaChange();
	void GFXCambioEcualizador();
	void GFXVolumenMaximoChange(int value);
	void GFXChangeLoudnessState();
    void UpdateTypeSystem();
    void UpdateEcProfile();
    void UpdateLoudnessState();
    void UpdateMaxVolume();
protected:
private:
    UserData buff_data;
};

#endif // GENERALMENUVIEW_HPP
