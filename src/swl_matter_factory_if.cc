#include "swl_matter_factory_if.h"
namespace SolarWindLisp
{
Future * SimpleMatterFactory::create_future(IMatter * expr,
        ScopedEnv * env, InterpreterIF * interpreter)
{
    return Future::create(expr, env, interpreter);
}
} // namespace SolarWindLisp

