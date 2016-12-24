#include "type.h"

using namespace chi;
using namespace one;


Type::Type( CSPtr<Definition>& def ) : def(def) {}

Type::Type( bool is_str ) : _is_str(is_str), def(0) {}

bool Type::is_def() const	{ return this->def != 0; }

bool Type::is_num() const	{ return this->def == 0 && !this->_is_str; }

bool Type::is_str() const	{ return this->def == 0 && this->_is_str; }
