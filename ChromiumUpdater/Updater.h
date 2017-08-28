#pragma once
#include <SDKDDKVer.h>
#include <fstream>
#include <stdio.h>
#include <thread>
#include <string>
#include <windows.h>

DWORD WINAPI LookupLoop(LPVOID params);

class ChromiumUpdater {
  friend class FileHandler;

public:
  static std::wstring LookupLatestVersion(void);
  static std::wstring LookupCurrentVersion(void);
  static bool InstallExeInDirectory(void);
  static bool DownloadLatestVersion(void);
  static bool DownloadLatestVersionAndHandleButtons(void);
  static bool DoUpdate(void);
  static bool autoUpdateEnabled;
  static bool quietModeEnabled;
  static bool safetyLock;
  static HWND windowHandle;
  static DWORD loopThreadID;
  static HANDLE loopThreadHandle;
};
