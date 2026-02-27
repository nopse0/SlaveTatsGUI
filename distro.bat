set root="C:\Build\SlaveTatsGUI\SlaveTatsGUI"
set pluginsrc=%root%\build\release-msvc
set configsrc=%root%\config
set dist=%root%\dist\SlaveTatsGUI
mkdir %dist%
mkdir %dist%\SKSE
mkdir %dist%\SKSE\Plugins
mkdir %dist%\SKSE\Plugins\SlaveTatsNG
set plugindest=%dist%\SKSE\Plugins
copy %pluginsrc%\*.dll %plugindest%
copy %pluginsrc%\*.pdb %plugindest%
copy %configsrc%\*.json %plugindest%\SlaveTatsNG
