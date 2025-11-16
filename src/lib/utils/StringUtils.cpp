#include "lib/utils/StringUtils.h"
#include <sstream>

using namespace std;


vector<string> StringUtils::split(const string& s, char delimiter) {
    auto tokens = vector<string>();
    auto token = string();
    auto tokenStream = istringstream(s);
    
    while (getline(tokenStream, token, delimiter))
        tokens.push_back(token);
    
    return tokens;
}
