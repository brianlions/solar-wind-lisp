#ifndef _SOLAR_WIND_LISP_STRING_TOKENS_H_
#define _SOLAR_WIND_LISP_STRING_TOKENS_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

namespace SolarWindLisp
{

class StringTokens
{
private:
    const char ** tokens_;
    char * input_string_;
    uint32_t num_of_tokens_;
    uint32_t max_tokens_;
    uint32_t hint_size_;

public:
    enum
    {
        DEFAULT_HINT_SIZE = 32
    };

    StringTokens(uint32_t hint_size = DEFAULT_HINT_SIZE) :
        tokens_(NULL), input_string_(NULL), num_of_tokens_(0), max_tokens_(0),
        hint_size_(hint_size ? hint_size : DEFAULT_HINT_SIZE)
    {
    }

    ~StringTokens()
    {
        if (tokens_) {
            free(tokens_);
        }
    }

    void clear()
    {
        input_string_ = NULL;
        num_of_tokens_ = 0;
    }

    bool split(char * input, const char * delim = " \f\n\r\t\v")
    {
        clear();
        char * token = NULL;
        char * saved = NULL;
        char * str = input;
        while ((token = strtok_r(str, delim, &saved))) {
            if (!save_token_addr(token)) {
                return false;
            }
            str = NULL;
        }
        return true;
    }

    uint32_t num_of_tokens() const
    {
        return num_of_tokens_;
    }

    const char * token(uint32_t idx) const
    {
        return (idx < num_of_tokens_) ? (*(tokens_ + idx)) : NULL;
    }

private:
    bool save_token_addr(char * ptr)
    {
        if (num_of_tokens_ == max_tokens_) {
            if (!max_tokens_) {
                max_tokens_ = hint_size_;
            }
            else {
                max_tokens_ += hint_size_;
            }
            const char ** all;
            if (!tokens_) {
                if (!(all = (const char **) malloc(max_tokens_ * sizeof(tokens_[0])))) {
                    return false;
                }
            }
            else {
                if (!(all = (const char **) realloc(tokens_, max_tokens_ * sizeof(tokens_[0])))) {
                    max_tokens_ -= hint_size_;
                    return false;
                }
            }
            tokens_ = all;
        }
        *(tokens_ + num_of_tokens_++) = ptr;
        return true;
    }
};

} // namespace SolarWindLisp

#endif // _SOLAR_WIND_LISP_STRING_TOKENS_H_
