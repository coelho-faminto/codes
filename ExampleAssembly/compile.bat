@echo off

set CSCBIN=C:\Windows\Microsoft.NET\Framework\v4.0.30319\csc.exe

%CSCBIN% /target:exe /out:ExampleAssembly.exe /platform:anycpu program.cs