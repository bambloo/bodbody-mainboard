#ifndef LOGINVIEW_HPP
#define LOGINVIEW_HPP

#include <gui_generated/login_screen/LoginViewBase.hpp>
#include <gui/login_screen/LoginPresenter.hpp>

class LoginView : public LoginViewBase
{
public:
    LoginView();
    virtual ~LoginView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
    void  centerPinArea();
    touchgfx::Callback<LoginView, const touchgfx::ButtonWithLabel&, const touchgfx::ClickEvent&> pinClickedCallback;
    void pinClicked(const touchgfx::ButtonWithLabel& source, const touchgfx::ClickEvent& evt);
    int pinLen;
};

#endif // LOGINVIEW_HPP
