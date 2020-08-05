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


std::optional<MsgInfo> createMsgInfo(const fs::path& path, const std::vector<fs::path>& inputPaths, const std::vector<fs::path>& searchPaths) {
	std::ifstream fin(path);
	std::string line;
	fs::path p = path;
	MsgInfo msgInfo(p.remove_filename().parent_path().parent_path().filename().string(), path.filename().stem().string());
	while (std::getline(fin, line)) {
		ltrim(line);
		if (line[0] == '#') continue;
		if (line.length() == 0) continue;
		line = rtrim_copy(line);
		auto tokens = split(line);
		auto tv = typedValue(tokens);
		if (!tv) return std::nullopt;
		msgInfo.typedValues.emplace_back(std::move(tv.value()));
	}
	auto hash = parse_md5(path, msgInfo.packageName, inputPaths, searchPaths);
	msgInfo.setHash(hash.value());
	return msgInfo;
}



int parseMsgDir(fs::path& inputPath, const fs::path& outputPath, const std::vector<fs::path>& inputPaths, const std::vector<fs::path>& searchPaths) {
	fs::path msgs_fullpath = inputPath / "msg";
	if (fs::file_type::directory != fs::status(msgs_fullpath).type()) {
		return -2;
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
	cmakeFile << "configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/" << pkgName << ".wix.patch.in" << std::endl;
	cmakeFile << "  ${CMAKE_CURRENT_BINARY_DIR}/" << pkgName << ".wix.patch)" << std::endl;

	cmakeFile << cmk[2];
	cmakeFile << "set(CPACK_WIX_PATCH_FILE \"${CMAKE_CURRENT_BINARY_DIR}/" << pkgName << ".wix.patch\")" << std::endl;
	cmakeFile << cmk[3];

	fs::create_directories(outputPath / inputPath.filename() / "cmake");
	std::ofstream cmakeInFile(outputPath / inputPath.filename() / "cmake" / ( pkgName + "Config.cmake.in"));
	cmakeInFile << "set(" << pkgName << "_VERSION 0.0.1)\n"
		<< "@PACKAGE_INIT@\n"
		<< "set_and_check(" << pkgName << "__DIR \"@PACKAGE_PACKER_BASE_DIR@\")\n"
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