Name "agio"
Outfile "agio_setup.exe"

InstallDir "$PROGRAMFILES\agio"
VIAddVersionKey "CompanyName" "Soremed"
VIAddVersionKey "FileDescription" "Agio calc"
VIAddVersionKey "FileVersion" "1.0.0"
VIAddVersionKey "ProductName" "agio"
VIProductVersion "1.0.0.0"
VIAddVersionKey "LegalCopyright" "Copyright (C) 2023 Soremed"

!include LogicLib.nsh

Function CheckGTKInstalled
    ReadRegStr $0 HKLM "SOFTWARE\Gtk" ""
    ReadRegStr $1 HKLM "SOFTWARE\Wow6432Node\Gtk" ""

    ${If} $0 != ""
        StrCpy $1 1
    ${ElseIf} $1 != ""
        StrCpy $1 1
    ${Else}
        StrCpy $1 0
    ${EndIf}
FunctionEnd

Section
    Call CheckGTKInstalled

    ${If} $1 == 0
        DetailPrint "GTK3 not found. Installing GTK3..."
        SetOutPath "$INSTDIR\dep"
        File "dep\gtk3-runtime-3.24.31-2022-01-04-ts-win64.exe"
        ExecWait "$INSTDIR\dep\gtk3-runtime-3.24.31-2022-01-04-ts-win64.exe"
    ${Else}
        DetailPrint "GTK3 is already installed."
    ${EndIf}

    CreateDirectory "$LOCALAPPDATA\agio"
    CreateDirectory "$LOCALAPPDATA\agio\data"
    CreateDirectory "$LOCALAPPDATA\agio\data\COPHADIS"
    CreateDirectory "$LOCALAPPDATA\agio\data\CPRE"
    CreateDirectory "$LOCALAPPDATA\agio\data\GDIS"
    CreateDirectory "$LOCALAPPDATA\agio\data\GPM"
    CreateDirectory "$LOCALAPPDATA\agio\data\Lodimed"
    CreateDirectory "$LOCALAPPDATA\agio\data\Soremed"

    SetOutPath "$LOCALAPPDATA\agio\data"
    File "data\logo.png"
    SetOutPath "$LOCALAPPDATA\agio\data\COPHADIS"
    File "data\COPHADIS\logo.png"
    File "data\COPHADIS\module.json"
    SetOutPath "$LOCALAPPDATA\agio\data\CPRE"
    File "data\CPRE\logo.png"
    File "data\CPRE\module.json"
    SetOutPath "$LOCALAPPDATA\agio\data\GDIS"
    File "data\GDIS\logo.png"
    File "data\GDIS\module.json"
    SetOutPath "$LOCALAPPDATA\agio\data\GPM"
    File "data\GPM\logo.png"
    File "data\GPM\module.json"
    SetOutPath "$LOCALAPPDATA\agio\data\Lodimed"
    File "data\Lodimed\logo.png"
    File "data\Lodimed\module.json"
    SetOutPath "$LOCALAPPDATA\agio\data\Soremed"
    File "data\Soremed\logo.png"
    File "data\Soremed\module.json"

    SetOutPath "$INSTDIR"
    File "bin\agio.exe"
    File "dll\libxlsxio_read.dll"
    File "dll\libxlsxio_write.dll"
    File "dll\libxlsxwriter.dll"
    File "dll\libzip.dll"
    File "dll\libzstd.dll"
    File "dll\msys-2.0.dll"
    File "dll\msys-z.dll"
    File "dll\libjansson-4.dll"
    File "data\logo\logo.ico"

    CreateShortcut "$SMPROGRAMS\agio.lnk" "$INSTDIR\agio.exe" "" "$INSTDIR\agio.exe"
    CreateShortcut "$DESKTOP\agio.lnk" "$INSTDIR\agio.exe" "" "$INSTDIR\agio.exe"
    CreateShortCut "$DESKTOP\agio.lnk" "$INSTDIR\agio.exe" "" "$INSTDIR\logo.ico"
    WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd

Section "Uninstall"
    Delete "$INSTDIR\agio.exe"
    Delete "$SMPROGRAMS\agio.lnk"
    Delete "$DESKTOP\agio.lnk"
    Delete "$INSTDIR\Uninstall.exe"

    RMDir /r "$APPDATA\agio"
    RMDir /r "$INSTDIR"
SectionEnd
