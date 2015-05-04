/*
 * Copyright (C) 2015 Luca Sciullo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <sys/select.h>
#include <vector>
#include <poll.h>

#include "Server.h"

#include "MyGraphicEngine.h"
#include "Engine.h"
#include "MyGameEngine.h"
#include "MyControlEngine.h"
#include "GUI.h"



using namespace std;

//check if threads were assigned at a client with 0 ants assigned
bool
Server::check_ants_per_client() {
	int i = 0;
	bool ret = true;
	for(i = 0; i < n_clients; i++) {
		if(assigned_threads[i] == 0 && assigned_ants[i] > 0 ) {
			ret = false;
			break;
		}
	}
	return ret;
}


//generate random distribution of "what" for clients
int*
Server::random_threads_number(int what) {

	float *number = new float[n_clients];
	int *random = new int[n_clients];
	srand((unsigned)time(NULL));
	float tmp = 0;
	int sum = 0;
	do{
		printf("creating random numbers..\n");
		sum = 0;
		sum = 0;
		for (int i = 0; i < n_clients; i++) {
			number[i] = 1 + rand() % what; //at least one per client

		}
		//normalization of the sum
		for (int i = 0; i < n_clients; i++) {
			tmp += number[i];	
		}
		tmp = what/tmp;
		for (int i = 0; i < n_clients; i++) {
			number[i] = number[i]*tmp+0.5; //add 0.5 to round the float
		}
		for (int i = 0; i < n_clients; i++) {
			sum += (int)number[i];	

		}

	} while(sum != what);
	for (int i = 0; i < n_clients; i++) {
		random[i] = (int)number[i];
	}
	delete[] number;
	return random;

}

//check if server can die
void
Server::check_finish(Server *s) {
	int *a = NULL;
	a = new int;
	*a = 1;
	while(s->has_finished == 0 && s->end_server == 0) {
		sleep(5);
		
		printf("Checking end...\n");
		if(s->n_clients == 0) {
			s->end_server = 1;
		}
		if(s->gui->to_die)
			s->has_finished = s->n_clients; //set the number of clients to whom send the die flag

	}
	pthread_exit(a); //safely closing thread
}


//assign function to thread basing on its index
void*
Server::thread_functions(void *thread_arg) {
	struct thread_struct *serv = (struct thread_struct*) thread_arg;
	Server *s = (Server*)serv->cl;
	int ind = serv->a;
	if(ind == 0) {
		s->main();
	} else {
		
#if !NO_GUI
		check_finish(s);
#endif
	}
	delete serv;
	return NULL;
}

//delete thread after they died
int
Server::delete_thread() {
	int t = 0;
	void *ptr;
	int error;

	for(t = 0; t < server_threads; t++) {

		error = pthread_join(main_threads[t], &ptr);
		if(error != 0){
			printf("pthread_join() failed: error= %d\n", error);
			exit(-1);
		} else {
			printf("thread %d is dying\n", *((int*)ptr));
			free(ptr); /*deallocating structure in which pthread_join returns value*/
		}
	}
	printf("Every thread is dead! Closing..\n");
	exit(0);
	return 1;


}

//create threads
int
Server::create_thread() {
	int t = 0;
	int ris = 0;

	struct thread_struct *arg;
	for(t = 0; t < server_threads; t++) {
		arg = new struct thread_struct;
		arg->a = t;
		arg->cl = (void*)this;
		ris = pthread_create(&main_threads[t], NULL, thread_functions, arg);

		if(ris) {
			printf("ERROR; return code from pthread_create() is %d\n", ris);
			exit(-1);
		}
	}
	return 1;
}

//make a package to send to a client "client"
int
Server::make_package(struct server_mes* s_msg, int client) {

	int i = 0;
	int j = 0;
	int close_conn = 0;
	struct ant_msg tmp;
	std::vector<Ant *> *ants;

#if !NO_GUI
	ants = gui->ants;
#else
	ants = NULL;
#endif
	if(has_finished != 0) { //if server has finished, let's send the flag of death to the client
		s_msg->to_die = 1;
		close_conn = 1;
		if(has_finished == 1)
			end_server = 1;
		 else if(has_finished != 0)
			has_finished--;
	} else {
		s_msg->to_die = 0;
	}
	
	//set values for all the fields of struct msg_ant
	s_msg->n_client = client;
	s_msg->n_threads = assigned_threads[client];
	s_msg->n_ants = assigned_ants[client];
	s_msg->indeces[0] =  indeces_per_client[client][0];
	s_msg->indeces[1] =  indeces_per_client[client][1];

#if !NO_GUI
	if(!ants->empty() && indeces_per_client[client][0] != -1 && indeces_per_client[client][1] != -1) {
		for(j = 0, i = indeces_per_client[client][0] ; j < assigned_ants[client]; i++, j++) {
			tmp.x = (*ants)[i]->pos.nLine;
			tmp.y = (*ants)[i]->pos.ncell;

			tmp.flag = (*ants)[i]->problem;
			memcpy(&(s_msg->pos[i]), &tmp, sizeof(ant_msg));
		}
	}
#endif

	return close_conn;
}

