#ifndef TESTPRESENTER_HPP
#define TESTPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class TestView;

class TestPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    TestPresenter(TestView& v);

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

    virtual ~TestPresenter() {}

private:
    TestPresenter();

    TestView& view;
};

#endif // TESTPRESENTER_HPP
