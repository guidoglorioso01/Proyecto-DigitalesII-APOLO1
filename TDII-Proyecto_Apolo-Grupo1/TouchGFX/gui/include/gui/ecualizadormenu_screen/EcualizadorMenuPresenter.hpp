#ifndef ECUALIZADORMENUPRESENTER_HPP
#define ECUALIZADORMENUPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class EcualizadorMenuView;

class EcualizadorMenuPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    EcualizadorMenuPresenter(EcualizadorMenuView& v);

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

    virtual ~EcualizadorMenuPresenter() {}

private:
    EcualizadorMenuPresenter();

    EcualizadorMenuView& view;
};

#endif // ECUALIZADORMENUPRESENTER_HPP
