#include <string>
#include <filesystem>
#include <fstream>
#include <optional>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <sstream>
#include <iostream>


#include "nanorosmsgparser.h"

namespace fs = std::filesystem;


namespace {
	void ltrim(std::string& s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
			return !std::isspace(ch);
			}));
	}

	void rtrim(std::string& s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
			return !std::isspace(ch);
			}).base(), s.end());
	}

	std::string rtrim_copy(const std::string& s) {
		std::string ss = s;
		rtrim(ss);
		return ss;
	}

	void trim(std::string& s) {
		ltrim(s);
		rtrim(s);
	}

	std::vector<std::string> split(const std::string& str, const char sep = ' ') {
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

	struct TypedValue {
		std::string typeName;
		std::string valueName;
		std::string comment;
		TypedValue(const std::string& tn, const std::string& vn) : typeName(tn), valueName(vn), comment("") {}
		TypedValue(const std::string& tn, const std::string& vn, const std::string& cmt) : typeName(tn), valueName(vn), comment(cmt) {}

	};

	std::string join(std::vector<std::string>::const_iterator it, const std::vector<std::string>::const_iterator& end, const std::string& sep) {
		std::stringstream ss;
		while(true) {
			ss << *it;
			++it;
			if (it == end) break;
			ss << sep;
		}
		return ss.str();
	}

	std::optional<TypedValue> typedValue(const std::vector<std::string>& tokens) {
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
		return TypedValue{ tokens[0], tokens[1] , comment};
	}

	struct MsgInfo {
		std::string packageName;
		std::string typeName;
		std::string md5sum;
		std::vector<TypedValue> typedValues;
		MsgInfo(const std::string& pkg, const std::string& name, const std::string& md5="*") : packageName(pkg), typeName(name), md5sum(md5) {}
	};


}



bool is_array(const std::string& typeName) {
	if (typeName.find("[]") == typeName.length() - 2) {
		return true;
	}
	return false;
}

std::string arrayElemType(const std::string& typeName) {
	return typeName.substr(0, typeName.length() - 2);
}


std::optional<std::string> to_cxx_primitiveTypeName(const std::string& typeName);

std::optional<std::string> to_cxx_typeName(const std::string& typeName) {
	if (is_array(typeName)) {
		auto primitiveType = to_cxx_primitiveTypeName(arrayElemType(typeName));
		if (primitiveType) {
			return "std::vector<" + primitiveType.value() + ">";
		}
		return std::nullopt;
	}
	return to_cxx_primitiveTypeName(typeName);
}


std::optional<std::string> to_cxx_primitiveTypeName(const std::string& typeName) {
	if (typeName == "bool") return "uint8_t";
	if (typeName == "int8") return "int8_t";
	if (typeName == "char") return "char";
	if (typeName == "uint8") return "uint8_t";
	if (typeName == "byte") return "uint8_t";
	if (typeName == "int16") return "int16_t";
	if (typeName == "uint16") return "uint16_t";
	if (typeName == "int32") return "int32_t";
	if (typeName == "uint32") return "uint32_t";
	if (typeName == "int64") return "int64_t";
	if (typeName == "uint64") return "uint64_t";
	if (typeName == "float32") return "float";
	if (typeName == "float64") return "double";
	if (typeName == "string") return "std::string";
	if (typeName == "time") return "ssr::nanoros::time";
	if (typeName == "duration") return "ssr::nanoros::duration";
	return std::nullopt;
}

std::string concat_typeName(const std::string& typeName, const std::string& pkgName) {
	auto primitiveTypeName = to_cxx_typeName(typeName);
	if (primitiveTypeName) {
		return primitiveTypeName.value();
	}

	if (is_array(typeName)) { // Array of Struct
		auto structTypeName = typeName.substr(0, typeName.length() - 2);
		if (structTypeName.find('/') == std::string::npos) {
			return "std::vector<" + pkgName + "::" + structTypeName + ">";
		}
		auto structPkgName = structTypeName.substr(0, structTypeName.find('/'));
		auto structElemName = structTypeName.substr(structTypeName.find('/'));

		return "std::vector<" + structPkgName + "::" + structElemName + ">";
	}
	if (typeName.find('/') == std::string::npos) {
		return pkgName + "::" + typeName;
	}
	auto structPkgName = typeName.substr(0, typeName.find('/'));
	auto structElemName = typeName.substr(typeName.find('/'));
	return structPkgName + "::" + structElemName;
}



