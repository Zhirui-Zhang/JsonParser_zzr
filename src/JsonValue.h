#ifndef JSON_VALUE_H
#define JSON_VALUE_H
#include <vector>
#include <map>
#include "JsonEnum.h"
#include "Json.h"

using namespace std;

namespace myJson {

class JsonValue {

public:
    // ctor dtor cctor rvalue etc
    JsonValue() noexcept;
    ~JsonValue() noexcept;
    JsonValue(const JsonValue& rhs) noexcept;
    JsonValue& operator=(const JsonValue& rhs) noexcept;

    // parse/stringify function
    int parse(const string& json) noexcept;
    void stringify(string& str) const noexcept;

    // all kinds of API provided for user, notice that all get-type functions can be set as const, which can be used in const objects, and set-type cannot
    JSON_TYPE get_type() const noexcept;
    void set_type(JSON_TYPE t) noexcept;

    double get_number() const noexcept;
    void set_number(double d) noexcept;

    const string& get_string() const noexcept;
    size_t get_string_length() const noexcept;
    void set_string(const string& str) noexcept;

    void set_array(const vector<JsonValue> &arr) noexcept;
    size_t get_array_size() const noexcept;
    size_t get_array_capacity() const noexcept;
    void reserve_array(size_t capacity) noexcept;
    void shrink_array() noexcept;
    void clear_array() noexcept;
    const JsonValue& get_array_element(size_t index) const noexcept;
    void pushback_array_element(const JsonValue& jv) noexcept;
    void popback_array_element() noexcept;
    void insert_array_element(size_t index, const JsonValue& jv) noexcept;
    void erase_array_element(size_t index, size_t count) noexcept;

    const map<string, JsonValue>& get_object() const noexcept;
    void set_object(const map<string, JsonValue>& obj) noexcept;
    size_t get_object_size() const noexcept;
    void clear_object() noexcept;
    bool find_object_key(const string& key) const noexcept;
    const JsonValue& get_object_value(const string& key) const noexcept;
    void set_object_value(const string& key, const JsonValue& val) noexcept;
    void remove_object_value(const string& key) noexcept;

private:
    // indicates type of current json
    JSON_TYPE m_type;

    // be careful that union can not be named here, otherwise deleted ctor error would generate
    union {
        double m_num;
        string m_str;
        vector<JsonValue> m_arr;
        map<string, JsonValue> m_obj;
    };

    // init/free function
    void init(const JsonValue& rhs) noexcept;
    void free() noexcept;

    // override for ==/!= operator
    friend bool operator==(const JsonValue& lhs, const JsonValue& rhs) noexcept;
    friend bool operator!=(const JsonValue& lhs, const JsonValue& rhs) noexcept;
};

bool operator==(const JsonValue& lhs, const JsonValue& rhs) noexcept;
bool operator!=(const JsonValue& lhs, const JsonValue& rhs) noexcept;

};

#endif