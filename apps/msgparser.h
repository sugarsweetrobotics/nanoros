#pragma once

#include <string>
#include <optional>
#include <vector>
#include <filesystem>

namespace msgparser {


	inline void ltrim(std::string& s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
			return !std::isspace(ch);
			}));
	}

	inline void rtrim(std::string& s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
			return !std::isspace(ch);
			}).base(), s.end());
	}

	inline std::string rtrim_copy(const std::string& s) {
		std::string ss = s;
		rtrim(ss);
		return ss;
	}

	inline void trim(std::string& s) {
		ltrim(s);
		rtrim(s);
	}


	inline bool is_array(const std::string& typeName) {
		if (typeName.find("[]") == typeName.length() - 2) {
			return true;
		}
		return false;
	}

	inline bool is_fixed_array(const std::string& typeName) {
		if (typeName.find("[") != std::string::npos &&
			(typeName.find("]") - typeName.find("[") > 1)) {
			return true;
		}
		return false;
	}

	inline int fixed_array_size(const std::string& typeName) {
		if (!is_fixed_array(typeName)) { return 0; }
		auto sizeStr = typeName.substr(typeName.rfind("[")+1, typeName.rfind("]")-typeName.rfind("[")-1);
		return atoi(sizeStr.c_str());
	}

	inline std::string arrayElemType(const std::string& typeName) {
		return typeName.substr(0, typeName.rfind("["));
	}

	inline std::vector<std::string> split(const std::string& str, const char sep = ' ') {
		std::vector<std::string> result;
		int64_t i = 0;
		do {
			const int64_t begin = i;
			while (str[i] != sep && str[i] != 0)
				i++;
			const auto buf = str.substr(begin, i - begin);
			if (buf.length() > 0) {
				result.push_back(buf);
			}
		} while (0 != str[i++]);
		return result;
	}

	std::string concat_default_value(const std::string& typeName);



	struct TypedValue {
		std::string typeName;
		std::string valueName;
		std::string comment;
		TypedValue(const std::string& tn, const std::string& vn) : typeName(tn), valueName(vn), comment("") {}
		TypedValue(const std::string& tn, const std::string& vn, const std::string& cmt) : typeName(tn), valueName(vn), comment(cmt) {}

	};

	struct Constant {
		std::string typeName;
		std::string valueName;
		std::string value;
		std::string comment;
		Constant(const std::string& tn, const std::string& vn, const std::string& v) : typeName(tn), valueName(vn), value(v), comment("") {}
		Constant(const std::string& tn, const std::string& vn, const std::string& v, const std::string& cmt) : typeName(tn), valueName(vn), value(v), comment(cmt) {}
	};

	inline std::string join(std::vector<std::string>::const_iterator it, const std::vector<std::string>::const_iterator& end, const std::string& sep) {
		std::stringstream ss;
		while (true) {
			ss << *it;
			++it;
			if (it == end) break;
			ss << sep;
		}
		return ss.str();
	}

	inline std::optional<TypedValue> typedValue(const std::vector<std::string>& tokens) {
		std::string comment = "";
		if (tokens.size() < 2) return std::nullopt;
		if (tokens.size() > 2) {
			if (tokens[2].find('#') != 0) {
				std::cout << "Invalid Msg Type Definition." << std::endl;
				return std::nullopt;
			}
			else {
				comment = join(tokens.begin() + 2, tokens.end(), " ");
			}
		}
		return TypedValue{ tokens[0], tokens[1] , comment };
	}

	struct MsgInfo {
		std::string packageName;
		std::string typeName;
		std::string md5sum;
		std::vector<TypedValue> typedValues;
		std::vector<Constant> constants;
		void setHash(const std::string& md5) { md5sum = md5; }
		MsgInfo(const std::string& pkg, const std::string& name, const std::string& md5 = "*") : packageName(pkg), typeName(name), md5sum(md5) {}
	};


	std::optional<std::string> to_cxx_primitiveTypeName(const std::string& typeName);

	std::optional<std::string> to_cxx_typeName(const std::string& typeName);



	std::vector<std::string> dependentTypeNames(const MsgInfo& msgInfo);



	std::optional<std::string> buildSrc(const MsgInfo& msgInfo);

	std::optional<std::string> buildHeader(const MsgInfo& msgInfo);


	std::optional<std::string> parse_md5(const std::string& fullName, const std::vector<std::filesystem::path>& inputPaths, const std::vector<std::filesystem::path>& searchPaths);

	std::optional<std::string> parse_md5(const std::filesystem::path& path, const std::string& pkgName, const std::vector<std::filesystem::path>& inputPaths, const std::vector<std::filesystem::path>& searchPaths);


	std::string concat_typeName(const std::string& typeName, const std::string& pkgName);

	std::string concat_typePath(const std::string& typeName, const std::string& pkgName);

	std::string concat_member(const TypedValue& tv, const std::string& pkgName);

}

