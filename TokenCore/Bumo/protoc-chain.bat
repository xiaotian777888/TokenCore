@echo off
protoc.exe -I=. --cpp_out=./ chain.proto
pause
