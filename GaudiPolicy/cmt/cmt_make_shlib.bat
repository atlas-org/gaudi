@echo off

rem #
set bin=..\%1%
set name=%2%

if not exist %bin%\%name%.arc exit

rem # Retreive other arguments in one "tmplinkopts" file :

set tmplinkopts=%bin%\%name%\tmplinkopts.dat

if exist %tmplinkopts% del %tmplinkopts% 

shift 
shift
set empty=''

:next
set x='%1'
IF %x% EQU %empty% goto end 
echo %1 >> %tmplinkopts%
shift
goto next

:end
set empty=
set x=

set indirectlinkopts=
if exist %tmplinkopts% set indirectlinkopts=@%tmplinkopts% 

rem
rem  Different detections for "component" libraries
rem  this is not very clean in the sense that it is linked
rem  to project-specific implementation.
rem   We need to define a better way to specify that option 
rem

if exist %bin%\%name%\%name%_dll.obj goto component
if exist %bin%\%name%\%name%_entries.obj goto componentnew

:linker

%CMTROOT%\%CMTBIN%\cmt.exe build windefs %bin%\%name%.arc >%bin%\%name%.def

lib.exe /nologo /machine:ix86 /def:%bin%\%name%.def /out:%bin%\%name%.lib

if exist %bin%\%name%.dll del %bin%\%name%.dll

link.exe /nologo /dll /out:%bin%\%name%.dll %bin%\%name%.exp %bin%\%name%.arc %indirectlinkopts%

goto return

:component
link.exe /nologo /dll /out:%bin%\%name%.dll %bin%\%name%\*.obj /machine:ix86 %indirectlinkopts%
goto return
 
:componentnew
link.exe /nologo /dll /out:%bin%\%name%.dll %bin%\%name%\*.obj /machine:ix86 %indirectlinkopts%

:return
if exist %tmplinkopts% del %tmplinkopts%
set tmplinkopts=
