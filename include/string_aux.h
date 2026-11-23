#ifndef FCG_STRING_AUX
#define FCG_STRING_AUX

#include <string>
#include <vector>

int find_index_string_vec(std::vector<const char*> vec, const char* str);

std::string str_tolower(std::string s);
std::string str_toupper(std::string s);

std::string str_trim(const std::string& str, const std::string& whitespace = " \t\n");

#endif // FCG_STRING_AUX
