/*
 * file name:           src/parser.cc
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Sat Nov 22 01:49:21 2014 CST
 */

#include "parser.h"

namespace SolarWindLisp
{
const char * ParserIF::S_TERM = "\0";
const char * ParserIF::S_BS = "\\";
const char * ParserIF::S_SQ = "\'";
const char * ParserIF::S_DQ = "\"";
const char * ParserIF::S_LP = "(";
const char * ParserIF::S_RP = ")";

const char ParserIF::C_TERM;
const char ParserIF::C_BS;
const char ParserIF::C_SQ;
const char ParserIF::C_DQ;
const char ParserIF::C_LP;
const char ParserIF::C_RP;

bool ParserIF::tokenize(lexical_tokens * p_result, const char * input,
        ssize_t input_length)
{
    p_result->clear();

    std::string current;

    char quote_char = C_TERM;
    bool escape_mode = false;

    for (ssize_t idx = 0;
            (input_length >= 0 && idx < input_length)
                    || (input_length < 0 && input[idx] != '\0'); ++idx) {
        char c = input[idx];
        if (escape_mode) {
            current += c;
            escape_mode = false;
        }
        else if (c == C_BS) {
            if (quote_char) {
                escape_mode = true;
            }
            else {
                current += c;
            }
        }
        else if (c == C_SQ || c == C_DQ) {
            if (!quote_char) {
                quote_char = c;
                current = quote_char;
            }
            else if (quote_char == c) {
                current += c;
                quote_char = C_TERM;
            }
            else {
                current += c;
            }
        }
        else if (isspace(c)) {
            if (quote_char) {
                current += c;
            }
            else if (current.length() > 0) {
                p_result->push_back(current);
                current = "";
            }
        }
        else if (c == C_LP || c == C_RP) {
            if (current.length() > 0) {
                p_result->push_back(current);
                current = "";
            }
            p_result->push_back(c == C_LP ? S_LP : S_RP);
        }
        else {
            current += c;
        }
    }

    if (current.length() > 0) {
        p_result->push_back(current);
    }

    return p_result->size() != 0;
}

MatterPtr ParserIF::parse(const char * input, ssize_t input_length)
{
    // setup
    _form_str = input;
    _form_len = input_length;

    if (!ParserIF::tokenize(&_tokens, _form_str, _form_len)) {
        return NULL;
    }

    MatterPtr result = _parse_tokens(false);

    // tear down
    _form_str = NULL;
    _form_len = 0;

    return result;
}

MatterPtr ParserIF::_parse_tokens(bool inner)
{
    std::string current;

    CompositeExprPtr result = CompositeExpr::create();
    while (_tokens.size() > 0) {
        current = _tokens.front();
        _tokens.pop_front();

        if (current == S_LP) {
            MatterPtr expr = _parse_tokens(true);
            if (!expr.get()) {
                return MatterPtr(NULL);
            }
            else {
                result->append_expr(expr);
            }
        }
        else if (current == S_RP) {
            if (inner) {
                return result;
            }
            else {
                // unmatched close parenthesis in input
                return MatterPtr(NULL);
            }
        }
        else {
            AtomPtr expr = Atom::create(current.c_str(), current.length());
            if (!expr.get()) {
                return MatterPtr(NULL);
            }
            else {
                result->append_expr(expr);
            }
        }
    } // end while

    if (inner) {
        // parser is in an inner context when the input is finished.
        return MatterPtr(NULL);
    }
    else {
        return result;
    }
}

} // namespace SolarWindLisp