std::string concat_typePath(const std::string& typeName, const std::string& pkgName) {
	auto primitiveTypeName = to_cxx_typeName(typeName);
	if (primitiveTypeName) {
		return primitiveTypeName.value();
	}
	if (typeName.find('/') == std::string::npos) {
		return pkgName + "/" + typeName;
	}
	return typeName;
}

std::string concat_member(const TypedValue& tv, const std::string& pkgName) {
	std::string output = concat_typeName(tv.typeName, pkgName) + " " + tv.valueName;
	if (tv.comment.length() > 0) {
		output += ("; // " + tv.comment);
	}
	return output;
}

std::string concat_default_value(const std::string& typeName) {
	if (typeName == "bool") return "false";
	if (typeName == "int8") return "0";
	if (typeName == "char") return "0";
	if (typeName == "uint8") return "0";
	if (typeName == "byte") return "0";
	if (typeName == "int16") return "0";
	if (typeName == "uint16") return "0";
	if (typeName == "int32") return "0";
	if (typeName == "uint32") return "0";
	if (typeName == "int64") return "0";
	if (typeName == "uint64") return "0";
	if (typeName == "float32") return "0.0f";
	if (typeName == "float64") return "0.0";
	if (typeName == "string") return "\"\"";
	if (typeName == "time") return "{0,0}";
	if (typeName == "duration") return "{0,0}";
	return "";
}

std::vector<std::string> dependentTypeNames(const MsgInfo& msgInfo) {
	std::vector<std::string> buf;
	for (auto& tv : msgInfo.typedValues) {
		auto typeName = tv.typeName;
		if (is_array(tv.typeName)) {
			typeName = tv.typeName.substr(0, tv.typeName.length() - 2);
		}

		if (to_cxx_typeName(typeName)) {}
		else {
			buf.push_back(concat_typePath(typeName, msgInfo.packageName));
		}
	}

	return buf;
}


