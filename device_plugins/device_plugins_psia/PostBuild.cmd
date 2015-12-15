RD /S /Q %1%2
if not exist %1%2 (mkdir %1%2)
xcopy /cvEYi %3* %1%2

@rem del %1device_plugins_psia*%4.dll
xcopy /cvYi %1device_plugins_psia*%4.dll				%1%2
