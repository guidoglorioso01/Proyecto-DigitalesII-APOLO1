#ifndef CROSSOVERMENUVIEW_HPP
#define CROSSOVERMENUVIEW_HPP

#include <Control_DatosUsuario.h>
#include <gui_generated/crossovermenu_screen/CrossOverMenuViewBase.hpp>
#include <gui/crossovermenu_screen/CrossOverMenuPresenter.hpp>
#include <texts/TextKeysAndLanguages.hpp>

class CrossOverMenuView : public CrossOverMenuViewBase
{
public:
    CrossOverMenuView();
    virtual ~CrossOverMenuView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    void GFXeditCrossOver();
    void UpdateBut();

protected:
    void UpdateGraph();
private:
	UserData buff_data;
};

#endif // CROSSOVERMENUVIEW_HPP
