DIR=/tmp/guesanumber
NAME_TAR=/tmp/Home.tar.gz
CUR_DIR=`pwd`
if [ ! -d "$DIR" ]
then
	mkdir "$DIR"
fi

cp *.c "$DIR"
tar -cvzf "$NAME_TAR" "$DIR"

cp "$NAME_TAR" "$CUR_DIR"/
