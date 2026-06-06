#ifndef BOOTVIEW_HPP
#define BOOTVIEW_HPP

#include "tx_api.h"
#include <gui_generated/boot_screen/bootViewBase.hpp>
#include <gui/boot_screen/bootPresenter.hpp>

class BootView : public BootViewBase
{
public:
    BootView();
    virtual ~BootView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    void gotoLoginView();
protected:
    Callback<BootView, const Image&, const ClickEvent&> clickCallback;
    TX_TIMER timer;
};

#endif // BOOTVIEW_HPP
