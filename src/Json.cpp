#include "Json.h"

namespace myJson {

// define all functions declared in Json.h
// ctor dtor cctor rvalue etc
Json::Json() noexcept : m_jv(new JsonValue()) {}

Json::Json(const Json& rhs) noexcept {
    m_jv.reset(new JsonValue(*(rhs.m_jv)));
}

Json& Json::operator=(const Json& rhs) noexcept {
    m_jv.reset(new JsonValue(*(rhs.m_jv)));
    return *this;
}

Json::Json(Json&& rhs) noexcept {
    m_jv.reset(rhs.m_jv.release());
}

Json& Json::operator=(Json&& rhs) noexcept {
    m_jv.reset(rhs.m_jv.release());
    return *this;
}

// parse/stringify function
int Json::parse(const string& json) noexcept {
    int res = m_jv->parse(json);
    return res;
}

void Json::stringify(string& str) const noexcept {
    m_jv->stringify(str);
}

// copy move swap function
void Json::copy(const Json& rhs) noexcept {
    m_jv.reset(new JsonValue(*(rhs.m_jv)));
}

void Json::move(Json& rhs) noexcept {
    m_jv.reset(new JsonValue(*(rhs.m_jv)));
    // after moving action was finished, call JsonValue dtor in rhs.m_jv to assure JsonValue was moved successfully
    // but not unique_ptr release function, coz it will destory rhs object, causing v2(in JsonTest.cpp) not available
    rhs.m_jv->~JsonValue();
}

void Json::swap(Json& rhs) noexcept {
    // add std here, or it will call member function first
    std::swap(m_jv, rhs.m_jv);
}

// all kinds of API provided for user 
JSON_TYPE Json::get_type() const noexcept {
    return m_jv->get_type();
}

void Json::set_type(JSON_TYPE type) noexcept {
    m_jv->set_type(type);
}

double Json::get_number() const noexcept {
    return m_jv->get_number();
}

void Json::set_number(double d) noexcept {
    m_jv->set_number(d);
}

const string& Json::get_string() const noexcept {
    return m_jv->get_string();
}   

size_t Json::get_string_length() const noexcept {
    return m_jv->get_string_length();
}

void Json::set_string(const string& str) noexcept {
    m_jv->set_string(str);
}

void Json::set_array() noexcept {
    // use tmp object as actual parameter to construct array
    m_jv->set_array(vector<JsonValue>());
}

size_t Json::get_array_size() const noexcept {
    return m_jv->get_array_size();
}

size_t Json::get_array_capacity() const noexcept {
    return m_jv->get_array_capacity();
}

void Json::reserve_array(size_t capacity) noexcept {
    m_jv->reserve_array(capacity);
}

void Json::shrink_array() noexcept {
    m_jv->shrink_array();
}

void Json::clear_array() noexcept {
    m_jv->clear_array();
}

const Json Json::get_array_element(size_t index) const noexcept {
    // create new Json object ptr to return JsonValue object, use const Json as return value to avoid local variable problem 
    Json res;
    res.m_jv.reset(new JsonValue(m_jv->get_array_element(index)));
    return res;
}

void Json::pushback_array_element(const Json& jv) noexcept {
    m_jv->pushback_array_element(*jv.m_jv);
}

void Json::popback_array_element() noexcept {
    m_jv->popback_array_element();
}

void Json::insert_array_element(size_t index, const Json& j) noexcept{
    m_jv->insert_array_element(index, *j.m_jv);
}

void Json::erase_array_element(size_t index, size_t count) noexcept {
    m_jv->erase_array_element(index, count);
}

size_t Json::get_object_size() const noexcept {
    return m_jv->get_object_size();
}

// using existed fuction in std::map
void Json::set_object() noexcept {
    m_jv->set_object(map<string, JsonValue>());
}

void Json::clear_object() noexcept {
    m_jv->clear_object();
}

bool Json::find_object_key(const string& key) const noexcept {
    return m_jv->find_object_key(key);
}

const Json Json::get_object_value(const string& key) const noexcept {
    Json res;
    res.m_jv.reset(new JsonValue(m_jv->get_object_value(key)));
    return res;
}

void Json::set_object_value(const string& key, const Json& val) noexcept {
    m_jv->set_object_value(key, *val.m_jv);
}

void Json::remove_object_value(const string& key) noexcept {
    m_jv->remove_object_value(key);
}

bool operator==(const Json& lhs, const Json& rhs) noexcept {
    return *lhs.m_jv == *rhs.m_jv;
}

bool operator!=(const Json& lhs, const Json& rhs) noexcept {
    return !(lhs == rhs);
}

};