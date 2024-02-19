#ifndef RESETMENUPRESENTER_HPP
#define RESETMENUPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class ResetMenuView;

class ResetMenuPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    ResetMenuPresenter(ResetMenuView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~ResetMenuPresenter() {}

private:
    ResetMenuPresenter();

    ResetMenuView& view;
};

#endif // RESETMENUPRESENTER_HPP
