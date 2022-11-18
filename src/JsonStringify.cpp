#include "JsonStringify.h"
#include <cassert>

namespace myJson {

// define all member functions declared in Generator class
Generator::Generator(const JsonValue& jv, string& res) : m_res(res) {
    stringify_value(jv);
}

void Generator::stringify_value(const JsonValue& jv) {
    // declare variables outside when jump into switch clauses, or error : jump to case label [-fpermissive]
    size_t i = 0;
    double tmp = 0;
    char buf[32] = {0};
    switch (jv.get_type()) {
        case JSON_NULL  : m_res += "null"; break;
        case JSON_TRUE  : m_res += "true"; break;
        case JSON_FALSE : m_res += "false"; break;
        case JSON_NUMBER :
            // to_string() is not accessible here, coz the precision will be changed, use %.17g to assign precision by your own
            sprintf(buf, "%.17g", jv.get_number());
            m_res += buf;
            break;
        case JSON_STRING :
            this->stringify_string(jv.get_string());
            break;
        case JSON_ARRAY :
            m_res += '[';
            for (i = 0; i < jv.get_array_size(); ++i) {
                if (i > 0) m_res += ',';
                this->stringify_value(jv.get_array_element(i));
            }
            m_res += ']';
            break;
        case JSON_OBJECT :
            m_res += '{';
            i = 0;
            for (auto itr : jv.get_object()) {
                if (i > 0) m_res += ',';
                this->stringify_string(itr.first);
                m_res += ':';
                this->stringify_value(itr.second);
                ++i;
            }
            m_res += '}';
            break;
        default :
            assert(0 && "invalid type");
            break;
    }
}

void Generator::stringify_string(const string& str) {
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