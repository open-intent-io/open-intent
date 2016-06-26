//
// Created by clement on 05/05/16.
//

#include "ResourceManager.hpp"

#include <fstream>
#include <sstream>

namespace intent
{
    namespace test
    {
        ResourceManager::ResourceManager(const std::string &resourceDirectory)
            : m_resourceDirectory(resourceDirectory)
        {

        }

        void ResourceManager::registerFile(const ResourceId::Id &ressourceId, const std::string &filename)
        {
            boost::filesystem::path file(filename);
            boost::filesystem::path fullPath = m_resourceDirectory / file;

            m_resourcePaths[ressourceId] = fullPath.c_str();

            std::ifstream f(fullPath.c_str());
            bool exists = f.good();

            if(exists)
            {
                f.open(fullPath.c_str(), std::ifstream::in);
                std::istreambuf_iterator<char> eos;
                std::string content(std::istreambuf_iterator<char>(f), eos);
                m_fileResources[ressourceId] = content;
            }
            else
            {
                throw ResourceManagerException(std::string("File \"") + fullPath.c_str() + "\" does not exist");
            }
        }

        const std::string &ResourceManager::getResource(const ResourceId::Id &ressourceId) const
        {
            std::unordered_map<int, std::string>::const_iterator it = m_fileResources.find(ressourceId);
            if(it != m_fileResources.end())
            {
                return it->second;
            }
            else
            {
                std::stringstream ss;
                ss << "Resource \"" << ressourceId << "\" not found";
                throw ResourceManagerException(ss.str());
            }
        }

        const std::string &ResourceManager::getResourcePath(const ResourceId::Id &ressourceId) const
        {
            std::unordered_map<int, std::string>::const_iterator it = m_resourcePaths.find(ressourceId);
            if(it != m_fileResources.end())
            {
                return it->second;
            }
            else
            {
                std::stringstream ss;
                ss << "Resource \"" << ressourceId << "\" not found";
                throw ResourceManagerException(ss.str());
            }
        }

    }
}
