set root="C:\Build\SlaveTatsGUI"
set dest="F:\Games\Skyrim-1.6.1170"
rem set dest="D:\Games\Skyrim-1.5.97"
set configsrc=%root%\config
rem set pluginsrc=%root%\build\debug-msvc
set pluginsrc=%root%\build\release-msvc
set plugindest=%dest%\overwrite
copy %pluginsrc%\*.dll %plugindest%\SKSE\Plugins\
copy %pluginsrc%\*.pdb %plugindest%\SKSE\Plugins\
copy %configsrc%\*.json %plugindest%\SKSE\Plugins\SlaveTatsNG
