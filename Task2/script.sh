!#/bin/bash

mkdir /tmp/guess_a_number

find /home/lexx/gl-kernel-basecamp-2019/Task1 -name "*.c" -exec cp {} /tmp/guess_a_number \;

cd /tmp/guess_a_number

tar -cvzf /tmp/guess_a_number/arc.tgz *

mv /tmp/guess_a_number/arc.tgz /home/lexx/gl-kernel-basecamp-2019/Task1/Release/
