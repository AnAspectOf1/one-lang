#ifndef _ONE_STREAM_H
#define _ONE_STREAM_H

#include <chi/buffer.h>
#include <chi/string.h>


namespace one {

	class StreamException {};
	class EndOfStreamException : public StreamException {};

	class Stream {
	public:
		virtual void close() = 0;
	};

	class Readable {
	public:
		virtual chi::Buffer<> read( chi::Size length ) = 0;

		virtual chi::Byte readByte() {
			chi::Buffer<> buffer = this->read(1);
			if ( buffer.count() < 1 )	throw EndOfStreamException();
			return buffer[0];
		}

		virtual char readChar() {
			chi::Buffer<> buffer = this->read(1);
			if ( buffer.count() < 1 )	throw EndOfStreamException();
			return buffer[0];
		}

		virtual chi::String<> readString( chi::Size length ) {
			chi::String<> string( length + 1 );
			chi::Buffer<> buffer = this->read( length );
			
			for ( chi::Size i = 0; i < buffer.count(); i++ ) {
				string[i] = buffer[i];
			}
			string[ buffer.count() ] = '\0';

			return string;
		}
	};

	class ReadStream : public virtual Stream, public Readable {};

	class Writable {
	public:
		virtual chi::Size write( const chi::Buffer<>& buffer ) = 0;
	};

	class WriteStream : public virtual Stream, public Writable {};
}

#endif//_ONE_STREAM_H
