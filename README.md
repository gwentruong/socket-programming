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

* Transfer files from clients to server
```
make files
./recv_file
./send_file <hostname> <file/path>
```

* MD5 hashing

String to hash
```
make md5
./md5 <string-to-hash> [<string> [...] ]
```

md5sum implementation
```
make md5check
./md5check <file> [<file> [...] ]
```

***
**References**
1. [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
2. [Ron Rivest, modified by Jim Kingdon, MD5 implementation, 1997, Cyclic Software](https://opensource.apple.com/source/cvs/cvs-27/cvs/lib/md5.c)
