/*
 * file name:           src/swl_script_reader.h
 *
 * author:              Brian Yi ZHANG
 * email:               brianlions@gmail.com
 * date created:        Wed Dec 03 22:12:29 2014 CST
 */

#ifndef _SOLAR_WIND_LISP_SCRIPT_READER_H_
#define _SOLAR_WIND_LISP_SCRIPT_READER_H_

#include <stdio.h>
#include <stddef.h>
#include "pretty_message.h"

namespace SolarWindLisp
{

class ScriptFileReader
{
public:
    ScriptFileReader() :
            _filename(NULL), _line_number(0), _filehandle(NULL), //
            _line_buf(NULL), _line_buf_size(0), _line_buf_offset(0)
    {
    }

    bool initialize(const char * filename = NULL);
    ssize_t get_next_statement(char ** stmt_buf, ssize_t * stmt_buf_size);

    const char * filename() const
    {
        return _filename;
    }

    size_t line_number() const
    {
        return _line_number;
    }

    bool eof() const
    {
        return feof(_filehandle);
    }

    ~ScriptFileReader()
    {
        if (_filename) {
            free(_filename);
        }

        if (_filehandle != stdin) {
            fclose(_filehandle);
        }

        if (_line_buf) {
            free(_line_buf);
        }
    }

private:
    /*
     * Description:
     *   Save `c' into buffer `stmt_buf' at `offset', reallocate buffer if
     *   needed.
     * Return value:
     *   0 if OK, -1 on error.
     */
    int save_char(int c, char ** stmt_buf, ssize_t * stmt_buf_size,
            ssize_t * offset);

    /*
     * Return value:
     *   -1 on error, or next char from input stream.
     */
    int get_next_char(bool skip_comment, int num_left_paren);

    char * _filename;
    size_t _line_number;
    FILE * _filehandle;
    char * _line_buf;
    size_t _line_buf_size;
    ssize_t _line_buf_offset;
};

} // namespace ScriptFileReader

#endif // _SOLAR_WIND_LISP_SCRIPT_READER_H_
