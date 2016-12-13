#ifndef _ONE_IO_H
#define _ONE_IO_H

#include "stream.h"
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <chi/buffer.h>


namespace one {

	class IoException : public StreamException {};

	class FileStream;
	class ReadFileStream;
	class WriteFileStream;

	class UnknownIoException : public IoException {
		friend FileStream;
		friend ReadFileStream;
		friend WriteFileStream;

		int code;

	protected:
		UnknownIoException() : code( errno ) {}

	public:
		const int& errorCode() const	{ return this->code; }
	};

	class FileStream : public virtual Stream {
	protected:
		int fd;

		FileStream( int fd = 0 ) : fd(fd) {}

	public:
		void close() {
			if ( ::close( this->fd ) != 0 )
				throw UnknownIoException();
		}
	};

	class ReadFileStream : public virtual FileStream, public virtual ReadStream {
	public:
		ReadFileStream() {}
		ReadFileStream( const char* filename ) {
			this->fd = open( filename, O_RDONLY );
			if ( this->fd == -1 )	throw UnknownIoException();
		}

		chi::Buffer<> read( chi::Size length ) {
			if ( length == 0 )	return chi::Buffer<>();

			chi::Buffer<> buffer( length );
			int read = ::read( this->fd, buffer.ptr(), length );
			if ( read == -1 )	throw UnknownIoException();

			if ( (chi::Size)read < length )
				buffer.shrink( length - read );
			return buffer;
		}

		chi::Byte readByte() {
			chi::Byte byte;
			int read = ::read( this->fd, &byte, 1 );
			if ( read == -1 )	throw UnknownIoException();
			if ( read == 0 )	throw EndOfStreamException();

			return byte;
		}
	};

	class WriteFileStream : public virtual FileStream, public virtual WriteStream {
	public:
		WriteFileStream() {}

		chi::Size write( const chi::Buffer<>& buffer ) {
			int written = ::write( this->fd, buffer.ptr(), buffer.size() );
			if ( written == -1 )	throw UnknownIoException();
			
			return written;
		}
	};

	class StderrStream : public WriteFileStream {
	public:
		StderrStream() : FileStream( 2 ) {}
	};

	class StdinStream : public ReadFileStream {
	public:
		StdinStream() : FileStream( 0 ) {}
	};

	class StdoutStream : public WriteFileStream {
	public:
		StdoutStream() : FileStream( 1 ) {}
	};

}

#endif//_ONE_IO_H
