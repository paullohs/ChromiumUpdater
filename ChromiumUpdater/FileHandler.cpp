#include "stdafx.h"
#include "Updater.h"
#include "FileHandler.h"
#include "Urlmon.h"
#include "curl-7.48.0\include\curl\curl.h"

std::wstring FileHandler::GetPath(PATH_ENUM en) {
  switch (en) {
  case PATH_CHROMEEXE:
    return emptyString;
    break;
  case PATH_SAVEFOLDER:
    return std::wstring(GetPath(PATH_APPDATALOCAL) + L"\\ChromiumUpdater");
    break;
  case PATH_LATESTINSTALLEREXE:
    return emptyString;
    break;
  case PATH_TEMPDIR:
    wchar_t tempDirBuffer[256];
    GetEnvironmentVariable(L"TMP", tempDirBuffer, 256);
    return tempDirBuffer;
    break;
  case PATH_APPDATALOCAL:
    wchar_t appDataLocalBuffer[256];
    GetEnvironmentVariable(L"LOCALAPPDATA", appDataLocalBuffer, 256);
    return appDataLocalBuffer;
    break;
  case PATH_LATESTLOOKUPURL:
    return L"https://www.googleapis.com/download/storage/v1/b/chromium-browser-snapshots/o/Win%2FLAST_CHANGE?alt=media";
    break;
  case PATH_LATESTDOWNLOADURL:
    GetFile(FILE_LATEST);
    return std::wstring(emptyString + L"https://www.googleapis.com/download/storage/v1/b/chromium-browser-snapshots/o/Win%2F" + ReadFile(FILE_LATEST) + L"%2Fmini_installer.exe?alt=media");
  case PATH_LATESTTXT:
    return GetPath(PATH_SAVEFOLDER) + L"\\latest.txt";
    break;
  case PATH_CURRENTTXT:
    return GetPath(PATH_SAVEFOLDER) + L"\\currentVersion.txt";
    break;
  case PATH_CHROMEDIR:
    TCHAR name[256 + 1];
    DWORD size = 256 + 1;
    GetUserNameW((TCHAR*)name, &size);
    return std::wstring(emptyString + L"C:\\Users\\" + name + L"\\AppData\\Local\\Chromium\\Application");
  }
  return emptyString;
}

std::wstring FileHandler::ReadFile(FILE_ENUM en) {
  switch (en) {
  case FILE_LATEST:
    return std::wstring((std::istreambuf_iterator<wchar_t>(std::wifstream(GetPath(PATH_LATESTTXT).c_str()))), std::istreambuf_iterator<wchar_t>());
    break;
  case FILE_CURR:
    return std::wstring((std::istreambuf_iterator<wchar_t>(std::wifstream(GetPath(PATH_CURRENTTXT)))), std::istreambuf_iterator<wchar_t>());
    break;
  }
  return emptyString;
}

bool FileHandler::GetFile(FILE_ENUM en) {
  switch (en) {
  case FILE_LATEST:
    URLDownloadToFile(0, GetPath(PATH_LATESTLOOKUPURL).c_str(), GetPath(PATH_LATESTTXT).c_str(), 0, 0);
    return true;
    break;
  }
  return false;
}

bool FileHandler::FolderStructureExists(void) {
  DWORD dwAttrib = GetFileAttributes(GetPath(PATH_SAVEFOLDER).c_str());
  return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool FileHandler::CreateFolderStructure(void) {
  if (FolderStructureExists()) {
    return false;
  }
  return CreateDirectory(GetPath(PATH_SAVEFOLDER).c_str(), NULL) != NULL;
}

bool FileHandler::InitializeFolder(void) {
  if (FolderStructureExists()) {
    return true;
  }
  return CreateFolderStructure();
}


bool FileHandler::SaveToFile(FILE_ENUM en, std::wstring &content) {
  switch (en) {
  case FILE_CURR:
    std::wofstream currentVersionFile(std::wstring(GetPath(PATH_SAVEFOLDER) + L"\\currentVersion.txt"));
    currentVersionFile << downloadedFile.substr(0, downloadedFile.find_first_of(L"."));
    currentVersionFile.close();
    return true;
  }
  return false;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
  size_t written = fwrite(ptr, size, nmemb, stream);
  return written;
}

std::string wtoc(const wchar_t *wchar) {
  char charBuff[256];
  wcstombs_s(nullptr, charBuff, wchar, 255);
  return std::string(charBuff);
}

bool FileHandler::GetFileFromWebWithConfirmation(const wchar_t *downloadUrl, const wchar_t *fileName) {
  CURL *curl = nullptr;
  FILE *fp = nullptr;
  CURLcode res;
  std::string cPath = wtoc(downloadUrl);
  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl = curl_easy_init();
  if (curl) {
    _wfopen_s(&fp, fileName, L"wb");
    curl_easy_setopt(curl, CURLOPT_URL, cPath.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    fclose(fp);
  }
  curl_global_cleanup();
  return (res == CURLE_OK);
}