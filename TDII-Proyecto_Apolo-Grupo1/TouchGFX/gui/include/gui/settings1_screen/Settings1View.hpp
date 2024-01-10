#ifndef SETTINGS1VIEW_HPP
#define SETTINGS1VIEW_HPP

#include <gui_generated/settings1_screen/Settings1ViewBase.hpp>
#include <gui/settings1_screen/Settings1Presenter.hpp>

class Settings1View : public Settings1ViewBase
{
public:
    Settings1View();
    virtual ~Settings1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // SETTINGS1VIEW_HPP
