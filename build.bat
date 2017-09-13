
rmdir ./build


cmake.exe -G"Visual Studio 12 2013" -DOPCUASTACK_INSTALL_PREFIX=C:\install -H./src/ -B./build 


set DESTDIR=C:\install
C:\Windows\Microsoft.NET\Framework\v4.0.30319\MSBuild.exe ./build/INSTALL.vcxproj



pause
