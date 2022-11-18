#include "JsonParser.h"
#include <cassert>  // assert()
#include <cctype>   // isdigit()
#include <cerrno>   // errno, ERANGE
#include <cmath>    // HUGE_VAL
#include <cstdlib>  // strtod

namespace myJson {

// using inline function instead of macro define
inline void expect(const char* &c, const char ch) {
    assert(*c == ch);
    ++c;
}

// define all functions declared in Parser class
// ctor : Return const pointer to null-terminated contents. This is a handle to internal data. Do not modify or dire things may happen.
Parser::Parser(JsonValue& jv, const string& json) : m_jv(jv), m_json(json.c_str()) {}

// overall process to parse a json
int Parser::parse() {
    int ret;
    parse_whitespace();
    // OMG I wrote ret == parse_value() once here, what a disaster!!!
    if ((ret = parse_value()) == PARSE_OK) {
        parse_whitespace();
        if (*m_json != '\0') {
            m_jv.set_type(JSON_NULL);
            ret = PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}

// skip all unnecessary spaces
void Parser::parse_whitespace() noexcept {
    while (*m_json == ' ' || *m_json == '\t' || *m_json == '\n' || *m_json == '\r') {
        ++m_json;
    }
}

// parse null/false/true three different types
int Parser::parse_literal(const string& literal, JSON_TYPE type) {
    size_t i;
    expect(m_json, literal[0]);
    for (i = 0; i < literal.size() - 1; ++i) {
        if (m_json[i] != literal[i + 1]) {
            return PARSE_INVALID_VALUE;
        }
    }
    m_json += i;
    m_jv.set_type(type);
    return PARSE_OK;
}

// parse number according to standard https://github.com/miloyip/json-tutorial/blob/master/tutorial02/images/number.png 
int Parser::parse_number() {
    const char* p = m_json;
    if (*p == '-') ++p;
    if (*p == '0') ++p;
    else {
        if (!isdigit(*p)) return PARSE_INVALID_VALUE;
        while (isdigit(*p)) ++p;
    }
    if (*p == '.') {
        ++p;
        if (!isdigit(*p)) return PARSE_INVALID_VALUE;
        while (isdigit(*p)) ++p;
    }
    if (*p == 'e' || *p == 'E') {
        ++p;
        if (*p == '+' || *p == '-') ++p;
        if (!isdigit(*p)) return PARSE_INVALID_VALUE;
        while (isdigit(*p)) ++p;
    }
    errno = 0;
    // strtod : Convert a string to a floating-point number.
    double num = strtod(m_json, NULL);
    if (errno == ERANGE && (num == HUGE_VAL || num == -HUGE_VAL)) return PARSE_NUMBER_TOO_BIG;
    m_jv.set_type(JSON_NUMBER);
    m_jv.set_number(num);
    m_json = p;
    return PARSE_OK;
}

// read four hexadecimal digits
const char* Parser::parse_hex4(const char* &p, unsigned& u) {
    u = 0;
    for (int i = 0; i < 4; ++i) {
        char ch = *p++;
        u <<= 4;
        if (isdigit(ch)) {
            u |= ch - '0';
        } else if (ch >= 'A' && ch <= 'F') {
            u |= ch - 'A' + 10;
        } else if (ch >= 'a' && ch <= 'f') {
            u |= ch - 'a' + 10;
        } else {
            return NULL;
        }
    }
    return p;
}

// parse UTF-8 code
void Parser::parse_encode_utf8(string &str, unsigned u) const noexcept {
    if (u <= 0x7F) {
        str += (char)(u & 0xFF);
    } else if (u <= 0x7FF) {
        str += (char)(0xC0 | ((u >> 6) & 0xFF));
        str += (char)(0x80 | (u & 0x3F));
    } else if (u <= 0xFFFF) {
        str += (char)(0xE0 | ((u >> 12) & 0xFF));
        str += (char)(0x80 | ((u >> 6) & 0x3F));
        str += (char)(0x80 | (u & 0x3F));
    } else {
        assert(u <= 0x10FFFF);
        str += (char)(0xF0 | ((u >> 18) & 0xFF));
        str += (char)(0x80 | ((u >> 12) & 0x3F));
        str += (char)(0x80 | ((u >> 6) & 0x3F));
        str += (char)(0x80 | (u & 0x3F));
    }
}

int Parser::parse_string_raw(string& tmp) {
    expect(m_json, '\"');
    const char* p = m_json;
    unsigned u1, u2;
    while (true) {
        char ch = *p++;
        switch (ch) {
            case '\"' :
                m_json = p;
                return PARSE_OK;
            case '\\' :
                switch (*p++) {
                    case '\"' : tmp += '\"'; break;
                    case '\\' : tmp += '\\'; break;
                    case '/'  : tmp += '/';  break;
                    case 'b'  : tmp += '\b'; break;
                    case 'f'  : tmp += '\f'; break;
                    case 'n'  : tmp += '\n'; break;
                    case 'r'  : tmp += '\r'; break;
                    case 't'  : tmp += '\t'; break;
                    case 'u'  : 
                        if (parse_hex4(p, u1) == NULL) return PARSE_INVALID_UNICODE_HEX;
                        if (u1 >= 0xD800 && u1 <= 0xDBFF) {
                            if (*p++ != '\\') return PARSE_INVALID_UNICODE_SURROGATE;
                            if (*p++ != 'u') return PARSE_INVALID_UNICODE_SURROGATE;
                            if (parse_hex4(p, u2) == NULL) return PARSE_INVALID_UNICODE_HEX;
                            if (u2 < 0xDC00 || u2 > 0XDFFF) return PARSE_INVALID_UNICODE_SURROGATE;
                            u1 = (((u1 - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
                        }
                        parse_encode_utf8(tmp, u1);
                        break;
                    default : return PARSE_INVALID_STRING_ESCAPE;
                }
                break;
            case '\0' :
                return PARSE_MISS_QUOTATION_MARK;
            default : 
                if ((unsigned char)ch < 0x20) return PARSE_INVALID_STRING_CHAR;
                tmp += ch;
        }
    }
}

int Parser::parse_string() {
    int ret;
    string tmp;
    if ((ret = parse_string_raw(tmp)) == PARSE_OK) {
        m_jv.set_string(tmp);
    }
    return ret;
}

// using tmp vetor to store elements in array
int Parser::parse_array() {
    int ret;
    expect(m_json, '[');
    parse_whitespace();
    vector<JsonValue> tmp;
    if (*m_json == ']') {
        ++m_json;
        m_jv.set_array(tmp);
        return PARSE_OK;
    }
    while (true) {
        if ((ret = parse_value()) != PARSE_OK) {
            m_jv.set_type(JSON_NULL);    
            break;
        }
        tmp.push_back(m_jv);
        parse_whitespace();
        if (*m_json == ',') {
            ++m_json;
            parse_whitespace();
        } else if (*m_json == ']') {
            ++m_json;
            m_jv.set_array(tmp);
            // omit break once here
            break;
        } else {
            m_jv.set_type(JSON_NULL); 
            ret = PARSE_MISS_COMMA_OR_SQUARE_BRACKET;
            break;
        }
    }
    return ret;
}

// using tmp string & tmp map to store key & map in object
int Parser::parse_object() {
    int ret;
    expect(m_json, '{');
    string tmpKey;
    map<string, JsonValue> tmpMap;
    parse_whitespace();
    if (*m_json == '}') {
        ++m_json;
        m_jv.set_object(tmpMap); 
        return PARSE_OK;
    }
    while (true) {
        if (*m_json != '\"') {
            m_jv.set_type(JSON_NULL);
            ret = PARSE_MISS_KEY;
            break;
        }
        if ((ret = parse_string_raw(tmpKey)) != PARSE_OK) {
            m_jv.set_type(JSON_NULL);
            break;
        }
        parse_whitespace();
        if (*m_json++ != ':') {
            m_jv.set_type(JSON_NULL);
            ret = PARSE_MISS_COLON;
            break;
        }
        parse_whitespace();
        if ((ret = parse_value()) != PARSE_OK) {
            m_jv.set_type(JSON_NULL);
            break;
        }
        tmpMap[tmpKey] = m_jv;
        tmpKey = "";
        parse_whitespace();
        if (*m_json == ',') {
            ++m_json;
            parse_whitespace();
        } else if (*m_json == '}') {
            ++m_json;
            m_jv.set_object(tmpMap); 
            return PARSE_OK;
        } else {
            m_jv.set_type(JSON_NULL);
            ret = PARSE_MISS_COMMA_OR_CURLY_BRACKET;
            // forgot to break here, TEST_ERROR error happened
            break;
        }
    }
    return ret;
}

int Parser::parse_value() {
    switch (*m_json) {
        case 't' : return parse_literal("true", JSON_TRUE);
        case 'f' : return parse_literal("false", JSON_FALSE);
        case 'n' : return parse_literal("null", JSON_NULL);
        case '\"' : return parse_string();
        case '[' : return parse_array();
        case '{' : return parse_object();
        default : return parse_number();
        case '\0' : return PARSE_EXPECT_VALUE;
    }
}

};