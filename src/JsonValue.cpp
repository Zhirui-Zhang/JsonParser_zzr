#include "JsonValue.h"
#include "JsonParser.h"
#include "JsonStringify.h"
#include <cassert>

using namespace std;

namespace myJson {

// define all functions declared in JsonValue.h
// ctor dtor cctor rvalue etc
JsonValue::~JsonValue() noexcept {
    json_free();
}

JsonValue::JsonValue(const JsonValue& rhs) noexcept {
    json_free();
    *this = rhs;
}

JsonValue& JsonValue::operator=(const JsonValue& rhs) noexcept {
    json_free();
    m_type = rhs.m_type;
    switch (m_type) {
        case JSON_NUMBER : 
            m_var = std::get<0>(rhs.m_var);     // 0 -> double
            break;
        case JSON_STRING : 
            m_var = std::get<string>(rhs.m_var);
            break;
        // using exised function to initialize array
        case JSON_ARRAY :
            for (auto& ele : std::get<vector<JsonValue>>(rhs.m_var)) {
                json_pushback_array_element(ele);
            }
            break;
        case JSON_OBJECT :
            for (auto& [key, val] : std::get<map<string, JsonValue>>(rhs.m_var)) {
                json_set_object_value(key, val);
            }
            break;
        default :
            break;
    }
    return *this;
}

// using std::move to realize rvalue ctor
JsonValue::JsonValue(JsonValue&& rhs) noexcept {
    json_free();
    *this = std::move(rhs);
}

JsonValue& JsonValue::operator=(JsonValue&& rhs) noexcept {
    json_free();
    m_type = std::move(rhs.m_type);
    m_var = std::move(rhs.m_var);
    return *this;
}

// parse/stringify function
int JsonValue::json_parse(const string& json) noexcept {
    Parser p(*this, json);
    return p.parse();
}

void JsonValue::json_stringify(string& str) const noexcept {
    Generator(*this, str);
}

// init/free function
void JsonValue::json_free() noexcept {
    // switch (m_type) {
    //     case JSON_STRING : 
    //         std::get<string>(m_var).~string();
    //         break;
    //     // using exised function to initialize array
    //     case JSON_ARRAY :
    //         json_clear_array();
    //         std::get<vector<JsonValue>>(m_var).~vector<JsonValue>();
    //         break;
    //     case JSON_OBJECT :
    //         json_clear_object();
    //         std::get<vector<pair<string, JsonValue>>>.~vector<pair<string, JsonValue>>();
    //         break;
    //     default :
    //         break;
    // }
    m_type = JSON_NULL;
    m_var.~variant();   // is this really work?
}

// copy move swap function
void JsonValue::json_copy(const JsonValue& rhs) noexcept {
    json_free();
    *this = rhs;
}

void JsonValue::json_move(JsonValue& rhs) noexcept {
    json_free();
    *this = rhs;
    rhs.json_free();
}

void JsonValue::json_swap(JsonValue& rhs) noexcept {
    if (*this != rhs) {
        JsonValue tmp(rhs);
        rhs = *this;
        *this = tmp;
        tmp.json_free();
    }
}

// all kinds of API provided for user 
JSON_TYPE JsonValue::json_get_type() const noexcept {
    return m_type;
}

void JsonValue::json_set_type(JSON_TYPE type) noexcept {
    m_type = type;
}

// void json_set_type(JSON_TYPE t) noexcept;

bool JsonValue::json_get_boolean() const noexcept {
    assert(m_type == JSON_TRUE || m_type == JSON_FALSE);
    return m_type == JSON_TRUE;
}

void JsonValue::json_set_boolean(bool b) noexcept {
    json_free();
    m_type = b ? JSON_TRUE : JSON_FALSE;
}

double JsonValue::json_get_number() const noexcept {
    assert(m_type == JSON_NUMBER);
    return std::get<double>(m_var);
}

void JsonValue::json_set_number(double d) noexcept {
    json_free();
    m_type = JSON_NUMBER;
    m_var = d;
}

// using existed fuction in std::string
const string JsonValue::json_get_string() const noexcept {
    assert(m_type == JSON_STRING);
    return std::get<string>(m_var);
}   

size_t JsonValue::json_get_string_length() const noexcept {
    assert(m_type == JSON_STRING);
    return std::get<string>(m_var).size();
}

void JsonValue::json_set_string(const string& str) noexcept {
    assert(!str.empty());
    json_free();
    m_type = JSON_STRING;
    m_var = str;
}

// using existed fuction in std::vector
void JsonValue::json_set_array(size_t capacity) noexcept {
    json_free();
    m_type = JSON_ARRAY;
    // need a better method to initialize m_var
    vector<JsonValue> tmp(capacity);
    m_var = tmp;
    tmp.~vector();
}

size_t JsonValue::json_get_array_size() const noexcept {
    assert(m_type == JSON_ARRAY);
    return std::get<vector<JsonValue>>(m_var).size();
}

size_t JsonValue::json_get_array_capacity() const noexcept {
    assert(m_type == JSON_ARRAY);
    return std::get<vector<JsonValue>>(m_var).capacity();
}

void JsonValue::json_reserve_array(size_t capacity) noexcept {
    assert(m_type == JSON_ARRAY);
    std::get<vector<JsonValue>>(m_var).reserve(capacity);
}

void JsonValue::json_shrink_array() noexcept {
    assert(m_type == JSON_ARRAY);
    std::get<vector<JsonValue>>(m_var).shrink_to_fit();
}

void JsonValue::json_clear_array() noexcept {
    assert(m_type == JSON_ARRAY);
    std::get<vector<JsonValue>>(m_var).clear();
}

const JsonValue& JsonValue::json_get_array_element(size_t index) const noexcept {
    assert(m_type == JSON_ARRAY);
    return std::get<vector<JsonValue>>(m_var)[index];
}

void JsonValue::json_pushback_array_element(const JsonValue& jv) noexcept {
    assert(m_type == JSON_ARRAY);
    std::get<vector<JsonValue>>(m_var).push_back(jv);
}

void JsonValue::json_popback_array_element() noexcept {
    assert(m_type == JSON_ARRAY);
    std::get<vector<JsonValue>>(m_var).pop_back();
}

void JsonValue::json_insert_array_element(const JsonValue& jv, size_t index) noexcept{
    assert(m_type == JSON_ARRAY && json_get_array_size() >= index);
    std::get<vector<JsonValue>>(m_var).insert(std::get<vector<JsonValue>>(m_var).begin() + index, jv);
}

void JsonValue::json_erase_array_element(size_t index, size_t count) noexcept {
    assert(m_type == JSON_ARRAY && json_get_array_size() >= index + count);
    std::get<vector<JsonValue>>(m_var).erase(std::get<vector<JsonValue>>(m_var).begin() + index, std::get<vector<JsonValue>>(m_var).begin() + index + count);
}

// using existed fuction in std::map
const map<string, JsonValue>& JsonValue::json_get_object() const noexcept {
    assert(m_type == JSON_OBJECT);
    return std::get<map<string, JsonValue>>(m_var);
}

size_t JsonValue::json_get_object_size() const noexcept {
    assert(m_type == JSON_OBJECT);
    return std::get<map<string, JsonValue>>(m_var).size();
}

void JsonValue::json_clear_object() noexcept {
    assert(m_type == JSON_OBJECT);
    std::get<map<string, JsonValue>>(m_var).clear();
}

bool JsonValue::json_find_object_key(const string& key) const noexcept {
    assert(m_type == JSON_OBJECT);
    return std::get<map<string, JsonValue>>(m_var).find(key) != std::get<map<string, JsonValue>>(m_var).end();
}

const JsonValue& JsonValue::json_get_object_value(const string& key) const noexcept {
    assert(m_type == JSON_OBJECT && json_find_object_key(key));
    // notice that we use 'at' here instead of [] to extract element in map
    return std::get<map<string, JsonValue>>(m_var).at(key);
}

void JsonValue::json_set_object_value(const string& key, const JsonValue& val) noexcept {
    assert(m_type == JSON_OBJECT && json_find_object_key(key));
    std::get<map<string, JsonValue>>(m_var).at(key) = val;
}

void JsonValue::json_remove_object_value(const string& key) noexcept {
    assert(m_type == JSON_OBJECT && json_find_object_key(key));
    std::get<map<string, JsonValue>>(m_var).erase(key);
}

bool operator==(const JsonValue& lhs, const JsonValue& rhs) noexcept {
    if (lhs.m_type != rhs.m_type) {
        return false;
    }
    switch (lhs.m_type) {
        case JSON_NUMBER :
            return std::get<double>(lhs.m_var) == std::get<double>(rhs.m_var);
            break;
        case JSON_STRING :
            return std::get<string>(lhs.m_var) == std::get<string>(rhs.m_var);
            break;    
        case JSON_ARRAY :
            if (lhs.json_get_array_size() != rhs.json_get_array_size()) {
                return false;
            }
            for (size_t i = 0; i < lhs.json_get_array_size(); ++i) {
                if (lhs.json_get_array_element(i) != rhs.json_get_array_element(i)) {
                    return false;
                }
            }
            break;
        case JSON_OBJECT :
            if (lhs.json_get_object_size() != rhs.json_get_object_size()) {
                return false;
            }
            for (auto& [key, val] : std::get<map<string, JsonValue>>(lhs.m_var)) {
                if (!rhs.json_find_object_key(key) || val != rhs.json_get_object_value(key)) {
                    return false;
                }
            }
            break;
        default:
            break;
    }
    return true;
}

bool operator!=(const JsonValue& lhs, const JsonValue& rhs) noexcept {
    return !(lhs == rhs);
}

};