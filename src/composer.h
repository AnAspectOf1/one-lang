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
	class Type;

	class TypeNotFoundException : public Exception {};

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

		chi::CSPtr<Statement> evaluate( chi::CSPtr<Statement>& statement, const Type& type_name ) const;
		chi::CSPtr<Statement> evaluateDefinition( const Type& type, chi::CSPtr<DefinitionStatement> statement, const chi::Map<chi::CSPtr<DefinitionStatement>>& argument_index ) const;
		chi::CSPtr<Statement> evaluateIdentity( const Type& type, chi::CSPtr<IdentityStatement> statement ) const;
		chi::CSPtr<Statement> evaluateScope( const Type& type, chi::CSPtr<ScopeStatement> statement ) const;
		chi::CSPtr<Statement> evaluateStatement( const Type& type, chi::CSPtr<Statement> statement ) const;
		chi::CSPtr<DefinitionStatement> findDefinition( const chi::StringBase& name ) const;
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

	class Type {
	protected:
		bool _is_str;

	public:
		const DefinitionStatement* def;

		Type( const DefinitionStatement* def ) : def(def) {}
		Type( bool is_str ) : _is_str(is_str), def(0) {}
		Type( const ComposerContext* context, const chi::StringBase& type_name ) {
			if ( type_name == "str" ) {
				this->def = 0;
				this->_is_str = true;
			}
			else if ( type_name == "num" ) {
				this->def = 0;
				this->_is_str = false;
			}
			else {
				this->def = context->findDefinition( type_name );
				if ( this->def == 0 )
					throw TypeNotFoundException();
			}
		}

		bool is_def() const	{ return this->def != 0; }
		bool is_num() const	{ return this->def == 0 && !this->_is_str; }
		bool is_str() const	{ return this->def == 0 && this->_is_str; }
	};
}

#endif//_ONE_COMPOSER_H