std::optional<std::string> buildHeader(const MsgInfo& msgInfo) {
	std::stringstream ss;
	ss << hdr_part[0] << std::endl;
	for (auto& name : dependentTypeNames(msgInfo)) {
		auto pkgName = name.substr(0, name.find('/'));
		auto typeName = name.substr(name.find('/')+1);
		ss << "#include \"" << pkgName << "/msg/" << typeName << ".h\"" << std::endl;
	}

	ss << hdr_part[1] << std::endl;
	// ネームスペース組み立て
	ss << "    namespace " << msgInfo.packageName << " {" << std::endl;
	// クラス宣言
	ss << "        struct " << msgInfo.typeName << " : public ROSMsg {" << std::endl;

	// メンバ変数宣言
	for (auto& tv : msgInfo.typedValues) {
		ss << "            " << concat_member(tv, msgInfo.packageName) << ";" << std::endl;
	}

	ss << "            " << std::endl;
	// コンストラクタ（引数あり）
	ss << "            " << msgInfo.typeName << "(";
	for (int i = 0; i < msgInfo.typedValues.size(); i++) {
		auto& tv = msgInfo.typedValues[i];
		ss << "const " << concat_typeName(tv.typeName, msgInfo.packageName) << "& _" << tv.valueName;
		if (i != msgInfo.typedValues.size() - 1) ss << ", ";
	}
	ss << "):";
	for (int i = 0; i < msgInfo.typedValues.size(); i++) {
		auto& tv = msgInfo.typedValues[i];
		ss << tv.valueName << "(_" << tv.valueName << ")";
		if (i != msgInfo.typedValues.size() - 1) ss << ", ";
	}
	ss << "{}" << std::endl;
	// デフォルトコンストラクタ
	ss << "            " << msgInfo.typeName << "():";
	for (int i = 0; i < msgInfo.typedValues.size(); i++) {
		auto& tv = msgInfo.typedValues[i];
		ss << tv.valueName << "(" << concat_default_value(tv.typeName) << ")";
		if (i != msgInfo.typedValues.size() - 1) ss << ", ";
	}
	ss << "{}" << std::endl;
	// デストラクタ
	ss << "            virtual ~" << msgInfo.typeName << "(){}" << std::endl;
	ss << "\n";
	// 文字列生成
	ss << "            virtual std::string prettyString(const std::string& indent=\"\") const {" << std::endl;
	ss << "                std::stringstream ss;" << std::endl;
	for (int i = 0; i < msgInfo.typedValues.size(); i++) {
		auto& tv = msgInfo.typedValues[i];
		if (is_array(tv.typeName)) { // もし配列型だったら・・・
			auto elemTypeName = tv.typeName.substr(0, tv.typeName.length() - 2);
			if (to_cxx_typeName(elemTypeName)) {
				ss << "                ss << indent << \"" << tv.valueName << "\" << \": [\";" << std::endl;
				ss << "                auto it = " << tv.valueName << ".begin();" << std::endl;
				ss << "                while (true) {" << std::endl;
				ss << "                    ss << *it;" << std::endl;
				ss << "                    ++it;" << std::endl;
				ss << "                    if (it == " << tv.valueName << ".end()) break;" << std::endl;
				ss << "                    ss << ',';" << std::endl;
				ss << "                }" << std::endl;
				ss << "                ss << ']' << std::endl;" << std::endl;
			}
			else {
				ss << "                ss << indent << \"" << tv.valueName << ":\" << std::endl;" << std::endl;
				ss << "                for(auto& elem : " << tv.valueName << ") {" << std::endl;
				ss << "                    ss << indent << \" - \" << std::endl;" << std::endl;
				ss << "                    ss << elem.prettyString(indent + \"    \");" << std::endl;
				ss << "                }" << std::endl;
			}
		}
		else { // 配列型ではない場合
			if (to_cxx_typeName(tv.typeName)) {
				ss << "                ss << indent << \"" << tv.valueName << "\" << ':' << " << tv.valueName << " << std::endl;" << std::endl;
			}
			else {
				ss << "                ss << indent << \"" << tv.valueName << ":\" << std::endl;" << std::endl;
				ss << "                ss << " << tv.valueName << ".prettyString(indent + \"    \");" << std::endl;
			}
		}
	}
	ss << "                return ss.str();" << std::endl;
	ss << "            }" << std::endl;
	ss << "        };" << std::endl;
	ss << "    }" << std::endl;
	// タイプ文字列取得関数
	ss << "    template<>" << std::endl;
	ss << "    std::string msgTypeName<" << msgInfo.packageName << "::" << msgInfo.typeName << ">() { return \""
		<< msgInfo.packageName << "/" << msgInfo.typeName << "\"; }" << std::endl;
	ss << "}" << std::endl;
	return ss.str();
}

