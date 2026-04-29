Option Explicit
Dim ws,batScriptFile
Set ws = WScript.CreateObject("Wscript.Shell")
batScriptFile = "C:/CooCox/CoIDE/bin\start_gdbserver.bat"
ws.run Chr(34)&batScriptFile&Chr(34),0,false
