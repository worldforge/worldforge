// Copyright 1999 Stefanus Du Toit
// For licensing details see the file COPYING
// This file comes with ABSOLUTELY NO WARRANTY.

#ifndef OBSERVER_H
#define OBSERVER_H

#include <string>
#include "Variable.h"

class Observer {
public:
    Observer(const string& section, const string& name);
    virtual ~Observer();

    Variable getValue() const;

    void update();

private:
    Variable m_value;
    string m_section, m_name;
};

#endif
