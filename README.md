The program has 2 part. Unreal engine is the client program. The server program is Witten in C with UDP sockets. Make sure to connect to same network incase you are running this locally. 

#server
you will find the server program under server directory. CD into the folder and run the following commands
-> gcc server -o server
-> ./server <server_ip> <server_port> <client_ip>

The server should be connected to rasberry pi. Connect the appropriate pins. The arduino program can be  found under arduino/concatenated. 

#client 

Download the unreal engine program then enter server and port information. The client program will start to receive data from sensors, hence control the simulation.