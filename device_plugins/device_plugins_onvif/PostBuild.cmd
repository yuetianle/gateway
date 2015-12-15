RD /S /Q %1%2
if not exist %1%2 (mkdir -v -p %1%2\python27\{Lib,Dlls})
xcopy /cvEYi %3* %1%2
xcopy /cvEYi %4* %1%2\python27\Lib
xcopy /cvEYi %4..\Dlls %1%2\python27\Dlls 
@rem del %1device_plugins_onvif*%5.dll
xcopy /cvYi %1device_plugins_onvif*%5.dll				%1%2
