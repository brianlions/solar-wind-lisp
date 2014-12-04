#include <string.h>
#include "swl_interpreter.h"
#include "swl_script_reader.h"
namespace SolarWindLisp {

bool ScriptFileReader::initialize(const char * filename)
{
    static const size_t DEFAULT_LINE_BUF_SIZE = 4096;

    if (!(_line_buf = (char *) malloc(DEFAULT_LINE_BUF_SIZE))) {
        PRETTY_MESSAGE(stderr, "cannot allocate line buffer");
        return false;
    }

    _line_buf_size = DEFAULT_LINE_BUF_SIZE;
    _line_buf[0] = '\0';

    if (!filename || !strcmp(filename, "-")) {
        _filename = strdup("stdin");
    }
    else {
        _filename = strdup(filename);
    }

    if (!_filename) {
        PRETTY_MESSAGE(stderr, "cannot make a copy of the file name");
        return false;
    }

    if (!strcmp(_filename, "stdin")) {
        _filehandle = stdin;
        return true;
    }
    else {
        if (!(_filehandle = fopen(_filename, "r"))) {
            PRETTY_MESSAGE(stderr, "cannot open file `%s'", _filename);
            return false;
        }

        return true;
    }
}

ssize_t ScriptFileReader::get_next_statement(char ** stmt_buf, ssize_t * stmt_buf_size)
{
    int left_paren = 0;
    int c = 0;
    int skip_comment = true;
    ssize_t stmt_offset = 0;

    bool bare_expr = false;
    char quote_char = '\0';
    bool escape_mode = false;
    while ((c = get_next_char(skip_comment, left_paren)) != -1) {
        skip_comment = false;

        if (escape_mode) {
            escape_mode = false;
            save_char(c, stmt_buf, stmt_buf_size, &stmt_offset);
        }
        else if (c == '\\') {
            if (quote_char) {
                escape_mode = true;
            }
            save_char(c, stmt_buf, stmt_buf_size, &stmt_offset);
        }
        else if (c == '\'' || c == '\"') {
            if (!quote_char) {
                quote_char = c;
                save_char(c, stmt_buf, stmt_buf_size, &stmt_offset);
            }
            else if (quote_char == c) {
                quote_char = '\0';
                save_char(c, stmt_buf, stmt_buf_size, &stmt_offset);
            }
            else {
                save_char(c, stmt_buf, stmt_buf_size, &stmt_offset);
            }
        }
        else if (c == '\r' || c == '\n') {
            if (bare_expr) {
                return stmt_offset;
            }

            skip_comment = true;
            save_char(c, stmt_buf, stmt_buf_size, &stmt_offset);
        }
        else if (c == '(') {
            if (bare_expr) {
                bare_expr = false;
            }
            left_paren++;
            save_char(c, stmt_buf, stmt_buf_size, &stmt_offset);
        }
        else if (c == ')') {
            if (left_paren <= 0) {
                // unexpected ')' char
                return -1;
            }
            else if (--left_paren == 0) {
                // found an expression
                save_char(c, stmt_buf, stmt_buf_size, &stmt_offset);
                return stmt_offset;
            }
            else {
                save_char(c, stmt_buf, stmt_buf_size, &stmt_offset);
            }
        }
        else {
            if (!left_paren) {
                bare_expr = true;
            }
            save_char(c, stmt_buf, stmt_buf_size, &stmt_offset);
        }
    }

    return -1;
}

int ScriptFileReader::save_char(int c, char ** stmt_buf,
        ssize_t * stmt_buf_size, ssize_t * offset)
{
    static const size_t DELTA = 1024;

    if (*offset < *stmt_buf_size - 1) {
        (*stmt_buf)[*offset] = c;
        (*stmt_buf)[*offset + 1] = '\0';
        *offset += 1;
        return 0;
    }

    char * ptr = (char *) realloc(*stmt_buf, *stmt_buf_size + DELTA);
    if (!ptr) {
        return -1;
    }

    *stmt_buf = ptr;
    *stmt_buf_size = *stmt_buf_size + DELTA;
    (*stmt_buf)[*offset] = c;
    (*stmt_buf)[*offset + 1] = '\0';
    *offset += 1;
    return 0;
}

int ScriptFileReader::get_next_char(bool skip_comment, int num_left_paren)
{
    int c = _line_buf[_line_buf_offset++];
    if (c) {
        return c;
    }

    _line_buf_offset = 0;
    ssize_t input_len = 0;
    while (true) {
        if (_filehandle == stdin) {
            fprintf(stderr, "%s", InterpreterIF::prompt(num_left_paren == 0));
        }

        input_len = getline(&_line_buf, &_line_buf_size, _filehandle);
        ++_line_number;
        if (input_len == -1) {
            break;
        }
        if (_line_buf[0] == '\n') {
            continue;
        }

        if (!skip_comment) {
            break;
        }

        // find first non-whitespace char
        ssize_t i = 0;
        for (; i < input_len; ++i) {
            if (!isspace(_line_buf[i])) {
                break;
            }
        }

        if (i == input_len - 1 || _line_buf[i] == ';' || _line_buf[i] == '\n') {
            continue;
        }
        else {
            _line_buf_offset = i;
            break;
        }
    }

    if (input_len == -1) {
        return -1;
    }

    return _line_buf[_line_buf_offset++];
}

} // namespace SolarWindLisp
