#ifndef _ONE_COMPOSER_H
#define _ONE_COMPOSER_H

#include "exception.h"
#include "indexer.h"
#include "statement.h"
#include <chi/buffer.h>
#include <chi/stream.h>
#include <chi/string.h>


namespace one {

	class Composer;

	class UnsupportedFormatException : public Exception {
		chi::String<> format;

	public:
		UnsupportedFormatException( const chi::StringBase& format ) : format(format) {}
	};

	// The composer context holds information about the current context the Composer is composing for.
	// Most of its members are shared pointers (SPtr) because many contexts are mostly similar and thus hold a lot of the same values.
	class ComposerContext {
		friend Composer;

		const ComposerContext* parent;

	public:
		ComposerContext() : parent(0) {
			this->string_format.alloc( chi::String<>( "utf8" ) );
			this->number_format.alloc( chi::String<>( "uint8" ) );
		}
		ComposerContext( const ComposerContext* parent ) : parent(parent), string_format(parent->string_format), number_format(parent->number_format), space(parent->space) {}

		chi::CSPtr<chi::StringBase> filename;

		// Formats to use for strings and numbers
		chi::SPtr<chi::StringBase> string_format;
		chi::SPtr<chi::StringBase> number_format;

		// The namespace we are in
		chi::Array<chi::SPtr<chi::StringBase>> space;
		
		// The index for the local context or the argument index for the local definition
		chi::SPtr<Index> index;

		bool complies( const Statement* statement, const chi::StringBase& type_name ) const;
		chi::CSPtr<DefinitionStatement> findDefinition( const chi::StringBase& name ) const;

		bool complyCheckDefinition( const ComposerContext& context, const DefinitionStatement* type, const DefinitionStatement* statement, const chi::Map<chi::CSPtr<DefinitionStatement>>& argument_index ) const;
		bool complyCheckFormat( const ComposerContext& context, const DefinitionStatement* type, const FormatStatement* statement ) const;
		bool complyCheckIdentity( const ComposerContext& context, const DefinitionStatement* type, const IdentityStatement* statement ) const;
		bool complyCheckScope( const ComposerContext& context, const DefinitionStatement* type, const ScopeStatement* statement ) const;
		bool complyCheckStatement( const ComposerContext& context, const DefinitionStatement* type, const Statement* statement ) const;
	};

	class Composer {
		const StatementList* document;
		chi::WriteStream& stream;
		chi::String<> filename;

		void composeFormat( const ComposerContext& context, const FormatStatement* statement );
		void composeIdentity( const ComposerContext& context, const IdentityStatement* statement );
		void composeScope( const ComposerContext& context, const ScopeStatement* statement );
		void composeStatement( const ComposerContext& context, const Statement* statement );
		void composeStatements( const ComposerContext& context, const StatementList& statements );
		void composeString( const ComposerContext& context, const StringStatement* statement );

	public:
		Composer( chi::WriteStream& stream, const StatementList* document, const chi::StringBase& filename = chi::String<>() );

		void compose();
	};
}

#endif//_ONE_COMPOSER_H
