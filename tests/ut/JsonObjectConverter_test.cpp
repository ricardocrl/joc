#include "JsonObjectConverter.hpp"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <map>
#include <list>

using namespace ::testing;
using namespace joc;
using namespace std::chrono;

struct TestStruct : public JsonObject
{
    TestStruct()
        : JsonObject({
            {"a", a},
            {"b", b},
            {"c", c},
        }){};
    int a{0};
    std::string b{"default"};
    std::optional<double> c;
};

struct TestStructWithList : public JsonObject
{
    TestStructWithList()
        : JsonObject({{"my_list", list}}){};

    std::list<TestStruct> list;
};

struct JsonObjectTest : public Test
{
};

TEST_F(JsonObjectTest, FromJson_Works)
{
    TestStruct t;
    t.refreshFromJson(R"({"a": 50, "b": "T1"})"_json);

    EXPECT_EQ(t.a, 50);
    EXPECT_EQ(t.b, "T1");
    EXPECT_EQ(t.c, std::nullopt);

    t.refreshFromJson(R"({"b": "T3"})"_json);

    EXPECT_EQ(t.b, "T3");
    EXPECT_EQ(t.c, std::nullopt);

    t.refreshFromJson(R"({"a": 40, "b": "T4", "c": 4.0})"_json);

    EXPECT_EQ(t.a, 40);
    EXPECT_EQ(t.b, "T4");
    EXPECT_TRUE(t.c.has_value());
    EXPECT_EQ(t.c.value(), 4.0);

    EXPECT_EQ(t.toJson().dump(), R"({"a":40,"b":"T4","c":4.0})");

    t.c = std::nullopt;
    EXPECT_EQ(t.toJson().dump(), R"({"a":40,"b":"T4"})");

    t.b = "trapatoni";
    t.a = 60;
    EXPECT_EQ(t.toJson().dump(), R"({"a":60,"b":"trapatoni"})");
}

TEST_F(JsonObjectTest, ToJson_Works)
{
    TestStruct t;
    t.a = 40;
    t.b = "T4";
    t.c = 4.0;

    EXPECT_EQ(t.toJson().dump(), R"({"a":40,"b":"T4","c":4.0})");
    t.c = std::nullopt;
    EXPECT_EQ(t.toJson().dump(), R"({"a":40,"b":"T4"})");
    t.b = "trapatoni";
    t.a = 60;
    EXPECT_EQ(t.toJson().dump(), R"({"a":60,"b":"trapatoni"})");
}

TEST_F(JsonObjectTest, Copy_Works)
{
    TestStruct t1, t2;
    t1.a = 100;
    t1.b = "100";
    t1.c = 100.0;

    t2 = t1;
    EXPECT_EQ(t1.a, t2.a);
    EXPECT_EQ(t1.b, t2.b);
    EXPECT_EQ(t1.c, t2.c);
    EXPECT_EQ(t1.toJson().dump(), R"({"a":100,"b":"100","c":100.0})");
    EXPECT_EQ(t2.toJson().dump(), R"({"a":100,"b":"100","c":100.0})");

    t1.a = 1000;
    t1.b = "1000";
    t1.c = 1000.0;
    EXPECT_EQ(t2.a, 100);
    EXPECT_EQ(t2.b, "100");
    EXPECT_EQ(t2.c, 100.0);
    EXPECT_EQ(t1.toJson().dump(), R"({"a":1000,"b":"1000","c":1000.0})");
    EXPECT_EQ(t2.toJson().dump(), R"({"a":100,"b":"100","c":100.0})");

    t2.a = 2000;
    t2.b = "2000";
    t2.c = 2000.0;
    EXPECT_EQ(t1.a, 1000);
    EXPECT_EQ(t1.b, "1000");
    EXPECT_EQ(t1.c, 1000.0);
    EXPECT_EQ(t1.toJson().dump(), R"({"a":1000,"b":"1000","c":1000.0})");
    EXPECT_EQ(t2.toJson().dump(), R"({"a":2000,"b":"2000","c":2000.0})");
}

