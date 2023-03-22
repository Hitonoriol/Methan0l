#include <lang/core/library/LibCLI.h>

#include <parser/Parser.h>
#include <expression/IdentifierExpr.h>

namespace mtl
{

METHAN0L_LIBRARY(LibCLI)

void LibCLI::load()
{
	context->get_parser().register_prefix_opr(TokenType::AT);

	/* Access a previously saved value in CAS mode:
	 * `@n`, where `n` is a positive integer */
	prefix_operator(TokenType::AT, LazyUnaryOpr([&](auto &expr) {
		auto idx = val(expr);
		auto &runner = interactive_runner();

		/* `@_` evaluates to the previously saved value */
		return runner.get_saved_value(
			IdentifierExpr::get_name_or_default(expr) == "_" ? 0 : idx.template as<size_t>()
		);
	}));

	function("pause_on_exit", [&](bool value) {
		context->get_env_hook<bool>(CLIHooks::NO_EXIT) = value;
	});

	function("enable_cas_mode", [&](bool value) {
		interactive_runner().enable_cas_mode(value);
		return value;
	});

	function("reset_cas", [&] {
		interactive_runner().reset_cas();
	});
}

} /* namespace mtl */
