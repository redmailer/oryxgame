@echo off

echo start

set PROTOC_EXE=.\protocolbuf-windows\bin\protoc

set SOURCE_PATH=.\pbtxt

set OUTPUT_PATH=.\pb

del %OUTPUT_PATH%\*.* /f /s /q

for /f "delims=" %%a in ('dir  /a-d/b %SOURCE_PATH%\*.proto' ) do (
	echo %PROTOC_EXE% --cpp_out=%OUTPUT_PATH% --proto_path=%SOURCE_PATH% %SOURCE_PATH%\%%a
	%PROTOC_EXE% --cpp_out=%OUTPUT_PATH% --proto_path=%SOURCE_PATH% %SOURCE_PATH%\%%a
)

echo.
echo.
echo.
echo done
pause