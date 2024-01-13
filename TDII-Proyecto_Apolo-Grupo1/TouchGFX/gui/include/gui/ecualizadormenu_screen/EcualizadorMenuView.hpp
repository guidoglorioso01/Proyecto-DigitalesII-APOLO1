#ifndef ECUALIZADORMENUVIEW_HPP
#define ECUALIZADORMENUVIEW_HPP

#include <Control_DatosUsuario.h>
#include <gui_generated/ecualizadormenu_screen/EcualizadorMenuViewBase.hpp>
#include <gui/ecualizadormenu_screen/EcualizadorMenuPresenter.hpp>


class EcualizadorMenuView : public EcualizadorMenuViewBase
{
public:
    EcualizadorMenuView();
    virtual ~EcualizadorMenuView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    void UpdateGraph();
protected:

private:
	UserData buff_data;
};

#endif // ECUALIZADORMENUVIEW_HPP
