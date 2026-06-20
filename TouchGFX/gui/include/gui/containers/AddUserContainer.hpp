#ifndef ADDUSERCONTAINER_HPP
#define ADDUSERCONTAINER_HPP

#include <gui_generated/containers/AddUserContainerBase.hpp>

class AddUserContainer : public AddUserContainerBase
{
public:
    AddUserContainer();
    virtual ~AddUserContainer() {}

    virtual void initialize();
protected:
};

#endif // ADDUSERCONTAINER_HPP
