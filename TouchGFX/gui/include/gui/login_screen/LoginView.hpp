#ifndef LOGINVIEW_HPP
#define LOGINVIEW_HPP

#include "touchgfx/widgets/ButtonWithIcon.hpp"
#include "touchgfx/widgets/TextArea.hpp"
#include "touchgfx/widgets/TextAreaWithWildcard.hpp"
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
    touchgfx::Callback<LoginView, const touchgfx::ButtonWithLabel&, const touchgfx::ClickEvent&> pinClickedCallback;
    touchgfx::Callback<LoginView, const touchgfx::ButtonWithIcon&, const touchgfx::ClickEvent&> funClickedCallback;
    touchgfx::Callback<LoginView, const touchgfx::TextAreaWithOneWildcard&, const touchgfx::ClickEvent&> changePinClickedCallback;
    void pinClicked(const touchgfx::ButtonWithLabel& source, const touchgfx::ClickEvent& evt);
    void funClicked(const touchgfx::ButtonWithIcon& source, const touchgfx::ClickEvent& evt);
    void changePinClicked(const touchgfx::TextAreaWithOneWildcard& source, const touchgfx::ClickEvent& evt);

    int pinLen;
    uint32_t pinVal;
};

#endif // LOGINVIEW_HPP
