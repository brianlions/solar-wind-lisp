/*
 * file name:           include/convertor.h
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Sun Jun 14 12:16:27 2015 CST
 */

#ifndef _SOLAR_WIND_LISP_CONVERTOR_H_
#define _SOLAR_WIND_LISP_CONVERTOR_H_

#include "expr.h"
#include "protobuf_utils.h"

namespace SolarWindLisp
{

using namespace ProtobufUtils;

class Convertor
{
public:
    static bool convert(const Dissector::field_value_t * from, AtomPtr& to)
    {
        return convert(from, to.get());
    }

    static bool convert(const Dissector::field_value_t * from, Atom * to)
    {
        if (!from || !to) {
            return false;
        }

#define ES_FUNKTIONIERT(t, s, d)        \
case FieldDescriptor::CPPTYPE_##t:      \
    to->set_##d(from->data._##s);       \
    break;
        switch (from->type) {
            ES_FUNKTIONIERT(INT32,   i32,    i32);
            ES_FUNKTIONIERT(UINT32,  u32,    u32);
            ES_FUNKTIONIERT(INT64,   i64,    i64);
            ES_FUNKTIONIERT(UINT64,  u64,    u64);
            ES_FUNKTIONIERT(DOUBLE,  double, double);
            ES_FUNKTIONIERT(FLOAT,   float,  double);
            ES_FUNKTIONIERT(BOOL,    bool,   bool);
            ES_FUNKTIONIERT(ENUM,    i32,    i32);
            ES_FUNKTIONIERT(MESSAGE, msg,    pointer);
            case FieldDescriptor::CPPTYPE_STRING:
                to->parse_cstr(from->data._str, strlen(from->data._str));
                break;
            default:
                return false;
                break;
        } // switch
#undef ES_FUNKTIONIERT
        return true;
    }

    static bool convert(const AtomPtr& from, Dissector::field_value_t * to);
    static bool convert(const Atom * from, Dissector::field_value_t * to);
};

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISP_CONVERTOR_H_
