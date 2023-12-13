#ifndef GENERALMENUPRESENTER_HPP
#define GENERALMENUPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class GeneralMenuView;

class GeneralMenuPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    GeneralMenuPresenter(GeneralMenuView& v);

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

    virtual ~GeneralMenuPresenter() {}

private:
    GeneralMenuPresenter();

    GeneralMenuView& view;
};

#endif // GENERALMENUPRESENTER_HPP
