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

using namespace msgparser;

namespace fs = std::filesystem;

// TOOD: �ˑ���p�b�P�[�W���L�����Ă����āA�R���p�C�����̃C���N���[�h�̏��ɉ����鏕���ɂ���
std::vector<fs::path> dependentPackages;


std::optional<Constant> makeConstant(const std::vector<std::string> tokens) {
	std::string comment = "";
	auto subtokens = split(tokens[0], ' ');
		
	std::string typeName = subtokens[0];
	std::string valueName = subtokens[1];
	std::string value = tokens[1];
	/*
	if (tokens.size() >= 2 && tokens[1].find("=") != std::string::npos) {
		if (subtokens.size() == 1) { // LIKE uint8 VALUE= 3
			valueName = subtokens[0];
			std::string s = tokens[2];
			trim(s);
			value = s;
		} else { /// Like uint8 VALUE=1
			valueName = subtokens[0];
			value = subtokens[1];
		}
	} else if (tokens.size() > 2 && tokens[2].find("=") == 0) { 
		if (tokens[2] == "=") {	// Like uint8 VALUE = 3


		} else { // Like uint8 VALUE =3

		}

	}
	*/
	return Constant(typeName, valueName, value);
	//return TypedValue{ tokens[0], tokens[1] , comment };
}


std::optional<MsgInfo> createMsgInfo(std::istream& fin, const std::string& pkgName, const std::string& msgName, const std::vector<fs::path>& inputPaths, const std::vector<fs::path>& searchPaths);

std::optional<MsgInfo> createMsgInfo(const fs::path& path, const std::vector<fs::path>& inputPaths, const std::vector<fs::path>& searchPaths) {
	std::ifstream fin(path);
	std::string msgName = path.filename().stem().string();
	fs::path p = path;
	std::string pkgName = p.remove_filename().parent_path().parent_path().filename().string();

	return createMsgInfo(fin, pkgName, msgName, inputPaths, searchPaths);
}



std::optional<MsgInfo> createMsgInfo(std::istream& fin, const std::string& pkgName, const std::string& msgName, const std::vector<fs::path>& inputPaths, const std::vector<fs::path>& searchPaths) {
// 	std::ifstream fin(path);
	std::string line;
	//fs::path p = path;
	MsgInfo msgInfo(pkgName, msgName);
	while (std::getline(fin, line)) {
		std::string comment = "";
		ltrim(line);
		if (line[0] == '#') continue;
		if (line.length() == 0) continue;
		line = rtrim_copy(line);
		/// TODO: アルゴリズムはよくできる
		/// まずコメントとそれ以外に分解して，
		/// 次にイコール記号があるか判定してConstantかTypedValueかに分岐した方が得策だな
		if (line.find('#') != std::string::npos) {
			comment = line.substr(line.find('#')+1);
			line = line.substr(0, line.find('#'));
		}
		if (line.find("=") != std::string::npos) {

			auto tokens = split(line, '=');
			auto c = makeConstant(tokens);
			if (!c) continue;
			c->comment = comment;
			msgInfo.constants.emplace_back(std::move(c.value()));

		} else {
			auto tokens = split(line);
			auto tv = typedValue(tokens);
			if (!tv) continue;
			tv->comment = comment;
			msgInfo.typedValues.emplace_back(std::move(tv.value()));
		}
	}
	///auto hash = parse_md5(/*path, msgInfo.packageName*/, inputPaths, searchPaths);
	auto hash = parse_md5(pkgName + "/" + msgName, inputPaths, searchPaths);
	msgInfo.setHash(hash.value());
	return msgInfo;
}


