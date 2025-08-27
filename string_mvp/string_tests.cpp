#include "string.h"
#include <gtest/gtest.h>
#include <sstream>
#include <limits>

// ---------- Конструкторы ----------
TEST(ConstructorTest, FromCString) {
    String s("hello");
    EXPECT_EQ(s.size(), 5);
    EXPECT_STREQ(s.data(), "hello");
}

TEST(ConstructorTest, EmptyCString) {
    std::string s("");
    EXPECT_EQ(s.size(), 0);
    EXPECT_STREQ(s.data(), "");
}

TEST(ConstructorTest, FillConstructorChar) {
    String s(5, 'a');
    EXPECT_EQ(s.size(), 5);
    EXPECT_STREQ(s.data(), "aaaaa");
}

TEST(ConstructorTest, FillConstructorNullChar) {
    String s(5, '\0');
    EXPECT_EQ(s.size(), 0);  // должно обнулять
    EXPECT_STREQ(s.data(), "");
}

TEST(ConstructorTest, CopyConstructor) {
    String s1("copy");
    String s2(s1);
    EXPECT_EQ(s1, s2);
    EXPECT_NE(s1.data(), s2.data()); // разные буферы
}

TEST(ConstructorTest, AssignmentOperator) {
    String a("foo");
    String b("bar");
    b = a;
    EXPECT_EQ(a, b);
}

TEST(ConstructorTest, DefaultConstructor) {
    String s;
    EXPECT_EQ(s.size(), 0);
    EXPECT_TRUE(s.empty());
}

// ---------- Операторы сравнения ----------
TEST(OperatorTest, ComparisonEqual) {
    String a("abc"), b("abc");
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(OperatorTest, ComparisonDifferent) {
    String a("abc"), b("abd");
    EXPECT_TRUE(a != b);
    EXPECT_TRUE(a < b);
    EXPECT_TRUE(b > a);
    EXPECT_TRUE(a <= b);
    EXPECT_TRUE(b >= a);
}

// ---------- Доступ к символам ----------
TEST(OperatorTest, Indexing) {
    String s("hello");
    EXPECT_EQ(s[0], 'h');
    s[0] = 'H';
    EXPECT_EQ(s[0], 'H');
}

TEST(MethodTest, FrontBack) {
    String s("world");
    EXPECT_EQ(s.front(), 'w');
    EXPECT_EQ(s.back(), 'd');
    s.back() = '!';
    EXPECT_STREQ(s.data(), "worl!");
}

// ---------- Размер и вместимость ----------
TEST(MethodTest, SizeCapacityLength) {
    String s("test");
    EXPECT_EQ(s.size(), 4);
    EXPECT_EQ(s.length(), 4);
    EXPECT_GE(s.capacity(), 5); // минимум size+1
}

// ---------- push_back / pop_back ----------
TEST(MethodTest, PushBack) {
    String s;
    for (int i = 0; i < 20; i++) s.push_back('x');
    EXPECT_EQ(s.size(), 20);
    EXPECT_TRUE(s.capacity() >= 21);
}

TEST(MethodTest, PopBack) {
    String s("abc");
    s.pop_back();
    EXPECT_EQ(s.size(), 2);
    EXPECT_STREQ(s.data(), "ab");
    s.pop_back();
    s.pop_back();
    EXPECT_EQ(s.size(), 0);
    EXPECT_STREQ(s.data(), "");
}

// ---------- Операторы += и + ----------
TEST(OperatorTest, PlusEqualChar) {
    String s("a");
    s += 'b';
    EXPECT_STREQ(s.data(), "ab");
}

TEST(OperatorTest, PlusEqualString) {
    String s1("abc"), s2("def");
    s1 += s2;
    EXPECT_STREQ(s1.data(), "abcdef");
}

TEST(OperatorTest, PlusOperator) {
    String a("foo"), b("bar");
    String c = a + b;
    EXPECT_STREQ(c.data(), "foobar");
}

// ---------- find / rfind ----------
TEST(MethodTest, FindExisting) {
    String s("abracadabra");
    EXPECT_EQ(s.find("bra"_str), 1);
    EXPECT_EQ(s.find("abra"_str), 0);
    EXPECT_EQ(s.rfind("bra"_str), 8);
}

TEST(MethodTest, FindNotFound) {
    String s("hello");
    EXPECT_EQ(s.find("xyz"_str), s.size());
    EXPECT_EQ(s.rfind("123"_str), s.size());
}

// ---------- substr ----------
TEST(MethodTest, SubstrMiddle) {
    String s("abcdef");
    String sub = s.substr(2, 3);
    EXPECT_STREQ(sub.data(), "cde");
}

TEST(MethodTest, SubstrWhole) {
    String s("abcdef");
    String sub = s.substr(0, s.size());
    EXPECT_STREQ(sub.data(), "abcdef");
}

TEST(MethodTest, SubstrZero) {
    String s("abcdef");
    String sub = s.substr(3, 0);
    EXPECT_EQ(sub.size(), 0);
}

// ---------- empty / clear ----------
TEST(MethodTest, EmptyClear) {
    String s("abc");
    EXPECT_FALSE(s.empty());
    s.clear();
    EXPECT_TRUE(s.empty());
    EXPECT_EQ(s.size(), 0);
}

// ---------- shrink_to_fit ----------
TEST(MethodTest, ShrinkToFit) {
    String s("abc");
    size_t oldCap = s.capacity();
    s.shrink_to_fit();
    EXPECT_EQ(s.capacity(), s.size() + 1);
    EXPECT_LE(s.capacity(), oldCap);
}

// ---------- data ----------
TEST(MethodTest, DataCheck) {
    String s("hello");
    EXPECT_STREQ(s.data(), "hello");
    EXPECT_EQ(strcmp(s.data(), static_cast<const char*>(s)), 0);
}

// ---------- Ввод/вывод ----------
TEST(IOTest, Output) {
    String s("world");
    std::ostringstream oss;
    oss << s;
    EXPECT_EQ(oss.str(), "world");
}

TEST(IOTest, Input) {
    std::istringstream iss("hello world");
    String s;
    iss >> s;
    EXPECT_EQ(s, "hello"_str);
}

// ---------- Литералы ----------
TEST(LiteralTest, UserDefinedLiteral) {
    String s = "test"_str;
    EXPECT_EQ(s, String("test"));
}

//---------- Граничные случаи ----------
TEST(BoundaryTest, LargeString) {
    const size_t N = 10000;
    String s;
    for (size_t i = 0; i < N; i++) s.push_back('x');
    EXPECT_EQ(s.size(), N);
    for (size_t i = 0; i < 100; i++) s.pop_back();
    EXPECT_EQ(s.size(), N - 100);
}

TEST(BoundaryTest, SingleChar) {
    String s("a");
    EXPECT_EQ(s.size(), 1);
    EXPECT_EQ(s.front(), 'a');
    EXPECT_EQ(s.back(), 'a');
}

TEST(BoundaryTest, OnlyNullChar) {
    String s("\0");
    EXPECT_EQ(s.size(), 0); // strlen("\0") == 0
    EXPECT_TRUE(s.empty());
}
