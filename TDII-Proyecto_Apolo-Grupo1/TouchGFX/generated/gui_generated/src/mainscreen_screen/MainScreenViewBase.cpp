/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/mainscreen_screen/MainScreenViewBase.hpp>
#include <touchgfx/canvas_widget_renderer/CanvasWidgetRenderer.hpp>
#include <touchgfx/Color.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <images/BitmapDatabase.hpp>

MainScreenViewBase::MainScreenViewBase() :
    buttonCallback(this, &MainScreenViewBase::buttonCallbackHandler),
    sliderValueConfirmedCallback(this, &MainScreenViewBase::sliderValueConfirmedCallbackHandler)
{
    touchgfx::CanvasWidgetRenderer::setupBuffer(canvasBuffer, CANVAS_BUFFER_SIZE);
    
    __background.setPosition(0, 0, 480, 320);
    __background.setColor(touchgfx::Color::getColorFromRGB(0, 0, 0));
    add(__background);

    BackGroundAndTitle.setPosition(0, 0, 480, 320);
    template1.setXY(0, 0);
    BackGroundAndTitle.add(template1);

    Title.setXY(181, 0);
    Title.setColor(touchgfx::Color::getColorFromRGB(255, 227, 216));
    Title.setLinespacing(0);
    Title.setTypedText(touchgfx::TypedText(T___SINGLEUSE_K4NI));
    BackGroundAndTitle.add(Title);

    add(BackGroundAndTitle);

    MusicPlayer.setPosition(30, 57, 431, 182);
    ButtPause.setXY(180, 31);
    ButtPause.setBitmaps(touchgfx::Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_ICON_ROUNDED_TINY_OUTLINE_DISABLED_ID), touchgfx::Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_ICON_ROUNDED_TINY_OUTLINE_DISABLED_ID));
    ButtPause.setLabelText(touchgfx::TypedText(T_STOP_LABBEL));
    ButtPause.setLabelColor(touchgfx::Color::getColorFromRGB(21, 29, 54));
    ButtPause.setLabelColorPressed(touchgfx::Color::getColorFromRGB(255, 255, 255));
    ButtPause.setAction(buttonCallback);
    MusicPlayer.add(ButtPause);

    ProgressMusic.setXY(110, 148);
    ProgressMusic.setProgressIndicatorPosition(0, 0, 200, 5);
    ProgressMusic.setRange(0, 100);
    ProgressMusic.setBackground(touchgfx::Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_LINEPROGRESS_BACKGROUNDS_IMAGE_PLAIN_THIN_SMALL_0_ID));
    ProgressMusicPainter.setBitmap(touchgfx::Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_LINEPROGRESS_FILLERS_PLAIN_THIN_SMALL_DARK_0_ID));
    ProgressMusic.setPainter(ProgressMusicPainter);
    ProgressMusic.setStart(0, 2);
    ProgressMusic.setEnd(200, 2);
    ProgressMusic.setLineWidth(5);
    ProgressMusic.setLineEndingStyle(touchgfx::Line::BUTT_CAP_ENDING);
    ProgressMusic.setValue(60);
    MusicPlayer.add(ProgressMusic);

    ButtBefore.setXY(12, 31);
    ButtBefore.setBitmaps(touchgfx::Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_ICON_ROUNDED_TINY_OUTLINE_DISABLED_ID), touchgfx::Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_ICON_ROUNDED_TINY_OUTLINE_DISABLED_ID));
    ButtBefore.setLabelText(touchgfx::TypedText(T___SINGLEUSE_NJAW));
    ButtBefore.setLabelColor(touchgfx::Color::getColorFromRGB(21, 29, 54));
    ButtBefore.setLabelColorPressed(touchgfx::Color::getColorFromRGB(255, 255, 255));
    ButtBefore.setAction(buttonCallback);
    MusicPlayer.add(ButtBefore);

    ButtNext.setXY(341, 31);
    ButtNext.setBitmaps(touchgfx::Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_ICON_ROUNDED_TINY_OUTLINE_DISABLED_ID), touchgfx::Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_ICON_ROUNDED_TINY_OUTLINE_DISABLED_ID));
    ButtNext.setLabelText(touchgfx::TypedText(T___SINGLEUSE_CXAE));
    ButtNext.setLabelColor(touchgfx::Color::getColorFromRGB(21, 29, 54));
    ButtNext.setLabelColorPressed(touchgfx::Color::getColorFromRGB(255, 255, 255));
    ButtNext.setAction(buttonCallback);
    MusicPlayer.add(ButtNext);

    add(MusicPlayer);

    backgroundVolume.setPosition(0, 0, 82, 320);
    backgroundVolume.setColor(touchgfx::Color::getColorFromRGB(138, 112, 112));
    backgroundVolume.setVisible(false);
    add(backgroundVolume);

    ButtVolume.setXY(12, 249);
    ButtVolume.setBitmaps(touchgfx::Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_ICON_ROUNDED_TINY_OUTLINE_DISABLED_ID), touchgfx::Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_ICON_ROUNDED_TINY_OUTLINE_DISABLED_ID), touchgfx::Bitmap(BITMAP_ICON_THEME_IMAGES_AV_VOLUME_UP_50_50_E8F6FB_SVG_ID), touchgfx::Bitmap(BITMAP_ICON_THEME_IMAGES_AV_VOLUME_UP_50_50_E8F6FB_SVG_ID));
    ButtVolume.setIconXY(6, 5);
    ButtVolume.setAction(buttonCallback);
    add(ButtVolume);

    volumeSlider.setXY(30, 34);
    volumeSlider.setBitmaps(touchgfx::Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_SLIDER_VERTICAL_THIN_TRACK_SMALL_ID), touchgfx::Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_SLIDER_VERTICAL_THIN_FILLER_SMALL_ID), touchgfx::Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_SLIDER_VERTICAL_THIN_ROUND_DARK_ID));
    volumeSlider.setupVerticalSlider(9, 12, 0, 0, 200);
    volumeSlider.setValueRange(0, 100);
    volumeSlider.setValue(0);
    volumeSlider.setVisible(false);
    volumeSlider.setStopValueCallback(sliderValueConfirmedCallback);
    add(volumeSlider);

    ButtSettings.setXY(401, 249);
    ButtSettings.setBitmaps(touchgfx::Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_ICON_ROUNDED_TINY_OUTLINE_DISABLED_ID), touchgfx::Bitmap(BITMAP_ALTERNATE_THEME_IMAGES_WIDGETS_BUTTON_ICON_ROUNDED_TINY_OUTLINE_DISABLED_ID), touchgfx::Bitmap(BITMAP_ICON_THEME_IMAGES_ACTION_SETTINGS_50_50_E8F6FB_SVG_ID), touchgfx::Bitmap(BITMAP_ICON_THEME_IMAGES_ACTION_SETTINGS_50_50_E8F6FB_SVG_ID));
    ButtSettings.setIconXY(6, 6);
    ButtSettings.setAction(buttonCallback);
    add(ButtSettings);
}

