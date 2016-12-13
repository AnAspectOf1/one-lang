#ifndef _ONE_COMPOSER_H
#define _ONE_COMPOSER_H

#include "statement.h"
#include "stream.h"
#include <chi/buffer.h>
#include <chi/dynamic.h>
#include <chi/linked.h>


namespace one {

	class Composer {
		chi::Linked& document;

	public:
		Composer( chi::Linked& document );

		chi::DynamicBuffer compose( chi::Linked& statements );
		chi::DynamicBuffer composeDocument();
		chi::DynamicBuffer composeStatement( Statement* statement );
		chi::Buffer<> composeString( StringStatement* statement );
	};
}

#endif//_ONE_COMPOSER_H