int parseMsgDir(fs::path& inputPath, const fs::path& outputPath, const std::vector<fs::path>& inputPaths, const std::vector<fs::path>& searchPaths) {
	fs::path msgs_fullpath = inputPath / "msg";
	if (fs::file_type::directory != fs::status(msgs_fullpath).type()) {
		return 0;
	}
	fs::path msg_outputDir(outputPath / inputPath.filename() / "msg");
	if (!fs::create_directories(msg_outputDir)) {
		std::cerr << "Can not create path (" << msg_outputDir << ")" << std::endl;
		//return -1;
	}
	std::vector<std::string> msgNames;
	for (const auto& f : fs::directory_iterator(msgs_fullpath)) {
		if (f.path().extension() == ".msg") {

			auto msgInfo = createMsgInfo(f.path(), inputPaths, searchPaths);
			if (!msgInfo) continue;
			auto hdr = buildHeader(msgInfo.value());
			auto src = buildSrc(msgInfo.value());
			if (!hdr || !src) continue;
			std::ofstream hdrFile((msg_outputDir / f.path().stem()).replace_extension(".h"));
			hdrFile << hdr.value();
			std::ofstream srcFile((msg_outputDir / f.path().stem()).replace_extension(".cpp"));
			srcFile << src.value();
			auto msgFilePath((msg_outputDir / f.path().stem()).replace_extension(".msg"));

			if (!fs::is_regular_file(msgFilePath)) {
				fs::copy_file(f.path(), msgFilePath);
			}

			msgNames.push_back(msgInfo->typeName);
		}
	}
	auto pkgName = inputPath.filename().string();


	std::ofstream cmakeFile(msg_outputDir / "CMakeLists.txt");
	// TODO: CMake�̎�������
	cmakeFile << "# cmake for " << inputPath.filename() / "msg" << std::endl;
	cmakeFile << "set(_" << pkgName << "_PACKERS)";
	
    cmakeFile << msgcmk[0];
	for (auto& msgName : msgNames) {
		cmakeFile << "add_msg_packer(" << pkgName << " " << msgName << ")" << std::endl;
		cmakeFile << "set(_" << pkgName << "_PACKERS " << "${_" << pkgName << "_PACKERS} " << pkgName << "_" << msgName << ")" << std::endl;

	}
	cmakeFile << "set(" << pkgName << "_PACKERS " << "${_" << pkgName << "_PACKERS} PARENT_SCOPE)" << std::endl;


	cmakeFile << "INSTALL(TARGETS ${" << pkgName << "_PACKERS}" << std::endl;
	cmakeFile << "	RUNTIME DESTINATION ${RUNTIME_INSTALL_DIR}" << std::endl;
	cmakeFile << "	LIBRARY DESTINATION ${LIB_INSTALL_DIR}" << std::endl;
	cmakeFile << "	ARCHIVE DESTINATION ${LIB_INSTALL_DIR}" << std::endl;
	cmakeFile << "	PUBLIC_HEADER DESTINATION include/${PROJECT_NAME})" << std::endl;

	return 0;
}


std::optional<MsgInfo> createSrvInfo(const fs::path& path, const std::vector<fs::path>& inputPaths, const std::vector<fs::path>& searchPaths) {
	return std::nullopt;
}


int parseSrvDir(fs::path& inputPath, const fs::path& outputPath, const std::vector<fs::path>& inputPaths, const std::vector<fs::path>& searchPaths) {
	fs::path srvs_fullpath = inputPath / "srv";
	if (fs::file_type::directory != fs::status(srvs_fullpath).type()) {
		return 0;
	}
	fs::path srv_outputDir(outputPath / inputPath.filename() / "srv");
	if (!fs::create_directories(srv_outputDir)) {
		std::cerr << "Can not create path (" << srv_outputDir << ")" << std::endl;
		//return -1;
	}
	std::vector<std::string> srvNames;
	for (const auto& f : fs::directory_iterator(srvs_fullpath)) {
		if (f.path().extension() == ".srv") {

			auto srvInfo = createSrvInfo(f.path(), inputPaths, searchPaths);
			if (!srvInfo) continue;
			auto hdr = buildHeader(srvInfo.value());
			auto src = buildSrc(srvInfo.value());
			if (!hdr || !src) continue;
			std::ofstream hdrFile((srv_outputDir / f.path().stem()).replace_extension(".h"));
			hdrFile << hdr.value();
			std::ofstream srcFile((srv_outputDir / f.path().stem()).replace_extension(".cpp"));
			srcFile << src.value();
			auto srvFilePath((srv_outputDir / f.path().stem()).replace_extension(".srv"));
			if (!fs::is_regular_file(srvFilePath)) {
				fs::copy_file(f.path(), srvFilePath);
			}

			srvNames.push_back(srvInfo->typeName);
		}
	}
	auto pkgName = inputPath.filename().string();
	//auto srvName = inputPath.filename().string();


	std::ofstream cmakeFile(srv_outputDir / "CMakeLists.txt");
	// TODO: CMake�̎�������
	cmakeFile << "# cmake for " << inputPath.filename() / "srv" << std::endl;
	cmakeFile << "set(_" << pkgName << "_PACKERS)";
	
    cmakeFile << msgcmk[0];
	for (auto& srvName : srvNames) {
		cmakeFile << "add_srv_packer(" << pkgName << " " << srvName << ")" << std::endl;
		cmakeFile << "set(_" << pkgName << "_PACKERS " << "${_" << pkgName << "_PACKERS} " << pkgName << "_" << srvName << ")" << std::endl;
	}
	cmakeFile << "set(" << pkgName << "_PACKERS " << "${_" << pkgName << "_PACKERS} PARENT_SCOPE)" << std::endl;


	cmakeFile << "INSTALL(TARGETS ${" << pkgName << "_PACKERS}" << std::endl;
	cmakeFile << "	RUNTIME DESTINATION ${RUNTIME_INSTALL_DIR}" << std::endl;
	cmakeFile << "	LIBRARY DESTINATION ${LIB_INSTALL_DIR}" << std::endl;
	cmakeFile << "	ARCHIVE DESTINATION ${LIB_INSTALL_DIR}" << std::endl;
	cmakeFile << "	PUBLIC_HEADER DESTINATION include/${PROJECT_NAME})" << std::endl;

	return 0;
}