void
Server::unpack_packet(struct client_mes* c_msg) {
	int client = c_msg->n_client;
	int i = 0;
	int j = 0;

	if(client == -1) {
		return; //first message from a client..
	}

	for(j = 0, i = indeces_per_client[client][0] ; j < c_msg->n_ants; i++, j++) {
		//we copy just the new number of positions
#if !NO_GUI
		if(c_msg->pos[i].flag == 1) {

			gui->move_ant(i, c_msg->pos[i].x, c_msg->pos[i].y); //set move_to_cell per each ant
		}
#endif
	}

#if !NO_GUI
	obs->check_ant_positions(indeces_per_client[client][0], assigned_ants[client]); //call observer to see what it needs to be done
#endif

}

//read message from a client
int
Server::read_from_client (int filedes, int client) {
	int res = 0;
	int len = 0;
	int close_conn = 0;
	struct server_mes s_msg;
	char buffer[1024];

	struct client_mes *c_msg = NULL;
	memset(&s_msg, 0, sizeof(server_mes));


	//printf("Descriptor %d is readable\n", filedes);

	res = recv(filedes, buffer, sizeof(buffer), 0); //tcp recv

	c_msg = (client_mes*)buffer;

	if (res < 0) {
		if (errno != EWOULDBLOCK)
		{
			perror("recv() failed");
			close_conn = 1;
			return close_conn;
		}
	}
	if (res == 0){ //connection closed by a client
		printf("Connection closed\n");
		close_conn = 1;
		return close_conn;
	}

	/* Data was received*/
	len = res;
	unpack_packet(c_msg); //read the content of the message
#if VERBOSE
	for(int i = 0; i < assigned_ants[client]; i++) {
		printf("Received from client %d:  (%d,%d)\n", client, c_msg->pos[indeces_per_client[client][0]+i].x, c_msg->pos[indeces_per_client[client][0]+i].y);
	}
#endif
	close_conn = make_package(&s_msg, client); //prepare message for client

	/* Echo the data back to the client*/

	res = send(filedes, &s_msg , sizeof(s_msg), 0); //tcp send
	if (res < 0) {
		perror(" send() failed");
		close_conn = 1;
		return close_conn;
	}


#if VERBOSE
	for(int i = 0; i < assigned_ants[client]; i++) {
		printf("Sent to client %d:  (%d,%d)\n", client, s_msg.pos[indeces_per_client[client][0]+i].x, s_msg.pos[indeces_per_client[client][0]+i].y);
	}

#endif
	return close_conn;	
	

}


