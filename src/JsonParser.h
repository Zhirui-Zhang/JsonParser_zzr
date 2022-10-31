#ifndef JSON_PARSER_H
#define JSON_PARSER_H
#include "JsonValue.h"
#include <string>
using namespace std;

namespace myJson {

// define all return types occur when parsing a json
enum PARSE_TYPE {
    PARSE_OK = 0,
    PARSE_EXPECT_VALUE,
    PARSE_INVALID_VALUE,
    PARSE_NOT_SINGULAR,
    PARSE_NUMBER_TOO_BIG,
    PARSE_NUMBER_MISS_QUOTATION_MARK,
    PARSE_INVALID_STRING_ESCAPE,
    PARSE_INVALID_STRING_CHAR,
    PARSE_INVALID_UNICODE_HEX,
    PARSE_INVALID_UNICODE_SURROGATE,
    PARSE_MISS_COMMA_OR_SQUARE_BRACKET,
    PARSE_MISS_KEY,
    PARSE_MISS_COLON,
    PARSE_MISS_COMMA_OR_CURLY_BRACKET
};

class Parser {
public:
    Parser();
    ~Parser();
    // only one way to construct a Parser class, thus copy constructor was set as deleted
    Parser(JsonValue& jv, const string& json);
    // only port provided for outside to parse a json
    int parse();

private:
    Parser(const Parser&) = delete;
    // all necessary API functions provided by Parser class, notice that some funcitons should not set as noexcept
    void parse_whitespace() noexcept;
    int parse_literal(const string& literal, JSON_TYPE type);
    int parse_number();
    const char* parse_hex4(const char* p, unsigned* u);
    void parse_encode_utf8(string &str, unsigned u) const noexcept;
    int parse_string_raw(string& tmp);
    int parse_string();
    int parse_array();
    int parse_object();
    int parse_value();

private:
    JsonValue& m_jv;
    const char* m_json;
};

};

#endif