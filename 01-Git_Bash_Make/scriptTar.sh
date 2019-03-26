if [ ! -d "/tmp/guesanumber" ]
then
	mkdir /tmp/guesanumber
fi

cp homework01.c /tmp/guesanumber/
tar -cvzf /tmp/Home.tar.gz /tmp/guesanumber/
