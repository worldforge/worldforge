// Copyright 1999 Stefanus Du Toit
// For licensing details see the file COPYING
// This file comes with ABSOLUTELY NO WARRANTY.

#ifndef VARCONF_OBSERVER_H
#define VARCONF_OBSERVER_H

#include <string>
#include "Variable.h"

namespace varconf {

class Observer {
public:
    Observer(const std::string& section, const std::string& name);
    virtual ~Observer();

    Variable getValue() const;

    void update();

private:
    Variable m_value;
    std::string m_section, m_name;
};

}

#endif
