; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "ELOAD"
!include "VersionNo.nsi"
!define PRODUCT_PUBLISHER "Daelim (C)"
!define PRODUCT_WEB_SITE "http://www.9sq.co.kr"
!define PRODUCT_DIR_REGKEY "Software\${PRODUCT_NAME}"
!define PRODUCT_UNINST_KEY "Software\${PRODUCT_NAME}\Uninstall"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; MUI 1.67 compatible ------
!include "MUI.nsh"
!include "Sections.nsh"
!include "zipdll.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "Daelim.bmp" ; optional
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

Var projectSelected

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "Setup ELoad_NO_DATABASE_${PRODUCT_VERSION}.exe"
InstallDir "$PROGRAMFILES\ELOAD"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""

; Welcome page
!insertmacro MUI_PAGE_WELCOME

; Directory page
!insertmacro MUI_PAGE_DIRECTORY

; Component를 선택할 수 있도록 한다.
Page Components

; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\ELoad.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "Korean"

BrandingText "Daelim(C)"

; MUI end ------

ShowInstDetails show
ShowUnInstDetails show

Section "ELOAD" SEC01
  SectionIn RO

  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File "ELoad.exe"
  File "w9xpopen.exe"
  File "python26.zip"
  File "*.pyd"
  File "FPSPR70.ocx"
  File "AutoUp.ini"
  File "uicfg.xml"
  File "readme.txt"
  
  SetOutPath "$INSTDIR\Backup"
  File "Backup\*.*"
  
  SetOutPath "$INSTDIR\python"
  File "python\*.py"

  SetOutPath "$INSTDIR\Setting"
  File "Setting\ELoad.ini"
  File "Setting\*.TBL"
  File "Setting\LOAD.PROP_GROUP"
  File "Setting\LOAD.ITEM"
  File "Setting\*.PROP"
  File "Setting\ERROR_CODE.DAT"
  File "Setting\SYSTEM.BUSGROUP"  ; 2011.02.10
  
  SetOutPath "$INSTDIR\Excel Format"
  File "Excel Format\*.xls"
    
  SetOutPath "$INSTDIR\images"
  File "images\*.bmp"
  
  SetOutPath "$INSTDIR\AutoUp"
  Rename "$INSTDIR\Setting\Training.ini" "$INSTDIR\Setting\ELoad.ini"

  ;2010.12.14
  SetOutPath "$INSTDIR\Manual"
  File "Manual\*.pdf"
  
  CreateDirectory "$INSTDIR\temp"
  CreateDirectory "$SMPROGRAMS\e-LOAD"
  CreateShortCut "$SMPROGRAMS\e-LOAD\e-LOAD v${PRODUCT_VERSION}.lnk" "$INSTDIR\ELoad.exe"
  CreateShortCut "$DESKTOP\ELOAD  v${PRODUCT_VERSION}.lnk" "$INSTDIR\ELoad.exe"
SectionEnd

Section "DLL" SEC02
  SectionIn RO
        
  SetOutPath "$INSTDIR"
  File "AutoUp.dll"
  File "isgui_vc90.dll"
  File "isutil_vc90.dll"
  File "IsUtil2008.dll"
  File "log4cxx.dll"
  File "cximagecrt.dll"
  File "GdiPlus.dll"
  File "mfc90.dll"
  File "msado15.dll"
  File "msvcp90.dll"
  File "msvcr90.dll"
  File "msvcrt.dll"
  File "python26.dll"
  File "pythoncom26.dll"
  File "pywintypes26.dll"
  File "WinGraphviz.dll"
  File "Zip.dll"
  File "Zlib1.dll"
SectionEnd

Section "PROJECT" SEC03
        SectionIn 1
        
        SetOutPath "$INSTDIR\PROJECT"
        File "Backup\PROJECT\PROJECT.zip"
        
        StrCpy $projectSelected "1"
SectionEnd

Section "VC++ 2008 Redistributable" SEC04
        SectionIn 2

        SetOutPath "$INSTDIR\Temp"
        File "vcredist_x86_sp1.exe"

        StrCpy $projectSelected "2"
SectionEnd

Section -AdditionalIcons
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\e-LOAD\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\e-LOAD\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

