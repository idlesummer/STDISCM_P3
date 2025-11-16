#pragma once
#include <string>
#include <vector>
#include <sstream>

/// <summary>
/// Utility class for string operations
/// </summary>
class StringUtils
{
public:
    /// <summary>
    /// Splits a string by a delimiter character
    /// Example: split("path/to/file.png", '/') -> ["path", "to", "file.png"]
    /// </summary>
    static std::vector<std::string> split(const std::string& s, char delimiter);
};
