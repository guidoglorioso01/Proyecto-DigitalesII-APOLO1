#ifndef CANALESMENUPRESENTER_HPP
#define CANALESMENUPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class CanalesMenuView;

class CanalesMenuPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    CanalesMenuPresenter(CanalesMenuView& v);

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

    virtual ~CanalesMenuPresenter() {}

private:
    CanalesMenuPresenter();

    CanalesMenuView& view;
};

#endif // CANALESMENUPRESENTER_HPP
