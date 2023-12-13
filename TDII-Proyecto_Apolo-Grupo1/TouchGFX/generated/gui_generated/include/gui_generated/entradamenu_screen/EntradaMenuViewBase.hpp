/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#ifndef ENTRADAMENUVIEWBASE_HPP
#define ENTRADAMENUVIEWBASE_HPP

#include <gui/common/FrontendApplication.hpp>
#include <mvp/View.hpp>
#include <gui/entradamenu_screen/EntradaMenuPresenter.hpp>
#include <touchgfx/widgets/Box.hpp>
#include <touchgfx/containers/Container.hpp>
#include <gui/containers/Template.hpp>
#include <touchgfx/widgets/TextArea.hpp>
#include <touchgfx/containers/Slider.hpp>
#include <touchgfx/widgets/ButtonWithLabel.hpp>
#include <gui/containers/HomeAndBack.hpp>

class EntradaMenuViewBase : public touchgfx::View<EntradaMenuPresenter>
{
public:
    EntradaMenuViewBase();
    virtual ~EntradaMenuViewBase();
    virtual void setupScreen();

    /*
     * Virtual Action Handlers
     */
    virtual void GFXInputAudio()
    {
        // Override and implement this function in EntradaMenu
    }
    virtual void GFXCanalSelecIN()
    {
        // Override and implement this function in EntradaMenu
    }
    virtual void GFXBalanceChange(int value)
    {
        // Override and implement this function in EntradaMenu
    }

protected:
    FrontendApplication& application() {
        return *static_cast<FrontendApplication*>(touchgfx::Application::getInstance());
    }

    /*
     * Member Declarations
     */
    touchgfx::Box __background;
    touchgfx::Container BackGroundAndTitle;
    Template template1;
    touchgfx::TextArea Title;
    touchgfx::TextArea LabelVolumenCanal_1;
    touchgfx::Slider sliderBalance;
    touchgfx::ButtonWithLabel ButtEntradaAudio;
    touchgfx::TextArea EntradaAudioLabel_1;
    touchgfx::ButtonWithLabel ButtCanalAudioIN;
    touchgfx::TextArea CanalAudioLabel;
    HomeAndBack homeAndBack1;
    touchgfx::TextArea textArea1;
    touchgfx::TextArea textArea1_1;

private:

    /*
     * Callback Declarations
     */
    touchgfx::Callback<EntradaMenuViewBase, const touchgfx::AbstractButton&> buttonCallback;
    touchgfx::Callback<EntradaMenuViewBase, const touchgfx::Slider&, int> sliderValueChangedCallback;

    /*
     * Callback Handler Declarations
     */
    void buttonCallbackHandler(const touchgfx::AbstractButton& src);
    void sliderValueChangedCallbackHandler(const touchgfx::Slider& src, int value);

};

#endif // ENTRADAMENUVIEWBASE_HPP
