#ifndef LOGINVIEW_HPP
#define LOGINVIEW_HPP

#include "touchgfx/widgets/ButtonWithIcon.hpp"
#include "touchgfx/widgets/ButtonWithLabel.hpp"
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
    Callback<LoginView, const touchgfx::ButtonWithLabel&, const touchgfx::ClickEvent&> pinNumClickedCallback;
    Callback<LoginView, const touchgfx::ButtonWithIcon&, const touchgfx::ClickEvent&> funBtnClickedCallback;
    Callback<LoginView, const touchgfx::ButtonWithLabel&, const touchgfx::ClickEvent&> pinBtnClickedCallback;
    Callback<LoginView, const touchgfx::TextAreaWithOneWildcard&, const touchgfx::ClickEvent&> pinTxtClickedCallback;

    Callback<LoginView, Unicode::UnicodeChar> keyboardCallback;

    void pinNumClicked(const touchgfx::ButtonWithLabel& source, const touchgfx::ClickEvent& evt);
    void funBtnClicked(const touchgfx::ButtonWithIcon& source, const touchgfx::ClickEvent& evt);
    void pinBtnClicked(const touchgfx::ButtonWithLabel& source, const touchgfx::ClickEvent& evt);
    void pinTxtClicked(const touchgfx::TextAreaWithOneWildcard& source, const touchgfx::ClickEvent& evt);
    void keyboardEventHandler(Unicode::UnicodeChar c);

    int pinLen;
    uint32_t pinVal;
private:
    const TextAreaWithOneWildcard *currentPinArea;
};

#endif // LOGINVIEW_HPP
