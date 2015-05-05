#MDACS: a multithreading and distributed simulator of ant colonies

MDACS is a multithreading and distributed simulator of ant colonies.

##Goal
 The goal of
this program is to simulate transporting and clustering objects by an ant
colony. It consists of representing an ant colony dropping and picking different types
of objects on heaps depending on their density. Ants will move and explore an area
represented as a 2D grid.

##How it works
* initialization of objects, ants and grid is done randomly by the server
* both server and clients are multithreading
* number of threads and ants per client is randomly decided by server
* server synchronizes clients
* clients send their ants positions to server. Server can accept or not
* client(s) handle(s) transporting and clustering of ants given by the server
* three different configurations are provided: easy, hard, and extreme


##Features
* chance to execute the program on a single computer or on multiple computers
connected to a network
* clients can be on the same server's computer or on multiple computers

##Requirements
* Linux distribution
* GLUT is required for the GUI

##GUI
A very basic GUI is used for showing colony's progress. Thanks to [Guillaume
Perez](http://www.i3s.unice.fr/~gperez/) for his GLUT based library.


##Usage
Clone the repository on all computers you want to use:
```
$ git clone --recursive git@github.com:lukesmolo/MDACS.git
```
Go to _src_ and choose which configuration to use by copying and renaming the _Config_ file you want to use in Config.h.

For example:
```
$ cd src
$ cp Config.h.hard Config.h
```
####If you want to use just one computer for both server and clients
Compile the program:
```
$ make
```

Open a terminal for each client and run the server and clients in different
terminals.<br>
For example, on terminal 1:
```
$ ./main
```
On terminal 2 and 3:
```
$ ./client
```

If you use rxvt-unicode, you can just use:
```
$ make run
```
####If you want to use more computers
On each client modify the **DEFAULT_SERVER_ADDRESS**  macro in Config.h, inserting the server
IP address.<br>
For example:
```
#define DEFAULT_SERVER_ADDRESS "192.168.1.18"
```
Compile the program on each computer involved:
```
$ make
```
Run server on the server computer:
```
$ ./main
```
Run client on each client computer:
```
$ ./client
```

Of course, you can have more clients on the same computer. Just use more
terminals. <br>
If you modify Config.h file, please clean all and compile again:
```
$ make clean
$ make
```
##Warning
Sometimes the server or a client seems not working. This is actually caused by the
random distribution function that spreads in a very bad way threads/ants. Please, just restart the program and hopefully
this time everything will be ok.

##Credits
__libEngine.a__, the GLUT based library used in this project for GUI, was written
by [Guillaume Perez](http://www.i3s.unice.fr/~gperez/). Very soon his code will
be available.
##License
MDACS is released under the GPLv2 license.



##TODO
* improve client's ant algorithm for deciding how to move ants