TEST_F(JsonObjectTest, CopyList_Works)
{
    TestStructWithList myStruct;
    myStruct.refreshFromJson(R"(
{
  "my_list": [
   {"a": 40, "b": "T4", "c": 4.0},
   {"a": 50, "b": "T5", "c": 5.0},  
   {"a": 60, "b": "T6", "c": 6.0}
  ]
})"_json);

    EXPECT_EQ(myStruct.list.size(), 3);

    auto s0 = *myStruct.list.begin();
    auto s1 = *std::next(myStruct.list.begin());
    auto s2 = *std::next(myStruct.list.begin(), 2);

    myStruct.list.clear();

    EXPECT_EQ(s0.a, 40);
    EXPECT_EQ(s0.b, "T4");
    EXPECT_EQ(s0.c, 4.0);
    EXPECT_EQ(s1.a, 50);
    EXPECT_EQ(s1.b, "T5");
    EXPECT_EQ(s1.c, 5.0);
    EXPECT_EQ(s2.a, 60);
    EXPECT_EQ(s2.b, "T6");
    EXPECT_EQ(s2.c, 6.0);

    EXPECT_EQ(myStruct.toJson().dump(), R"({"my_list":[]})");

    myStruct.list.emplace_back(s0);
    myStruct.list.emplace_back(s1);
    myStruct.list.emplace_back(s2);
    EXPECT_EQ(myStruct.toJson().dump(),
              R"({"my_list":[{"a":40,"b":"T4","c":4.0},{"a":50,"b":"T5","c":5.0},{"a":60,"b":"T6","c":6.0}]})");

    auto structCopy = myStruct;

    EXPECT_EQ(structCopy.toJson().dump(),
              R"({"my_list":[{"a":40,"b":"T4","c":4.0},{"a":50,"b":"T5","c":5.0},{"a":60,"b":"T6","c":6.0}]})");

    for (auto& elem : myStruct.list)
    {
        elem.a = 888;
    }
    for (auto& elem : structCopy.list)
    {
        elem.c.reset();
    }

    EXPECT_EQ(myStruct.toJson().dump(),
              R"({"my_list":[{"a":888,"b":"T4","c":4.0},{"a":888,"b":"T5","c":5.0},{"a":888,"b":"T6","c":6.0}]})");
    EXPECT_EQ(structCopy.toJson().dump(), R"({"my_list":[{"a":40,"b":"T4"},{"a":50,"b":"T5"},{"a":60,"b":"T6"}]})");
}

TEST_F(JsonObjectTest, OptionalStringWithConverter_Works)
{
    struct Button : public JsonObject
    {
        Button() : JsonObject({{"button_state", isOn}}){};

        std::optional<bool> isOn;
    };

    Button button;
    EXPECT_EQ(button.toJson().dump(), R"({})");
    button.isOn = true;
    EXPECT_EQ(button.toJson().dump(), R"({"button_state":true})");
    button.isOn = false;
    EXPECT_EQ(button.toJson().dump(), R"({"button_state":false})");
    button.isOn = std::nullopt;
    EXPECT_EQ(button.toJson().dump(), R"({})");

    struct ButtonBox : public JsonObject
    {
        ButtonBox()
            : JsonObject({{"buttons", buttonList}}){};
        std::list<Button> buttonList;
    };

    ButtonBox buttonBox;
    EXPECT_EQ(buttonBox.toJson().dump(), R"({"buttons":[]})");

    for (auto i = 0; i < 10; ++i)
    {
        buttonBox.buttonList.emplace_back(Button());
    }
    EXPECT_EQ(buttonBox.toJson().dump(), R"({"buttons":[{},{},{},{},{},{},{},{},{},{}]})");

    for (auto i = 0; i < 10; ++i)
    {
        auto it = buttonBox.buttonList.begin();
        std::advance(it, i);
        it->isOn = (i < 5 ? std::optional<bool>(false) : ((i < 8) ? std::optional<bool>(true) : std::nullopt));
    }
    EXPECT_EQ(
        buttonBox.toJson().dump(),
        R"({"buttons":[{"button_state":false},{"button_state":false},{"button_state":false},{"button_state":false},{"button_state":false},{"button_state":true},{"button_state":true},{"button_state":true},{},{}]})");
}

