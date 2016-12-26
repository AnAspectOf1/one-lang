#include "composer.h"
#include "exception.h"
#include "file.h"
#include "parser.h"
#include <qi/exception.h>
#include <qi/file.h>
#include <qi/io.h>
#include <cstdio>

#define PRINT_ERROR( MSG... ) \
	{ fprintf( stderr, "[One]\t" ); fprintf( stderr, MSG ); fprintf( stderr, ".\n" ); }

using namespace qi;
using namespace one;


struct FilePosInfo {
	String<> line;
	unsigned int line_nr, column;
};

FilePosInfo findFilePos( const FilePos& pos ) {
	pos.file->seek( 0 );

	FilePosInfo info;
	try {
		info.line = pos.file->readLine();
		info.line_nr = 0;
		unsigned int our_pos = pos.pos;
		while ( our_pos > info.line.length() ) {
			our_pos -= info.line.length() + 1;
			info.line = pos.file->readLine();
			info.line_nr++;
		}
		info.column = our_pos;
	}
	catch ( EndOfStreamException& e ) {
		fprintf(stderr, "Can't find position %d in file.\n", pos.pos );
	}

	return info;
}

String<> mimicSpace( const char* line, const Size length ) {
	String<> space( length, ' ' );

	for ( Size i = 0; i < length; i++ ) {
		if ( line[i] == '\t' )
			space[i] = '\t';
	}

	return space;
}

void printParseException( const ParseException& e) {
	FilePosInfo info = findFilePos( e.file_pos );
	if ( *e.file_pos.filename == "" ) 
		fprintf( stderr, "Parse error on line %d, column %d:\t%s.\n\t%s\n", info.line_nr+1, info.column+1, e.message().ptr(), info.line.ptr() );
	else
		fprintf( stderr, "Parse error in %s:%d:%d:\t%s.\n\t%s\n", e.file_pos.filename->ptr(), info.line_nr+1, info.column+1, e.message().ptr(), info.line.ptr() );
	fprintf( stderr, "\t%s^\n", mimicSpace( info.line.ptr(), info.column ).ptr() );
}


int main( int argc, const char** argv ) {

	try {
		StdinStream stdin;
		BufferedReadStream buffered_stdin( &stdin );
		StdoutStream stdout;

		StatementList document;

		if ( argc < 2 ) {
			try {
				document = Parser( &buffered_stdin ).parse();
				Composer( stdout, &document ).compose();
			}
			catch ( ParseException& e ) {
				printParseException( e );
				return 1;
			}
		}
		else {
			const char* filename = argv[1];
			ReadFile input_file = ReadFile::open( filename );
			try {
				document = Parser( &input_file, filename ).parse();
				Composer( stdout, &document ).compose();
			}
			catch ( ParseException& e ) {
				printParseException( e );
				return 1;
			}
		}
	}
	catch ( qi::AllocException& e ) {
		PRINT_ERROR( "Out of memory" );
	}
	// Only use the following global catch statements in releases, for debugging sessions we are better off having the application get killed in order to find backtraces.
#ifdef NDEBUG	
	catch ( qi::IoException& e ) {
		PRINT_ERROR( "I/O error occurred: %s", e.message()->ptr() )
	}
	catch ( qi::Exception& e ) {
		PRINT_ERROR( "Uncaught qi exception occurred" );
	}
	catch ( one::Exception& e ) {
		PRINT_ERROR( "Uncaught One exception occurred" );
	}
#endif

	return 0;
}
