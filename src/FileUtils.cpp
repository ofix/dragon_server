#include "FileUtils.h"

bool FileUtils::isFileExists(const std::string& path) {
#if defined(_WIN32)
    struct _stat info;
    if (_stat(path.c_str(), &info) == 0) {
        return true;
    }
    return false;
#else
    struct stat st;
    if (stat(path.c_str(), &st) == 0) {
        return true;
    }
    return false;
#endif
}

// 加载整个文件到内存
std::string FileUtils::loadFile(const std::string& filePath) {
    std::ifstream ifs{filePath.c_str(), std::ios::in | std::ios::binary | std::ios::ate};
    if (ifs.good()) {
        std::ifstream::pos_type file_size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        std::vector<char> bytes(file_size);
        ifs.read(bytes.data(), file_size);
        return std::string(bytes.data(), file_size);
    }
    return "";
}

std::vector<std::string> FileUtils::loadDir(const std::string& path,
                                            const std::string& file_suffix) {
    std::vector<std::string> result;
    return result;
}

// 保存文件
bool FileUtils::saveFile(const std::string& filePath, const std::string& content) {
    std::ofstream out(filePath);
    out << content;
    out.close();
    return true;
}

// 递归创建文件夹
bool FileUtils::mkDirs(const std::string& path) {
#if defined(_WIN32)
    int ret = _mkdir(path.c_str());
#else
    mode_t mode = 0755;
    int ret = mkdir(path.c_str(), mode);
#endif

    if (ret == 0) {
        return true;
    }

    switch (errno) {
        case ENOENT: {
            size_t pos = path.find_last_of('/');
            if (pos == std::string::npos)
#if defined(_WIN32)
                pos = path.find_last_of('\\');
            if (pos == std::string::npos)
#endif
                return false;
            if (!mkDirs(path.substr(0, pos)))
                return false;
        }
#if defined(_WIN32)
            return 0 == _mkdir(path.c_str());
#else
            return 0 == mkdir(path.c_str(), mode);
#endif

        case EEXIST:
            return isFileExists(path);

        default:
            return false;
    }
}

std::regex FileUtils::buildRegexFromMap(const std::map<std::string, std::string>& map) {
    std::string pattern_str = "(";
    auto it = map.begin();
    if (it != map.end()) {
        pattern_str += it->first;
        for (++it; it != map.end(); ++it)
            pattern_str += "|" + it->first;
    }
    pattern_str += ")";
    return std::regex(pattern_str);
}

std::string FileUtils::multiRegexReplace(
    const std::string& text,
    const std::map<std::string, std::string>& replacement_map) {
    auto regex = buildRegexFromMap(replacement_map);
    std::string result;
    std::sregex_iterator it(text.begin(), text.end(), regex);
    std::sregex_iterator end;

    size_t last_pos = 0;
    for (; it != end; ++it) {
        result += text.substr(last_pos, it->position() - last_pos);
        result += replacement_map.at(it->str());
        last_pos = it->position() + it->length();
    }
    result += text.substr(last_pos, text.size() - last_pos);

    return result;
}