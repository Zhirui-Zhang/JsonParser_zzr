#include "JsonStringify.h"
#include <cassert>
using namespace std;

namespace myJson {

// define all member functions declared in Generator class
Generator::Generator(const JsonValue& jv, string& res) : m_res(res) {
    m_res.clear();
    stringifyValue(jv);
}

void Generator::stringifyValue(const JsonValue& jv) {
    switch (jv.json_get_type()) {
        case JSON_NULL  : m_res += "null"; break;
        case JSON_TRUE  : m_res += "true"; break;
        case JSON_FALSE : m_res += "false"; break;
        case JSON_NUMBER :
            double tmp = jv.json_get_number();
            m_res += to_string(tmp);
            break;
        case JSON_STRING :
            this->stringifyString(jv.json_get_string());
            break;
        case JSON_ARRAY :
            m_res += '[';
            for (size_t i = 0; i < jv.json_get_array_size(); ++i) {
                if (i > 0) m_res += ',';
                this->stringifyValue(jv.json_get_array_element(i));
            }
            m_res += ']';
            break;
        case JSON_OBJECT :
            m_res += '{';
            size_t i = 0;
            for (auto& [key, val] : jv.json_get_object()) {
                if (i > 0) m_res += ',';
                this->stringifyString(key);
                m_res += ':';
                this->stringifyValue(val);
                ++i;
            }
            m_res += '}';
            break;
        default :
            assert(0 && "invalid type");
            break;
    }
}

void Generator::stringifyString(const string& str) {
    m_res += "\"";
    for (auto ch : str) {
        switch(ch) {
            case '\"' : m_res += "\\\""; break;
            case '\\' : m_res += "\\\\"; break;
            case '\b' : m_res += "\\b";  break;
            case '\f' : m_res += "\\f";  break;
            case '\n' : m_res += "\\n";  break;
            case '\r' : m_res += "\\r";  break;
            case '\t' : m_res += "\\t";  break;
            default :
                if (ch < 0x20) {
                    char buf[7] = {0};
                    sprintf(buf, "\\u%04X", ch);
                    m_res += buf;
                } else {
                    m_res += ch;
                }
        }
    }
    m_res += "\"";
}
    
};