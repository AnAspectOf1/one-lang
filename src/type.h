#ifndef _ONE_TYPE_H
#define _ONE_TYPE_H

#include <chi/ptr.h>


namespace one {

	class Definition;

	
	class Type {
	protected:
		bool _is_str;

	public:
		chi::CSPtr<Definition> def;

		Type() {}
		Type( chi::CSPtr<Definition>& def );
		Type( bool is_str );

		bool is_def() const;
		bool is_num() const;
		bool is_str() const;
	};
}

#endif//_ONE_TYPE_H
