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

#include "nanorosmsgparser.h"
#include "msgparser.h"

using namespace msgparser;

namespace fs = std::filesystem;




std::optional<std::string> msgparser::buildHeader(const MsgInfo& msgInfo) {
	std::stringstream ss;
	ss << hdr_part[0] << std::endl;
	for (auto& name : dependentTypeNames(msgInfo)) {
		auto pkgName = name.substr(0, name.find('/'));
		auto typeName = name.substr(name.find('/') + 1);
		ss << "#include \"" << pkgName << "/msg/" << typeName << ".h\"" << std::endl;
	}

	ss << hdr_part[1] << std::endl;
	// �l�[���X�y�[�X�g�ݗ���
	ss << "    namespace " << msgInfo.packageName << " {" << std::endl;
	// �N���X�錾
	ss << "        struct " << msgInfo.typeName << " : public ROSMsg {" << std::endl;

	// �����o�ϐ��錾
	for (auto& tv : msgInfo.typedValues) {
		ss << "            " << concat_member(tv, msgInfo.packageName) << ";" << std::endl;
	}

	ss << "            " << std::endl;
	// �R���X�g���N�^�i��������j
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
	ss << " {}" << std::endl;
	// �f�t�H���g�R���X�g���N�^
	ss << "            " << msgInfo.typeName << "():";
	for (int i = 0; i < msgInfo.typedValues.size(); i++) {
		auto& tv = msgInfo.typedValues[i];
		ss << tv.valueName << "(" << concat_default_value(tv.typeName) << ")";
		if (i != msgInfo.typedValues.size() - 1) ss << ", ";
	}
	ss << " {}" << std::endl;
	// �f�X�g���N�^
	ss << "            virtual ~" << msgInfo.typeName << "() {}" << std::endl;
	ss << "\n";
	// �����񐶐�
	ss << "            virtual std::string prettyString(const std::string& indent=\"\") const {" << std::endl;
	ss << "                std::stringstream ss;" << std::endl;
	for (int i = 0; i < msgInfo.typedValues.size(); i++) {
		auto& tv = msgInfo.typedValues[i];
		if (is_array(tv.typeName)) { // �����z��^��������E�E�E
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
		else { // �z��^�ł͂Ȃ��ꍇ
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
	// �^�C�v������擾�֐�
	ss << "    template<>" << std::endl;
	ss << "    std::string msgTypeName<" << msgInfo.packageName << "::" << msgInfo.typeName << ">() { return \""
		<< msgInfo.packageName << "/" << msgInfo.typeName << "\"; }" << std::endl;
	ss << "}" << std::endl;
	return ss.str();
}

std::optional<std::string> msgparser::buildSrc(const MsgInfo& msgInfo) {
	std::stringstream ss;
	ss << src_part[0];
	// �w�b�_�t�@�C��
	ss << "#include \"" << msgInfo.typeName << ".h\"" << std::endl;
	// �l�[���X�y�[�X�g�ݗ���
	ss << "namespace ssr::nanoros {" << std::endl;
	ss << "    namespace " << msgInfo.packageName << " {" << std::endl;
	// �N���X�錾
	ss << "        class " << msgInfo.typeName << "Stub : public ROSMsgStub {" << std::endl;
	ss << "        private:" << std::endl;
	ss << "            using DataType = " << msgInfo.typeName << ";" << std::endl;
	ss << "        public:" << std::endl;
	// �R���X�g���N�^�A�f�X�g���N�^
	ss << "            " << msgInfo.typeName
		<< "Stub() {}" << std::endl;
	ss << "            virtual ~" << msgInfo.typeName << "Stub() {}" << std::endl;
	ss << "        public:" << std::endl;
	// Stub�p���\�b�h
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
	ss << "#define PACKERFACTORY_EXPORT __declspec(dllexport)" << std::endl;
	ss << "#else" << std::endl;
	ss << "#define PACKERFACTORY_EXPORT" << std::endl;
	ss << "#endif" << std::endl;
	ss << std::endl;
	ss << "extern \"C\" {" << std::endl;
	ss << "    PACKERFACTORY_EXPORT void init_msg_" << msgInfo.packageName << "_" << msgInfo.typeName << "(void* factory) {" << std::endl;
	ss << "        static_cast<ssr::nanoros::ROSMsgStubFactory*>(factory)->registerStub(std::make_shared<ssr::nanoros::" << msgInfo.packageName << "::" << msgInfo.typeName << "Stub>());" << std::endl;
	ss << "    }" << std::endl;
	ss << "}" << std::endl;

	return ss.str();
}



std::optional<std::string> msgparser::to_cxx_typeName(const std::string& typeName) {
	if (is_array(typeName)) {
		auto primitiveType = to_cxx_primitiveTypeName(arrayElemType(typeName));
		if (primitiveType) {
			return "std::vector<" + primitiveType.value() + ">";
		}
		return std::nullopt;
	}
	return to_cxx_primitiveTypeName(typeName);
}


std::vector<std::string> msgparser::dependentTypeNames(const MsgInfo& msgInfo) {
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




std::string msgparser::concat_default_value(const std::string& typeName) {
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


std::optional<std::string> msgparser::to_cxx_primitiveTypeName(const std::string& typeName) {
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






extern std::vector<fs::path> dependentPackages;

std::optional<std::string> msgparser::parse_md5(const std::string& fullName, const std::vector<fs::path>& inputPaths, const std::vector<fs::path>& searchPaths) {
	auto tokens = split(fullName);
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
	std::stringstream ss;
	int i = 0;
	while (std::getline(fin, line)) {
		ltrim(line);
		if (line[0] == '#') continue;
		if (line.length() == 0) continue;
		line = rtrim_copy(line);
		auto tokens = split(line);
		auto tv = typedValue(tokens);
		if (!tv) return std::nullopt;
		if (i != 0) ss << (char)0x0a;// << (char)0x0a;
		if (is_array(tokens[0])) {
			// TODO: 
			std::cout << "Path:" << path << " has array member...(" << line << ")" << std::endl;
			auto elemName = tokens[0].substr(0, tokens[0].length()-2);
			
			if (!to_cxx_typeName(elemName)) { // Not primitive type

				if (elemName.find('/') == std::string::npos) {
					// msg struct�Ȃ񂾂��ǁA'/'���܂܂�Ȃ��Ƃ������ƂŁA����f�B���N�g����msg������Ƃ킩��
					auto h = parse_md5((path.parent_path() / elemName).replace_extension(".msg"), pkgName, inputPaths, searchPaths);
					ss << h.value()  << " " << tokens[1];
				}
				else {
					auto h = parse_md5(tokens[0], inputPaths, searchPaths);
					ss << h.value() << "[]" <<  " " << tokens[1];
				}
			}
			else {
				ss << elemName << "[]" << " " << tokens[1];
			}

		}
		else {
			if (!to_cxx_typeName(tokens[0])) { // Not primitive type

				if (tokens[0].find('/') == std::string::npos) {
					// msg struct�Ȃ񂾂��ǁA'/'���܂܂�Ȃ��Ƃ������ƂŁA����f�B���N�g����msg������Ƃ킩��
					auto h = parse_md5((path.parent_path() / tokens[0]).replace_extension(".msg"), pkgName, inputPaths, searchPaths);
					ss << h.value() << " " << tokens[1];
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
		i++;
	}
	return md5.getHashFromString(ss.str());

}



std::string msgparser::concat_typeName(const std::string& typeName, const std::string& pkgName) {
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



std::string msgparser::concat_typePath(const std::string& typeName, const std::string& pkgName) {
	auto primitiveTypeName = to_cxx_typeName(typeName);
	if (primitiveTypeName) {
		return primitiveTypeName.value();
	}
	if (typeName.find('/') == std::string::npos) {
		return pkgName + "/" + typeName;
	}
	return typeName;
}


std::string msgparser::concat_member(const TypedValue& tv, const std::string& pkgName) {
	std::string output = concat_typeName(tv.typeName, pkgName) + " " + tv.valueName;
	if (tv.comment.length() > 0) {
		output += ("; // " + tv.comment);
	}
	return output;
}
