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
#include <intent/utils/TrigramHelper.hpp>
#include "intent/intent_service/Term.hpp"
#include "intent/intent_service/TermIndex.hpp"

using namespace intent;

TEST(TrigramUtilsTest, PadRightTest) {

    std::string oneletter("a");
    TrigramHelper::padRight(oneletter, 3, ' ');
    EXPECT_EQ("a  ", oneletter);

    std::string twoletters("aa");
    TrigramHelper::padRight(twoletters, 3, ' ');
    EXPECT_EQ("aa ", twoletters);

    std::string threeletters("aaa");
    TrigramHelper::padRight(threeletters, 3, ' ');
    EXPECT_EQ("aaa", threeletters);

    std::string longer("aaaa");
    TrigramHelper::padRight(longer, 3, ' ');
    EXPECT_EQ("aaaa", longer);

    std::string empty("");
    TrigramHelper::padRight(empty, 3, ' ');
    EXPECT_EQ("", empty);
}

TEST(TrigramUtilsTest, AllTrigramGenerationTest) {

    std::string coca("Coca-cola");
    std::vector<std::string> trigrams;
    TrigramHelper::generateTrigrams(coca, trigrams);
    EXPECT_THAT(trigrams, ::testing::ElementsAreArray({" co", "coc", "oca", "ca-", "a-c", "-co", "col", "ola", "la "}));
    trigrams.clear();

    std::string oneletter("I");
    TrigramHelper::generateTrigrams(oneletter, trigrams);
    EXPECT_THAT(trigrams, ::testing::ElementsAre(" i "));
    trigrams.clear();

    std::string twoletters("de");
    TrigramHelper::generateTrigrams(twoletters, trigrams);
    EXPECT_THAT(trigrams, ::testing::ElementsAre(" de", "de "));
}
