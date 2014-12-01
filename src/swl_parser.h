#ifndef _SOLAR_WIND_LISP_PARSER_H_
#define _SOLAR_WIND_LISP_PARSER_H_

#include <stddef.h>
#include <string>
#include <queue>

#include "swl_expr.h"

namespace SolarWindLisp
{

class IParser
{
public:

    typedef std::deque<std::string> lexical_tokens;

    IParser() :
            _form_str(NULL), _form_len(0)
    {
    }

    virtual ~IParser()
    {
    }

    static bool tokenize(lexical_tokens *result, const char * input,
            ssize_t input_length = -1);
    IMatter * parse(const char * input, ssize_t input_length = -1);
    virtual const char * name() const = 0;

private:

    static const char * S_TERM;
    static const char * S_BS;
    static const char * S_SQ;
    static const char * S_DQ;
    static const char * S_LP;
    static const char * S_RP;

    static const char C_TERM = '\0';
    static const char C_BS = '\\';
    static const char C_SQ = '\'';
    static const char C_DQ = '"';
    static const char C_LP = '(';
    static const char C_RP = ')';

    IMatter * _parse_tokens(bool inner);

    const char * _form_str;
    ssize_t _form_len;
    lexical_tokens _tokens;
};

class SimpleParser: public IParser
{
public:
    SimpleParser()
    {
    }

    virtual ~SimpleParser()
    {
    }

    const char * name() const
    {
        return "SimpleParser";
    }
};

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISP_PARSER_H_
