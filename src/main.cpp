#include "composer.h"
#include "exception.h"
#include "io.h"
#include "parser.h"
#include <chi/exception.h>
#include <cstdio>

#define PRINT_ERROR( MSG... ) \
	{ fprintf( stderr, "[One]\t" ); fprintf( stderr, MSG ); fprintf( stderr, ".\n" ); }

using namespace chi;
using namespace one;

int main( int argc, const char** argv ) {

	try {
		StdinStream stdin;
		StdoutStream stdout;

		StatementList document;
		if ( argc < 2 )
			document = Parser( stdin ).parse();
		else {
			const char* filename = argv[1];
			ReadFileStream input_file( filename );
			document = Parser( input_file ).parse();
		}

		DynamicBuffer output = Composer( document ).compose();
		stdout.write( output );
	}
	catch ( chi::AllocException& e ) {
		PRINT_ERROR( "Out of memory" );
	}
	catch ( chi::Exception& e ) {
		PRINT_ERROR( "Uncaught Chi exception occurred" );
	}
	catch ( one::ParseException& e ) {
		PRINT_ERROR( "Parse error on line %d, column %d: %s", e.line, e.column, e.message().ptr() );
	}
	catch ( one::Exception& e ) {
		PRINT_ERROR( "Uncaught One exception occurred" );
	}

	return 0;
}
