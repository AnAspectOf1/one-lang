#ifndef _ONE_TRACKER_STREAM_H
#define _ONE_TRACKER_STREAM_H

#include <qi/int.h>
#include <qi/stream.h>

namespace one {

	class TrackerStream : public qi::ReadStream, public qi::Seekable {
	private:
		void _countNewlines( const qi::Buffer<> buffer ) {
			for ( qi::Size i = 0; i < buffer.count(); i++ ) {
				if ( buffer[i] == '\n' ) {
					this->newlines_read++;
					this->cur_line_read = 0;
				}
				else
					this->cur_line_read++;
			}
		}

	protected:
		qi::ReadStream* stream;
		qi::Seekable* seekable;
		unsigned int bytes_read;
		unsigned int newlines_read;
		unsigned int cur_line_read;

	public:
		TrackerStream( qi::ReadStream* stream, qi::Seekable* seekable ) : stream(stream), seekable(seekable), bytes_read(0), newlines_read(0), cur_line_read(0) {}

		unsigned int bytesRead() const	{ return this->bytes_read; }
		unsigned int newlinesRead() const	{ return this->newlines_read; }
		unsigned int curLineRead() const	{ return this->cur_line_read; }

		qi::Size position() const	{ return this->seekable->position(); }

		void move( qi::Size pos )	{ return this->seekable->move( pos ); }

		void read( qi::BufferBase& buffer ) {
			this->stream->read( buffer );
			this->bytes_read += buffer.count();
			this->_countNewlines( buffer );
		}

		qi::Byte readByte() {
			qi::Byte byte = this->stream->readByte();
			this->bytes_read++;
			if ( byte == '\n' ) {
				this->newlines_read++;
				this->cur_line_read = 0;
			}
			else
				this->cur_line_read++;
			return byte;
		}

		void seek( qi::Size pos )	{ return this->seekable->seek( pos ); }

		void close() {
			this->stream->close();
		}
	};
}

#endif//_ONE_TRACKER_STREAM_H
