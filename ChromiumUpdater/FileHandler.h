#pragma once
#include <SDKDDKVer.h>
#include <fstream>
#include <stdio.h>
#include <thread>
#include <string>
#include <windows.h>

enum PATH_ENUM {
  PATH_CHROMEEXE = 0,
  PATH_LATESTTXT = 1,
  PATH_CURRENTTXT = 5,
  PATH_LATESTINSTALLEREXE = 2,
  PATH_LATESTDOWNLOADURL = 9,
  PATH_SAVEFOLDER = 7,
  PATH_LATESTLOOKUPURL = 6,
  PATH_TEMPDIR = 3,
  PATH_UINSTALLERURL = 4,
  PATH_CHROMEDIR = 10,
  PATH_APPDATALOCAL = 11
};

enum FILE_ENUM {
  FILE_LATEST = 0,
  FILE_CURR = 1,
};

class FileHandler {
  friend class ChromiumUpdater;
public:
  static std::wstring emptyString;
private:
  static std::wstring GetPath(PATH_ENUM en);
  static std::wstring ReadFile(FILE_ENUM en);
  static bool GetFile(FILE_ENUM en);
  static bool SaveToFile(FILE_ENUM en, std::wstring &content);
  static bool GetFileFromWebWithConfirmation(const wchar_t *downloadUrl, const wchar_t *fileName);
  static bool FolderStructureExists(void);
  static bool CreateFolderStructure(void);
  static bool InitializeFolder(void);
  static std::wstring downloadedFile;
};
