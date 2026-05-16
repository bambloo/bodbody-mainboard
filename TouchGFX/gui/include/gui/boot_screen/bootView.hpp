#ifndef BOOTVIEW_HPP
#define BOOTVIEW_HPP

#include <gui_generated/boot_screen/bootViewBase.hpp>
#include <gui/boot_screen/bootPresenter.hpp>

class BootView : public BootViewBase
{
public:
    BootView();
    virtual ~BootView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    void clickHandler(const Image &, const touchgfx::ClickEvent& evt);
protected:
    Callback<BootView, const Image&, const ClickEvent&> clickCallback; 
};

#endif // BOOTVIEW_HPP
