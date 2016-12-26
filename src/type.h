#ifndef _ONE_TYPE_H
#define _ONE_TYPE_H

#include <qi/ptr.h>


namespace one {

	class Definition;

	
	class Type {
	protected:
		bool _is_str;

	public:
		qi::CSPtr<Definition> def;

		Type() {}
		Type( qi::CSPtr<Definition>& def );
		Type( bool is_str );

		bool is_def() const;
		bool is_num() const;
		bool is_str() const;
	};
}

#endif//_ONE_TYPE_H
