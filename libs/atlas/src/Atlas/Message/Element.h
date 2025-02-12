// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Stefanus Du Toit, Karsten-O. Laux and Al Riddoch

// $Id$

#ifndef ATLAS_MESSAGE_ELEMENT_H
#define ATLAS_MESSAGE_ELEMENT_H

#include <Atlas/Exception.h>
#include <Atlas/float.h>

#include <string>
#include <map>
#include <vector>
#include <cinttypes>


namespace Atlas::Message {

/// An exception class issued when the wrong type is requested in as().
class WrongTypeException : public Exception {
public:
	WrongTypeException() noexcept: Exception("Wrong Message::Element type") {}
};

class Element;

typedef std::int64_t IntType;
typedef double FloatType;
typedef void* PtrType;
typedef std::string StringType;
typedef std::map<std::string, Element> MapType;
typedef std::vector<Element> ListType;

/**
 * Multi-type container
 */
class Element {
public:
	enum Type {
		TYPE_NONE,
		TYPE_INT,
		TYPE_FLOAT,
		TYPE_PTR,
		TYPE_STRING,
		TYPE_MAP,
		TYPE_LIST
	};

private:

	/// Clear all values.
	void clear(Type new_type = TYPE_NONE);

public:
	/// Construct an empty object.
	Element()
			: t(TYPE_NONE) {
	}

	///
	~Element() {
		clear();
	}

	/// Copy an existing object.
	Element(const Element& obj);

	/// Move an existing object.
	/// The existing Element will have its type set to "None".
	Element(Element&& obj) noexcept;

	/// Set type to int, and value to v.
	Element(int v)
			: t(TYPE_INT), i(v) {
	}

	Element(long v)
			: t(TYPE_INT), i(v) {
	}

	Element(long long v)
			: t(TYPE_INT), i(v) {
	}

	/// Set type to int, and value to v.
	Element(bool v)
			: t(TYPE_INT), i(v) {
	}

	/// Set type to double, and value to v.
	Element(FloatType v)
			: t(TYPE_FLOAT), f(v) {
	}

	/// Set type to PtrType, and value to v.
	Element(PtrType v)
			: t(TYPE_PTR), p(v) {
	}

	/// Set type to std::string, and value to v.
	Element(const char* v)
			: t(TYPE_STRING) {
		if (v) {
			s = new DataType<StringType>(v);
		} else
			s = new DataType<StringType>();
	}

	/// Set type to std::string, and value to v.
	Element(const StringType& v)
			: t(TYPE_STRING) {
		s = new DataType<StringType>(v);
	}

	/// Set type to std::string, and move v.
	Element(StringType&& v)
			: t(TYPE_STRING) {
		s = new DataType<StringType>(std::move(v));
	}

	/// Set type to MapType, and value to v.
	Element(const MapType& v)
			: t(TYPE_MAP) {
		m = new DataType<MapType>(v);
	}

	/// Set type to MapType, and move v.
	Element(MapType&& v)
			: t(TYPE_MAP) {
		m = new DataType<MapType>(std::move(v));
	}

	/// Set type to ListType, and value to v.
	Element(const ListType& v)
			: t(TYPE_LIST) {
		l = new DataType<ListType>(v);
	}

	/// Set type to ListType, and move v.
	Element(ListType&& v)
			: t(TYPE_LIST) {
		l = new DataType<ListType>(std::move(v));
	}

	/// overload assignment operator !
	Element& operator=(const Element& obj);

	/**
	 * The object being moved from will have its type set to "None".
	 * @param obj
	 * @return
	 */
	Element& operator=(Element&& obj) noexcept;

	Element& operator=(int v) {
		if (TYPE_INT != t) {
			clear(TYPE_INT);
		}
		i = v;
		return *this;
	}

	Element& operator=(long v) {
		if (TYPE_INT != t) {
			clear(TYPE_INT);
		}
		i = v;
		return *this;
	}

	Element& operator=(long long v) {
		if (TYPE_INT != t) {
			clear(TYPE_INT);
		}
		i = v;
		return *this;
	}

	Element& operator=(bool v) {
		if (TYPE_INT != t) {
			clear(TYPE_INT);
		}
		i = v;
		return *this;
	}

	Element& operator=(FloatType v) {
		if (TYPE_FLOAT != t) {
			clear(TYPE_FLOAT);
		}
		f = v;
		return *this;
	}

