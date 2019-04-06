ip:
	gcc -g -Wall showip.c -o showip

tcp:
	gcc -g -Wall server.c -o server
	gcc -g -Wall client.c -o client

udp:
	gcc -g -Wall listener.c -o listener
	gcc -g -Wall talker.c -o talker

chat:
	gcc -g -Wall chatserver.c -o chatserver

files:
	gcc -g -Wall send_file.c md5sum/md5.c -o send_file
	gcc -g -Wall recv_file.c md5sum/md5.c -o recv_file
md5:
	gcc -g -Wall md5sum/md5.c md5sum/test.c -o md5

md5check:
	gcc -g -Wall md5sum/md5.c md5sum/main.c -o md5check

http:
	gcc -g -Wall http.c -o http 
