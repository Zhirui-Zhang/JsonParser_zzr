#include <iostream>
#include <cstring>
#include <cstdlib>
#include <algorithm>    // sort algorithm
#include "src/Json.h"

// define static variables for test
static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

using namespace std;
using namespace myJson;

// using macro definition for test, notice that \ should be used in contiuing context
#define EXPECT_EQ_BASE(expect, actual) \
    do { \
        ++test_count; \
        if ((expect) == (actual)) { \
            ++test_pass; \
        } else { \
            cerr << __FILE__ << ": " << __LINE__ << ": expect: " << expect <<  ", actual: " << actual << endl; \
            main_ret = 1; \
        } \
    } while(0)

#define TEST_LITERAL(expect, json) \
    do { \
        Json v; \
        EXPECT_EQ_BASE(PARSE_OK, v.parse(json)); \
        EXPECT_EQ_BASE((expect), v.get_type()); \
    } while(0)

static void test_parse_literal() {
    TEST_LITERAL(JSON_NULL, "null");
    TEST_LITERAL(JSON_TRUE, "true");
    TEST_LITERAL(JSON_FALSE, "false");
}

#define TEST_NUMBER(expect, json) \
    do { \
        Json v; \
        EXPECT_EQ_BASE(PARSE_OK, v.parse(json)); \
        EXPECT_EQ_BASE(JSON_NUMBER, v.get_type()); \
        EXPECT_EQ_BASE((expect), v.get_number()); \
    } while(0)

