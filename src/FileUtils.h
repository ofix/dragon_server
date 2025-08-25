#pragma once

#include <errno.h>  // errno, ENOENT, EEXIST
#include <sys/stat.h>
#include <fstream>
#include <regex>
#include <string>
#include <vector>
#include <map>

#if defined(_WIN32)
#include <direct.h>  // _mkdir
#endif

class FileUtils {
   public:
    static bool isFileExists(const std::string& path);
    static bool mkDirs(const std::string& path);
    static std::vector<std::string> loadDir(const std::string& path,
                                            const std::string& file_suffix);
    static std::string loadFile(const std::string& filePath);
    static bool saveFile(const std::string& filePath, const std::string& content);
    static std::regex buildRegexFromMap(const std::map<std::string, std::string>& map);
    static std::string multiRegexReplace(
        const std::string& text,
        const std::map<std::string, std::string>& replacement_map);
};