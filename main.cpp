#include <iostream>
#include <regex>
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <vector>
bool isChar(byte b) {
	return (b >= 32 && b <= 126) || b == 10 || b == 13 || b == 9;
}

int main()
{
	std::string dump;
	MEMORY_BASIC_INFORMATION minfo;
	byte* buff{ nullptr };
	PROCESSENTRY32 process_entry = {
	sizeof(PROCESSENTRY32)
	};
	HANDLE processes_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (Process32First(processes_snapshot, &process_entry)) {
		while (Process32Next(processes_snapshot, &process_entry)) {
			if (_wcsicmp(process_entry.szExeFile, L"svchost.exe") == 0) {
				CloseHandle(processes_snapshot);
				HANDLE hproc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_entry.th32ProcessID);
				for (unsigned char* p = 0; VirtualQueryEx(hproc, p, &minfo, sizeof(minfo)) == sizeof(minfo); p += minfo.RegionSize) {
					//std::cout << (LPCVOID)  p << std::endl;
					if (minfo.Protect == PAGE_NOACCESS) continue;
					if (minfo.State == MEM_COMMIT && minfo.Type == MEM_PRIVATE) {
						byte* buff = new byte[(INT64)minfo.RegionSize];
						SIZE_T bytesRead = 0;
						ReadProcessMemory(hproc, (int*)p, buff, minfo.RegionSize, &bytesRead);
						for (int i = 0; i < bytesRead; i++) {
							bool cFlag = isChar(buff[i]);
							if (cFlag)  dump.push_back((char)buff[i]);
						}
					}
				}
			}
		}
	}
	std::vector<std::string> ret;
	std::regex r("192.168.+");
	std::smatch m;
	int i = 0;
	std::sregex_iterator it(dump.begin(), dump.end(), r);
	std::sregex_iterator end;
	while (it != end) {
		for (unsigned i = 0; i < it->size(); ++i) {
			//if ((*it)[i].str().find("protected") == std::string::npos) {
			ret.push_back((*it)[i].str());
			//};
		}
		it++;
	}
	for (auto& i : ret) std::cout << i << std::endl;
	//return 0;
}
