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

        std::string readFile(const boost::filesystem::path& filepath)
        {
            std::ifstream f(filepath.c_str());
            bool exists = f.good();

            if(exists)
            {
                f.open(filepath.c_str(), std::ifstream::in);
                std::istreambuf_iterator<char> eos;
                std::string content(std::istreambuf_iterator<char>(f), eos);
                return content;
            }
            else
            {
                throw ResourceManagerException(std::string("File \"") + filepath.c_str() + "\" does not exist");
            }
        }

        void ResourceManager::registerFile(const ResourceId::Id &ressourceId, const std::string &filename)
        {
            boost::filesystem::path file(filename);
            boost::filesystem::path fullPath = m_resourceDirectory / file;

            m_resourcePaths[ressourceId] = fullPath.c_str();
            m_fileResources[ressourceId] = readFile(fullPath);
        }

        const std::string& ResourceManager::getResource(const ResourceId::Id &ressourceId) const
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

        const std::string& ResourceManager::getResourcePath(const ResourceId::Id &ressourceId) const
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

        std::string ResourceManager::read(const std::string& filepath) const
        {
            boost::filesystem::path file(filepath);
            boost::filesystem::path fullPath = m_resourceDirectory / file;
            return readFile(fullPath);
        }

    }
}
