/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#ifndef SETTINGS1VIEWBASE_HPP
#define SETTINGS1VIEWBASE_HPP

#include <gui/common/FrontendApplication.hpp>
#include <mvp/View.hpp>
#include <gui/settings1_screen/Settings1Presenter.hpp>
#include <touchgfx/widgets/Box.hpp>
#include <touchgfx/containers/Container.hpp>
#include <gui/containers/Template.hpp>
#include <touchgfx/widgets/TextArea.hpp>
#include <touchgfx/widgets/ButtonWithIcon.hpp>
#include <touchgfx/widgets/ButtonWithLabel.hpp>
#include <gui/containers/HomeAndBack.hpp>

class Settings1ViewBase : public touchgfx::View<Settings1Presenter>
{
public:
    Settings1ViewBase();
    virtual ~Settings1ViewBase();
    virtual void setupScreen();

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
    touchgfx::ButtonWithIcon Home;
    touchgfx::ButtonWithLabel ButtSetGeneral;
    touchgfx::ButtonWithLabel ButtSetEcualizador;
    touchgfx::ButtonWithLabel ButtSetCanales;
    touchgfx::ButtonWithLabel ButtSetInAudio;
    HomeAndBack homeAndBack1;

private:

    /*
     * Callback Declarations
     */
    touchgfx::Callback<Settings1ViewBase, const touchgfx::AbstractButton&> buttonCallback;

    /*
     * Callback Handler Declarations
     */
    void buttonCallbackHandler(const touchgfx::AbstractButton& src);

};

#endif // SETTINGS1VIEWBASE_HPP
