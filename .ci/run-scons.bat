C:/Python27/python.exe -m pip install --upgrade pip distribute setuptools wheel
C:/Python27/python.exe -m pip install "scons==3.0.1"
cd build/scons
scons develop_all -s dist %SCONSMODE% %SCONSOPTS% MODE=RELEASE
cd ../..