	Element& operator=(PtrType v) {
		if (TYPE_PTR != t) {
			clear(TYPE_PTR);
		}
		p = v;
		return *this;
	}

	Element& operator=(const char* v) {
		if (TYPE_STRING != t || !s->unique()) {
			clear(TYPE_STRING);
			s = new DataType<StringType>(std::string(v));
		} else {
			*s = StringType(v);
		}
		return *this;
	}

	Element& operator=(const StringType& v) {
		if (TYPE_STRING != t || !s->unique()) {
			clear(TYPE_STRING);
			s = new DataType<StringType>(v);
		} else {
			*s = v;
		}
		return *this;
	}

	Element& operator=(StringType&& v) {
		if (TYPE_STRING != t || !s->unique()) {
			clear(TYPE_STRING);
			s = new DataType<StringType>(v);
		} else {
			*s = v;
		}
		return *this;
	}

	Element& operator=(const MapType& v) {
		if (TYPE_MAP != t || !m->unique()) {
			clear(TYPE_MAP);
			m = new DataType<MapType>(v);
		} else {
			*m = v;
		}
		return *this;
	}

	Element& operator=(MapType&& v) {
		if (TYPE_MAP != t || !m->unique()) {
			clear(TYPE_MAP);
			m = new DataType<MapType>(std::move(v));
		} else {
			*m = v;
		}
		return *this;
	}

	Element& operator=(const ListType& v) {
		if (TYPE_LIST != t || !l->unique()) {
			clear(TYPE_LIST);
			l = new DataType<ListType>(v);
		} else {
			*l = v;
		}
		return *this;
	}

	Element& operator=(ListType&& v) {
		if (TYPE_LIST != t || !l->unique()) {
			clear(TYPE_LIST);
			l = new DataType<ListType>(std::move(v));
		} else {
			*l = v;
		}
		return *this;
	}

	/// Check for equality with another Element.
	bool operator==(const Element& o) const noexcept;

	/// Check for inequality with anything we can check equality with
	template<class C>
	bool operator!=(C c) const {
		return !(*this == c);
	}

	/// Check for equality with a int.
	bool operator==(int v) const {
		return (t == TYPE_INT && i == v);
	}

	/// Check for equality with a int.
	bool operator==(long v) const {
		return (t == TYPE_INT && i == v);
	}

	bool operator==(long long v) const {
		return (t == TYPE_INT && i == v);
	}

	/// Check for equality with a double.
	bool operator==(FloatType v) const {
		return t == TYPE_FLOAT && Equal(f, v);
	}

	/// Check for equality with a pointer.
	bool operator==(PtrType v) const {
		return t == TYPE_PTR && p == v;
	}

	/// Check for equality with a const char *.
	bool operator==(const char* v) const {
		if (t == TYPE_STRING)
			return (s->_data == v);
		return false;
	}

	/// Check for equality with a std::string.
	bool operator==(const StringType& v) const {
		if (t == TYPE_STRING)
			return (s->_data == v);
		return false;
	}

	/// Check for equality with a MapType.
	bool operator==(const MapType& v) const {
		if (t == TYPE_MAP)
			return (m->_data == v);
		return false;
	}

	/// Check for equality with a ListType.
	bool operator==(const ListType& v) const {
		if (t == TYPE_LIST)
			return (l->_data == v);
		return false;
	}

	/// Get the current type.
	Type getType() const { return t; }

	/// Check whether the current type is nothing.
	bool isNone() const { return (t == TYPE_NONE); }

	/// Check whether the current type is int.
	bool isInt() const { return (t == TYPE_INT); }

	/// Check whether the current type is double.
	bool isFloat() const { return (t == TYPE_FLOAT); }

	/// Check whether the current type is pointer.
	bool isPtr() const { return (t == TYPE_PTR); }

	/// Check whether the current type is numeric.
	bool isNum() const { return ((t == TYPE_FLOAT) || (t == TYPE_INT)); }

	/// Check whether the current type is std::string.
	bool isString() const { return (t == TYPE_STRING); }

	/// Check whether the current type is MapType.
	bool isMap() const { return (t == TYPE_MAP); }

	/// Check whether the current type is ListType.
	bool isList() const { return (t == TYPE_LIST); }

	/// Retrieve the current value as a int.
	IntType asInt() const {
		if (t == TYPE_INT) return i;
		throw WrongTypeException();
	}

	IntType Int() const {
		return i;
	}