std::optional<std::string> buildSrc(const MsgInfo& msgInfo) {
	std::stringstream ss;
	ss << src_part[0];
	// ヘッダファイル
	ss << "#include \"" << msgInfo.typeName << ".h\"" << std::endl;
	// ネームスペース組み立て
	ss << "namespace ssr::nanoros {" << std::endl;
	ss << "    namespace " << msgInfo.packageName << " {" << std::endl;
	// クラス宣言
	ss << "        class " << msgInfo.typeName << "Stub : public ROSMsgStub {" << std::endl;
	ss << "        private:" << std::endl;
	ss << "            using DataType = " << msgInfo.typeName << ";" << std::endl;
	ss << "        public:" << std::endl;
	// コンストラクタ、デストラクタ
	ss << "            " << msgInfo.typeName 
		<< "Stub() {}" << std::endl;
	ss << "            virtual ~" << msgInfo.typeName << "Stub() {}" << std::endl;
	ss << "        public:" << std::endl;
	// Stub用メソッド
	ss << "            virtual std::string md5sum() const override { return \"" << msgInfo.md5sum << "\"; }" << std::endl;
	ss << "            virtual std::string typeName() const override { return \"" << msgInfo.packageName << "/" << msgInfo.typeName << "\"; }" << std::endl;
	ss << "            virtual std::shared_ptr<const ROSMsg> toMsg(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) override {" << std::endl;
	ss << "                auto val = std::make_shared<DataType>();" << std::endl;
	for (auto& tv : msgInfo.typedValues) {
		if (is_array(tv.typeName)) {
			auto elemTypeName = tv.typeName.substr(0, tv.typeName.length() - 2);
			if (to_cxx_typeName(elemTypeName)) {
				ss << "                pushValue(val->" << tv.valueName << ", msg, popedCount);" << std::endl;
			}
			else {
				ss << "                pushValue(val->" << tv.valueName << ", getMsgStub(\"" << concat_typePath(elemTypeName, msgInfo.packageName) << "\"), msg, popedCount);" << std::endl;
			}
		}
		else {
			if (to_cxx_typeName(tv.typeName)) {
				ss << "                setValue(val, val->" << tv.valueName << ", msg->pop<" << to_cxx_typeName(tv.typeName).value() << ">(popedCount));" << std::endl;
			}
			else {
				// TODO: Not checked
				ss << "                setValue(val->" << tv.valueName << ", getMsgStub(\"" << concat_typePath(tv.typeName, msgInfo.packageName) << "\"), msg, popedCount);" << std::endl;
			}
		}
	}
	ss << "                return val;" << std::endl;
	ss << "            }" << std::endl;
	ss << "            virtual std::shared_ptr<TCPROSPacket> toPacket(const ROSMsg& msg) override {" << std::endl;
	ss << "                const auto val = dynamic_cast<const DataType&>(msg);" << std::endl;
	ss << "                auto pkt = std::make_shared<TCPROSPacket>();" << std::endl;
	for (auto& tv : msgInfo.typedValues) {
		if (is_array(tv.typeName)) {

			auto elemTypeName = tv.typeName.substr(0, tv.typeName.length() - 2);
			if (to_cxx_typeName(elemTypeName)) {
				ss << "                pkt->pushVector(val." << tv.valueName << ");" << std::endl;
			}
			else {
				ss << "                pushVectorValue(pkt, getMsgStub(\"" << concat_typePath(elemTypeName, msgInfo.packageName) << "\"), val." << tv.valueName << ");" << std::endl;
			}

		}
		else {
			if (to_cxx_typeName(tv.typeName)) {
				ss << "                pkt->push(val." << tv.valueName << ");" << std::endl;
			}
			else {
				// TODO: Not checked
				ss << "                pushValue(pkt, getMsgStub(\"" << concat_typePath(tv.typeName, msgInfo.packageName) << "\"), val." << tv.valueName << ");" << std::endl;
			}
		}
	}
	ss << "                return pkt;" << std::endl;
	ss << "            }" << std::endl;
	ss << "            virtual std::shared_ptr<ROSMsg> fromJSON(const std::shared_ptr<const JSONObject> json) override {" << std::endl;
	ss << "                if (!json) return nullptr;" << std::endl;
	ss << "                auto val = std::make_shared<DataType>();" << std::endl;
	for (auto& tv : msgInfo.typedValues) {
		if (is_array(tv.typeName)) {
			auto elemTypeName = tv.typeName.substr(0, tv.typeName.length() - 2);
			
			if (to_cxx_typeName(elemTypeName)) {
				// TODO:
				ss << "                setArrayValue(val->" << tv.valueName << ", json, \"" << tv.valueName << "\");" << std::endl;
			}
			else {
				ss << "                setArrayValue(val->" << tv.valueName << ", getMsgStub(\"" << concat_typePath(elemTypeName, msgInfo.packageName) << "\"), json, \"" << tv.valueName << "\");" << std::endl;
			}
		}
		else {
			if (to_cxx_typeName(tv.typeName)) {
				ss << "                setValue<" << to_cxx_typeName(tv.typeName).value() << ">(val->" << tv.valueName << ", json, \"" << tv.valueName << "\");" << std::endl;
			}
			else {
				// TODO: Not Checked
				ss << "                setValue(val->" << tv.valueName << ", getMsgStub(\"" << concat_typePath(tv.typeName, msgInfo.packageName) << "\"), json, \"" << tv.valueName << "\");" << std::endl;
			}
		}
	}
	ss << "                return val;" << std::endl;
	ss << "            }" << std::endl;
	ss << "        };" << std::endl;
	ss << "    }" << std::endl;
	ss << "}" << std::endl;
	ss << std::endl;
	ss << "#ifdef WIN32" << std::endl;
	ss << "#define STUBFACTORY_EXPORT __declspec(dllexport)" << std::endl;
	ss << "#else" << std::endl;
	ss << "#define STUBFACTORY_EXPORT" << std::endl;
	ss << "#endif" << std::endl;
	ss << std::endl;
	ss << "extern \"C\" {" << std::endl;
	ss << "    STUBFACTORY_EXPORT void init_msg_" << msgInfo.packageName << "_" << msgInfo.typeName << "(void* factory) {" << std::endl;
	ss << "        static_cast<ssr::nanoros::ROSMsgStubFactory*>(factory)->registerStub(std::make_shared<ssr::nanoros::" << msgInfo.packageName << "::" << msgInfo.typeName << "Stub>());" << std::endl;
	ss << "    }" << std::endl;
	ss << "}" << std::endl;
	
	return ss.str();
}

