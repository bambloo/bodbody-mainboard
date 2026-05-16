#ifndef BOOTVIEW_HPP
#define BOOTVIEW_HPP

#include <gui_generated/boot_screen/bootViewBase.hpp>
#include <gui/boot_screen/bootPresenter.hpp>

class bootView : public bootViewBase
{
public:
    bootView();
    virtual ~bootView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // BOOTVIEW_HPP
