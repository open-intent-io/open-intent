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
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "intent/utils/Tokenizer.hpp"

using namespace ::testing;

namespace intent
{
    namespace test
    {
        TEST(TokenizerTest, split_string_into_tokens_taking_regexp_with_dots_into_account)
        {
            std::vector<std::string> tokens;
            std::string input = "Hello, my phone number is 06.78.66.55.44.";
            std::vector<std::string> regexpList;
            regexpList.push_back("([0-9]{2}.){4}[0-9]{2}");
            Tokenizer tokenizer(",. ", regexpList);

            tokenizer.tokenize(input, tokens);

            EXPECT_THAT(tokens, ElementsAre("Hello", "my", "phone", "number", "is", "06.78.66.55.44"));
        }

        TEST(TokenizerTest, split_string_into_tokens_taking_regexp_with_spaces_into_account)
        {
            std::vector<std::string> tokens;
            std::string input = "Hello, my phone number is 06 78 66 55 44.";
            std::vector<std::string> regexpList;
            regexpList.push_back("([0-9]{2}\\s){4}[0-9]{2}");
            Tokenizer tokenizer(",. ", regexpList);

            tokenizer.tokenize(input, tokens);

            EXPECT_THAT(tokens, ElementsAre("Hello", "my", "phone", "number", "is", "06 78 66 55 44"));
        }

        TEST(TokenizerTest, split_string_into_tokens_taking_several_regexp_into_account)
        {
            std::vector<std::string> tokens;
            std::string input = "Hello, phone number 06 78 66 55 44 and email john.doe@gmail.com, yeah.";
            std::vector<std::string> regexpList;
            regexpList.push_back("[.a-z0-9]+@[a-z0-9-]+\\.[a-z]+");
            regexpList.push_back("([0-9]{2}\\s){4}[0-9]{2}");
            Tokenizer tokenizer(",. ", regexpList);

            tokenizer.tokenize(input, tokens);

            EXPECT_THAT(tokens, ElementsAre("Hello", "phone", "number", "06 78 66 55 44", "and",
                                            "email", "john.doe@gmail.com", "yeah"));
        }
    }
}
