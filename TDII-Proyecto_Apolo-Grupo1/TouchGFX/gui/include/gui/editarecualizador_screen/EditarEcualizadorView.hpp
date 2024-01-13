#ifndef EDITARECUALIZADORVIEW_HPP
#define EDITARECUALIZADORVIEW_HPP

#include <Control_DatosUsuario.h>
#include <gui_generated/editarecualizador_screen/EditarEcualizadorViewBase.hpp>
#include <gui/editarecualizador_screen/EditarEcualizadorPresenter.hpp>


extern SemaphoreHandle_t semCalcCoefs;

class EditarEcualizadorView : public EditarEcualizadorViewBase
{
public:
    EditarEcualizadorView();
    virtual ~EditarEcualizadorView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    void GFXSliderEcu1(int value);
    void GFXSliderEcu2(int value);
    void GFXSliderEcu3(int value);
    void GFXSliderEcu4(int value);
    void GFXSliderEcu5(int value);
    void GFXSliderEcu6(int value);
    void GFXSliderEcu7(int value);

    void UpdateSlides();
protected:

private:
	UserData buff_data;
};

#endif // EDITARECUALIZADORVIEW_HPP
