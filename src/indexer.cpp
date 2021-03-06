#include "indexer.h"
#include "parser.h"

#include "definition.h"

using namespace chi;
using namespace one;


Indexer::Indexer( const StatementList* stats ) : document(stats) {}

void Indexer::countDefinitions( chi::Size* defs, chi::Size* labels ) const {

	*defs = 0; *labels = 0; Size doc_count = this->document->count();
	for ( Size i = 0; i < doc_count; i++ ) {
		if ( this->document->at(i)->type() == StatementType_Definition )
			(*defs)++;
		else if ( this->document->at(i)->type() == StatementType_Label )
			(*labels)++;
	}
}

Index Indexer::index( const Index* parent ) const {
	Size def_count, label_count;
	this->countDefinitions( &def_count, &label_count );

	Index index( parent );
	index.definitions.grow( def_count + label_count );
	ArrayMap<SPtr<Definition>>& definitions = index.definitions;

	// First populate all the definitions without parameters
	Size j = 0;
	for ( Size i = 0; j < def_count; i++ ) {
		CSPtr<Statement> statement = this->document->at(i);

		if ( statement->type() == StatementType_Definition ) {
			CSPtr<DefinitionStatement> def_stat = statement;

			SPtr<Definition> def; def.alloc(
				Definition(
					def_stat->pos,
					def_stat->body
				)
			);
			def->name = def_stat->name;

			definitions.at(j) = MapEntry<SPtr<Definition>>( *def_stat->name, def );
			j++;
		}
	}

	// Then populate all the parameters in the definitions so that they may use eachother as types in the parameter list
	j = 0;
	for ( Size i = 0; j < def_count; i++ ) {
		CSPtr<Statement> statement = this->document->at(i);

		if ( statement->type() == StatementType_Definition ) {
			const DefinitionStatement* def_stat = statement.cast<DefinitionStatement>();
			SPtr<Definition>& def = definitions.at(j).value;

			def->params.grow( def_stat->params.count() );
			for ( Size k = 0; k < def_stat->params.count(); k++ ) {
				const ParameterStatement param_stat = def_stat->params[k];
				
				if ( param_stat.type_name == 0 || param_stat.type_name->length() == 0 )
					def->params.at(k) = MapEntry<Parameter>( *param_stat.name, Parameter( param_stat.name_pos ) );
				else if ( *param_stat.type_name == "str" || *param_stat.type_name == "num" )
					def->params.at(k) = MapEntry<Parameter>( *param_stat.name, Parameter( Type(true), param_stat.type_name, param_stat.name_pos, param_stat.type_pos ) );
				else
					try {
						Type type = index.findType( *param_stat.type_name );
				
						def->params.at(k) = MapEntry<Parameter>(
							*param_stat.name,
							Parameter( type, param_stat.type_name, param_stat.name_pos, param_stat.type_pos )
						);
					}
					catch ( TypeNotFoundException& ) {
						throw ParseException( def_stat->pos.move( param_stat.type_pos ),
							OS"Type \"" + *param_stat.type_name + "\" of parameter \"" + *param_stat.name + "\" not found" );					
					}
			}

			j++;
		}
	}

	// Oh and do the labels as well
	j = 0;
	for ( Size i = 0; j < label_count; i++ ) {
		const Statement* statement = this->document->at(i);

		if ( statement->type() == StatementType_Label ) {
			const LabelStatement* label_stat = (const LabelStatement*)statement;

			SPtr<Definition> def; def.alloc(
				Definition(
					label_stat->pos,
					label_stat->body
				)
			);
			def->name = label_stat->name;

			definitions.at(def_count + j) = MapEntry<SPtr<Definition>>( *label_stat->name, def );
			j++;
		}
	}

	return index;
}
