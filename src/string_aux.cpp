#include "string_aux.h"

#include <cctype>
#include <cstring>
#include <algorithm>

int find_index_string_vec(std::vector<const char*> vec, const char* str)
{
    for (int j = 0; j < (int)vec.size(); j++){
        if (!strcmp(vec[j],str)) {
            return j;
        }
    }
    return -1;
}

std::string str_tolower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
    return s;
}

std::string str_toupper(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::toupper(c); });
    return s;
}

//source
//https://stackoverflow.com/questions/1798112/removing-leading-and-trailing-spaces-from-a-string
std::string str_trim(const std::string& str, const std::string& whitespace)
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}
