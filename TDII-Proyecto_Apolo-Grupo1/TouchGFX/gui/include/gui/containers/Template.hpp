#ifndef TEMPLATE_HPP
#define TEMPLATE_HPP

#include <gui_generated/containers/TemplateBase.hpp>

class Template : public TemplateBase
{
public:
    Template();
    virtual ~Template() {}

    virtual void initialize();
protected:
};

#endif // TEMPLATE_HPP
