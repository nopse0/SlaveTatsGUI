set root="C:\Build\SlaveTatsGUI"
rem set dest="F:\Games\Skyrim-1.6.1170"
set dest="D:\Games\Skyrim-1.5.97"
set configsrc=%root%\config
set pluginsrc=%root%\build\debug-msvc
rem set pluginsrc=%root%\build\release-msvc
set plugindest=%dest%\overwrite
copy %pluginsrc%\*.dll %plugindest%\SKSE\Plugins\
copy %pluginsrc%\*.pdb %plugindest%\SKSE\Plugins\
copy %configsrc%\*.json %plugindest%\SKSE\Plugins\SlaveTatsNG
