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
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <string>
#include "intent/intent_service/TermIndex.hpp"
#include "intent/intent_service/Term.hpp"

using namespace intent;
using namespace ::testing;

TEST(TermIndexTest, InsertTest) {

    std::string coca("Coca-cola");
    std::string coca_alias1("Coca");
    std::string coca_alias2("Coke");

    Term cocaTerm;
    cocaTerm.term = coca;
    cocaTerm.entityId = 1;
    cocaTerm.termId = 0;
    cocaTerm.alias = {coca_alias1, coca_alias2};

    TermIndex termIndex;
    termIndex.pushTerm(cocaTerm);

    std::set<int> termsSet;

    termIndex.getTermSet("coc", termsSet);
    EXPECT_THAT(termsSet, ElementsAre(0));
    termsSet.clear();

    termIndex.getTermSet("cok", termsSet);
    EXPECT_THAT(termsSet, ElementsAre(0));
    termsSet.clear();

    termIndex.getTermSet("ca ", termsSet);
    EXPECT_THAT(termsSet, ElementsAre(0));
    termsSet.clear();

    termIndex.getTermSet("ke ", termsSet);
    EXPECT_THAT(termsSet, ElementsAre(0));
    termsSet.clear();

    termIndex.getTermSet(" co", termsSet);
    EXPECT_THAT(termsSet, ElementsAre(0));
    termsSet.clear();
}

TEST(TermIndexTest, FetchTest) {

}

TEST(TermIndexTest, find_term_with_part_of_word)
{
    std::string thermostat("thermostat");

    Term thermostatTerm;
    thermostatTerm.term = thermostat;
    thermostatTerm.entityId = 1;
    thermostatTerm.termId = 0;

    TermIndex termIndex;
    termIndex.pushTerm(thermostatTerm);

    EXPECT_EQ(-1, termIndex.findTerm("the").termId);
}


TEST(TermIndexTest, trigram_must_choose_right_term_if_its_length_is_less_than_3_chars)
{
    std::string on("on");
    std::string off("off");

    Term t1, t2;
    t1.term = on;
    t1.entityId = 1;
    t1.termId = 0;

    t2.term = off;
    t2.entityId = 1;
    t2.termId = 1;

    TermIndex termIndex;
    termIndex.pushTerm(t1);
    termIndex.pushTerm(t2);

    EXPECT_EQ(0, termIndex.findTerm("on").termId);
    EXPECT_EQ(1, termIndex.findTerm("off").termId);
}

TEST(TermIndexTest, test_recognize_and_choose_multi_word_term)
{
    std::string eau_de_vie("eau de vie");
    std::string eau_de_zilia("eau de zilia");

    Term t1, t2;
    t1.term = eau_de_vie;
    t1.entityId = 1;
    t1.termId = 0;

    t2.term = eau_de_zilia;
    t2.entityId = 1;
    t2.termId = 1;

    TermIndex termIndex;
    termIndex.pushTerm(t1);
    termIndex.pushTerm(t2);

    int tokensPopped = 1;
    EXPECT_EQ(0, termIndex.findTerm("eau", std::vector<std::string>({"eau","de","vie"}), tokensPopped).termId);
    EXPECT_EQ(3, tokensPopped);
    EXPECT_EQ(1, termIndex.findTerm("eau", std::vector<std::string>({"eau","de","zilia"}), tokensPopped).termId);
    EXPECT_EQ(3, tokensPopped);
}

TEST(TermIndexTest, test_not_recognize_multi_words)
{
    std::string eau_de_vie("eau de vie");
    std::string eau_de_zilia("eau de zilia");

    Term t1, t2;
    t1.term = eau_de_vie;
    t1.entityId = 1;
    t1.termId = 0;

    t2.term = eau_de_zilia;
    t2.entityId = 1;
    t2.termId = 1;

    TermIndex termIndex;
    termIndex.pushTerm(t1);
    termIndex.pushTerm(t2);

    int tokensPopped = 1;
    EXPECT_EQ(-1, termIndex.findTerm("eau", std::vector<std::string>({"eau","vie"}), tokensPopped).termId);
    EXPECT_EQ(1, tokensPopped);
    EXPECT_EQ(-1, termIndex.findTerm("eau", std::vector<std::string>({"eau","de"}), tokensPopped).termId);
    EXPECT_EQ(1, tokensPopped);
}

TEST(TermIndexTest, test_recognize_multi_words_even_if_greedy_perfect_match)
{
    std::string eau("eau");
    std::string eau_de_zilia("eau de zilia");

    Term t1, t2;
    t1.term = eau;
    t1.entityId = 1;
    t1.termId = 0;

    t2.term = eau_de_zilia;
    t2.entityId = 1;
    t2.termId = 1;

    TermIndex termIndex;
    termIndex.pushTerm(t1);
    termIndex.pushTerm(t2);

    int tokensPopped = 1;
    EXPECT_EQ(0, termIndex.findTerm("eau", std::vector<std::string>({"eau", "et", "un", "coca"}), tokensPopped).termId);
    EXPECT_EQ(1, tokensPopped);
    EXPECT_EQ(1, termIndex.findTerm("eau", std::vector<std::string>({"eau", "de", "zilia"}), tokensPopped).termId);
    EXPECT_EQ(3, tokensPopped);
}
