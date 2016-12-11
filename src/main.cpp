#include "exception.h"
#include "io.h"
#include "parser.h"
#include <chi/exception.h>
#include <chi/linked.h>
#include <cstdio>

#define PRINT_ERROR( MSG... ) \
	{ fprintf( stderr, MSG ); fprintf( stderr, ".\n" ); }

using namespace chi;
using namespace one;

int main() {
	StdinStream stdin;
	try {
		Linked document = Parser( stdin ).parse();

		// TODO: Expand all definitions from main
	}
	catch ( chi::Exception& e ) {
		PRINT_ERROR( "Uncaught Chi exception occurred" );
	}
	catch ( one::Exception& e ) {
		PRINT_ERROR( "Uncaught One exception occurred" );
	}

	return 0;
}