void static test_parse_number() {
    TEST_NUMBER(0.0, "0");
    TEST_NUMBER(0.0, "-0");
    TEST_NUMBER(0.0, "-0.0");
    TEST_NUMBER(1.0, "1");
    TEST_NUMBER(-1.0, "-1");
    TEST_NUMBER(1.5, "1.5");
    TEST_NUMBER(-1.5, "-1.5");
    TEST_NUMBER(3.1416, "3.1416");
    TEST_NUMBER(1E10, "1E10");
    TEST_NUMBER(1e10, "1e10");
    TEST_NUMBER(1E+10, "1E+10");
    TEST_NUMBER(1E-10, "1E-10");
    TEST_NUMBER(-1E10, "-1E10");
    TEST_NUMBER(-1e10, "-1e10");
    TEST_NUMBER(-1E+10, "-1E+10");
    TEST_NUMBER(-1E-10, "-1E-10");
    TEST_NUMBER(1.234E+10, "1.234E+10");
    TEST_NUMBER(1.234E-10, "1.234E-10");
    TEST_NUMBER(0.0, "1e-10000"); /* must underflow */

    TEST_NUMBER(1.0000000000000002, "1.0000000000000002"); /* the smallest number > 1 */
    TEST_NUMBER( 4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
    TEST_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
    TEST_NUMBER( 2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
    TEST_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
    TEST_NUMBER( 2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
    TEST_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
    TEST_NUMBER( 1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
    TEST_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");
}

#define TEST_STRING(expect, json) \
    do { \
        Json v; \
        EXPECT_EQ_BASE(PARSE_OK, v.parse(json)); \
        EXPECT_EQ_BASE(JSON_STRING, v.get_type()); \
        EXPECT_EQ_BASE(0, memcmp((expect), v.get_string().c_str(), v.get_string_length())); \
    } while(0)

static void test_parse_string() {
    TEST_STRING("", "\"\"");
    TEST_STRING("Hello", "\"Hello\"");
    TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
    TEST_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
    TEST_STRING("Hello\0World", "\"Hello\\u0000World\"");
    TEST_STRING("\x24", "\"\\u0024\"");         /* Dollar sign U+0024 */
    TEST_STRING("\xC2\xA2", "\"\\u00A2\"");     /* Cents sign U+00A2 */
    TEST_STRING("\xE2\x82\xAC", "\"\\u20AC\""); /* Euro sign U+20AC */
    TEST_STRING("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\"");  /* G clef sign U+1D11E */
    TEST_STRING("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\"");  /* G clef sign U+1D11E */
}

static void test_parse_array() {
    size_t i, j;
    Json v;

    EXPECT_EQ_BASE(PARSE_OK, v.parse("[ ]"));
    EXPECT_EQ_BASE(JSON_ARRAY, v.get_type());
    EXPECT_EQ_BASE(0, v.get_array_size());

    EXPECT_EQ_BASE(PARSE_OK, v.parse("[ null , false , true , 123 , \"abc\" ]"));
    EXPECT_EQ_BASE(JSON_ARRAY, v.get_type());
    EXPECT_EQ_BASE(5, v.get_array_size());
    EXPECT_EQ_BASE(JSON_NULL, v.get_array_element(0).get_type());
    EXPECT_EQ_BASE(JSON_FALSE, v.get_array_element(1).get_type());
    EXPECT_EQ_BASE(JSON_TRUE, v.get_array_element(2).get_type());
    EXPECT_EQ_BASE(JSON_NUMBER, v.get_array_element(3).get_type());
    EXPECT_EQ_BASE(JSON_STRING, v.get_array_element(4).get_type());
    EXPECT_EQ_BASE(123.0, v.get_array_element(3).get_number());
    EXPECT_EQ_BASE("abc", v.get_array_element(4).get_string());

    EXPECT_EQ_BASE(PARSE_OK, v.parse("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]"));
    EXPECT_EQ_BASE(JSON_ARRAY, v.get_type());
    EXPECT_EQ_BASE(4, v.get_array_size());
    for (i = 0; i < 4; ++i) {
        Json a = v.get_array_element(i);
        EXPECT_EQ_BASE(JSON_ARRAY, a.get_type());
        EXPECT_EQ_BASE(i, a.get_array_size());
        for (j = 0; j < i; ++j) {
            Json e = a.get_array_element(j);
            EXPECT_EQ_BASE(JSON_NUMBER, e.get_type());
            EXPECT_EQ_BASE((double)j, e.get_number());
        }
    }
}

static void test_parse_object() {
    size_t i;
    Json v;

    EXPECT_EQ_BASE(PARSE_OK, v.parse(" { } "));
    EXPECT_EQ_BASE(JSON_OBJECT, v.get_type());
    EXPECT_EQ_BASE(0, v.get_object_size());

    EXPECT_EQ_BASE(PARSE_OK, v.parse(
        " { "
        "\"n\" : null , "
        "\"f\" : false , "
        "\"t\" : true , "
        "\"i\" : 123 , "
        "\"s\" : \"abc\", "
        "\"a\" : [ 1, 2, 3 ],"
        "\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
        " } "
    ));
    EXPECT_EQ_BASE(JSON_OBJECT, v.get_type());
    EXPECT_EQ_BASE(7, v.get_object_size());
    EXPECT_EQ_BASE(true, v.find_object_key("n"));
    EXPECT_EQ_BASE(JSON_NULL, v.get_object_value("n").get_type());
    EXPECT_EQ_BASE(true, v.find_object_key("f"));
    EXPECT_EQ_BASE(JSON_FALSE, v.get_object_value("f").get_type());
    EXPECT_EQ_BASE(true, v.find_object_key("t"));
    EXPECT_EQ_BASE(JSON_TRUE, v.get_object_value("t").get_type());
    EXPECT_EQ_BASE(true, v.find_object_key("i"));
    EXPECT_EQ_BASE(JSON_NUMBER, v.get_object_value("i").get_type());
    EXPECT_EQ_BASE(123.0, v.get_object_value("i").get_number());
    EXPECT_EQ_BASE(true, v.find_object_key("s"));
    EXPECT_EQ_BASE(JSON_STRING, v.get_object_value("s").get_type());
    EXPECT_EQ_BASE("abc", v.get_object_value("s").get_string());
    EXPECT_EQ_BASE(true, v.find_object_key("a"));
    EXPECT_EQ_BASE(JSON_ARRAY, v.get_object_value("a").get_type());
    EXPECT_EQ_BASE(3, v.get_object_value("a").get_array_size());
    for (i = 0; i < 3; ++i) {
        Json o = v.get_object_value("a").get_array_element(i);
        EXPECT_EQ_BASE(JSON_NUMBER, o.get_type());
        EXPECT_EQ_BASE((double)(i + 1.0), o.get_number());
    }
    EXPECT_EQ_BASE(true, v.find_object_key("o"));
    Json o = v.get_object_value("o");
    EXPECT_EQ_BASE(JSON_OBJECT, o.get_type());
    EXPECT_EQ_BASE(3, o.get_object_size());
    for (i = 0; i < 3; ++i) {
        string key = to_string(i + 1);
        EXPECT_EQ_BASE(true, o.find_object_key(key));
        EXPECT_EQ_BASE(JSON_NUMBER, o.get_object_value(key).get_type());
        EXPECT_EQ_BASE((double)(i + 1.0), o.get_object_value(key).get_number());
    }
}

#define TEST_PARSE_ERROR(error, json) \
    do { \
        Json v; \
        EXPECT_EQ_BASE((error), v.parse(json)); \
        EXPECT_EQ_BASE(JSON_NULL, v.get_type()); \
    } while(0)

static void test_parse_expect_value() {
    TEST_PARSE_ERROR(PARSE_EXPECT_VALUE, "");
    TEST_PARSE_ERROR(PARSE_EXPECT_VALUE, " ");
}

static void test_parse_invalid_value() {
    TEST_PARSE_ERROR(PARSE_INVALID_VALUE, "nul");
    TEST_PARSE_ERROR(PARSE_INVALID_VALUE, "?");

    /* invalid number */
    TEST_PARSE_ERROR(PARSE_INVALID_VALUE, "+0");
    TEST_PARSE_ERROR(PARSE_INVALID_VALUE, "+1");
    TEST_PARSE_ERROR(PARSE_INVALID_VALUE, ".123"); /* at least one digit before '.' */
    TEST_PARSE_ERROR(PARSE_INVALID_VALUE, "1.");   /* at least one digit after '.' */
    TEST_PARSE_ERROR(PARSE_INVALID_VALUE, "INF");
    TEST_PARSE_ERROR(PARSE_INVALID_VALUE, "inf");
    TEST_PARSE_ERROR(PARSE_INVALID_VALUE, "NAN");
    TEST_PARSE_ERROR(PARSE_INVALID_VALUE, "nan");

    /* invalid value in array */
    TEST_PARSE_ERROR(PARSE_INVALID_VALUE, "[1,]");
    TEST_PARSE_ERROR(PARSE_INVALID_VALUE, "[\"a\", nul]");
}

static void test_parse_root_not_singular() {
    TEST_PARSE_ERROR(PARSE_ROOT_NOT_SINGULAR, "null x");

    /* invalid number */
    TEST_PARSE_ERROR(PARSE_ROOT_NOT_SINGULAR, "0123"); /* after zero should be '.' or nothing */
    TEST_PARSE_ERROR(PARSE_ROOT_NOT_SINGULAR, "0x0");
    TEST_PARSE_ERROR(PARSE_ROOT_NOT_SINGULAR, "0x123");
}

static void test_parse_number_too_big() {
    TEST_PARSE_ERROR(PARSE_NUMBER_TOO_BIG, "1e309");
    TEST_PARSE_ERROR(PARSE_NUMBER_TOO_BIG, "-1e309");
}

static void test_parse_miss_quotation_mark() {
    TEST_PARSE_ERROR(PARSE_MISS_QUOTATION_MARK, "\"");
    TEST_PARSE_ERROR(PARSE_MISS_QUOTATION_MARK, "\"abc");
}

static void test_parse_invalid_string_escape() {
    TEST_PARSE_ERROR(PARSE_INVALID_STRING_ESCAPE, "\"\\v\"");
    TEST_PARSE_ERROR(PARSE_INVALID_STRING_ESCAPE, "\"\\'\"");
    TEST_PARSE_ERROR(PARSE_INVALID_STRING_ESCAPE, "\"\\0\"");
    TEST_PARSE_ERROR(PARSE_INVALID_STRING_ESCAPE, "\"\\x12\"");
}

static void test_parse_invalid_string_char() {
    TEST_PARSE_ERROR(PARSE_INVALID_STRING_CHAR, "\"\x01\"");
    TEST_PARSE_ERROR(PARSE_INVALID_STRING_CHAR, "\"\x1F\"");
}

static void test_parse_invalid_unicode_hex() {
    TEST_PARSE_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u\"");
    TEST_PARSE_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u0\"");
    TEST_PARSE_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u01\"");
    TEST_PARSE_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u012\"");
    TEST_PARSE_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u/000\"");
    TEST_PARSE_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\uG000\"");
    TEST_PARSE_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u0/00\"");
    TEST_PARSE_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u0G00\"");
    TEST_PARSE_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u0/00\"");
    TEST_PARSE_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u00G0\"");
    TEST_PARSE_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u000/\"");
    TEST_PARSE_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u000G\"");
    TEST_PARSE_ERROR(PARSE_INVALID_UNICODE_HEX, "\"\\u 123\"");
}

