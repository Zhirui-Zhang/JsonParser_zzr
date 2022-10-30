#ifndef JSON_VALUE_H
#define JSON_VALUE_H
// #include "Json.h"
#include <string>
#include <vector>
#include <map>
// #include <utility>  // pair<string, JsonValue>
#include <variant>
// notice that maybe you should set "cppStandard": "c++17" in c_cpp_properties.json
using namespace std;

namespace myJson {

// enum for all JSON_TYPE
enum JSON_TYPE {
    JSON_NULL = 0,
    JSON_FALSE,
    JSON_TRUE,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT
};

// forward declaration
bool operator==(const JsonValue& lhs, const JsonValue& rhs) noexcept;
bool operator!=(const JsonValue& lhs, const JsonValue& rhs) noexcept;

class JsonValue {
public:
    // JsonValue class doesn't provide any interfaces, thus all members can be set as private

    // ctor dtor cctor rvalue etc
    JsonValue() noexcept { json_init(); }
    ~JsonValue() noexcept;
    JsonValue(const JsonValue& rhs) noexcept;
    JsonValue& operator=(const JsonValue& rhs) noexcept;
    JsonValue(JsonValue&& rhs) noexcept;
    JsonValue& operator=(JsonValue&& rhs) noexcept;

    // parse/stringify function
    void json_parse(const string& json) noexcept;
    void json_parse_file(const string& filename) noexcept;
    void json_stringify(string& str) const noexcept;

    // init/free function
    inline void json_init() noexcept { m_type = JSON_NULL; }
    void json_free() noexcept;

    // copy move swap function
    void json_copy(const JsonValue& rhs) noexcept;
    void json_move(JsonValue& rhs) noexcept;
    void json_swap(JsonValue& rhs) noexcept;

    // override for ==/!= operator
    friend bool operator==(const JsonValue& lhs, const JsonValue& rhs) noexcept;
    friend bool operator!=(const JsonValue& lhs, const JsonValue& rhs) noexcept;

    // all kinds of API provided for user 
    JSON_TYPE json_get_type() const noexcept;
    // void json_set_type(JSON_TYPE t) noexcept;

    bool json_get_boolean() const noexcept;
    void json_set_boolean(bool b) noexcept;

    double json_get_number() const noexcept;
    void json_set_number(double d) noexcept;

    const string json_get_string() const noexcept;
    size_t json_get_string_length() const noexcept;
    void json_set_string(const string& str) noexcept;

    void json_set_array(size_t capacity) noexcept;
    size_t json_get_array_size() const noexcept;
    size_t json_get_array_capacity() const noexcept;
    void json_reserve_array(size_t capacity) noexcept;
    void json_shrink_array() noexcept;
    void json_clear_array() noexcept;
    const JsonValue& json_get_array_element(size_t index) const noexcept;
    void json_pushback_array_element(const JsonValue& jv) noexcept;
    void json_popback_array_element() noexcept;
    void json_insert_array_element(const JsonValue& jv, size_t index) noexcept;
    void json_erase_array_element(size_t index, size_t count) noexcept;

    size_t json_get_object_size() const noexcept;
    void json_clear_object() noexcept;
    bool json_find_object_key(const string& key) const noexcept;
    const JsonValue& json_get_object_value(const string& key) const noexcept;
    void json_set_object_value(const string& key, const JsonValue& val) noexcept;
    void json_remove_object_value(const string& key) noexcept;

private:
    // indicates type of current json
    JSON_TYPE m_type;
    // C++17 new characteristic, same function as union, but more safer, reference to https://zh.cppreference.com/w/cpp/utility/variant 
    // variant<double, string, vector<JsonValue>, vector<pair<string, JsonValue>>> m_var;
    std::variant<double, string, vector<JsonValue>, map<string, JsonValue>> m_var;
};

};

#endif