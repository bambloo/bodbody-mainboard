#ifndef LOGINPRESENTER_HPP
#define LOGINPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class LoginView;

class LoginPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    LoginPresenter(LoginView& v);

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

    virtual ~LoginPresenter() {}

    bool checkPin(uint32_t pin);

private:
    LoginPresenter();

    LoginView& view;
};

#endif // LOGINPRESENTER_HPP
