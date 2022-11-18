#ifndef JSON_STRINGIFY_H
#define JSON_STRINGIFY_H
#include "JsonValue.h"

namespace myJson {

// define Generator class, stringify from existed json to string
class Generator {
public:
    Generator(const JsonValue& jv, string& res);
    // notice that if we don't define dtor here, error "undefined reference" will occur
    ~Generator() {}

private:
    Generator(const Generator&) = delete;
    void stringify_value(const JsonValue& jv);
    void stringify_string(const string& str);

private:
    // store stringify info from json
    string& m_res;
};

};

#endif