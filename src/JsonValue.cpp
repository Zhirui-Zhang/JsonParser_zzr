#include "JsonValue.h"
#include "JsonParser.h"
#include "JsonStringify.h"
#include <cassert>

namespace myJson {

// define all functions declared in JsonValue.h
// ctor dtor cctor rvalue etc
JsonValue::JsonValue() noexcept : m_type(JSON_NULL) {}

JsonValue::~JsonValue() noexcept {
    free();
}

JsonValue::JsonValue(const JsonValue& rhs) noexcept {
    init(rhs);
}

JsonValue& JsonValue::operator=(const JsonValue& rhs) noexcept {
    free();
    init(rhs);
    return *this;
}

// parse/stringify function
int JsonValue::parse(const string& json) noexcept {
    Parser p(*this, json);
    int res = p.parse();
    return res;
}

void JsonValue::stringify(string& str) const noexcept {
    Generator(*this, str);
}

// init/free function
void JsonValue::init(const JsonValue& rhs) noexcept {
    m_type = rhs.m_type;
    switch (m_type) {
        case JSON_NUMBER : 
            m_num = rhs.m_num;     // 0 -> double
            break;
        case JSON_STRING : 
            new(&m_str) string(rhs.m_str);
            break;
        case JSON_ARRAY :
            // m_arr = vector<JsonValue>(rhs.m_arr);
            new(&m_arr) vector<JsonValue>(rhs.m_arr);
            break;
        case JSON_OBJECT :
            // m_obj = map<string, JsonValue>(rhs.m_obj);
            new(&m_obj) map<string, JsonValue>(rhs.m_obj);
            break;
        default :
            break;
    }
}

void JsonValue::free() noexcept {
    // using exised function to destroy JsonValue
    switch (m_type) {
        case JSON_STRING : 
            m_str.~basic_string();
            break;
        case JSON_ARRAY :
            m_arr.~vector<JsonValue>();
            break;
        case JSON_OBJECT :
            m_obj.~map<string, JsonValue>();
            break;
        default :
            break;
    }
    m_type = JSON_NULL;
}

// all kinds of API provided for user 
JSON_TYPE JsonValue::get_type() const noexcept {
    return m_type;
}

void JsonValue::set_type(JSON_TYPE type) noexcept {
    free();
    m_type = type;
}

double JsonValue::get_number() const noexcept {
    assert(m_type == JSON_NUMBER);
    return m_num;
}

void JsonValue::set_number(double d) noexcept {
    free();
    m_type = JSON_NUMBER;
    m_num = d;
}

const string& JsonValue::get_string() const noexcept {
    assert(m_type == JSON_STRING);
    return m_str;
}   

size_t JsonValue::get_string_length() const noexcept {
    assert(m_type == JSON_STRING);
    return m_str.size();
}

void JsonValue::set_string(const string& str) noexcept {
    if (m_type == JSON_STRING) {
        m_str = str;
    } else {
        free();
        m_type = JSON_STRING;
        new(&m_str) string(str);
    }
}

void JsonValue::set_array(const vector<JsonValue> &arr) noexcept {
    if (m_type == JSON_ARRAY) {
        m_arr = arr;
    } else {
        free();
        m_type = JSON_ARRAY;
        new(&m_arr) vector<JsonValue>(arr);
    }
}

size_t JsonValue::get_array_size() const noexcept {
    assert(m_type == JSON_ARRAY);
    return m_arr.size();
}

size_t JsonValue::get_array_capacity() const noexcept {
    assert(m_type == JSON_ARRAY);
    return m_arr.capacity();
}

void JsonValue::reserve_array(size_t capacity) noexcept {
    assert(m_type == JSON_ARRAY);
    m_arr.reserve(capacity);
}

void JsonValue::shrink_array() noexcept {
    assert(m_type == JSON_ARRAY);
    m_arr.shrink_to_fit();
}

void JsonValue::clear_array() noexcept {
    assert(m_type == JSON_ARRAY);
    m_arr.clear();
}

const JsonValue& JsonValue::get_array_element(size_t index) const noexcept {
    assert(m_type == JSON_ARRAY);
    return m_arr[index];
}

void JsonValue::pushback_array_element(const JsonValue& jv) noexcept {
    assert(m_type == JSON_ARRAY);
    m_arr.push_back(jv);
}

void JsonValue::popback_array_element() noexcept {
    assert(m_type == JSON_ARRAY);
    m_arr.pop_back();
}

void JsonValue::insert_array_element(size_t index, const JsonValue& jv) noexcept{
    assert(m_type == JSON_ARRAY && get_array_size() >= index);
    m_arr.insert(m_arr.begin() + index, jv);
}

void JsonValue::erase_array_element(size_t index, size_t count) noexcept {
    assert(m_type == JSON_ARRAY && get_array_size() >= index + count);
    m_arr.erase(m_arr.begin() + index, m_arr.begin() + index + count);
}

size_t JsonValue::get_object_size() const noexcept {
    assert(m_type == JSON_OBJECT);
    return m_obj.size();
}

const map<string, JsonValue>& JsonValue::get_object() const noexcept {
    assert(m_type == JSON_OBJECT);
    return m_obj;
}

// using existed fuction in std::map
void JsonValue::set_object(const map<string, JsonValue>& obj) noexcept {
    if (m_type == JSON_OBJECT) {
        m_obj = obj;
    } else {
        free();
        m_type = JSON_OBJECT;
        new(&m_obj) map<string, JsonValue>(obj);
    }
}

void JsonValue::clear_object() noexcept {
    assert(m_type == JSON_OBJECT);
    m_obj.clear();
}

bool JsonValue::find_object_key(const string& key) const noexcept {
    assert(m_type == JSON_OBJECT);
    return m_obj.find(key) != m_obj.end();
}

const JsonValue& JsonValue::get_object_value(const string& key) const noexcept {
    assert(m_type == JSON_OBJECT && find_object_key(key));
    // notice that we use 'at' here instead of [] to extract element in map
    return m_obj.at(key);
}

void JsonValue::set_object_value(const string& key, const JsonValue& val) noexcept {
    // it is not neccessary to assure that key has existed, coz we are gonna insert new pair [key, val] now
    assert(m_type == JSON_OBJECT);
    if (find_object_key(key)) m_obj[key] = val;
    else m_obj.insert({key, val});
}

void JsonValue::remove_object_value(const string& key) noexcept {
    assert(m_type == JSON_OBJECT && find_object_key(key));
    m_obj.erase(key);
}

bool operator==(const JsonValue& lhs, const JsonValue& rhs) noexcept {
    if (lhs.m_type != rhs.m_type) {
        return false;
    }
    switch (lhs.m_type) {
        case JSON_NUMBER :
            return lhs.m_num == rhs.m_num;
            break;
        case JSON_STRING :
            return lhs.m_str == rhs.m_str;
            break;    
        case JSON_ARRAY :
            if (lhs.get_array_size() != rhs.get_array_size()) {
                return false;
            }
            for (size_t i = 0; i < lhs.get_array_size(); ++i) {
                if (lhs.get_array_element(i) != rhs.get_array_element(i)) {
                    return false;
                }
            }
            break;
        case JSON_OBJECT :
            if (lhs.get_object_size() != rhs.get_object_size()) {
                return false;
            }
            for (auto itr : lhs.m_obj) {
                if (!rhs.find_object_key(itr.first) || itr.second != rhs.get_object_value(itr.first)) {
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