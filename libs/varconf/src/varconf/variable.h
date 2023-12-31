/*
 *  variable.h - interface for typeless value container class
 *  Copyright (C) 2001, Stefanus Du Toit, Joseph Zupko
 *            (C) 2001-2006 Alistair Riddoch
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 *  Contact:  Joseph Zupko
 *            jaz147@psu.edu
 *
 *            189 Reese St.
 *            Old Forge, PA 18518
 */

#ifndef VARCONF_VARIABLE_H
#define VARCONF_VARIABLE_H

#include "varconf_defs.h"

#include <sigc++/trackable.h>

#include <string>
#include <iostream>
#include <vector>

namespace varconf {

typedef enum {
	GLOBAL = 0x1u << 0u,
	USER = 0x1u << 1u,
	INSTANCE = 0x1u << 2u
} Scope;

class VARCONF_API VarBase : virtual public sigc::trackable {
public:
	VarBase();

	VarBase(const VarBase& c);

	VarBase(bool b);

	VarBase(int i);

	VarBase(double d);

	VarBase(std::string s);

	VarBase(const char* s);

	virtual ~VarBase();

	VARCONF_API friend std::ostream& operator<<(std::ostream& out, const VarBase& v);

	VARCONF_API friend bool operator==(const VarBase& one, const VarBase& two);

	VARCONF_API friend bool operator!=(const VarBase& one, const VarBase& two);

	VarBase& operator=(const VarBase& c);

	virtual VarBase& operator=(bool b);

	virtual VarBase& operator=(int i);

	virtual VarBase& operator=(double d);

	virtual VarBase& operator=(const std::string& s);

	virtual VarBase& operator=(const char* s);

	virtual explicit operator bool() const;

	virtual explicit operator int() const;

	virtual explicit operator double() const;

	virtual explicit operator std::string() const;

	virtual bool is_bool();

	virtual bool is_int();

	virtual bool is_double();

	virtual bool is_string();

	Scope scope() const { return m_scope; }

	void setScope(Scope s) { m_scope = s; }

private:
	mutable bool m_have_bool;
	mutable bool m_have_int;
	mutable bool m_have_double;
	bool m_have_string;

	mutable bool m_val_bool;
	mutable int m_val_int;
	mutable double m_val_double;
	std::string m_val;

protected:
	Scope m_scope;
};

VARCONF_API std::ostream& operator<<(std::ostream& out, const VarBase& v);

VARCONF_API bool operator==(const VarBase& one, const VarBase& two);

// The next two classes manage a reference count to
// a pointer to class VarBase.

class VarBox {
public:
	VarBox(VarBase* vb) : m_var(vb), m_ref(1) {}

	~VarBox() { delete m_var; }

	void ref() { ++m_ref; }

	void unref() { if (--m_ref == 0) delete this; }

	VarBase* elem() { return m_var; }

	VarBox(const VarBox&) = delete;

	VarBox& operator=(const VarBox&) = delete;

private:

	VarBase* m_var;
	unsigned long m_ref;
};

class VARCONF_API VarPtr {
public:
	VarPtr(VarBase* vb) : m_box(new VarBox(vb)) {}

	VarPtr(const VarPtr& vp) : m_box(vp.m_box) { m_box->ref(); }

	~VarPtr() { m_box->unref(); }

	VarPtr& operator=(const VarPtr& vp) {
		if (vp.m_box != m_box) {
			m_box->unref();
			m_box = vp.m_box;
			m_box->ref();
		}

		return *this;
	}

	VarBase& elem() const { return *m_box->elem(); }

	VarBase* operator->() const { return m_box->elem(); }

private:
	VarBox* m_box;
};

class Variable;

class VARCONF_API Variable : public VarPtr {
public:
	Variable() : VarPtr(new VarBase()) {}

	Variable(const Variable& c);

	Variable(VarBase* vb) : VarPtr(vb) {}

	Variable(bool b) : VarPtr(new VarBase(b)) {}

	Variable(int i) : VarPtr(new VarBase(i)) {}

	Variable(double d) : VarPtr(new VarBase(d)) {}

	Variable(const std::string& s) : VarPtr(new VarBase(s)) {}

	Variable(const char* s) : VarPtr(new VarBase(s)) {}

	virtual ~Variable();

	friend std::ostream& operator<<(std::ostream& out, const Variable& v) { return (out << v.elem()); }

	friend bool operator==(const Variable& one, const Variable& two) { return (one.elem() == two.elem()); }

	friend bool operator!=(const Variable& one, const Variable& two) { return (one.elem() != two.elem()); }

	Variable& operator=(const Variable& c);

	Variable& operator=(VarBase* vb);

	Variable& operator=(bool b);

	Variable& operator=(int i);

	Variable& operator=(double d);

	Variable& operator=(const std::string& s);

	Variable& operator=(const char* s);

	explicit operator bool() const { return bool(this->elem()); }

	explicit operator int() const { return int(this->elem()); }

	explicit operator double() const { return double(this->elem()); }

	explicit operator std::string() const { return std::string(this->elem()); }

	std::string as_string() const { return std::string(this->elem()); }

	// This is sort of funky. The corresponding functions in VarBase
	// can't be const, since the versions in dynvar::Base call
	// set_val(), which certainly isn't const. These versions
	// can be const, however, since (const Variable) is a pointer
	// to VarBase, not (const VarBase).

	bool is_bool() const { return (*this)->is_bool(); }

	bool is_int() const { return (*this)->is_int(); }

	bool is_double() const { return (*this)->is_double(); }

	bool is_string() const { return (*this)->is_string(); }
};

} // namespace varconf

#endif