; 설치하고 난 후...
Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "File"    "$INSTDIR\ELoad.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "Version" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\ELoad.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  
#  StrCmp $projectSelected "" no_project
  ; Zip 파일을 Extract한다.
  IfFileExists $INSTDIR\PROJECT\PROJECT.zip 0 +5
        Delete "$INSTDIR\PROJECT\ELOAD-BACKUP.MDB"
        Rename $INSTDIR\PROJECT\ELOAD.MDB $INSTDIR\PROJECT\ELOAD-BACKUP.MDB
        CopyFiles $INSTDIR\BACKUP\ELOAD.MDB $INSTDIR\PROJECT ; 2012.09.19 added by humkyung
        ZipDLL::extractall "$INSTDIR\PROJECT\PROJECT.ZIP" "$INSTDIR\PROJECT\000000"
        Delete "$INSTDIR\PROJECT\PROJECT.ZIP"

  ; VC++ 2008 재배포한을 설치한다.
  IfFileExists "$INSTDIR\Temp\vcredist_x86_sp1.exe" 0 no_project
        ExecWait "$INSTDIR\Temp\vcredist_x86_sp1.exe"
        Delete "$INSTDIR\Temp\vcredist_x86_sp1.exe"
no_project:

  RegDll "$INSTDIR\FPSPR70.ocx"
  RegDll "$INSTDIR\WinGraphviz.dll"
  Sleep 1000
  
  ExecShell "open" "$INSTDIR\readme.txt"
SectionEnd

Function .onInit
         ReadRegStr $R0 HKLM "${PRODUCT_UNINST_KEY}" "UninstallString"
         StrCmp $R0 "" done
         
         MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION \
                    "${PRODUCT_NAME} is already installed. $\n$\nClick  `OK` to remove the \
                    previous version or `Cancel` to cancel this upgrade." \
                    IDOK uninst
                    Abort done
                    
uninst:
       ClearErrors
       ; Wait for completing Uninstaller
       ExecWait '"$R0" _?=$INSTDIR'
;       abort   - no more needed
       
       ifErrors no_remove_uninstaller
       no_remove_uninstaller:
done:
     ; SEC03,SEC04을 unselect한다.
     !insertmacro UnselectSection ${SEC03}
     !insertmacro UnselectSection ${SEC04}
FunctionEnd

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name)는(은) 완전히 제거되었습니다."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "$(^Name)을(를) 제거하시겠습니까?" IDYES +2
  Abort
FunctionEnd

; 언인스톨시~~~
Section Uninstall
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  
  UnRegDLL "$INSTDIR\FPSPR70.ocx"
  UnRegDLL "$INSTDIR\WinGraphviz.dll"
  
  Delete "$INSTDIR\*.*"
  
  Delete "$INSTDIR\Excel Format\*.*"
  RMDir "$INSTDIR\Excel Format"
  
  Delete "$INSTDIR\Backup\*.*"
  RMDir "$INSTDIR\Backup"

  Delete "$INSTDIR\Setting\ELoad.ini"
  Delete "$INSTDIR\Setting\*.PROP"
  Delete "$INSTDIR\Setting\*.PROP_GROUP"
  Delete "$INSTDIR\Setting\*.ITEM"
  Delete "$INSTDIR\Setting\*.DAT"
  Delete "$INSTDIR\Setting\*.TBL"
  Delete "$INSTDIR\Setting\*.BUSGROUP"
  RMDir "$INSTDIR\Setting"
  
  Delete "$INSTDIR\python\*.*"
  RMDir "$INSTDIR\python"
  Delete "$INSTDIR\images\*.bmp"
  RMDir "$INSTDIR\images"
  Delete "$INSTDIR\AutoUp\*.*"
  RMDir "$INSTDIR\AutoUp"
  ;2010.12.14
  Delete "$INSTDIR\Manual\*.*"
  RMDir "$INSTDIR\Manual"
  
  Delete "$INSTDIR\temp\*.*"
  RMDir  "$INSTDIR\temp"
  
  Delete "$SMPROGRAMS\e-LOAD\Uninstall.lnk"
  Delete "$SMPROGRAMS\e-LOAD\Website.lnk"
  Delete "$DESKTOP\ELOAD  v${PRODUCT_VERSION}.lnk"
  Delete "$SMPROGRAMS\e-LOAD\e-LOAD v${PRODUCT_VERSION}.lnk"

  RMDir "$SMPROGRAMS\e-LOAD"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd
