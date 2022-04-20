# joc

## Overview

The objective of joc project is to facilitate schema association and JSON conversion,
of any(*) user defined C++ data type.

Example:

```cpp
struct Contact : public JsonObject
{
  Contact() : JsonObject({{"name", name},
                          {"address", address},
                          {"age", age},
                          {"e-mail", email}
                         }) {}
  std::string name;
  std::string address;
  int age;
  std::optional<std::string> email;
};

struct ContactBook : public JsonObject
{
  ContactBook() : JsonObject({{"owner", owner},
                              {"contact_list", contactList}
                             }) {}
  std::string owner;
  std::list<Contact> contactList;
};
```

...and can be used like so:

```cpp
Contact mary;
mary.name    = "Mary";
mary.age     = 52;
mary.address = "Olof Str. 4";

Contact peter;
peter.refreshFromJson(nlohmann::json({{"name", "Peter"},{"address", "Rua das Amendoas 4"}, {"age", 34}}));

ContactBook myBook;
myBook.owner = "John";
myBook.contactList = {mary, peter};

std::cout << mary.toJson().dump(2) << std::endl;
std::cout << peter.toJson().dump(2) << std::endl;
std::cout << myBook.toJson().dump(2) << std::endl;
```

...with output:

```
Optional field: 'e-mail': is not provided
{
  "address": "Olof Str. 4",
  "age": 52,
  "name": "Mary",
  "type": "Friend"
}
{
  "address": "Rua das Amendoas 4",
  "age": 34,
  "name": "Peter",
  "type": "Work"
}
{
  "contact_list": [
    {
      "address": "Olof Str. 4",
      "age": 52,
      "name": "Mary",
      "type": "Friend"
    },
    {
      "address": "Rua das Amendoas 4",
      "age": 34,
      "name": "Peter",
      "type": "Work"
    }
  ],
  "owner": "John"
}
```

(*)- This is work in progress, some things are not possible such as:
- Representing a JSON other than a JSON object. Any C++ class will be represented as
  a JSON object. Each member will be present with a key-value pair in the JSON object.
  However, the *value* on a kay-value JSON pair can be a JSON array, by means of
  a std::list or std::vector, or another JSON object, via std::map or a joc JsonObject.
- Move semantics should not be hard to implement but it's not included.

## Install

Make sure git submodules are initialized and up to date. joc lib is built on top of nlohmann::json library.

```
git submodule update --init --recursive
```

## Compile sample

The main CMakeLists.txt includes building the `joc` library, plus a sample executable.

```
$ mkdir -p build && cd build
$ cmake ..
$ make
$ ./joc_sample
```

## Compile unit-tests

Jump to tests/ directory and run the same commands as shown above:

```
$ cd tests/
$ mkdir -p build && cd build
$ cmake ..
$ make
$ ./jsonobjectconverter_test
```
