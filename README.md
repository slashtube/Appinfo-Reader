# Appinfo-Reader
C Tool for reading the appinfo.vdf file used by Valve.  
The file is located in the `appcache` directory found in the Steam folder.

The tool currently only supports version 41 of the file.
## Usage
```
git clone https://github.com/slashtube/Appinfo-Reader.git
cd Appinfo-Reader/
make
```
You can now either move the `appinfo.vdf` file in the current directory or move the `getinfo` executable in the `appcache` folder and then execute the
program using
```
./getinfo appinfo.vdf > out.txt
```