std::optional<MsgInfo> createMsgInfo(const fs::path& path) {
	std::ifstream fin(path);
	std::string line;
	fs::path p = path;
	MsgInfo msgInfo(p.remove_filename().parent_path().parent_path().filename().string(), path.filename().stem().string());
	while (std::getline(fin, line)) {
		ltrim(line);
		if (line[0] == '#') continue;
		if (line.length() == 0) continue;
		auto tv = typedValue(split(rtrim_copy(line)));
		if (!tv) return std::nullopt;
		msgInfo.typedValues.emplace_back(std::move(tv.value()));
	}
	return msgInfo;
}

std::optional<std::pair<std::string, std::string>> parse_msg(const fs::path& path) {
	auto msgInfo = createMsgInfo(path);
	if (!msgInfo) return std::nullopt;
	auto hdr = buildHeader(msgInfo.value());
	auto src = buildSrc(msgInfo.value());
	if (!hdr || !src) return std::nullopt;
	return std::pair<std::string, std::string>(*hdr, *src);
}


int main(const int argc, const char* argv[]) {

	if (argc == 1) return -1;
	fs::path inputPath = argv[1];
	fs::path msgs_fullpath = inputPath / "msg";
	if (fs::file_type::directory != fs::status(msgs_fullpath).type()) {
		return -2;
	}
	fs::path msg_outputDir(inputPath.filename() / "msg");
	if (!fs::create_directories(msg_outputDir)) {
		std::cerr << "Can not create path (" << msg_outputDir << ")" << std::endl;
		//return -1;
	}
	for (const auto& f : fs::directory_iterator(msgs_fullpath)) {
		if (f.path().extension() == ".msg") {
			auto result = parse_msg(f.path());
			if (!result) continue;

			auto& [hdr, src] = result.value();
			std::ofstream hdrFile((msg_outputDir / f.path().stem()).replace_extension(".h"));
			hdrFile << hdr;
			std::ofstream srcFile((msg_outputDir / f.path().stem()).replace_extension(".cpp"));
			srcFile << src;
		}
	}
	return 0;
}