int
Server::main() {
	int rc, on = 1;
	int close_conn; //variable used to check if server has to die
	int tmp = 0;
	int *thread_ind = new int;
	*thread_ind = 0; //it is the first thread created;


	struct sockaddr_in   addr;
	int    timeout;
	struct sockaddr_in Cli;

	vector<struct pollfd> fds;
	struct pollfd server_fds;
	struct pollfd tmp_fds;
	int poll_number = 0;
	int i = 0;
	int new_socketfd;
	unsigned int size;
	struct pollfd *fds_pointer;

	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sock < 0)
	{
		perror("socket() failed");
		exit(-1);
	}

	/* Allow socket descriptor to be reuseable                   */
	rc = setsockopt(server_sock, SOL_SOCKET,  SO_REUSEADDR,
			(char *)&on, sizeof(on));
	if (rc < 0)
	{
		perror("setsockopt() failed");
		close(server_sock);
		exit(-1);
	}

	/* Set socket to be nonblocking.*/
	rc = ioctl(server_sock, FIONBIO, (char *)&on);
	if (rc < 0)
	{
		perror("ioctl() failed");
		close(server_sock);
		exit(-1);
	}

	/* Bind the socket                                           */
	memset(&addr, 0, sizeof(addr));
	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);         /* wildcard */
	addr.sin_port        = htons(port);
	rc = bind(server_sock, (struct sockaddr *)&addr, sizeof(addr));
	if (rc < 0) {
		perror("bind() failed");
		close(server_sock);
		exit(-1);
	}

	/* Set the listen back log                                   */
	rc = listen(server_sock, 32);
	if (rc < 0)
	{
		perror("listen() failed");
		close(server_sock);
		exit(-1);
	}

	/* Set up the initial listening socket                        */
	server_fds.fd = server_sock;
	server_fds.events = POLLIN;

	timeout = (3 * 1000); //timeout of 3 seconds for listen

	do {
		printf("Waiting on poll()...\n");
		rc = poll(&server_fds, 1, timeout);

		/* Check to see if the poll call failed.                   */
		if (rc < 0) {
			perror("  poll() failed");
			break;
		}
		if(rc > 0) {
			do {
				memset ( &Cli, 0, sizeof(Cli) );
				size = sizeof(Cli);
				printf ("accept()\n");
				new_socketfd = accept(server_sock, (struct sockaddr*) &Cli, &size);
				if (new_socketfd < 0)
				{
					perror ("accept");
					exit (EXIT_FAILURE);
				}


			} while( (new_socketfd<0)&&(errno==EINTR) );

			tmp_fds.fd = new_socketfd;
			tmp_fds.events = POLLIN;	
			sockets.push_back(new_socketfd);
			fds.push_back(tmp_fds);
			fprintf(stderr,"connection from %s : %d\n", inet_ntoa(Cli.sin_addr), ntohs(Cli.sin_port));
			poll_number++;

		}
		/* Check to see if timeout expired. */
		if (rc == 0) {
			printf("poll() timed out\n");
			break;
		}
	} while (rc != 0); /* End of initial phase */


	n_clients = poll_number;
	if(!n_clients) { //if there are not clients, just close the server
		end_server = 1;
		printf("No clients connected\n");
	} else {

		assigned_threads = random_threads_number(client_threads); //assign a random number of threads per each client

		do {
			if(assigned_ants != NULL)
				delete[] assigned_ants;
			assigned_ants = random_threads_number(n_ants);
		}while(!check_ants_per_client()); //check that a client has threads if some ants were assigned to it
		indeces_per_client = new int*[n_clients];

		for(int j = 0; j < n_clients; j++) {
			if(assigned_ants[j] > 0) {
			indeces_per_client[j] = new int[2]; //two indeces for client
			indeces_per_client[j][0] = tmp;
			tmp+= assigned_ants[j];
			indeces_per_client[j][1] = tmp - 1;
			} else {
				indeces_per_client[j] = new int[2];
				indeces_per_client[j][0] = -1;
				indeces_per_client[j][1] = -1;
			}

		}
#if !NO_GUI
		gui->assign_ant_color(n_clients, indeces_per_client); //assignment of colours to ants of the same client
#endif
		printf("%d\n", end_server);
		
		while (!end_server){

			n_clients = poll_number;

			if(!fds.empty()) {
				fds_pointer = &fds[0];
			}
			poll_number = fds.size();

			printf("Waiting on poll()...\n");
			rc = poll(&fds[0], poll_number, -1);

			/* Check to see if the poll call failed.                   */
			if (rc < 0) {
				perror("poll() failed");
				exit(0);

			} else {
				for (i = 0; i < poll_number; i++) { //when I delete from the list of poll a client, I have to consider it whan I pass the index of a client

					close_conn = 0;
					if(fds[i].revents == 0)
						continue;

					if(fds[i].revents &  (POLLNVAL|POLLERR|POLLHUP))
					{
						printf("Error! revents = %d\n", fds[i].revents);
						close_conn = 1;
					} else {
						close_conn = read_from_client(fds[i].fd, return_client_index(fds[i].fd));
					}
					if (close_conn > 0) {
						printf("socket %d closed\n",  fds[i].fd);
						close(fds[i].fd);
						fds[i].fd = -1;
						fds[i].revents = 0;

					}
				}
			}/* End of loop through pollable descriptors              */
			for(i = poll_number-1; i > -1; i--) {
				if(fds[i].fd == -1) {
					fds.erase(fds.begin()+ i);
				}
			}
			if(fds.empty()) {
				end_server = 1;
				printf("No clients anymore, Server is dying..\n");
				n_clients = 0;

			}


		} /* End of serving running.    */
		delete[] assigned_threads;
		delete[] assigned_ants;
		for(i = 0; i < n_clients; i++) {
			delete[] indeces_per_client[i];
		}
		delete[] indeces_per_client;

	}
	close(server_sock);

#if !NO_GUI
	pthread_exit(thread_ind); //close the thread safely
#else
	delete thread_ind;
	return 1;
#endif
}

//basing on the number of socket, return the index of the client
int
Server::return_client_index(int sock) {
	int i = 0;
	while(sock != sockets[i])
		i++;
	return i;
}

int
main(int argc, char * argv[]) {

#if !NO_GUI
	std::vector<BLine *> Blines;
	std::vector<Ant *> ants;
	std::vector<Object* > objs;
	std::vector<Heap* > heaps;

	GUI *gui = new GUI(&Blines, &ants, &objs, &heaps);
	Observer *obs = new Observer(gui);

	Server server(gui, obs);

	server.create_thread();
	server.gui->main();

	server.delete_thread();
#else
	Server server(NULL, NULL);
	server.main();
#endif
	return 0;
}
