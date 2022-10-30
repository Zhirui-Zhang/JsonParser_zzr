#ifndef JSON_H
#define JSON_H
#include <string>
#include <cassert>
#include <memory>   // unique_ptr
#include <cstddef>  // size_t
using namespace std;

namespace myJson {

// forward declaration
class JsonValue;    

class Json {
public:
    // ctor dtor cctor rvalue etc
    Json() noexcept;
    ~Json() noexcept;
    Json(const Json& rhs) noexcept;
    Json& operator=(const Json& rhs) noexcept;
    Json(Json&& rhs) noexcept;
    Json& operator=(Json&& rhs) noexcept;

    // override for == != operator
    bool operator==(const Json& rhs) noexcept;
    bool operator!=(const Json& rhs) noexcept;

    // copy move swap function
    void json_copy(const Json& rhs) noexcept;
    void json_move(Json& rhs) noexcept;
    void json_swap(Json& rhs) noexcept;

    // parse/stringify function
    void json_parse(const string& json) noexcept;
    void json_parse_file(const string& filename) noexcept;
    void json_stringify(string& str) const noexcept;

    // all kinds of API provided for user 
    JSON_TYPE json_get_type() const noexcept;

    void json_set_null() noexcept;

    int json_get_boolean() const noexcept;
    void json_set_boolean(bool b) noexcept;

    double json_get_number() const noexcept;
    void json_set_number(double n) noexcept;

    const string json_get_string() const noexcept;
    size_t json_get_string_length() const noexcept;
    void json_set_string(const string& str) noexcept;

    void json_set_array(size_t capacity) noexcept;
    size_t json_get_array_size() const noexcept;
    size_t json_get_array_capacity() const noexcept;
    void json_reserve_array(size_t capacity) noexcept;
    void json_shrink_array() noexcept;
    void json_clear_array() noexcept;
    Json json_get_array_element(size_t index) const noexcept;
    Json json_pushback_array_element(const Json& j) noexcept;
    void json_popback_array_element() noexcept;
    Json json_insert_array_element(const Json& j, size_t index);
    void json_erase_array_element(size_t index, size_t count) noexcept;

    void json_set_object(size_t capacity) noexcept;
    size_t json_get_object_size() const noexcept;
    size_t json_get_object_capacity() const noexcept;
    void json_reserve_object(size_t capacity) noexcept;
    void json_shrink_object() noexcept;
    void json_clear_object() noexcept;
    const string& json_get_object_key(size_t index) const noexcept;
    size_t json_get_object_key_length(size_t index) const noexcept;
    Json json_get_object_value(size_t index) const noexcept;
    size_t json_find_object_index(const string& key) const noexcept;
    Json json_find_object_value(const string& key) const noexcept;
    Json json_set_object_value(const string& key, const Json& val) noexcept;
    void json_remove_object_value(size_t index) noexcept;

private:
    // create an small ptr to JsonValue class, Json class provides API while JsonValue class takes charge of realization
    // unique ptr has a counter inside and release automatically when counter equals to 0, so there is no init/free function
    unique_ptr<myJson::JsonValue> m_jv;
};

};

#endif