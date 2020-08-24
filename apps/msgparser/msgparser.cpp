/**
 * 
 * coding: utf-8
 */

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


std::optional<std::string> msgparser::buildHeader(const MsgInfo& msgInfo) {
	std::stringstream ss;
	ss << hdr_part[0] << std::endl;
	for (auto& name : dependentTypeNames(msgInfo)) {
		auto pkgName = name.substr(0, name.find('/'));
		auto typeName = name.substr(name.find('/') + 1);
		ss << "#include \"" << pkgName << "/msg/" << typeName << ".h\"" << std::endl;
	}

	ss << hdr_part[1] << std::endl;
	// ネームスペースの作成
	ss << "    namespace " << msgInfo.packageName << " {" << std::endl;
	// 構造体の宣言
	ss << "        struct " << msgInfo.typeName << " : public ROSMsg {" << std::endl;

	// Const変数の宣言
	for(auto& c : msgInfo.constants) {
		ss << "            " << to_cxx_primitiveTypeName(c.typeName).value() << " "
           << c.valueName << " = " << c.value << ";";
		if (c.comment.length() > 0) {
			ss << " // " << c.comment;
		}
		ss << std::endl;
	}

    ss << std::endl;
	// メンバ変数の宣言
	for (auto& tv : msgInfo.typedValues) {
		ss << "            " << concat_member(tv, msgInfo.packageName) << ";" << std::endl;
	}

	ss << "            " << std::endl;
	// コンストラクタ

	if (msgInfo.typedValues.size() > 0) {
		ss << "            " << msgInfo.typeName << "(";
		// コンストラクタの引数の生成
		for (int i = 0; i < msgInfo.typedValues.size(); i++) {
			auto& tv = msgInfo.typedValues[i];
			ss << "const " << concat_typeName(tv.typeName, msgInfo.packageName) << "& _" << tv.valueName;
			if (i != msgInfo.typedValues.size() - 1) ss << ", ";
		}
		ss << "):";
		// 引数をメンバに代入する部分
		for (int i = 0; i < msgInfo.typedValues.size(); i++) {
			auto& tv = msgInfo.typedValues[i];
			if (!is_fixed_array(tv.typeName)) {
				if (i != 0) ss << ", ";
				ss << tv.valueName << "(_" << tv.valueName << ")";
			}
		}
		ss << " {";
		for (int i = 0; i < msgInfo.typedValues.size(); i++) {
			auto& tv = msgInfo.typedValues[i];
			if (is_fixed_array(tv.typeName)) {
				auto elemName = arrayElemType(tv.typeName);
				auto size = fixed_array_size(tv.typeName);
				if (is_primitive_typename(elemName)) {
					ss << std::endl;
					ss << "            for(int i = 0;i < " << size << ";i++) {" << std::endl;
					ss << "                " << tv.valueName << "[i] = 0;" << std::endl;
					ss << "            }" << std::endl;
				}
			}
		}
		ss << "}" << std::endl;
	}

	// デフォルトコンストラクタ
	ss << "            " << msgInfo.typeName << "()";

	if (msgInfo.typedValues.size() > 0) {
		ss << ':';
		// 各メンバのデフォルト値の設定
		for (int i = 0; i < msgInfo.typedValues.size(); i++) {
			auto& tv = msgInfo.typedValues[i];
			ss << tv.valueName << "(" << concat_default_value(tv.typeName) << ")";
			if (i != msgInfo.typedValues.size() - 1) ss << ", ";
		}
	}
	ss << " {}" << std::endl;
	// デストラクタ
	ss << "            virtual ~" << msgInfo.typeName << "() {}" << std::endl;
	ss << "\n";
	// JSON文字列への変換
	ss << "            virtual std::string toJSONString() const {" << std::endl;
	ss << "                std::stringstream ss;" << std::endl;
	ss << "                ss << '{';" << std::endl;
	for (int i = 0; i < msgInfo.typedValues.size(); i++) {
		auto& tv = msgInfo.typedValues[i];
		if (is_array(tv.typeName)) { // メンバ変数のタイプが配列だったら
			auto elemTypeName = tv.typeName.substr(0, tv.typeName.length() - 2);
			if (to_cxx_typeName(elemTypeName)) {
				ss << "                ss << \"\\\"" << tv.valueName << "\\\"\" << \": [\";" << std::endl;
				ss << "                for(int i = 0;i < " << tv.valueName << ".size();i++) {" << std::endl;
				if (elemTypeName == "string") {
				  ss << "                    ss << \"\\\"\" << " << tv.valueName << "[i] << \"\\\"\";" << std::endl;
				} else {
				  ss << "                    ss << " << tv.valueName << "[i];" << std::endl;
				}
				ss << "                    if (i == " << tv.valueName << ".size() - 1) break;" << std::endl;
				ss << "                    ss << ',';" << std::endl;
				ss << "                }" << std::endl;
				ss << "                ss << \"]\";" << std::endl;
			}
			else {
				ss << "                ss << \"\\\"" << tv.valueName << "\\\": [\";" << std::endl;
				ss << "                for(int i = 0; i < " << tv.valueName << ".size();i++) {" << std::endl;
				ss << "                    ss << " << tv.valueName << "[i].toJSONString();" << std::endl;
				ss << "                    if (i == " << tv.valueName << ".size() -1) break;" << std::endl;
				ss << "                    ss << ',';" << std::endl;
				ss << "                }" << std::endl;
				ss << "                ss << \"]\";" << std::endl;
			}
		}
		else if (is_fixed_array(tv.typeName)) {
			auto size = fixed_array_size(tv.typeName);
			auto elemTypeName = arrayElemType(tv.typeName);


			if (is_fixed_array(elemTypeName)) {

				auto size2 = fixed_array_size(elemTypeName);
				elemTypeName = arrayElemType(elemTypeName);
				if (to_cxx_typeName(elemTypeName)) {
					ss << "                ss << \"\\\"" << tv.valueName << "\\\"\" << \": [\";" << std::endl;
					ss << "                for(int i = 0;i < " << size << ";i++) {" << std::endl;
					ss << "                    ss << \"[\";" << std::endl;
					ss << "                    for(int j = 0;j < " << size2 << ";j++) {" << std::endl;
					if (elemTypeName == "string") {
					  ss << "                        ss << \"\\\"\" << " << tv.valueName << "[i][j] << \"\\\"\";" << std::endl;
					} else {
					  ss << "                        ss << " << tv.valueName << "[i][j];" << std::endl;
					}
  					ss << "                        if (j == " << size2 -1 << ") break;" << std::endl;
					ss << "                        ss << ',';" << std::endl;
					ss << "                    }" << std::endl;
					ss << "                    ss << \"]\";" << std::endl;
  					ss << "                    if (i == " << size -1 << ") break;" << std::endl;
					ss << "                    ss << ',';" << std::endl;
					ss << "                }" << std::endl;
					ss << "                ss << \"]\";" << std::endl;
				} 
				else {

					ss << "                ss << \"\\\"" << tv.valueName << "\\\"\" << \": [\";" << std::endl;
					ss << "                for(int i = 0;i < " << size << ";i++) {" << std::endl;
					ss << "                    ss << \"[\";" << std::endl;
					ss << "                    for(int j = 0;j < " << size2 << ";j++) {" << std::endl;
					ss << "                        ss << " << tv.valueName << "[i][j].toJSONString();" << std::endl;

  					ss << "                        if (j == " << size2 -1 << ") break;" << std::endl;
					ss << "                        ss << ',';" << std::endl;
					ss << "                    }" << std::endl;
					ss << "                    ss << \"]\";" << std::endl;
  					ss << "                    if (i == " << size -1 << ") break;" << std::endl;
					ss << "                    ss << ',';" << std::endl;
					ss << "                }" << std::endl;
					ss << "                ss << \"]\";" << std::endl;
				}
			} else {
				if (to_cxx_typeName(elemTypeName)) {
					ss << "                ss << \"\\\"" << tv.valueName << "\\\"\" << \": [\";" << std::endl;
					ss << "                for(int i = 0;i < " << size << ";i++) {" << std::endl;
					if (elemTypeName == "string") {
					  ss << "                    ss << \"\\\"\" << " << tv.valueName << "[i] << \"\\\"\";" << std::endl;
					} else {
					  
					  ss << "                    ss << " << tv.valueName << "[i];" << std::endl;
					}
					ss << "                    if (i == " << size -1 << ") break;" << std::endl;
					ss << "                    ss << ',';" << std::endl;
					ss << "                }" << std::endl;
					ss << "                ss << \"]\";" << std::endl;
				} 
				else {
					ss << "                ss << \"\\\"" << tv.valueName << "\\\": \";" << std::endl;
					ss << "                for(int i = 0; i < " << size << ";i++) {" << std::endl;
					ss << "                    ss << " << tv.valueName << "[i].toJSONString();" << std::endl;
					ss << "                    if (i == " << size-1 << ") break;" << std::endl;
					ss << "                    ss << ',';" << std::endl;
					ss << "                }" << std::endl;
					ss << "                ss << \"]\";" << std::endl;
				}				
			}

		}
		else { // メンバ変数が配列でなければ
			if (is_primitive_typename(tv.typeName)) {
			  if (tv.typeName == "string") {
			    ss << "                ss << \"\\\"" << tv.valueName << "\\\"\" << \": \" << \"\\\"\" << " <<  tv.valueName << " << \"\\\"\";" << std::endl;
			  } else {

			    ss << "                ss << \"\\\"" << tv.valueName << "\\\"\" << \": \" << " << tv.valueName << ';' << std::endl;
			  }
			}
			else {
				ss << "                ss << \"\\\"" << tv.valueName << "\\\": \";" << std::endl;
				ss << "                ss << " << tv.valueName << ".toJSONString();" << std::endl;
			}
		}
		if (i == msgInfo.typedValues.size() -1) break;
		ss << "                ss << ',';" << std::endl;
	}
	ss << "                ss << '}';" << std::endl;
	ss << "                return ss.str();" << std::endl;
	ss << "            }" << std::endl;
	ss << "        };" << std::endl;
	ss << "    }" << std::endl;
	// Type Name String getter
	ss << "    template<>" << std::endl;
	ss << "    std::string msgTypeName<" << msgInfo.packageName << "::" << msgInfo.typeName << ">() { return \""
		<< msgInfo.packageName << "/" << msgInfo.typeName 
		<< "\"; }" << std::endl;

	// Type Declaration Show String getter
	ss << "    template<>" << std::endl;
	ss << "    std::string showTypeInfo<" << msgInfo.packageName << "::" << msgInfo.typeName << ">(const std::string& indent) {" << std::endl;
	ss << "    std::stringstream ss;" << std::endl;
	for (auto c : msgInfo.constants) {
		ss << "      ss << indent << \"" << c.typeName << " " << c.valueName << " = " << c.value << "\\n\";"<< std::endl;
	}
	for (auto tv : msgInfo.typedValues) {
		auto typeName = tv.typeName;
		if (typeName.find("[") != std::string::npos) { // Not Array
			typeName = typeName.substr(0, typeName.find("["));
		}
		//ss << "// typeName=" << typeName << std::endl;
		auto fullTypeName = tv.typeName;
		if ((!is_primitive_typename(typeName)) && (fullTypeName.find('/') == std::string::npos)) {
			if (fullTypeName == "Header") fullTypeName = "std_msgs/Header";
			else if (fullTypeName == "time") fullTypeName = "time";
			else if (fullTypeName == "duration") fullTypeName = "duration";
			else fullTypeName = msgInfo.packageName + "/" + fullTypeName;
		}
		typeName = fullTypeName;
		if (fullTypeName.find("[") != std::string::npos) { // Not Array
			typeName = fullTypeName.substr(0, fullTypeName.find("["));
		}
		//ss << "// typeName2=" << typeName << std::endl;
		//ss << "// fullTypeName=" << fullTypeName << std::endl;
		ss << "      ss << indent << \"" << fullTypeName << " " << tv.valueName << "\\n\";" << std::endl;
		if (!is_primitive_typename(typeName)) {
			auto cppTypeName = typeName;
			if (typeName == "time") cppTypeName = "ssr::nanoros::time";
			else if (typeName == "duration") cppTypeName = "ssr::nanoros::duration";
			else {
				auto tokens = split(typeName, '/');
				cppTypeName = tokens[0] + "::" + tokens[1];
			}
			ss << "         ss << showTypeInfo<" << cppTypeName << ">(indent+\"  \");" << std::endl;
		}
	}
	ss << "    return ss.str();" << std::endl;
	ss << "    }" << std::endl;

	// Namespace
	ss << "}" << std::endl;
	return ss.str();
}

