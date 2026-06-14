#ifndef TESTVIEW_HPP
#define TESTVIEW_HPP

#include <gui_generated/test_screen/TestViewBase.hpp>
#include <gui/test_screen/TestPresenter.hpp>

class TestView : public TestViewBase
{
public:
    TestView();
    virtual ~TestView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // TESTVIEW_HPP
