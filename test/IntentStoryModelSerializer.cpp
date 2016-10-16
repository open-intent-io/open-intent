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
// Created by clement on 07/05/16.
//

//
// Created by clement on 07/05/16.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "launcher/TestContext.hpp"
#include "json.hpp"

#include "intent/utils/Deserializer.hpp"
#include "intent/intent_story_service/IntentStoryServiceModel.hpp"
#include "intent/intent_story_service/IntentStoryModelSerializer.hpp"

using namespace ::testing;

namespace intent
{
    namespace test
    {
        class IntentStoryModelSerializerTest : public ::testing::Test
        {
        public:
            void SetUp()
            {
                const intent::test::ResourceManager &resourceManager = intent::test::gTestContext->getResourceManager();

                std::string jsonContent = resourceManager.getResource(
                        test::ResourceManager::ResourceId::INTENT_DICTIONARY_DESERIALIZATION_JSON_EXAMPLE);

                nlohmann::json json = nlohmann::json::parse(jsonContent);

                Deserializer deserializer;
                m_intentStoryServiceModel = deserializer.deserialize<IntentStoryServiceModel>(json);
            }

            IntentStoryServiceModel m_intentStoryServiceModel;
        };

        TEST_F(IntentStoryModelSerializerTest, test_serialization_of_graph)
        {
            IntentStoryModelSerializer serializer;

            std::stringstream ss;
            serializer.serialize(ss, m_intentStoryServiceModel);

            ASSERT_EQ("digraph G {\n0[label=<root>];\n1[label=<credit_card>];\n2[label=<pay_2_conso>];\n3[label=<how_many>];\n0->1 [label=< <table BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\"><tr><td>intent_id</td><td>order1</td></tr><tr><td>intent_desc</td><td></td></tr><tr><td>entities</td><td>[@beverage, @number]</td></tr><tr><td>action</td><td></td></tr><tr><td>reply</td><td></td></tr></table> >];\n0->2 [label=< <table BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\"><tr><td>intent_id</td><td>order2</td></tr><tr><td>intent_desc</td><td></td></tr><tr><td>entities</td><td>[@beverage, @number]</td></tr><tr><td>action</td><td></td></tr><tr><td>reply</td><td></td></tr></table> >];\n0->3 [label=< <table BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\"><tr><td>intent_id</td><td>order_no_number</td></tr><tr><td>intent_desc</td><td></td></tr><tr><td>entities</td><td>[@beverage]</td></tr><tr><td>action</td><td></td></tr><tr><td>reply</td><td></td></tr></table> >];\n}\n", ss.str());
        }
    }
}
