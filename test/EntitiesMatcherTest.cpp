/*
|---------------------------------------------------------|
|    ___                   ___       _             _      |
|   / _ \ _ __   ___ _ __ |_ _|_ __ | |_ ___ _ __ | |_    |
|  | | | | '_ \ / _ \ '_ \ | || '_ \| __/ _ \ '_ \| __|   |
|  | |_| | |_) |  __/ | | || || | | | ||  __/ | | | |_    |
|   \___/| .__/ \___|_| |_|___|_| |_|\__\___|_| |_|\__|   |
|        |_|                                              |
|                                                         |
|     - The users first...                                |
|                                                         |
|     Authors:                                            |
|        - Clement Michaud                                |
|        - Sergei Kireev                                  |
|                                                         |
|     Version: 1.0.0                                      |
|                                                         |
|---------------------------------------------------------|

The MIT License (MIT)
Copyright (c) 2016 - Clement Michaud, Sergei Kireev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
//
// Created by clement on 05/05/16.
//
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "intent/intent_service/DictionaryModel.hpp"
#include "intent/intent_service/EntitiesMatcher.hpp"
#include "intent/intent_service/IntentModel.hpp"
#include "intent/utils/RegexMatcher.hpp"

using namespace intent;
using namespace ::testing;


class EntitiesMatcherWishCoffeeTest : public ::testing::Test
{
public:
    void SetUp()
    {
        Term t1, t2;
        t1.term = "coffee";
        t1.alias.push_back("Starbucks");
        t1.entityId = 0;
        t1.termId = 0;

        t2.term = "would";
        t2.alias.push_back("would");
        t2.alias.push_back("want");
        t2.alias.push_back("wish");
        t2.entityId = 1;
        t2.termId = 1;

        m_dictionaryModel.dictionary.pushTerm(t1);
        m_dictionaryModel.dictionary.pushTerm(t2);

        m_dictionaryModel.entitiesByEntityId[0] = "@coffee";
        m_dictionaryModel.entitiesByEntityId[1] = "@wish";
    }

    DictionaryModel m_dictionaryModel;
};

class RegexMatcherTest : public ::testing::Test
{};

class EntitiesMatcherRegexTest : public ::testing::Test
{
public:
    static const int PHONE_NUMBER_ENTITY_ID = 0;
    void SetUp()
    {
        std::string phone_expression = "^[0-9]{10,}$";
        m_dictionaryModel.regexesByEntityId[phone_expression] = 0;
        m_dictionaryModel.entitiesByEntityId[0] = "@phonenumber";
    }

    DictionaryModel m_dictionaryModel;
};


TEST_F(EntitiesMatcherWishCoffeeTest, find_simple_intent_with_one_entity)
{
    EntitiesMatcher entitiesMatcher;

    Tokenizer::Tokens tokens;

    tokens.push_back("I");
    tokens.push_back("want");
    tokens.push_back("to");
    tokens.push_back("order");
    tokens.push_back("a");
    tokens.push_back("Starbucks");

    EntitiesMatcher::Variables variables = entitiesMatcher.match(tokens, m_dictionaryModel);

    EntitiesMatcher::Variable v1, v2;
    v1.term = 1;
    v1.entity = 1;
    v1.text = "want";

    v2.term = 0;
    v2.entity = 0;
    v2.text = "Starbucks";

    ASSERT_THAT(variables, ElementsAre(v1, v2));
}

TEST_F(EntitiesMatcherWishCoffeeTest, find_only_one_of_the_two_entities)
{
    EntitiesMatcher entitiesMatcher;

    Tokenizer::Tokens tokens;

    tokens.push_back("Please");
    tokens.push_back("give");
    tokens.push_back("me");
    tokens.push_back("a");
    tokens.push_back("Starbucks");

    EntitiesMatcher::Variables variables = entitiesMatcher.match(tokens, m_dictionaryModel);

    EntitiesMatcher::Variable v2;

    v2.term = 0;
    v2.entity = 0;
    v2.text = "Starbucks";

    ASSERT_THAT(variables, ElementsAre(v2));
}

TEST_F(EntitiesMatcherWishCoffeeTest, do_not_find_any_entity)
{
    EntitiesMatcher entitiesMatcher;

    Tokenizer::Tokens tokens;

    tokens.push_back("Please");
    tokens.push_back("give");
    tokens.push_back("me");
    tokens.push_back("a");
    tokens.push_back("Whisky");

    EntitiesMatcher::Variables variables = entitiesMatcher.match(tokens, m_dictionaryModel);

    ASSERT_THAT(variables, IsEmpty());
}


TEST_F(EntitiesMatcherWishCoffeeTest, find_entities_even_the_case_is_different)
{
    EntitiesMatcher entitiesMatcher;

    Tokenizer::Tokens tokens;

    tokens.push_back("Please");
    tokens.push_back("give");
    tokens.push_back("me");
    tokens.push_back("a");
    tokens.push_back("STARBUCKS"); // The expected term is Starbucks.

    EntitiesMatcher::Variables variables = entitiesMatcher.match(tokens, m_dictionaryModel);

    EntitiesMatcher::Variable v1;

    v1.term = 0;
    v1.entity = 0;
    v1.text = "STARBUCKS";

    ASSERT_THAT(variables, ElementsAre(v1));
}

TEST_F(RegexMatcherTest, test_phone_number)
{
    std::string number("0612345678");
    std::string not_number("0x000000");
    std::string regexp("^[0-9]{10,}$");

    ASSERT_TRUE(RegexMatcher::match(number, regexp));
    ASSERT_FALSE(RegexMatcher::match(not_number, regexp));
}

TEST_F(RegexMatcherTest, test_email)
{
    std::string email("abc@gmail.com");
    std::string not_email("abc@a.co.a");
    std::string regexp("^[_a-z0-9-]+(\\.[_a-z0-9-]+)*@[a-z0-9-]+(\\.[a-z0-9-]+)*(\\.[a-z]{2,4})$");

    ASSERT_TRUE(RegexMatcher::match(email, regexp));
    ASSERT_FALSE(RegexMatcher::match(not_email, regexp));
}


TEST_F(RegexMatcherTest, test_simple)
{
    std::string simple("abc");
    std::string regexp("^abc$");

    ASSERT_TRUE(RegexMatcher::match(simple, regexp));
}

TEST_F(EntitiesMatcherRegexTest, test_simple_regex_match)
{
    EntitiesMatcher entitiesMatcher;

    Tokenizer::Tokens tokens;

    tokens.push_back("Call");
    tokens.push_back("me");
    tokens.push_back("maybe");
    tokens.push_back("0612345678"); //valid phone number

    EntitiesMatcher::Variables variables = entitiesMatcher.match(tokens, m_dictionaryModel);

    EntitiesMatcher::Variable phonenumber;
    phonenumber.entity = PHONE_NUMBER_ENTITY_ID;
    phonenumber.term = -1;
    phonenumber.text = "0612345678";

    ASSERT_THAT(variables, ElementsAre(phonenumber));
}

TEST_F(EntitiesMatcherRegexTest, test_that_regex_markers_are_padded)
{
    std::string sentence("Bonjour j'aimerais [une kro]*");
    ASSERT_EQ("Bonjour j'aimerais  [ une kro ]  * ", RegexMatcher::padAroundRegexMarkersInSentence(sentence));
}

TEST_F(EntitiesMatcherRegexTest, test_that_regex_markers_are_encoded)
{
    ASSERT_TRUE(RegexMatcher::isRegexMarker("["));
    ASSERT_TRUE(RegexMatcher::isRegexMarker("]"));
    ASSERT_TRUE(RegexMatcher::isRegexMarker("("));
    ASSERT_TRUE(RegexMatcher::isRegexMarker(")"));
    ASSERT_TRUE(RegexMatcher::isRegexMarker("*"));
    ASSERT_TRUE(RegexMatcher::isRegexMarker("|"));
    ASSERT_FALSE(RegexMatcher::isRegexMarker("{"));
    ASSERT_FALSE(RegexMatcher::isRegexMarker("}"));

    for(size_t i = 0; i<RegexMatcher::REGEX_MARKERS.size(); ++i)
       ASSERT_EQ(0xFFFF-i,
                 static_cast<unsigned int>(RegexMatcher::regexMarkerToEntityId(
                         std::string({RegexMatcher::REGEX_MARKERS[i]}))));
}

TEST_F(EntitiesMatcherRegexTest, test_simple_regex_not_match)
{
    EntitiesMatcher entitiesMatcher;

    Tokenizer::Tokens tokens;

    tokens.push_back("Call");
    tokens.push_back("me");
    tokens.push_back("maybe");
    tokens.push_back("0x12345678"); //not a valid phone number

    EntitiesMatcher::Variables variables = entitiesMatcher.match(tokens, m_dictionaryModel);

    EntitiesMatcher::Variable phonenumber;
    phonenumber.entity = PHONE_NUMBER_ENTITY_ID;
    phonenumber.term = -1;
    phonenumber.text = "0x12345678";

    ASSERT_EQ(0, static_cast<int>(variables.size()));
}
