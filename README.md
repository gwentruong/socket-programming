# socket-programming
## Follow Beej's tutorial on network programming

**To compile and run**

* Show IP address of a domain name
```
make ip
./showip <domain.name>
```

* TCP server and client
```
make tcp
./server
./client <hostname>
```

* UDP listener and talker
```
make udp
./listener
./talker <hostname> <message>
```

* Multiperson chat server
```
make chat
./chatserver
```

***
**References**
1. [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