MainScreenViewBase::~MainScreenViewBase()
{
    touchgfx::CanvasWidgetRenderer::resetBuffer();
}

void MainScreenViewBase::setupScreen()
{
    template1.initialize();
}

void MainScreenViewBase::buttonCallbackHandler(const touchgfx::AbstractButton& src)
{
    if (&src == &ButtSettings)
    {
        //ActionSettings
        //When ButtSettings clicked change screen to Settings1
        //Go to Settings1 with no screen transition
        application().gotoSettings1ScreenNoTransition();
    }
    if (&src == &ButtNext)
    {
        //ActionNext
        //When ButtNext clicked call virtual function
        //Call function6
        function6();
    }
    if (&src == &ButtPause)
    {
        //ActionPause
        //When ButtPause clicked call virtual function
        //Call function9
        function9();
    }
    if (&src == &ButtBefore)
    {
        //ActionBefore
        //When ButtBefore clicked call virtual function
        //Call function12
        function12();
    }
    if (&src == &ButtVolume)
    {
        //ActionVolume
        //When ButtVolume clicked call virtual function
        //Call function3
        function3();
    }
}

void MainScreenViewBase::sliderValueConfirmedCallbackHandler(const touchgfx::Slider& src, int value)
{
    if (&src == &volumeSlider)
    {
        //Interaction1
        //When volumeSlider value confirmed call virtual function
        //Call volumeConfirmed
        volumeConfirmed(value);
    }
}
