/*
        2023-10-26
*/

#pragma once

#include <unistd.h>

#include <string>
#include <vector>
#include <map>

namespace lkup69
{
struct KeyVal {
        KeyVal(const std::string &k, const std::string &v)
                : key(k),
                  val(v)
        {
        }

        KeyVal(std::string &&k, std::string &v)
        {
                key = std::move(k);
                val = std::move(v);
        }
        
        std::string key;
        std::string val;
};

class IniParser
{
public:
        IniParser()  = default;
        ~IniParser() = default;

        IniParser(const IniParser &)             = delete;
        IniParser &operator()(const IniParser &) = delete;

        int ParseFile(const std::string &file);

        size_t NumSections(void)
        {
                return m_sectionMap.size();
        }

        std::string getSessionNameByIndex(size_t index);
        std::string Get(const std::string &section,
                        const std::string &key,
                        const std::string &defaultVal = std::string{});

        void Set(const std::string &section,
                 const std::string &key,
                 const std::string &val);

#ifdef UNIT_TEST
        void dump(void);
#endif
private:
        std::map<std::string, std::vector<KeyVal>> m_sectionMap;
};
}  // namespace lkup69