std::optional<std::string> msgparser::buildSrc(const MsgInfo& msgInfo) {
	std::stringstream ss;
	ss << src_part[0];
	// Header
	ss << "#include \"" << msgInfo.typeName << ".h\"" << std::endl;
	// Namespace
	ss << "namespace ssr::nanoros {" << std::endl;
	ss << "    namespace " << msgInfo.packageName << " {" << std::endl;
	// Class Desclare
	ss << "        class " << msgInfo.typeName << "Packer : public ROSMsgPacker {" << std::endl;
	ss << "        private:" << std::endl;
	ss << "            using DataType = " << msgInfo.typeName << ";" << std::endl;
	ss << "        public:" << std::endl;
	// Declare Constructor
	ss << "            " << msgInfo.typeName
		<< "Packer() {}" << std::endl;
	ss << "            virtual ~" << msgInfo.typeName << "Packer() {}" << std::endl;
	ss << "        public:" << std::endl;
	// Packer Interfaces
	ss << "            virtual std::string md5sum() const override { return \"" << msgInfo.md5sum << "\"; }" << std::endl;
	ss << "            virtual std::string typeName() const override { return \"" << msgInfo.packageName << "/" << msgInfo.typeName << "\"; }" << std::endl;
	ss << "            virtual std::string typeInfo(const std::string& indent=\"\") const override { return showTypeInfo<DataType>(indent); }" << std::endl;
	ss << "            virtual std::shared_ptr<const ROSMsg> toMsg(const std::optional<TCPROSPacket>& msg, int32_t& popedCount) override {" << std::endl;
	ss << "                auto val = std::make_shared<DataType>();" << std::endl;
	for (auto& tv : msgInfo.typedValues) {
		if (is_array(tv.typeName)) {
			auto elemTypeName = tv.typeName.substr(0, tv.typeName.length() - 2);
			if (to_cxx_typeName(elemTypeName)) {
				ss << "                pushValue(val->" << tv.valueName << ", msg, popedCount);" << std::endl;
			}
			else {
				ss << "                pushValue(val->" << tv.valueName << ", getMsgPacker(\"" << concat_typePath(elemTypeName, msgInfo.packageName) << "\"), msg, popedCount);" << std::endl;
			}
		}
		else if (is_fixed_array(tv.typeName)) {
			// TODO: Pattern Fixed Array

			auto size = fixed_array_size(tv.typeName);
			auto elemTypeName = arrayElemType(tv.typeName);
			if (is_fixed_array(elemTypeName)) {
				auto size2 = fixed_array_size(elemTypeName);
				elemTypeName = arrayElemType(elemTypeName);

				if (to_cxx_typeName(elemTypeName)) {
					ss << "                pushValue(val->" << tv.valueName << ", msg, popedCount);" << std::endl;
				}
				else {
					ss << "                pushValue(val->" << tv.valueName << ", getMsgPacker(\"" << concat_typePath(elemTypeName, msgInfo.packageName) << "\"), msg, popedCount);" << std::endl;
				}


			} else {

				if (to_cxx_typeName(elemTypeName)) {
					ss << "                pushValue(val->" << tv.valueName << ", " << size << ", msg, popedCount);" << std::endl;
				}
				else {
					ss << "                pushValue(val->" << tv.valueName << ", " << size << ", getMsgPacker(\"" << concat_typePath(elemTypeName, msgInfo.packageName) << "\"), msg, popedCount);" << std::endl;
				}

			}



		}
		else {
			if (to_cxx_typeName(tv.typeName)) {
				ss << "                setValue(val, val->" << tv.valueName << ", msg->pop<" << to_cxx_typeName(tv.typeName).value() << ">(popedCount));" << std::endl;
			}
			else {
				ss << "                setValue(val->" << tv.valueName << ", getMsgPacker(\"" << concat_typePath(tv.typeName, msgInfo.packageName) << "\"), msg, popedCount);" << std::endl;
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
				ss << "                pushVectorValue(pkt, getMsgPacker(\"" << concat_typePath(elemTypeName, msgInfo.packageName) << "\"), val." << tv.valueName << ");" << std::endl;
			}

		}
		else if (is_fixed_array(tv.typeName)) {
			// TODO: Pattern Fixed Array


			auto size = fixed_array_size(tv.typeName);
			auto elemTypeName = arrayElemType(tv.typeName);


			if (is_fixed_array(elemTypeName)) {

				auto size2 = fixed_array_size(elemTypeName);
				elemTypeName = arrayElemType(elemTypeName);
			} else {
				if (to_cxx_typeName(elemTypeName)) {

				    ss << "                for(int i = 0;i < " << size << ";i++) {" << std::endl;
					ss << "                    pkt->push(val." << tv.valueName << "[i]);" << std::endl;
					ss << "                }" << std::endl;
				}
				else {
					ss << "                for(int i = 0;i < " << size << ";i++) {" << std::endl;
					ss << "                    pushValue(pkt, getMsgPacker(\"" << concat_typePath(elemTypeName, msgInfo.packageName) << "\"), val." << tv.valueName << "[i]);" << std::endl;
					ss << "                }" << std::endl;
				}
			}
			


		}
		else {
			if (to_cxx_typeName(tv.typeName)) {
				ss << "                pkt->push(val." << tv.valueName << ");" << std::endl;
			}
			else {
				// TODO: Not checked
				ss << "                pushValue(pkt, getMsgPacker(\"" << concat_typePath(tv.typeName, msgInfo.packageName) << "\"), val." << tv.valueName << ");" << std::endl;
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
				ss << "                setArrayValue(val->" << tv.valueName << ", getMsgPacker(\"" << concat_typePath(elemTypeName, msgInfo.packageName) << "\"), json, \"" << tv.valueName << "\");" << std::endl;
			}
		}
		else if (is_fixed_array(tv.typeName)) {
			// TODO: Pattern Fixed Array


			auto size = fixed_array_size(tv.typeName);
			auto elemTypeName = arrayElemType(tv.typeName);
		
			if (to_cxx_typeName(elemTypeName)) {
				ss << "                setArrayValue(val->" << tv.valueName << ", " << size << ", json, \"" << tv.valueName << "\");" << std::endl;
			}
			else {
				ss << "                setArrayValue(val->" << tv.valueName << ", " << size << ", getMsgPacker(\"" << concat_typePath(elemTypeName, msgInfo.packageName) << "\"), json, \"" << tv.valueName << "\");" << std::endl;
			}

		}
		else {
			if (to_cxx_typeName(tv.typeName)) {
				ss << "                setValue<" << to_cxx_typeName(tv.typeName).value() << ">(val->" << tv.valueName << ", json, \"" << tv.valueName << "\");" << std::endl;
			}
			else {
				// TODO: Not Checked
				ss << "                setValue(val->" << tv.valueName << ", getMsgPacker(\"" << concat_typePath(tv.typeName, msgInfo.packageName) << "\"), json, \"" << tv.valueName << "\");" << std::endl;
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
	ss << "        static_cast<ssr::nanoros::ROSMsgPackerFactory*>(factory)->registerPacker(std::make_shared<ssr::nanoros::" << msgInfo.packageName << "::" << msgInfo.typeName << "Packer>());" << std::endl;
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
	else if (is_fixed_array(typeName)) {
		// TODO: Pattern Fixed Array


		auto size = fixed_array_size(typeName);
		auto elemTypeName = arrayElemType(typeName);


		if (is_fixed_array(elemTypeName)) {

			auto size2 = fixed_array_size(elemTypeName);
			elemTypeName = arrayElemType(elemTypeName);
		}
			


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

		if (is_fixed_array(typeName)) {
			typeName = tv.typeName.substr(0, tv.typeName.find('['));
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
		auto structElemName = structTypeName.substr(structTypeName.find('/')+1);

		return "std::vector<" + structPkgName + "::" + structElemName + ">";
	}
	else if (is_fixed_array(typeName)) {
		auto elemType = arrayElemType(typeName);
		auto size = fixed_array_size(typeName);
		auto primitiveTypeName2 = to_cxx_typeName(elemType);
		if (primitiveTypeName2) {
			return primitiveTypeName2.value();
		}
		if (elemType.find('/') == std::string::npos) {
			if (elemType == "Header") {
				return "std_msgs::Header";
			}
			return pkgName + "::" + typeName;
		}
		auto structPkgName = typeName.substr(0, typeName.find('/'));
		auto structElemName = typeName.substr(typeName.find('/')+1);
		return structPkgName + "::" + structElemName;


		// TODO: Multi Dimention Array

		
	}
	// もし'/'がなければ同一パッケージ内の要素
	if (typeName.find('/') == std::string::npos) {
		if (typeName == "Header") {
			return "std_msgs::Header";
		}
		return pkgName + "::" + typeName;
	}
	auto structPkgName = typeName.substr(0, typeName.find('/'));
	auto structElemName = typeName.substr(typeName.find('/')+1);
	return structPkgName + "::" + structElemName;
}



std::string msgparser::concat_typePath(const std::string& typeName, const std::string& pkgName) {
	auto primitiveTypeName = to_cxx_typeName(typeName);
	if (primitiveTypeName) {
		return primitiveTypeName.value();
	}
	// もし'/'がなければ同一パッケージの要素だが・・・
	if (typeName.find('/') == std::string::npos) {
		if (typeName == "Header") {
			return "std_msgs/Header";
		}
		return pkgName + "/" + typeName;
	}
	return typeName;
}


std::string msgparser::concat_member(const TypedValue& tv, const std::string& pkgName) {
	std::string output = concat_typeName(tv.typeName, pkgName) + " " + tv.valueName;
	if (is_fixed_array(tv.typeName)) {
		auto size = fixed_array_size(tv.typeName);
		if (size != 0) {
			output += "[";
			output += std::to_string(size);
			output += "]";
		} else {
			if (is_fixed_array(tv.typeName))
			output += " ";
		}
	}


	if (tv.comment.length() > 0) {
		output += ("; // " + tv.comment);
	} 
	return output;
}
