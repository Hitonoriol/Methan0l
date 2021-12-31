#ifndef SRC_TRANSLATOR_SCOPE_H_
#define SRC_TRANSLATOR_SCOPE_H_

#include <stdexcept>
#include <string>
#include <typeinfo>
#include <unordered_map>

#include "commons.h"

namespace mtl
{

class Scope
{
	private:
		SymbolMap symbols;
		bool transparent;

	public:
		Scope(bool transparent = true);

		bool defined(const std::string &name)
		{
			return symbols.find(name) != symbols.end();
		}

		bool register_symbol(const std::string &name)
		{
			if (defined(name))
				return false;

			symbols.insert(name);
			return true;
		}
};

} /* namespace mtl */

#endif /* SRC_TRANSLATOR_SCOPE_H_ */
