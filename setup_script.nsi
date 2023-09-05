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

    SetOutPath "$LOCALAPPDATA\agio\data"
    File "data\logo\logo.png"
    File "data\logo\COPHADIS"
    File "data\logo\CPRE"
    File "data\logo\GDIS"
    File "data\logo\GPM"
    File "data\logo\Lodimed"
    File "data\logo\Soremed"
    File "data\module.json"

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
