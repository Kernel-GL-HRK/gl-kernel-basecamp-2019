DIR=guesanumber
NAME_TAR=Home.tar.gz

if [ ! -d "/tmp/guesanumber" ]
then
	mkdir /tmp/"$DIR"
fi

cp *.c /tmp/"$DIR"
tar -cvzf /tmp/"$NAME_TAR" /tmp/"$DIR"
CUR_DIR=`pwd`
cp /tmp/"$NAME_TAR" "$CUR_DIR"/