int parseDir(fs::path& inputPath, const fs::path& outputPath, const std::vector<fs::path>& inputPaths, const std::vector<fs::path>& searchPaths) {
	int ret = parseMsgDir(inputPath, outputPath, inputPaths, searchPaths);
	if (ret < 0) return ret;

	std::ofstream cmakeFile(outputPath / inputPath.filename() / "CMakeLists.txt");
	// TODO: CMake�̎�������
	std::string pkgName = inputPath.filename().string();

	cmakeFile << "# cmake for " << inputPath.filename() << std::endl;
	cmakeFile << cmk[0];
	cmakeFile << "set(" << pkgName << "_VERSION 0.0.1)" << std::endl;
	cmakeFile << "project(" << inputPath.filename() << " LANGUAGES CXX VERSION ${" << pkgName << "_VERSION})" << std::endl;
	cmakeFile << cmk[1];
	cmakeFile << cmk[2];
	cmakeFile << "configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/" << pkgName << ".wix.patch.in" << std::endl;
	cmakeFile << "  ${CMAKE_CURRENT_BINARY_DIR}/" << pkgName << ".wix.patch)" << std::endl;

	cmakeFile << cmk[3];
	cmakeFile << "set(CPACK_WIX_PATCH_FILE \"${CMAKE_CURRENT_BINARY_DIR}/" << pkgName << ".wix.patch\")" << std::endl;
	cmakeFile << cmk[4];

	fs::create_directories(outputPath / inputPath.filename() / "cmake");
	std::ofstream cmakeInFile(outputPath / inputPath.filename() / "cmake" / ( pkgName + "Config.cmake.in"));
	cmakeInFile << "set(" << pkgName << "_VERSION 0.0.1)\n"
		<< "@PACKAGE_INIT@\n"
		<< "set_and_check(" << pkgName << "_DIR \"@PACKAGE_PACKER_BASE_DIR@\")\n"
		<< "check_required_components(nanoros)\n";

	std::ofstream cmakePatchFile(outputPath / inputPath.filename() / "cmake" / (pkgName + ".wix.patch.in"));
	cmakePatchFile << 
		"<CPackWiXPatch>\n"
		"  <CPackWiXFragment Id=\"CM_CP_bin." << pkgName << ".dll\" >\n"
		"    <Environment Id=\"CM_CP_bin." << pkgName << ".root\" Action=\"set\" Part=\"last\" Name=\"" << pkgName << "_ROOT\" Value=\"[INSTALL_ROOT]\" />\n"
		"    <Environment Id=\"CM_CP_bin." << pkgName << ".packer_path\" Action=\"set\" Part=\"last\" Name=\"" << pkgName << "_PACKER_DIRS\" Value=\"[INSTALL_ROOT]@PACKER_BASE_DIR@\" />\n"
		"  </CPackWiXFragment>\n"
		"</CPackWiXPatch>" << std::endl;


	std::ofstream licenseFile(outputPath / inputPath.filename() / "License.txt");
	licenseFile << "Input your License";
	return 0;
}

// TODO: �ˑ��֌W������p�b�P�[�W�̃f�B���N�g�����������Ƃ��Ď擾����H
// buildin����ʈ����������Ȃ�

int main(const int argc, const char* argv[]) {

	if (argc == 1) return -1;
	std::vector<fs::path> input_paths;
	std::vector<fs::path> search_paths;
	fs::path outputPath(".");
	for (int i = 1; i < argc; i++) {
		if (argv[i] == std::string("-o")) {
			i++;
			if (i < argc) {
				outputPath = argv[i];
			}
		}
		else if (argv[i] == std::string("-i")) {
			i++;
			if (i < argc) {
				search_paths.emplace_back(fs::path(argv[i]));
			}
		}
		else {
			input_paths.push_back(argv[i]);
		}
	}

	auto buf = std::getenv("NANOROS_PACKER_DIRS");
	if (buf) {
#if WIN32
		char sep = ';';
#else 
		char sep = ':';
#endif
		auto paths = split(buf, sep);
		for (auto& p : paths) {
			search_paths.emplace_back(fs::path(p));
		}
	}

	for (auto& i : input_paths) {
		parseDir(i, outputPath, input_paths, search_paths);
	}

	return 0;
}