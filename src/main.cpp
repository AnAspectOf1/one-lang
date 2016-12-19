#include "composer.h"
#include "exception.h"
#include "parser.h"
#include <chi/exception.h>
#include <chi/file.h>
#include <chi/io.h>
#include <cstdio>

#define PRINT_ERROR( MSG... ) \
	{ fprintf( stderr, "[One]\t" ); fprintf( stderr, MSG ); fprintf( stderr, ".\n" ); }

using namespace chi;
using namespace one;

int main( int argc, const char** argv ) {

	try {
		StdinStream stdin;
		BufferedReadStream bufferedStdin( &stdin );
		StdoutStream stdout;

		StatementList document;
		if ( argc < 2 )
			document = Parser( &bufferedStdin, &bufferedStdin ).parse();
		else {
			const char* filename = argv[1];
			ReadFile input_file = ReadFile::open( filename );
			document = Parser( &input_file, &input_file ).parse();
		}

		Composer( stdout, &document ).compose();
	}
	catch ( chi::AllocException& e ) {
		PRINT_ERROR( "Out of memory" );
	}
	catch ( one::ParseException& e ) {
		PRINT_ERROR( "Parse error on line %d, column %d: %s", e.line, e.column, e.message().ptr() );
	}
	// Only use the following global catch statements in releases, for debugging sessions we are better off having the application get killed in order to find backtraces.
#ifdef NDEBUG
	catch ( chi::Exception& e ) {
		PRINT_ERROR( "Uncaught Chi exception occurred" );
	}
	catch ( one::Exception& e ) {
		PRINT_ERROR( "Uncaught One exception occurred" );
	}
#endif

	return 0;
}
