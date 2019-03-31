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
	gcc -g -Wall send_file.c -o send_file
	gcc -g -Wall recv_file.c -o recv_file