	/// Retrieve the current value as a double.
	FloatType asFloat() const {
		if (t == TYPE_FLOAT) return f;
		throw WrongTypeException();
	}

	FloatType Float() const {
		return f;
	}

	/// Retrieve the current value as a pointer.
	PtrType asPtr() const {
		if (t == TYPE_PTR) return p;
		throw WrongTypeException();
	}

	PtrType Ptr() const {
		return p;
	}

	/// Retrieve the current value as a number.
	FloatType asNum() const {
		if (t == TYPE_FLOAT) return f;
		if (t == TYPE_INT) return FloatType(i);
		throw WrongTypeException();
	}

	/// Retrieve the current value as a const std::string reference.
	const std::string& asString() const {
		if (t == TYPE_STRING) return *s;
		throw WrongTypeException();
	}

	/// Retrieve the current value as a non-const std::string reference.
	std::string& asString() {
		if (t == TYPE_STRING) return *(s = s->makeUnique());
		throw WrongTypeException();
	}

	const StringType& String() const {
		return *s;
	}

	StringType& String() {
		return *(s = s->makeUnique());
	}

	/**
	 * Moves the string out of the Element.
	 *
	 * This will leave an empty string in the Element.
	 * @return
	 */
	StringType&& moveString() {
		if (t != TYPE_STRING) throw WrongTypeException();
		s = s->makeUnique();
		return s->move();
	}

	/// Retrieve the current value as a const MapType reference.
	const MapType& asMap() const {
		if (t == TYPE_MAP) return *m;
		throw WrongTypeException();
	}

	/// Retrieve the current value as a non-const MapType reference.
	MapType& asMap() {
		if (t == TYPE_MAP) return *(m = m->makeUnique());
		throw WrongTypeException();
	}

	const MapType& Map() const {
		return *m;
	}

	MapType& Map() {
		return *(m = m->makeUnique());
	}

	/**
	 * Moves the map out of the Element.
	 *
	 * This will leave an empty map in the Element.
	 * @return
	 */
	MapType&& moveMap() {
		if (t != TYPE_MAP) throw WrongTypeException();
		m = m->makeUnique();
		return m->move();
	}

	/// Retrieve the current value as a const ListType reference.
	const ListType& asList() const {
		if (t == TYPE_LIST) return *l;
		throw WrongTypeException();
	}

	/// Retrieve the current value as a non-const ListType reference.
	ListType& asList() {
		if (t == TYPE_LIST) return *(l = l->makeUnique());
		throw WrongTypeException();
	}

	const ListType& List() const {
		return *l;
	}

	ListType& List() {
		return *(l = l->makeUnique());
	}

	/**
	 * Moves the list out of the Element.
	 *
	 * This will leave an empty list in the Element.
	 * @return
	 */
	ListType&& moveList() {
		if (t != TYPE_LIST) throw WrongTypeException();
		l = l->makeUnique();
		return l->move();
	}

	static const char* typeName(Type);

protected:

	template<class C>
	class DataType {
	public:
		DataType() : _refcount(1), _data(nullptr) {}

		explicit DataType(const C& c) : _refcount(1), _data(c) {}

		explicit DataType(C&& c) : _refcount(1), _data(std::move(c)) {}

		DataType(const DataType&) = delete;

		DataType& operator=(const C& c) {
			_data = c;
			return *this;
		}

		DataType& operator=(const C&& c) {
			_data = std::move(c);
			return *this;
		}

		DataType& operator=(const DataType&) = delete;

		bool operator==(const DataType<C>& c) const { return _data == c._data; }

		void ref() { ++_refcount; }

		void unref() { if (--_refcount == 0) delete this; }

		bool unique() const { return _refcount == 1; }

		DataType* makeUnique() {
			if (unique())
				return this;
			unref(); // _refcount > 1, so this is fine
			return new DataType(_data);
		}

		operator C&() { return _data; }

		/**
		 * Moves the data out of the container.
		 *
		 * This will destroy the existing data.
		 * @return
		 */
		C&& move() {
			return std::move(_data);
		}
//        operator const C&() const {return _data;}

		unsigned long _refcount;
		C _data;
	};

	Type t;
	union {
		IntType i;
		FloatType f;
		void* p;
		DataType<StringType>* s;
		DataType<MapType>* m;
		DataType<ListType>* l;
	};
};

}
// namespace Atlas::Message


#endif // ATLAS_MESSAGE_ELEMENT_H
