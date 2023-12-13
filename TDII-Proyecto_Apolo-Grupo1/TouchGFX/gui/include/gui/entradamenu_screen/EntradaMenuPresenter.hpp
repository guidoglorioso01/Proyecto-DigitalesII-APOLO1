#ifndef ENTRADAMENUPRESENTER_HPP
#define ENTRADAMENUPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class EntradaMenuView;

class EntradaMenuPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    EntradaMenuPresenter(EntradaMenuView& v);

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

    virtual ~EntradaMenuPresenter() {}

private:
    EntradaMenuPresenter();

    EntradaMenuView& view;
};

#endif // ENTRADAMENUPRESENTER_HPP
