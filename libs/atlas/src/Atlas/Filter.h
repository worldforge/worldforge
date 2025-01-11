// This file may be redistributed and modified only under the terms of
// the GNU Lesser General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Michael Day, Stefanus Du Toit

// $Id$

#ifndef ATLAS_FILTER_H
#define ATLAS_FILTER_H

#include <iostream>
#include <string>
#include <memory>
#include <array>

namespace Atlas {

/** Atlas stream filter

Filters are used by Codec to transform the byte stream before transmission.
The transform must be invertible; that is to say, encoding a string and then
decoding it must result in the original string. Filters can be used for
compression, encryption or performing checksums and other forms of
transmission error detection. A compound filter can be created that acts like
a single filter, allowing various filters to be chained together in useful
ways such as compressing and then encrypting.

@see Codec
@see Negotiate
*/

class Filter {
public:

	explicit Filter(std::unique_ptr<Filter> = nullptr);

	Filter(const Filter&) = delete;

	Filter& operator=(const Filter&) = delete;

	virtual ~Filter();

	virtual void begin() = 0;

	virtual void end() = 0;

	virtual std::string encode(const std::string&) = 0;

	virtual std::string decode(const std::string&) = 0;

	enum Type {
		CHECKSUM,
		COMPRESSION,
		ENCRYPTION
	};

protected:

	std::unique_ptr<Filter> m_next;
};

typedef int int_type;

class filterbuf : public std::streambuf {

public:

	filterbuf(std::streambuf& buffer,
			  Filter& filter)
			: m_outBuffer(), m_inBuffer(), m_streamBuffer(buffer), m_filter(filter) {
		setp(m_outBuffer.data(), m_outBuffer.data() + (m_outBuffer.size() - 1));
		setg(m_inBuffer.data() + m_inPutback, m_inBuffer.data() + m_inPutback,
			 m_inBuffer.data() + m_inPutback);
	}

	~filterbuf() override;

protected:
	std::array<char, 10> m_outBuffer;

	static constexpr auto m_inPutback = 4;
	std::array<char, 10> m_inBuffer;

	int flushOutBuffer() {
		auto num = static_cast<int>(pptr() - pbase());
		std::string encoded = m_filter.encode(std::string(pbase(), pptr()));
		m_streamBuffer.sputn(encoded.c_str(), static_cast<std::streamsize>(encoded.size()));
		pbump(-num);
		return num;
	}

	int_type overflow(int_type c) override;

	int_type underflow() override;

	int sync() override;

private:

	std::streambuf& m_streamBuffer;
	Filter& m_filter;
};

} // Atlas namespace

#endif
