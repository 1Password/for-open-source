@echo off
REM build and cache boost on Windows N.B. if no output for 10 minutes it's killed by travis. 
if not exist "C:/local/boost/libs" (
        echo "Boost not cached, compiling it"
		wget -q -O boost.exe https://versaweb.dl.sourceforge.net/project/boost/boost-binaries/1.71.0/boost_1_71_0-msvc-14.1-64.exe
		echo "Boost downloaded"
		boost.exe /SILENT /DIR=C:\local\boost 
		echo "Boost installed"
) else echo "Boost already installed"