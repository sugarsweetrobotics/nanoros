#include <string>
#include <filesystem>
#include <fstream>
#include <optional>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <sstream>
#include <iostream>

#include "hl_md5wrapper.h"

#include "../nanorosmsgparser.h"
#include "msgparser.h"

using namespace msgparser;

namespace fs = std::filesystem;




extern std::vector<fs::path> dependentPackages;

std::optional<std::string> msgparser::parse_md5(const std::string& fullName, const std::vector<fs::path>& inputPaths, const std::vector<fs::path>& searchPaths) {
	auto tokens = split(fullName, '/');
	if (tokens.size() < 2) return std::nullopt;

	auto pkgName = tokens[0];
	auto typeName = tokens[1];

	for (auto inputPath : inputPaths) {
		if (inputPath.filename() == pkgName) {
			auto filePath = (inputPath / "msg" / typeName).replace_extension(".msg");
			if (fs::is_regular_file(filePath)) {
				dependentPackages.push_back(pkgName);
				return parse_md5(filePath, pkgName, inputPaths, searchPaths);
			}
		}
	}

	for (auto searchPath : searchPaths) {
		auto filePath = (searchPath / pkgName / "msg" / typeName).replace_extension(".msg");
		if (fs::is_regular_file(filePath)) {
			dependentPackages.push_back(pkgName);
			return parse_md5(filePath, pkgName, inputPaths, searchPaths);
		}
	}


	return std::nullopt;
}


std::optional<std::string> msgparser::parse_md5(const fs::path& path, const std::string& pkgName, const std::vector<fs::path>& inputPaths, const std::vector<fs::path>& searchPaths) {
	std::ifstream fin(path);
	std::string line;
	fs::path p = path;
	md5wrapper md5;
	std::stringstream consts;
	std::stringstream ss;
	int i = 0;
	while (std::getline(fin, line)) {
		ltrim(line);
		if (line[0] == '#') continue;
		if (line.length() == 0) continue;
		line = rtrim_copy(line);
		std::string comment = "";
		if (line.find('#') != std::string::npos) {
			comment = line.substr(line.find('#')+1);
			line = line.substr(0, line.find('#'));
		}
		auto tokens = split(line);
		if (line.find('=') != std::string::npos) {
			 // TODO: 
			auto tokens = split(line, '=');
			auto subtokens = split(tokens[0]);
			std::string typeName = subtokens[0];
			std::string valueName = subtokens[1];
			std::string valueValue = tokens[1];
			consts << typeName << " " << valueName << " " << valueValue;

		} else {
			auto tv = typedValue(tokens);
			if (!tv) return std::nullopt;
			if (i != 0) ss << (char)0x0a;// << (char)0x0a;
			if (is_array(tokens[0])) {
				// std::cout << "Path:" << path << " has array member...(" << line << ")" << std::endl;
				auto elemName = tokens[0].substr(0, tokens[0].length()-2);
				
				if (!to_cxx_typeName(elemName)) { // Not primitive type
					if (elemName.find('/') == std::string::npos) {
						if (elemName == "Header") {
							auto h = parse_md5("std_msgs/Header", inputPaths, searchPaths);
							ss << h.value()  << "[] " << tokens[1];

						} else {
							// msg struct�Ȃ񂾂��ǁA'/'���܂܂�Ȃ��Ƃ������ƂŁA����f�B���N�g����msg������Ƃ킩��
							auto h = parse_md5((path.parent_path() / elemName).replace_extension(".msg"), pkgName, inputPaths, searchPaths);
							
							ss << h.value() /* << "[]" */ << " " << tokens[1];
						}
					}
					else {
						auto h = parse_md5(elemName, inputPaths, searchPaths);
						ss << h.value() /* << "[]"*/ <<  " " << tokens[1];
					}
				}
				else {
					ss << elemName << "[]" << " " << tokens[1];
				}

			}
			else if (is_fixed_array(tokens[0])) {
				// TODO: Pattern Fixed Array
				auto size = fixed_array_size(tokens[0]);
				auto elemName = arrayElemType(tokens[0]);
				if (!to_cxx_typeName(elemName)) { // Not primitive type
					if (elemName.find('/') == std::string::npos) {
						if (elemName == "Header") {
							auto h = parse_md5("std_msgs/Header", inputPaths, searchPaths);
							ss << h.value()  << "[" << size << "] " << tokens[1];

						} else {
							// msg struct�Ȃ񂾂��ǁA'/'���܂܂�Ȃ��Ƃ������ƂŁA����f�B���N�g����msg������Ƃ킩��
							auto h = parse_md5((path.parent_path() / elemName).replace_extension(".msg"), pkgName, inputPaths, searchPaths);
							
							ss << h.value()  << "[" << size << "] " << tokens[1];
						}
					}
					else {
						auto h = parse_md5(elemName, inputPaths, searchPaths);
						ss << h.value() << "[" << size << "]" <<  " " << tokens[1];
					}
				}
				else {
					ss << elemName << "[" << size << "]" << " " << tokens[1];
				}
			}
			else {
				if (!to_cxx_typeName(tokens[0])) { // Not primitive type

					if (tokens[0].find('/') == std::string::npos) {
                        if (tokens[0] == "Header") {
							auto h = parse_md5("std_msgs/Header", inputPaths, searchPaths);
							ss << h.value() << " " << tokens[1];
                        } else {
                            auto h = parse_md5((path.parent_path() / tokens[0]).replace_extension(".msg"), pkgName, inputPaths, searchPaths);
                            ss << h.value() << " " << tokens[1];
                        }
					}
					else {
						auto h = parse_md5(tokens[0], inputPaths, searchPaths);
						ss << h.value() << " " << tokens[1];
					}
				}
				else {
					ss << tokens[0] << " " << tokens[1];
				}
			}
		}
		i++;
	}
	return md5.getHashFromString(consts.str() + ss.str());

}

