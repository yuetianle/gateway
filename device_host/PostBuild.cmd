@rem RD /S /Q %1%2
@rem if not exist %1%2 (mkdir %1%2)
@rem xcopy /cvEYi %3* %1%2

@rem del %1device_plugins_hikvision*%4.dll
xcopy /cvYi %1device_host*%3.exe				%1%2
