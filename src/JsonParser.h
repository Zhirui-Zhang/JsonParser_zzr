#ifndef JSON_PARSER_H
#define JSON_PARSER_H
#include "JsonValue.h"

namespace myJson {

class Parser {
public:
    Parser();
    // notice that if we don't define dtor here, error "undefined reference" will occur
    ~Parser() {}
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
    const char* parse_hex4(const char* &p, unsigned& u);
    void parse_encode_utf8(string &str, unsigned u) const noexcept;
    int parse_string_raw(string& tmp);
    int parse_string();
    int parse_array();
    int parse_object();
    int parse_value();

private:
    // store & sync all info in input json
    JsonValue& m_jv;
    // same value as input string, using for parsing json
    const char* m_json;
};

};

#endif