#include "stdafx.h"
#include "Updater.h"
#include "FileHandler.h"
#include "Shellapi.h"
#include "Resource.h"

bool ChromiumUpdater::autoUpdateEnabled = false;
bool ChromiumUpdater::quietModeEnabled = false;
bool ChromiumUpdater::safetyLock = false;
std::wstring FileHandler::downloadedFile;

DWORD ChromiumUpdater::loopThreadID = NULL;
HANDLE ChromiumUpdater::loopThreadHandle = NULL;

std::wstring FileHandler::emptyString;
HWND ChromiumUpdater::windowHandle = NULL;


DWORD WINAPI LookupLoop(LPVOID params) {
  std::wstring current;
  std::wstring latest;

  while (true) {
    if (ChromiumUpdater::autoUpdateEnabled) {
      ChromiumUpdater::safetyLock = true;
      if ((current = ChromiumUpdater::LookupCurrentVersion()).compare(latest = ChromiumUpdater::LookupLatestVersion())) {
        if (!ChromiumUpdater::quietModeEnabled) {
          switch (MessageBox(nullptr, (FileHandler::emptyString + L"Install latest Version? Old: " + current + L" - New: " + latest + L"\n \"OK\" to install now. Cancel to ask again in 1 hour.").c_str(), L"Question", MB_OKCANCEL | MB_ICONQUESTION)) {
          case IDOK: {
            if (!ChromiumUpdater::DoUpdate()) {
              MessageBox(nullptr, L"Update Failed.", L"Error", MB_OK | MB_ICONSTOP);
              return 0;
            }
            continue;
          }
          }
        }
        else {
          if (!ChromiumUpdater::DoUpdate()) {
            MessageBox(nullptr, L"Update Failed.", L"Error", MB_OK | MB_ICONSTOP);
            return 0;
          }
          continue;
        }
      }
    }
    else {
      return 0;
    }
    ChromiumUpdater::safetyLock = false;
    Sleep(3600000);
  }
}

bool ChromiumUpdater::DoUpdate(void) {
  return DownloadLatestVersion() && InstallExeInDirectory();
}

bool ChromiumUpdater::DownloadLatestVersion(void) {
  if (!FileHandler::InitializeFolder()) {
    return false;
  }
  FileHandler::GetFile(FILE_LATEST);
  if (!FileHandler::GetFileFromWebWithConfirmation(FileHandler::GetPath(PATH_LATESTDOWNLOADURL).c_str(), (FileHandler::GetPath(PATH_SAVEFOLDER) + L"\\" + FileHandler::ReadFile(FILE_LATEST) + L".exe").c_str())) {
    return false;
  }
  FileHandler::downloadedFile = FileHandler::ReadFile(FILE_LATEST) + L".exe";
  return true;
}

std::wstring ChromiumUpdater::LookupLatestVersion(void) {
  if (!FileHandler::InitializeFolder()) {
    return false;
  }
  FileHandler::GetFile(FILE_LATEST);
  return FileHandler::ReadFile(FILE_LATEST);
}

std::wstring ChromiumUpdater::LookupCurrentVersion(void) {
  if (FileHandler::ReadFile(FILE_CURR).empty()) {
    MessageBox(nullptr, L"Install a new Version of Chromium with this tool to lookup the version in the future. Build number is usually not readable.", L"Information", MB_OK | MB_ICONWARNING);
  }
  return FileHandler::ReadFile(FILE_CURR);
}

bool ChromiumUpdater::InstallExeInDirectory(void) {
  ShellExecute(NULL, LPCWSTR(L"open"), LPCWSTR((FileHandler::GetPath(PATH_SAVEFOLDER) + L"\\" + FileHandler::downloadedFile).c_str()), NULL, NULL, SW_HIDE);
  FileHandler::SaveToFile(FILE_CURR, FileHandler::emptyString);
  return true;
}

bool ChromiumUpdater::DownloadLatestVersionAndHandleButtons(void) {
  bool returnVal = false;
  EnableWindow(GetDlgItem(ChromiumUpdater::windowHandle, IDB_DOWNLOAD_LATEST), false);
  EnableWindow(GetDlgItem(ChromiumUpdater::windowHandle, IDB_INSTALL_LATEST), false);
  if ((returnVal = DownloadLatestVersion())) {
    SetDlgItemTextW(ChromiumUpdater::windowHandle, IDB_INSTALL_LATEST, (FileHandler::ReadFile(FILE_LATEST) + L" ready!").c_str());
    EnableWindow(GetDlgItem(ChromiumUpdater::windowHandle, IDB_DOWNLOAD_LATEST), true);
  }
  EnableWindow(GetDlgItem(ChromiumUpdater::windowHandle, IDB_INSTALL_LATEST), true);
  return returnVal;
}