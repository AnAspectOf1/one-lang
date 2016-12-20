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


struct FilePosInfo {
	String<> line;
	unsigned int line_nr, column;
};

FilePosInfo findFilePos( ReadSeekStream* stream, unsigned int pos ) {
	stream->seek( 0 );

	FilePosInfo info;
	/*info.line = stream->readLine();
	info.line_nr = 1;
	while ( pos > info.line.length() ) {
		pos -= info.line.length();
		info.line = stream->readLine();
		info.line_nr++;
	}*/

	return info;
}


int main( int argc, const char** argv ) {

	try {
		StdinStream stdin;
		BufferedReadStream buffered_stdin( &stdin );
		StdoutStream stdout;

		StatementList document;
		ReadSeekStream* input_stream;

		try {
			if ( argc < 2 ) {
				input_stream = &buffered_stdin;
				document = Parser( &buffered_stdin ).parse();
			}
			else {
				const char* filename = argv[1];
				ReadFile input_file = ReadFile::open( filename );
				input_stream = &input_file;
				document = Parser( &input_file, filename ).parse();
			}

			Composer( stdout, &document ).compose();
		}
		catch ( ParseException& e ) {
			FilePosInfo info = findFilePos( input_stream, e.file_pos.pos );
			if ( e.file_pos.filename == "" ) 
				fprintf( stderr, "Parse error on line %d, column %d: %s.\n\t%s\n", info.line_nr, info.column, e.message().ptr(), info.line.ptr() );
			else
				fprintf( stderr, "Parse error in %s:%d:%d: %s.\n\t%s\n", e.file_pos.filename.ptr(), info.line_nr, info.column, e.message().ptr(), info.line.ptr() );
		}
	}
	catch ( chi::AllocException& e ) {
		PRINT_ERROR( "Out of memory" );
	}
	// Only use the following global catch statements in releases, for debugging sessions we are better off having the application get killed in order to find backtraces.
#ifdef NDEBUG	
	catch ( chi::IoException& e ) {
		PRINT_ERROR( "I/O error occurred: %s", e.message()->ptr() )
	}
	catch ( chi::Exception& e ) {
		PRINT_ERROR( "Uncaught Chi exception occurred" );
	}
	catch ( one::Exception& e ) {
		PRINT_ERROR( "Uncaught One exception occurred" );
	}
#endif

	return 0;
}
