/*
        2023-10-26
*/

#include "IniParser.hpp"

#include <iostream>
#include <fstream>
#include <regex>

using namespace std;
using namespace lkup69;

int IniParser::ParseFile(const string &file)
{
        ifstream ifs(file);

        if (ifs.is_open()) {
                string line;
                string section;
                string key;
                string val;

                auto trimmed__ = [](string &line) {
                        size_t pos;

                        if (line.empty())
                                return;

                        pos = line.rfind('\n');
                        if (pos != string::npos)
                                line.erase(pos, line.size() - pos);
                        pos = line.rfind('\r');
                        if (pos != string::npos)
                                line.erase(pos, line.size() - pos);
                        if (line[0] == ' ') {
                                pos = 0;
                                while (pos < line.size()) {
                                        if (line[pos] != ' ') {
                                                line.erase(0, pos);
                                                break;
                                        }

                                        ++pos;
                                }
                        }

                        pos = line.size() - 1;
                        while (pos > 0) {
                                if ((line[pos] != ' ') and (pos == (line.size() - 1))) {
                                        break;
                                } else if (line[pos] != ' ') {
                                        line.erase(pos + 1, line.size() - pos);

                                        break;
                                }
                                --pos;
                        }
                };

                auto keyVal__ = [](string &line, string &key, string &val) -> bool {
                        size_t pos;

                        pos = line.find('=');
                        if (pos != string::npos) {
                                size_t i;

                                for (i = pos - 1; i > 0; --i) {
                                        if (line[i] != ' ')
                                                break;
                                }
                                key = line.substr(0, i + 1);

                                for (i = pos + 1; i < line.size(); ++i) {
                                        if (line[i] != ' ')
                                                break;
                                }
                                val = line.substr(i, line.size() - i);

                                return true;
                        }

                        return false;
                };

                while (getline(ifs, line)) {
                        size_t posH;
                        size_t posT;

                        trimmed__(line);

                        posH = line.find('[');
                        posT = line.find(']');
                        if ((posH != string::npos) && (posT != string::npos)) {
                                section = line.substr(posH + 1, posT - 1);
                                continue;
                        }

                        if (keyVal__(line, key, val)) {
                                trimmed__(key);
                                trimmed__(val);
                                m_sectionMap[section].emplace_back(std::move(key), std::move(val));
                        }
                }
                return 0;
        }

        return -1;
}

string IniParser::getSessionNameByIndex(size_t index)
{
        if (index > m_sectionMap.size())
                return string();

        auto it = m_sectionMap.begin();
        for (size_t cnt = 0; cnt < index; ++cnt, ++it) {
        }

        return it->first;
}

string IniParser::Get(const string &section, const string &key, const string &defaultVal)
{
        if (m_sectionMap[section].size() > 0) {
                auto it = std::find_if(m_sectionMap[section].begin(),
                                       m_sectionMap[section].end(),
                                       [key](const KeyVal &keyval) {
                                               return keyval.key == key;
                                       });

                if (it != m_sectionMap[section].end())
                        return it->val;
        }

        return string{ defaultVal };
}

void IniParser::Set(const string &section, const string &key, const string &val)
{
        if (m_sectionMap[section].size() > 0) {
                auto it = std::find_if(m_sectionMap[section].begin(),
                                       m_sectionMap[section].end(),
                                       [key](const KeyVal &keyval) {
                                               return keyval.key == key;
                                       });

                if (it != m_sectionMap[section].end()) {
                        it->val = val;

                        return;
                }
        }
        
        m_sectionMap[section].emplace_back(std::forward<KeyVal>(KeyVal{ key, val }));
}

#ifdef UNIT_TEST
void IniParser::dump(void)
{
#if 0        
        size_t n = NumSections();

        for (size_t i = 0; i < n; ++i) {
                string section{ std::move(getSessionNameByIndex(i)) };

                cout << section << endl;
                for (auto &it : m_sectionMap[section])
                        cout << "\t" << it.key << "=" << it.val << endl;
        }
#else
        for (auto &section : m_sectionMap) {
                cout << "[" << section.first << "]" << endl;
                for (auto &keyval : section.second) {
                        cout << keyval.key << "=" << keyval.val << endl;
                }
        }
#endif
}

// g++ -DUNIT_TEST -o IniParser IniParser.cpp
int main(int argc, char *argv[])
{
        IniParser iniParser;

        if (iniParser.ParseFile(argv[1]) == 0) {
                iniParser.dump();
                cout << "-----------------------------------\n";
                cout << "Name:" << iniParser.Get("Firefox", "Name") << endl;
                cout << "UUU:" << iniParser.Get("Firefox", "UUU") << endl;
                cout << "UUU:" << iniParser.Get("Firefox", "UUU", "Unknow") << endl;
                cout << "-----------------------------------\n";
                iniParser.Set("Firefox", "Name", "AAA");
                iniParser.Set("Firefox", "BBB", "CCC");
                iniParser.dump();
        }

        return 0;
}
#endif
