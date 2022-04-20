#include "JsonObjectConverter.hpp"

#include <list>

enum ContactType {
  Family,
  Friend,
  Work
};

void to_json(nlohmann::json& j, const ContactType& contactType) {
  switch (contactType)
  {
  case ContactType::Family:
      j = "Family";
      return;
  case ContactType::Friend:
      j = "Friend";
      return;
  default:
      j = "Work";
  }
}

void from_json(const nlohmann::json& j, ContactType& contactType) {
  if (j == "Family") {
      contactType = ContactType::Family;
  } else if (j == "Friend") {
      contactType = ContactType::Friend;
  } else {
      contactType = ContactType::Work;
  }
}

struct Contact : public joc::JsonObject
{
  Contact() : joc::JsonObject({{"type", type},
                               {"name", name},
                               {"address", address},
                               {"age", age},
                               {"e-mail", email}
                              }) {}
  ContactType type;
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

int main(int argc, char **argv)
{
  // Build C++ object and output JSON to console
  Contact mary;
  mary.name    = "Mary";
  mary.age     = 52;
  mary.address = "Olof Str. 4";
  mary.type    = ContactType::Friend;
  std::cout << mary.toJson().dump(2) << std::endl;

  // Build nlohmann::json object and convert to our C++ object
  nlohmann::json peterJson = {{"name", "Peter"},{"address", "Rua das Amendoas 4"}, {"age", 34}, {"type", "Work"}};
  Contact peter;
  peter.refreshFromJson(peterJson);
  std::cout << peter.toJson().dump(2) << std::endl;

  // Change Peter age and add an e-mail
  peter.age = 30;
  peter.email = "peter@hisspace.com";
  std::cout << peter.toJson().dump(2) << std::endl;

  // Build a ContactBook and output JSON to console
  ContactBook myContactBook;
  myContactBook.owner = "Ricardo";
  myContactBook.contactList = {mary, peter};
  std::cout << myContactBook.toJson().dump(2) << std::endl;

  /* Output:
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
  "address": "Rua das Amendoas 4",
  "age": 30,
  "e-mail": "peter@hisspace.com",
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
      "age": 30,
      "name": "Peter",
      "type": "Work"
    }
  ],
  "owner": "Ricardo"
}
   */

  return 0;
}
