; Nullsoft NSIS installer script

SetCompressor /FINAL /SOLID lzma
SetCompressorDictSize 32

;--------------------------------
;Include Modern UI
  !include "MUI2.nsh"
  
  
;--------------------------------
;General

  ;Properly display all languages (Installer will not work on Windows 95, 98 or ME!)
  Unicode true

  ;Name and file
  Name "qMasterPassword"
  OutFile "qMasterPassword Setup.exe"

  ;Default installation folder
  InstallDir "$PROGRAMFILES\qMasterPassword"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\qMasterPassword" ""

  ;Request application privileges for Windows Vista
  RequestExecutionLevel admin

  
;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

  ;Show all languages, despite user's codepage
  !define MUI_LANGDLL_ALLLANGUAGES

  
;--------------------------------
;Language Selection Dialog Settings

  ;Remember the installer language
  !define MUI_LANGDLL_REGISTRY_ROOT "HKCU" 
  !define MUI_LANGDLL_REGISTRY_KEY "Software\qMasterPassword" 
  !define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"

  
;--------------------------------
;Pages

;  !insertmacro MUI_PAGE_LICENSE "${NSISDIR}\Docs\Modern UI\License.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages

  !insertmacro MUI_LANGUAGE "English" ;first language is the default language
  !insertmacro MUI_LANGUAGE "French"
  !insertmacro MUI_LANGUAGE "German"
  !insertmacro MUI_LANGUAGE "Spanish"
  !insertmacro MUI_LANGUAGE "SpanishInternational"
  !insertmacro MUI_LANGUAGE "SimpChinese"
  !insertmacro MUI_LANGUAGE "TradChinese"
  !insertmacro MUI_LANGUAGE "Japanese"
  !insertmacro MUI_LANGUAGE "Korean"
  !insertmacro MUI_LANGUAGE "Italian"
  !insertmacro MUI_LANGUAGE "Dutch"
  !insertmacro MUI_LANGUAGE "Danish"
  !insertmacro MUI_LANGUAGE "Swedish"
  !insertmacro MUI_LANGUAGE "Norwegian"
  !insertmacro MUI_LANGUAGE "NorwegianNynorsk"
  !insertmacro MUI_LANGUAGE "Finnish"
  !insertmacro MUI_LANGUAGE "Greek"
  !insertmacro MUI_LANGUAGE "Russian"
  !insertmacro MUI_LANGUAGE "Portuguese"
  !insertmacro MUI_LANGUAGE "PortugueseBR"
  !insertmacro MUI_LANGUAGE "Polish"
  !insertmacro MUI_LANGUAGE "Ukrainian"
  !insertmacro MUI_LANGUAGE "Czech"
  !insertmacro MUI_LANGUAGE "Slovak"
  !insertmacro MUI_LANGUAGE "Croatian"
  !insertmacro MUI_LANGUAGE "Bulgarian"
  !insertmacro MUI_LANGUAGE "Hungarian"
  !insertmacro MUI_LANGUAGE "Thai"
  !insertmacro MUI_LANGUAGE "Romanian"
  !insertmacro MUI_LANGUAGE "Latvian"
  !insertmacro MUI_LANGUAGE "Macedonian"
  !insertmacro MUI_LANGUAGE "Estonian"
  !insertmacro MUI_LANGUAGE "Turkish"
  !insertmacro MUI_LANGUAGE "Lithuanian"
  !insertmacro MUI_LANGUAGE "Slovenian"
  !insertmacro MUI_LANGUAGE "Serbian"
  !insertmacro MUI_LANGUAGE "SerbianLatin"
  !insertmacro MUI_LANGUAGE "Arabic"
  !insertmacro MUI_LANGUAGE "Farsi"
  !insertmacro MUI_LANGUAGE "Hebrew"
  !insertmacro MUI_LANGUAGE "Indonesian"
  !insertmacro MUI_LANGUAGE "Mongolian"
  !insertmacro MUI_LANGUAGE "Luxembourgish"
  !insertmacro MUI_LANGUAGE "Albanian"
  !insertmacro MUI_LANGUAGE "Breton"
  !insertmacro MUI_LANGUAGE "Belarusian"
  !insertmacro MUI_LANGUAGE "Icelandic"
  !insertmacro MUI_LANGUAGE "Malay"
  !insertmacro MUI_LANGUAGE "Bosnian"
  !insertmacro MUI_LANGUAGE "Kurdish"
  !insertmacro MUI_LANGUAGE "Irish"
  !insertmacro MUI_LANGUAGE "Uzbek"
  !insertmacro MUI_LANGUAGE "Galician"
  !insertmacro MUI_LANGUAGE "Afrikaans"
  !insertmacro MUI_LANGUAGE "Catalan"
  !insertmacro MUI_LANGUAGE "Esperanto"
  !insertmacro MUI_LANGUAGE "Asturian"
  !insertmacro MUI_LANGUAGE "Basque"
  !insertmacro MUI_LANGUAGE "Pashto"
  !insertmacro MUI_LANGUAGE "ScotsGaelic"
  !insertmacro MUI_LANGUAGE "Georgian"
  !insertmacro MUI_LANGUAGE "Vietnamese"
  !insertmacro MUI_LANGUAGE "Welsh"
  !insertmacro MUI_LANGUAGE "Armenian"

