!include "Version.txt"

; keyfinder.nsi
;
; This script installs KeyFinder in Program Files, remembers the directory, 
; has uninstall support and (optionally) installs start menu & desktop shortcuts.
;

;--------------------------------

; The name of the installer
Name "KeyFinder ${VERSION}"

; The file to write
OutFile "KeyFinder-${VERSION}.exe"

; The default installation directory
InstallDir $PROGRAMFILES\KeyFinder

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\KeyFinder" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------

; Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "KeyFinder (required)"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "avcodec-53.dll"
  File "avformat-53.dll"
  File "avutil-51.dll"
  File "COPYING.txt"
  File "KeyFinder.exe"
  File "keyfinder0.dll"
  File "libfftw3-3.dll"
  File "libgcc_s_dw2-1.dll"
  File "libsamplerate-0.dll"
  File "libtag.dll"
  File "mingwm10.dll"
  File "QtCore4.dll"
  File "QtGui4.dll"
  File "QtNetwork4.dll"
  File "QtXmlPatterns4.dll"
  File "vcredist_x86.exe"

  ExecWait '"$INSTDIR\vcredist_x86" /passive /norestart' 

  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\KeyFinder "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KeyFinder" "DisplayName" "KeyFinder"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KeyFinder" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KeyFinder" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KeyFinder" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\KeyFinder"
  CreateShortCut "$SMPROGRAMS\KeyFinder\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\KeyFinder\KeyFinder.lnk" "$INSTDIR\KeyFinder.exe" "" "$INSTDIR\KeyFinder.exe" 0
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Desktop Shortcut"

  CreateShortCut "$DESKTOP\KeyFinder.lnk" "$INSTDIR\KeyFinder.exe" "" "$INSTDIR\KeyFinder.exe" 0

SectionEnd
;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\KeyFinder"
  DeleteRegKey HKLM SOFTWARE\KeyFinder

  ; Remove files and uninstaller
  Delete $INSTDIR\KeyFinder.exe
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\KeyFinder\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\KeyFinder"
  RMDir "$INSTDIR"

SectionEnd