TEST_F(JsonObjectTest, NestedJsonConverters_Works)
{
    struct Tree : public JsonObject
    {
        Tree()
            : JsonObject({{"fruits", fruits}}){};
        int fruits{0};
    };

    struct House : public JsonObject
    {
        House()
            : JsonObject({{"tree", tree}}){};
        Tree tree;
    };

    House h;
    EXPECT_EQ(h.toJson().dump(), R"({"tree":{"fruits":0}})");

    h.tree.fruits = 15;
    EXPECT_EQ(h.toJson().dump(), R"({"tree":{"fruits":15}})");

    h.refreshFromJson(R"({"tree":{"fruits":700}})"_json);
    EXPECT_EQ(h.toJson().dump(), R"({"tree":{"fruits":700}})");
}

TEST_F(JsonObjectTest, NestedJsonConvertersAsOptional_Works)
{
    struct Tree : public JsonObject
    {
        Tree()
            : JsonObject({{"fruits", fruits}}){};
        std::optional<int> fruits;
    };

    struct House : public JsonObject
    {
        House()
            : JsonObject({{"tree", tree}}){};
        std::optional<Tree> tree;
    };

    House h;
    EXPECT_EQ(h.toJson().dump(), R"({})");
    h.tree = Tree();
    EXPECT_EQ(h.toJson().dump(), R"({"tree":{}})");

    h.tree->fruits = 15;
    EXPECT_EQ(h.toJson().dump(), R"({"tree":{"fruits":15}})");

    h.refreshFromJson(R"({"tree":{"fruits":700}})"_json);
    EXPECT_EQ(h.toJson().dump(), R"({"tree":{"fruits":700}})");
}

TEST_F(JsonObjectTest, MapsConvertedToJsonObjects_Works)
{
    struct TestStructWithMap : public JsonObject
    {
        TestStructWithMap()
            : JsonObject({
                {"my_map", map},
            }){};
        std::map<std::string, TestStruct> map;
    };

    TestStructWithMap mapStruct;
    mapStruct.refreshFromJson(R"({"my_map":{"test":{"a": 40, "b": "T4", "c": 4.0}}})"_json);

    EXPECT_EQ(mapStruct.map["test"].a, 40);
    EXPECT_EQ(mapStruct.map["test"].b, "T4");
    EXPECT_EQ(mapStruct.map["test"].c, 4.0);

    EXPECT_EQ(mapStruct.toJson().dump(), R"({"my_map":{"test":{"a":40,"b":"T4","c":4.0}}})");
}

TEST_F(JsonObjectTest, Copies_Work)
{
    TestStruct t;
    t.refreshFromJson(R"({"a": 50, "b": "T1"})"_json);

    TestStruct tCopy = t;
    EXPECT_EQ(tCopy.a, 50);
    EXPECT_EQ(tCopy.b, "T1");
    EXPECT_EQ(tCopy.c, std::nullopt);

    t.a = 10;
    t.b = "F";
    t.c = 5.11;

    EXPECT_EQ(t.a, 10);
    EXPECT_EQ(t.b, "F");
    EXPECT_EQ(t.c, 5.11);
    EXPECT_EQ(tCopy.a, 50);
    EXPECT_EQ(tCopy.b, "T1");
    EXPECT_EQ(tCopy.c, std::nullopt);

    t = tCopy;
    EXPECT_EQ(t.a, 50);
    EXPECT_EQ(t.b, "T1");
    EXPECT_EQ(t.c, std::nullopt);
}