;--------------------------------
;Reserve Files
  
  ;If you are using solid compression, files that are required before
  ;the actual installation should be stored first in the data block,
  ;because this will make your installer start faster.
  
  !insertmacro MUI_RESERVEFILE_LANGDLL

;--------------------------------
;Installer Sections

Section "Executables" SecExecutables
  
  SectionIn RO
  
  ; Vistual Studio C++ Redistributable
  SetOutPath "$INSTDIR" 
  File "windows_files\vcredist_x86_visual_studio_2013.exe" 	
  ExecWait '"$INSTDIR\vcredist_x86_visual_studio_2013.exe"  /passive /norestart'	
  Delete "$INSTDIR\vcredist_x86_visual_studio_2013.exe"
  
  SetOutPath "$INSTDIR"
  
  File "windows_files\qMasterPassword.exe"
  
  ; Qt files (from C:\Qt\5.4\mingw491_32)
  File "windows_files\icudt53.dll"
  File "windows_files\icuin53.dll"
  File "windows_files\icuuc53.dll"
  File "windows_files\libeay32.dll"
  File "windows_files\qt.conf"
  File "windows_files\Qt5Core.dll"
  File "windows_files\Qt5Gui.dll"
  File "windows_files\Qt5Widgets.dll"
  File "windows_files\Qt5Svg.dll"
  File "windows_files\libgcc_s_dw2-1.dll"
  File "windows_files\libstdc++-6.dll"
  File "windows_files\libwinpthread-1.dll"
  
  SetOutPath "$INSTDIR\plugins\platforms"
  File "windows_files\plugins\platforms\qminimal.dll"
  File "windows_files\plugins\platforms\qoffscreen.dll"
  File "windows_files\plugins\platforms\qwindows.dll"
  
  SetOutPath "$INSTDIR\plugins\imageformats"
  File "windows_files\plugins\imageformats\qico.dll"
  File "windows_files\plugins\imageformats\qjpeg.dll"
  File "windows_files\plugins\imageformats\qsvg.dll"
  
  
  ;Store installation folder
  WriteRegStr HKCU "Software\qMasterPassword" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd

Section "Start Menu Shortcuts" SecShortcuts

  CreateDirectory "$SMPROGRAMS\qMasterPassword"
  CreateShortcut "$SMPROGRAMS\qMasterPassword\Uninstall.lnk" "$INSTDIR\Uninstall.exe" "" "$INSTDIR\Uninstall.exe" 0
  CreateShortcut "$SMPROGRAMS\qMasterPassword\qMasterPassword.lnk" "$INSTDIR\qMasterPassword.exe" "" "$INSTDIR\qMasterPassword.exe" 0
  
SectionEnd

Section "Desktop Shortcut" SecDesktopShortcut

  CreateShortcut "$DESKTOP\qMasterPassword.lnk" "$INSTDIR\qMasterPassword.exe"
  
SectionEnd

;--------------------------------
;Installer Functions

Function .onInit

  !insertmacro MUI_LANGDLL_DISPLAY

FunctionEnd

;--------------------------------
;Descriptions

  ;USE A LANGUAGE STRING IF YOU WANT YOUR DESCRIPTIONS TO BE LANGAUGE SPECIFIC

  ;Assign descriptions to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecExecutables} "Executable files"
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

 
;--------------------------------
;Uninstaller Section

Section "Uninstall"

  Delete "$INSTDIR\qMasterPassword.exe"
  Delete "$INSTDIR\icudt53.dll"
  Delete "$INSTDIR\icuin53.dll"
  Delete "$INSTDIR\icuuc53.dll"
  Delete "$INSTDIR\libeay32.dll"
  Delete "$INSTDIR\qt.conf"
  Delete "$INSTDIR\Qt5Core.dll"
  Delete "$INSTDIR\Qt5Gui.dll"
  Delete "$INSTDIR\Qt5Widgets.dll"
  Delete "$INSTDIR\Qt5Svg.dll"
  Delete "$INSTDIR\libgcc_s_dw2-1.dll"
  Delete "$INSTDIR\libstdc++-6.dll"
  Delete "$INSTDIR\libwinpthread-1.dll"
  Delete "$INSTDIR\plugins\platforms\qminimal.dll"
  Delete "$INSTDIR\plugins\platforms\qoffscreen.dll"
  Delete "$INSTDIR\plugins\platforms\qwindows.dll"
  RMDir "$INSTDIR\plugins\platforms"
  Delete "$INSTDIR\plugins\imageformats\qico.dll"
  Delete "$INSTDIR\plugins\imageformats\qjpeg.dll"
  Delete "$INSTDIR\plugins\imageformats\qsvg.dll"
  RMDir "$INSTDIR\plugins\imageformats"
  RMDir "$INSTDIR\plugins"
  
  Delete "$DESKTOP\qMasterPassword.lnk"
  
  Delete "$SMPROGRAMS\qMasterPassword\Uninstall.lnk"
  Delete "$SMPROGRAMS\qMasterPassword\qMasterPassword.lnk"
  RMDir "$SMPROGRAMS\qMasterPassword"

  Delete "$INSTDIR\Uninstall.exe"

  RMDir "$INSTDIR"

  DeleteRegKey /ifempty HKCU "Software\qMasterPassword"

SectionEnd

;--------------------------------
;Uninstaller Functions

Function un.onInit

  !insertmacro MUI_UNGETLANGUAGE
  
FunctionEnd