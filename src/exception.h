#ifndef _ONE_EXCEPTION_H
#define _ONE_EXCEPTION_H

#ifndef NDEBUG
#include <cstdio>
#define ONE_ASSERT( COND, MSG... ) \
	if (COND) { fprintf( stderr, "[One]\tAssertion: %s:%s:%d: ", __FILE__, __func__, __LINE__ ); fprintf( stderr, MSG ); fprintf( stderr, ".\n" ); exit(1); }
#else
#define ONE_ASSERT( COND, MSG... ) {}
#endif


namespace one {

	class Exception {};
}

#endif//_ONE_EXCEPTION_H
