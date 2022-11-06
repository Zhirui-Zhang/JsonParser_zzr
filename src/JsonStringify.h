#ifndef JSON_STRINGIFY_H
#define JSON_STRINGIFY_H
#include "JsonValue.h"
#include <string>
using namespace std;

namespace myJson {

// define Generator class, stringify from existed json to string
class Generator {
public:
    Generator(const JsonValue& jv, string& res);
    ~Generator();

private:
    Generator(const Generator&) = delete;
    void stringifyValue(const JsonValue& jv);
    void stringifyString(const string& str);

private:
    string& m_res;

};

};

#endif