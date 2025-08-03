pushd %~dp0

GenProcedure.exe --path=../../GameServer/GameDB.xml --output=GenProcedures.h

IF ERRORLEVEL 1 PAUSE

XCOPY /Y GenProcedures.h "../../GameServer"

FOR %%f IN (*.h) DO (
    IF /I NOT "%%f"=="PacketHandler.h" DEL /F /Q "%%f"
)

PAUSE