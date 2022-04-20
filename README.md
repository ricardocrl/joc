# joc

## Overview

The objective of joc project is to facilitate schema association and JSON conversion,
of any(*) user defined C++ data type.

(*)- It is work in progress, and some things are not possible such as:
- Using a std::vector<T> member (where T can inherit from JsonObject) is problematic.
  Elements are not guaranteed to live in the same memory address all the time. This is
  currently a pre-requisite.
- Representing a JSON other than a JSON object. Any C++ class will be represented as
  a JSON object.
- ...

Example:

```
struct Contact : public joc::JsonObject
{
  Contact() : joc::JsonObject({{"name", name},
                               {"address", address},
                               {"age", age},
                               {"e-mail", email}
                              }) {}
  std::string name;
  std::string address;
  int age;
  std::optional<std::string> email;
};

struct ContactBook : public joc::JsonObject
{
  ContactBook() : joc::JsonObject({{"owner", owner},
                                   {"contact_list", contactList}
                                  }) {}
  std::string owner;
  std::list<Contact> contactList;
};
```

...and can be used like so:

```
Contact mary;
mary.name    = "Mary";
mary.age     = 52;
mary.address = "Olof Str. 4";
std::cout << mary.toJson().dump(2) << std::endl;

Contact peter;
peter.refreshFromJson(nlohmann::json({{"name", "Peter"},{"address", "Rua das Amendoas 4"}, {"age", 34}}));
```

## Install

Make sure git submodules are initialized and up to date. joc lib is built on top of nlohmann::json library.

> git submodule update --init --recursive

## Compile sample

The main CMakeLists.txt includes building the `joc` library, plus a sample executable.

> mkdir -p build && cd build
> cmake ..
> make
> ./joc_sample

## Compile unit-tests

Jump to tests/ directory and run the same commands as shown above:

> cd tests/
> mkdir -p build && cd build
> cmake ..
> make
> ./jsonobjectconverter_test
