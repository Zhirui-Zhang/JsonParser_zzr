# TinyJsonParser

> Thanks for [json-tutorial](https://github.com/miloyip/json-tutorial.git) provided by miloyip.

This is a small C++ project using for parsing/generating **json/string** format file.

## Characteristic

* Use standard **C++14** grammer and **STL** without any other libraries.

* Use **CMake** to complie codes and generate executable file automatically.

* Distinct functions are encapsulated in different classes respectively, which provides the only interface (i.e. **Json class**) for user.

## Usage

1. Download/Gitclone this source project
   
   ```git
   git clone git@github.com:Syopain/Json.git 
   ```

2. Create build directory and enter
   
   ```bash
   mkdir build && cd build
   ```

3. Set CMake compilation mode (Debug/Release)

> before this step, maybe you should config CMake enviroment at first
> 
> ```bash
> sudo apt-get install cmake
> ```

    ```bash
    cmake -dcmake_build_type=debug ..
    ```

4. Makefile & Run myJson project
   
   ```makefile
   make
   ./myJson
   ```

5. Final test result is showed as :
   
   ![JsonParser_zzr/result.png at 6da6ad99ffec113197a4e18029d538c4eb575588 · Zhirui-Zhang/JsonParser_zzr · GitHub](https://github.com/Zhirui-Zhang/JsonParser_zzr/blob/6da6ad99ffec113197a4e18029d538c4eb575588/root/result.png "test")

6. We can also use valgrind tool to check if memory leaks

> likewise, config valgrind environment before
> 
> ```bash
> sudo apt-get install valgrind
> ```

    ```bash
    valgrind --leak-check=full  ./myJson
    ```
   
   and the result will be showed as :
   
   ![JsonParser_zzr/memory check.png at 76d47a62c5a23ae94bf2863da848c49583bf6691 · Zhirui-Zhang/JsonParser_zzr · GitHub](https://github.com/Zhirui-Zhang/JsonParser_zzr/blob/76d47a62c5a23ae94bf2863da848c49583bf6691/root/memory%20check.png)

## Description

### Files

* root directory : store all attachments

* src directory : store declaration and definition for different classes, includJing :
  
  * JsonEnum.h : define `JSON_TYPE` and `PARSE_TYPE` enum structure
  
  * Json.h / Json.cpp : define smart pointer member `m_jv` to JsonValue and all member functions 
  
  * JsonValue.h / JsonValue.cpp : define `JSON_TYPE` as `m_type` member and `union` struct for Json info, etc
  
  * JsonParser.h / JsonParser.cpp : define all member functions using for parsing input string to json
  
  * JsonStringify.h / JsonStringify.cpp : define all member functions using for generating string from existed json

* JsonTest.cpp : test the whole project and verify parsing/generating functions

* CMakeLists.txt : create auto compilation, maybe you should config cmake enviroment first by 
  
  ```bash
  sudo apt-get install cmake
  ```

* README.md : introduction to this project

### Realization

* Json & JsonValue classes :
  
  * To reduce compilation dependency between files and avoid memory leak problem, the only member in Json is a smart pointer `m_jv` to JsonValue class, which will destroy and free memory automatically. 
  
  * In JsonValue class, we use `m_type` member to indicate **type** for current json. Besides, we use `union`struct to store json info because a json has only **one type** among **7 types**, which are :
    
    `null`,` true`,` false`,` number`,` string`,` array`and`object`
  
  * As for member functions, we use **parse/stringify** function to connect Parser/Generator class, **vector** container for array type and **map** container for object type, also define some common APIs such as `size()`, `clear()`, `insert()`, `erase()` etc.

* Parser class :
  
  * Provides various member functions to handle different situations, the overall `parse()` function calls `parse_value()` to parse specific `JSON_TYPE`, return `PARST_TYPE` to indicate parsing result. 
  
  * `parse_literal()` deals with `null`, `true` and `false` type, `parse_number()` deals with `number` type, follows the rule as
  
  ![JsonParser_zzr/number.png at 76d47a62c5a23ae94bf2863da848c49583bf6691 · Zhirui-Zhang/JsonParser_zzr · GitHub](https://github.com/Zhirui-Zhang/JsonParser_zzr/blob/76d47a62c5a23ae94bf2863da848c49583bf6691/root/number.png)
  
  * `parse_string()` deals with `string` type by calling `parse_string_raw()`, which only supports UTF-8 characters. Be careful for `\uXXXX` hexadecimal format, we use `parse_hex4()` to parse it and `parse_encode_utf8()` to decode this string. When `\uXXXX\uYYYY` surrogate pair occurs, following function
    
    ```matlab
    codepoint = 0x10000 + (H − 0xD800) × 0x400 + (L − 0xDC00)
    ```
    
    to transfer it, if the input string is invalid, i.e. `(unsigned char)ch < 0x20`, return PARSE_INVALID_STRING_CHAR.
  
  * `parse_array()` deals with `array` type, construct `vector<JsonValue>` tmp to store array info, set `m_jv` as tmp when meets `]` finally.
  
  * `parse_object()` deals with `obejct` type, construct `string` tmpKey and `map<string, JsonValue>` tmpMap to store [key, val] pair, set `m_jv` as tmpMap when meets `}` finally.

* Generator class :
  
  * Provides `stringify_value()` to stringify an existed json to string, pass the whole value in `m_res` string member. According to input parameter `jv.type()` to stringify different `JSON_TYPE`.

## Improvement

* Use C++17 new characteristic such as `std::variant` struct would be better than `union` struct, since the `ctor` and `dtor` in `union` will be complicated and make mistakes easily.

* Use `map<string, JsonValue>` as `JSON_OBJECT` container cannot keep the original sequence same as input string. Things will be better when using `vector<pair<string, JsonValue>>` struct instead, but **time complexity** will increase from `O(logn)` to `O(n)` in `find` and `remove` operation. Therefore, we should consider different containers between `Query-Oriented` task and `Storage-Oriented` task.


