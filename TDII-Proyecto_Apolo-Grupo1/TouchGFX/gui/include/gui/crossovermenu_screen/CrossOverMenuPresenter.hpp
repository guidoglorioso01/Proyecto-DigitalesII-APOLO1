#ifndef CROSSOVERMENUPRESENTER_HPP
#define CROSSOVERMENUPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class CrossOverMenuView;

class CrossOverMenuPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    CrossOverMenuPresenter(CrossOverMenuView& v);

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

    virtual ~CrossOverMenuPresenter() {}

private:
    CrossOverMenuPresenter();

    CrossOverMenuView& view;
};

#endif // CROSSOVERMENUPRESENTER_HPP
