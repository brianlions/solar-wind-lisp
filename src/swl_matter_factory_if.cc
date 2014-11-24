#include "swl_matter_factory_if.h"

namespace SolarWindLisp
{

IMatter * IMatterFactory::create(IMatter::matter_type_t t)
{
    switch (t) {
        case IMatter::matter_atom:
            return create_atom();
        case IMatter::matter_molecule:
            return create_molecule();
        case IMatter::matter_prim_proc:
            return create_prim_proc();
        case IMatter::matter_proc:
            return create_proc();
        case IMatter::matter_future:
            return create_future();
        default:
            return NULL;
    }
}
} // namespace SolarWindLisp
