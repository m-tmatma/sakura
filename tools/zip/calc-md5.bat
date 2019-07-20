set TARGETFILE=%1
set CMD_FIND=%SystemRoot%\System32\find.exe
certutil -hashfile %TARGETFILE% MD5  | %CMD_FIND% /v "MD5" | %CMD_FIND% /v "CertUtil" > %TARGETFILE%.md5
