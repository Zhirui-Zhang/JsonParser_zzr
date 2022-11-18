#ifndef JSON_H
#define JSON_H
#include <string>
#include <memory>   // unique_ptr
#include <cstddef>  // size_t
#include "JsonEnum.h"
#include "JsonValue.h"
using namespace std;

namespace myJson {

// forward delclaration
class JsonValue;

class Json {
public:
    // ctor dtor cctor rvalue etc
    Json() noexcept;
    ~Json() noexcept {}
    Json(const Json& rhs) noexcept;
    Json& operator=(const Json& rhs) noexcept;
    Json(Json&& rhs) noexcept;
    Json& operator=(Json&& rhs) noexcept;

    // parse/stringify function
    int parse(const string& json) noexcept;
    void stringify(string& str) const noexcept;

    // copy move swap function
    void copy(const Json& rhs) noexcept;
    void move(Json& rhs) noexcept;
    void swap(Json& rhs) noexcept;
    
    // all kinds of API provided for user 
    JSON_TYPE get_type() const noexcept;
    void set_type(JSON_TYPE type) noexcept;

    double get_number() const noexcept;
    void set_number(double n) noexcept;

    const string& get_string() const noexcept;
    size_t get_string_length() const noexcept;
    void set_string(const string& str) noexcept;

    void set_array() noexcept;
    size_t get_array_size() const noexcept;
    size_t get_array_capacity() const noexcept;
    void reserve_array(size_t capacity) noexcept;
    void shrink_array() noexcept;
    void clear_array() noexcept;
    const Json get_array_element(size_t index) const noexcept;
    void pushback_array_element(const Json& j) noexcept;
    void popback_array_element() noexcept;
    void insert_array_element(size_t index, const Json& j) noexcept;
    void erase_array_element(size_t index, size_t count) noexcept;

    void set_object() noexcept;
    size_t get_object_size() const noexcept;
    void clear_object() noexcept;
    bool find_object_key(const string& key) const noexcept;
    const Json get_object_value(const string& key) const noexcept;
    void set_object_value(const string& key, const Json& val) noexcept;
    void remove_object_value(const string& key) noexcept;

private:
    // create an smart ptr to JsonValue class, Json class provides API while JsonValue class takes charge of realization
    // unique ptr has a counter inside and release automatically when counter equals to 0, so all members can be destroyed without memory leak
    unique_ptr<JsonValue> m_jv;

    // override for == and != operator
    friend bool operator==(const Json& lhs, const Json& rhs) noexcept;
    friend bool operator!=(const Json& lhs, const Json& rhs) noexcept;
};

bool operator==(const Json& lhs, const Json& rhs) noexcept;
bool operator!=(const Json& lhs, const Json& rhs) noexcept;

};

#endif