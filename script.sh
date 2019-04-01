!#/bin/bash
Target_Path = /tmp/guesanumber/
Home_Path = /home/vvi/part1
Release_Path = /home/vvi/part1/release
find $(Home_Path) -name "*.[ch]*" -exec cp {} $(Target_Path)  \; || echo "no files"
tar cfz $(Target_Path)arc.tgz $(Target_Path)*
mv $(Target_Path)arc.tgz $(Release_Path)

