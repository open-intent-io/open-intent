//
// Created by clement on 05/05/16.
//

#ifndef INTENT_RESSOURCEMANAGER_HPP
#define INTENT_RESSOURCEMANAGER_HPP

#include <unordered_map>
#include <string>

#include <boost/filesystem.hpp>

namespace intent
{
    namespace test
    {

        class ResourceManager
        {
        public:

            struct ResourceId
            {
                enum Id
                {
                    INTENT_DICTIONARY_DESERIALIZATION_JSON_EXAMPLE,
                    ORDER_BEVERAGE_INTENT_JSON,
                    CHATBOT_MODE_JSON,
                    TEMPLATE_REPLIES_JSON,
                    QUERY_MODE_JSON,
                    INTERPRETER_MODEL,
                    INTERPRETER_MODEL_W_ERRORS,
                    CHATBOT_MODE_JSON_WITHOUT_INTENT_STORY
                };
            };

            ResourceManager(const std::string &resourceDirectory);

            void registerFile(const ResourceId::Id &ressourceId, const std::string &filename);

            const std::string &getResource(const ResourceId::Id &ressourceId) const;

            const std::string &getResourcePath(const ResourceId::Id &ressourceId) const;

        private:
            boost::filesystem::path m_resourceDirectory;
            std::unordered_map<int, std::string> m_fileResources;
            std::unordered_map<int, std::string> m_resourcePaths;
        };


        class ResourceManagerException
        {
        public:
            ResourceManagerException(const std::string &message)
                    : m_message(message)
            {
            };

            const std::string message() const
            {
                return m_message;
            }

        private:
            std::string m_message;
        };
    }
}


#endif //INTENT_RESSOURCEMANAGER_HPP