static void test_parse_invalid_unicode_surrogate() {
    TEST_PARSE_ERROR(PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\"");
    TEST_PARSE_ERROR(PARSE_INVALID_UNICODE_SURROGATE, "\"\\uDBFF\"");
    TEST_PARSE_ERROR(PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\\\\\"");
    TEST_PARSE_ERROR(PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\\uDBFF\"");
    TEST_PARSE_ERROR(PARSE_INVALID_UNICODE_SURROGATE, "\"\\uD800\\uE000\"");
}

static void test_parse_miss_comma_or_square_bracket() {
    TEST_PARSE_ERROR(PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[1");
    TEST_PARSE_ERROR(PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[1}");
    TEST_PARSE_ERROR(PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[1 2");
    TEST_PARSE_ERROR(PARSE_MISS_COMMA_OR_SQUARE_BRACKET, "[[]");
}

static void test_parse_miss_key() {
    TEST_PARSE_ERROR(PARSE_MISS_KEY, "{:1,");
    TEST_PARSE_ERROR(PARSE_MISS_KEY, "{1:1,");
    TEST_PARSE_ERROR(PARSE_MISS_KEY, "{true:1,");
    TEST_PARSE_ERROR(PARSE_MISS_KEY, "{false:1,");
    TEST_PARSE_ERROR(PARSE_MISS_KEY, "{null:1,");
    TEST_PARSE_ERROR(PARSE_MISS_KEY, "{[]:1,");
    TEST_PARSE_ERROR(PARSE_MISS_KEY, "{{}:1,");
    TEST_PARSE_ERROR(PARSE_MISS_KEY, "{\"a\":1,");
}

static void test_parse_miss_colon() {
    TEST_PARSE_ERROR(PARSE_MISS_COLON, "{\"a\"}");
    TEST_PARSE_ERROR(PARSE_MISS_COLON, "{\"a\",\"b\"}");
}

static void test_parse_miss_comma_or_curly_bracket() {
    TEST_PARSE_ERROR(PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":1");
    TEST_PARSE_ERROR(PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":1]");
    TEST_PARSE_ERROR(PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":1 \"b\"");
    TEST_PARSE_ERROR(PARSE_MISS_COMMA_OR_CURLY_BRACKET, "{\"a\":{}");
}

static void test_parse() {
    test_parse_literal();
    test_parse_number();
    test_parse_string();
    test_parse_array();
    test_parse_object();

    test_parse_expect_value();
    test_parse_invalid_value();
    test_parse_root_not_singular();
    test_parse_number_too_big();
    test_parse_miss_quotation_mark();
    test_parse_invalid_string_escape();
    test_parse_invalid_string_char();
    test_parse_invalid_unicode_hex();
    test_parse_invalid_unicode_surrogate();
    test_parse_miss_comma_or_square_bracket();
    test_parse_miss_key();
    test_parse_miss_colon();
    test_parse_miss_comma_or_curly_bracket();
}

// use roundtrip to test stringify function
#define TEST_ROUNDTRIP(json) \
    do { \
        Json v; \
        EXPECT_EQ_BASE(PARSE_OK, v.parse(json)); \
        string json2; \
        v.stringify(json2); \
        EXPECT_EQ_BASE((json), (json2)); \
    } while(0)

static void test_stringify_number() {
    TEST_ROUNDTRIP("0");
    TEST_ROUNDTRIP("-0");
    TEST_ROUNDTRIP("1");
    TEST_ROUNDTRIP("-1");
    TEST_ROUNDTRIP("1.5");
    TEST_ROUNDTRIP("-1.5");
    TEST_ROUNDTRIP("3.25");
    TEST_ROUNDTRIP("1e+20");
    TEST_ROUNDTRIP("1.234e+20");
    TEST_ROUNDTRIP("1.234e-20");

    TEST_ROUNDTRIP("1.0000000000000002"); /* the smallest number > 1 */
    TEST_ROUNDTRIP("4.9406564584124654e-324"); /* minimum denormal */
    TEST_ROUNDTRIP("-4.9406564584124654e-324");
    TEST_ROUNDTRIP("2.2250738585072009e-308");  /* Max subnormal double */
    TEST_ROUNDTRIP("-2.2250738585072009e-308");
    TEST_ROUNDTRIP("2.2250738585072014e-308");  /* Min normal positive double */
    TEST_ROUNDTRIP("-2.2250738585072014e-308");
    TEST_ROUNDTRIP("1.7976931348623157e+308");  /* Max double */
    TEST_ROUNDTRIP("-1.7976931348623157e+308");
}

static void test_stringify_string() {
    TEST_ROUNDTRIP("\"\"");
    TEST_ROUNDTRIP("\"Hello\"");
    TEST_ROUNDTRIP("\"Hello\\nWorld\"");
    TEST_ROUNDTRIP("\"\\\" \\\\ / \\b \\f \\n \\r \\t\"");
    TEST_ROUNDTRIP("\"Hello\\u0000World\"");
}

static void test_stringify_array() {
    TEST_ROUNDTRIP("[]");
    TEST_ROUNDTRIP("[null,false,true,123,\"abc\",[1,2,3]]");
}

static void test_stringify_object() {
    TEST_ROUNDTRIP("{}");

    // since map data structure doesn't remain original sequence as input, TEST_ROUNDTRIP doesn't work here
    // instead, we use simple cmp method here, after TEST_ROUNDTRIP, sort two jsons then cmp them
    string json1 = "{\"n\":null,\"f\":false,\"t\":true,\"i\":123,\"s\":\"abc\",\"a\":[1,2,3],\"o\":{\"1\":1,\"2\":2,\"3\":3}}";
    Json v;
    EXPECT_EQ_BASE(PARSE_OK, v.parse(json1));
    string json2;
    v.stringify(json2);
    sort(json1.begin(), json1.end());
    sort(json2.begin(), json2.end());
    EXPECT_EQ_BASE(json1, json2);
}

static void test_stringify() {
    TEST_ROUNDTRIP("null");
    TEST_ROUNDTRIP("false");
    TEST_ROUNDTRIP("true");

    test_stringify_number();
    test_stringify_string();
    test_stringify_array();
    test_stringify_object();
}

// v1 & v2 parse their json respectively, then compare their equality by == operator
#define TEST_EQUAL(json1, json2, equality) \
    do { \
        Json v1, v2; \
        EXPECT_EQ_BASE(PARSE_OK, v1.parse(json1)); \
        EXPECT_EQ_BASE(PARSE_OK, v2.parse(json2)); \
        EXPECT_EQ_BASE((equality), (v1 == v2)); \
    } while(0)

static void test_equal() {
    TEST_EQUAL("true", "true", true);
    TEST_EQUAL("true", "false", false);
    TEST_EQUAL("false", "false", true);
    TEST_EQUAL("null", "null", true);
    TEST_EQUAL("null", "0", false);
    TEST_EQUAL("123", "123", true);
    TEST_EQUAL("123", "456", false);
    TEST_EQUAL("\"abc\"", "\"abc\"", true);
    TEST_EQUAL("\"abc\"", "\"abcd\"", false);
    TEST_EQUAL("[]", "[]", true);
    TEST_EQUAL("[]", "null", false);
    TEST_EQUAL("[1,2,3]", "[1,2,3]", true);
    TEST_EQUAL("[1,2,3]", "[1,2,3,4]", false);
    TEST_EQUAL("[[]]", "[[]]", true);
    TEST_EQUAL("{}", "{}", true);
    TEST_EQUAL("{}", "null", false);
    TEST_EQUAL("{}", "[]", false);
    TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":2}", true);
    TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"b\":2,\"a\":1}", true);
    TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":3}", false);
    TEST_EQUAL("{\"a\":1,\"b\":2}", "{\"a\":1,\"b\":2,\"c\":3}", false);
    TEST_EQUAL("{\"a\":{\"b\":{\"c\":{}}}}", "{\"a\":{\"b\":{\"c\":{}}}}", true);
    TEST_EQUAL("{\"a\":{\"b\":{\"c\":{}}}}", "{\"a\":{\"b\":{\"c\":[]}}}", false);
}

static void test_copy() {
    Json v1, v2;
    v1.parse("{\"t\":true,\"f\":false,\"n\":null,\"d\":1.5,\"a\":[1,2,3]}");
    v2.copy(v1);
    EXPECT_EQ_BASE(true, (v1 == v2));
}

static void test_move() {
    Json v1, v2, v3;
    v1.parse("{\"t\":true,\"f\":false,\"n\":null,\"d\":1.5,\"a\":[1,2,3]}");
    v2.copy(v1);
    v3.move(v2);
    EXPECT_EQ_BASE(JSON_NULL, v2.get_type());
    EXPECT_EQ_BASE(true, (v3 == v1));
}

static void test_swap() {
    Json v1, v2;
    v1.set_string("Hello");
    v2.set_string("World!");
    v1.swap(v2);
    EXPECT_EQ_BASE("World!", v1.get_string());
    EXPECT_EQ_BASE("Hello", v2.get_string());
}

static void test_access_null() {
    Json v;
    v.set_string("a");
    v.set_type(JSON_NULL);
    EXPECT_EQ_BASE(JSON_NULL, v.get_type());
}

static void test_access_boolean() {
    Json v;
    v.set_string("a");
    v.set_type(JSON_TRUE);
    EXPECT_EQ_BASE(JSON_TRUE, v.get_type());
    v.set_type(JSON_FALSE);
    EXPECT_EQ_BASE(JSON_FALSE, v.get_type());
}

static void test_access_number() {
    Json v;
    v.set_string("a");
    v.set_number(1234.5);
    EXPECT_EQ_BASE(JSON_NUMBER, v.get_type());
    EXPECT_EQ_BASE(1234.5, v.get_number());
}   

static void test_access_string() {
    Json v;
    v.set_string("");
    EXPECT_EQ_BASE(JSON_STRING, v.get_type());
    v.set_string("Hello");
    EXPECT_EQ_BASE("Hello", v.get_string());
}

static void test_access_array() {
    Json a, e;
    size_t i, j;

    for (j = 0; j <= 5; j += 5) {
        // set array first, or error will occur when reserve array
        a.set_array();
        a.reserve_array(j);
        EXPECT_EQ_BASE(0, a.get_array_size());
        EXPECT_EQ_BASE(true, (a.get_array_capacity() >= a.get_array_size()));
        for (i = 0; i < 10; ++i) {
            e.set_number(i);
            a.pushback_array_element(e);
        }

        EXPECT_EQ_BASE(10, a.get_array_size());
        for (i = 0; i < 10; ++i) {
            EXPECT_EQ_BASE((double)i, a.get_array_element(i).get_number());
        }
    }

    a.popback_array_element();
    EXPECT_EQ_BASE(9, a.get_array_size());
    for (i = 0; i < 9; ++i) {
        EXPECT_EQ_BASE((double)i, a.get_array_element(i).get_number());
    }

    a.erase_array_element(4, 0);
    EXPECT_EQ_BASE(9, a.get_array_size());
    for (i = 0; i < 9; ++i) {
        EXPECT_EQ_BASE((double)i, a.get_array_element(i).get_number());
    }

    a.erase_array_element(8, 1);
    EXPECT_EQ_BASE(8, a.get_array_size());
    for (i = 0; i < 8; ++i) {
        EXPECT_EQ_BASE((double)i, a.get_array_element(i).get_number());
    }

    a.erase_array_element(0, 2);
    EXPECT_EQ_BASE(6, a.get_array_size());
    for (i = 0; i < 6; ++i) {
        EXPECT_EQ_BASE((double)i + 2, a.get_array_element(i).get_number());
    }

    for (i = 0; i < 2; ++i) {
        e.set_number(i);
        a.insert_array_element(i, e);
    }

    EXPECT_EQ_BASE(8, a.get_array_size());
    for (i = 0; i < 8; ++i) {
        EXPECT_EQ_BASE((double)i, a.get_array_element(i).get_number());
    }

    EXPECT_EQ_BASE(true, (a.get_array_capacity() > 8));
    a.shrink_array();
    EXPECT_EQ_BASE(8, a.get_array_size());
    EXPECT_EQ_BASE(8, a.get_array_capacity());
    for (i = 0; i < 8; ++i) {
        EXPECT_EQ_BASE((double)i, a.get_array_element(i).get_number());
    }

    // test if element is freed?
    e.set_string("Hello");
    a.pushback_array_element(e);   

    i = a.get_array_capacity();
    a.clear_array();
    EXPECT_EQ_BASE(0, a.get_array_size());
    EXPECT_EQ_BASE(i, a.get_array_capacity());
    a.shrink_array();
    EXPECT_EQ_BASE(0, a.get_array_capacity());
}

static void test_access_object() {
    Json o, v;
    size_t i, j;

    // set object first, or error will occur when get object size
    o.set_object();
    EXPECT_EQ_BASE(0, o.get_object_size());
    
    for (i = 0; i < 10; ++i) {
        string key = "a";
        key[0] += i;
        v.set_number(i);
        o.set_object_value(key, v);
    }
    EXPECT_EQ_BASE(10, o.get_object_size());
    for (i = 0; i < 10; ++i) {
        string key = "a";
        key[0] += i;
        EXPECT_EQ_BASE(true, o.find_object_key(key));
        v = o.get_object_value(key);
        EXPECT_EQ_BASE((double)i, v.get_number());
    }

    EXPECT_EQ_BASE(true, o.find_object_key("j"));
    o.remove_object_value("j");
    EXPECT_EQ_BASE(false, o.find_object_key("j"));
    EXPECT_EQ_BASE(9, o.get_object_size());

    EXPECT_EQ_BASE(true, o.find_object_key("a"));
    o.remove_object_value("a");
    EXPECT_EQ_BASE(false, o.find_object_key("a"));
    EXPECT_EQ_BASE(8, o.get_object_size());

    for (i = 0; i < 8; ++i) {
        string key = "b";
        key[0] += i;
        v = o.get_object_value(key);
        EXPECT_EQ_BASE((double)(i + 1), v.get_number());
    }

    // test if element is freed?
    v.set_string("World!");
    o.set_object_value("Hello", v);

    v = o.get_object_value("Hello");
    EXPECT_EQ_BASE("World!", v.get_string());

    o.clear_object();
    EXPECT_EQ_BASE(0, o.get_object_size());
}

static void test_access() {
    test_access_null();
    test_access_boolean();
    test_access_number();
    test_access_string();
    test_access_array();
    test_access_object();
}

int main(int argc, char* argv[]) {

    test_parse();
    test_stringify();
    test_equal();
    test_copy();
    test_move();
    test_swap();
    test_access();

    cout << test_pass << "/" << test_count << " passed, i.e. success rate is " << test_pass * 100.0 / test_count << " %." << endl;
    return main_ret;
}