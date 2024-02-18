#ifndef RESETMENUVIEW_HPP
#define RESETMENUVIEW_HPP

#include <gui_generated/resetmenu_screen/ResetMenuViewBase.hpp>
#include <gui/resetmenu_screen/ResetMenuPresenter.hpp>

class ResetMenuView : public ResetMenuViewBase
{
public:
    ResetMenuView();
    virtual ~ResetMenuView() {}
    virtual void setupScreen();
    virtual void ButtYesPress();
    virtual void tearDownScreen();
protected:
};

#endif // RESETMENUVIEW_HPP
