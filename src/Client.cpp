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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include <poll.h>
#include <vector>

#include "utils.h"
#include "Config.h"
#include "Client.h"


#define writesocket 1
#define readsocket 0


//synchronize threads before makin a message for server
void
Client::Sync() {
	pthread_mutex_lock(&sync_lock);

	sync_count++; /*increasing counter of threads already arrived*/

	if (sync_count < sync_max) { /*check if it needs to wait*/

		/*wait*/
		pthread_cond_wait(&sync_cond, &sync_lock);
		pthread_cond_signal (&sync_cond);
	}
	else    {
		pthread_cond_signal (&sync_cond);

	}

	pthread_mutex_unlock (&sync_lock);
	sync_count = 0; //before returning, need to set sync_count to 0 again
	return;


}


//assign randomly "what" to threads
int*
Client::random_threads_number(int what) {

	float *number = new float[n_threads];
	int *random = new int[n_threads];
	srand((unsigned)time(NULL));
	float tmp = 0;
	int sum = 0;
	do{
		sum = 0;
		for (int i = 0; i < n_threads; i++) {
			number[i] = 1+ rand() % what;
		}
		//normalization of the sum
		for (int i = 0; i < n_threads; i++) {
			tmp += number[i];	
		}
		tmp = what/tmp;
		for (int i = 0; i < n_threads; i++) {
			number[i] = number[i]*tmp+0.5; //add 0.5 to round the float
		}
		for (int i = 0; i < n_threads; i++) {
			sum += (int)number[i];	
		}

	} while(sum != what);
	for (int i = 0; i < n_threads; i++) {
		random[i] = (int)number[i];
	}
	delete[] number;
	return random;
}


//the ant algorithm. It just returns a new random position for each ant of threads
void
Client::ant_algorithm(struct ant_msg *positions, struct socketpair_arg *buf) {

	int i = 0;
	int j = 0;
	int *x_move;
	int *y_move;
	for(j = buf->indeces[0], i = 0; i < buf->n_ants; i++, j++) {
		x_move = &(buf->pos[j].x);
		y_move = &(buf->pos[j].y);

		if(positions[i].x == POS_NOT_SET ) { //x was never set
			positions[i].x = buf->pos[j].x;
		}
		if(positions[i].y == POS_NOT_SET) { //y was never set
			positions[i].y = buf->pos[j].y;
		}
		if(buf->pos[j].flag >= 0) { //if the last was not rejected by the server because of some reason
			positions[i].x = buf->pos[j].x; //take new positions
			positions[i].y = buf->pos[j].y;
		} else {
			buf->pos[j].x = positions[i].x ; //set previous coordinates
			buf->pos[j].y = positions[i].y ; //set previous coordinates
		}

		move_ant(x_move, y_move); //call th build_in function in Client.h that returns a new random position
	}
}

void
Client::thread(int ind) {
	int *a = new int;
	*a = ind;
	struct socketpair_arg buf;
	int res = 0;
	struct pollfd pfd_in;
	struct pollfd pfd_out;
	pfd_in.fd = pipes_in[ind][readsocket];
	pfd_out.fd = pipes_out[ind][writesocket];
	pfd_in.events = POLLIN;
	pfd_out.events = POLLOUT;
	int number_ants, i;
	struct ant_msg *positions = NULL;
	srand((unsigned)time(NULL));


	while(!end_client) {
		if(n_pkgs <= 0) { //if no package has arrived yet from server..
			continue;
		}
		res = poll(&pfd_in,1, -1);
		if(res < 0) {
			perror("Thread socketpair poll() failed, closing..");
			exit(0);
		} else if(res > 0) {
			if(pfd_in.revents & POLLIN) { //poll for reading from socketpair
				res = read (pfd_in.fd, &buf, sizeof(struct socketpair_arg));
				number_ants = buf.n_ants;
				if(number_ants == 0) { //useless thread
					printf("I am thread %d and since I have 0 ants, I am useless.\n", ind);

				} else if(positions == NULL) { //initialize positions array
					positions = new struct ant_msg[number_ants]; //buffer to store old ants positions, just in case we need to restore them
					for(i = 0; i < number_ants; i++) {
						positions[i].x = POS_NOT_SET;
						positions[i].y = POS_NOT_SET;
					}
					printf("I am thread %d and I have %d ants, from %d to %d\n", ind, number_ants, buf.indeces[0], buf.indeces[1]);
				}
			}
			if(n_pkgs > 0 && number_ants > 0) { //if the server has already sent previous information and some ants were assigned to this thread ..
				ant_algorithm(positions, &buf);
			}
			Sync(); //synchronize all threads
			res = poll(&pfd_out,1, -1); //poll for sending through socketpair
			if(res < 0) {
				perror("Thread socketpair poll() failed, closing..");
				exit(0);
			} else if(res > 0) {
				if(pfd_out.revents & POLLOUT) {
					res = send(pfd_out.fd, &buf, sizeof(struct socketpair_arg), MSG_NOSIGNAL);
				}
			}
		}
	}

	pthread_exit(a); //safe exit of the thread returning its index
}

//trick to avoid static problems of thread functions
void*
Client::thread_functions(void *client) {
	struct thread_struct *cl = (struct thread_struct*)client;
	Client *c = (Client*)cl->cl;
	int ind = cl->a;

	c->thread(ind);
	free(client); //free the argument passed to the thread
	return NULL;

}


//create threads according to the number received from the server
int
Client::create_thread() {
	int t = 0;
	int ris = 0;
	struct thread_struct *arg;
	for(t = 0; t < n_threads; t++) {
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

//delete thread safely
int
Client::delete_thread() {
	int t = 0;
	void *ptr;
	int error;

	for(t = 0; t < n_threads; t++) {

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
	return 1;


}

//read a package received from server
int
Client::unpack_package(struct server_mes* s_msg) {
	memcpy(&last_package, s_msg, sizeof(struct server_mes)); //buffer last msg from sever
	if(s_msg->to_die) { //if server asks to die..
		printf("Server has finished, Client now is dying..\n");
		return 1;
	}
	return 0;
}

//after every thread has answered, make a new package for the server
void
Client::make_package(struct client_mes* c_msg) {
	int i = 0;
	int j = 0;
	struct ant_msg tmp;
	c_msg->n_client = n_client;
	c_msg->n_ants = n_ants;
	std::vector<ant_msg> positions;
	for(j = 0, i = last_package.indeces[0]; j < n_ants; i++, j++) { //read from shared buffer, but according to ants assigned to each thread, positions to send and save them in the package to send
		tmp.x = last_package.pos[i].x;
		tmp.y = last_package.pos[i].y;
		tmp.flag = 1; //everthing is ok
		positions.push_back(tmp);
		memcpy(&(c_msg->pos[i]), &tmp, sizeof(tmp));
	}
}

//send to server the message
int
Client::write_to_server (int socketfd) {
	struct client_mes c_msg;
	int i, status, j, t;

	char buffer[1024]; //buffer for tcp recv

	memset(&c_msg, 0, sizeof(client_mes));
	struct server_mes *s_msg = NULL;
	int n, nread, tmp;
	int close_conn = 0;
	int *random_ants_per_thread;
	struct socketpair_arg sock_buf;

	//initialize package with default values
	c_msg.n_ants = -1;
	c_msg.n_client = -1;
	for(i = 0; i < N_ANTS; i++) {
		c_msg.pos[i].x = POS_NOT_SET;
		c_msg.pos[i].y = POS_NOT_SET;
	}
	if(n_pkgs > 0) { //if clients received at least the first message, threads has produced somthing. So, wait for them and read what they produced
		status = poll(pipes_pfd_in, n_threads, -1);
		if(status > 0) {	
			for(i = 0; i < n_threads; i++){ //two file descriptors per thread
				if(pipes_pfd_in[i].revents & POLLOUT) {
					//prepare the package for socketpair
						memcpy(&sock_buf.pos, last_package.pos, sizeof(last_package.pos)); //copy the last message in socketpair_arg
						sock_buf.n_ants = ants_per_thread[i][1]-ants_per_thread[i][0]+1; //the number of ants is the difference between indeces
						sock_buf.indeces[0] = ants_per_thread[i][0]+last_package.indeces[0]; //give indeces to thread
						sock_buf.indeces[1] = ants_per_thread[i][1]+last_package.indeces[1];

						status = send(pipes_pfd_in[i].fd, &sock_buf, sizeof(struct socketpair_arg), MSG_NOSIGNAL); //send message to thread

				}
			}

			status = poll(pipes_pfd_out, n_threads, -1); //poll for reading from socketpair
			if(status > 0) {	
				for(i = 0; i < n_threads; i++){
					if(pipes_pfd_out[i].revents & POLLIN) {
						status = read(pipes_pfd_out[i].fd, &sock_buf, sizeof(struct socketpair_arg));	 //receive from thread
						for(t = 0, j = sock_buf.indeces[0] ; t < sock_buf.n_ants; t++, j++) {
							last_package.pos[j].x = sock_buf.pos[j].x;
							last_package.pos[j].y = sock_buf.pos[j].y;

						}

					}
				}
			}
		}
if(n_pkgs > 0)
		make_package(&c_msg); //prepare package to send to server

	}
	
	n = write(socketfd, &c_msg, sizeof(client_mes)); //send package

	if(n < 0) {
		char msgerror[1024];
		sprintf(msgerror,"write() failed [err %d] ",errno);
		perror(msgerror);
		fflush(stdout);
		return(1);
	}

#if VERBOSE
	for(j = 0, i = last_package.indeces[0]; j < n_ants; i++, j++) {
		printf("Sent to server (%d,%d)\n",  c_msg.pos[i].x, c_msg.pos[i].y);
	}
#endif
	nread=0;
	n = recv(socketfd, buffer, sizeof(buffer), 0); //receive in tcp buffer


	if(n < 0) {
		char msgerror[1024];
		sprintf(msgerror,"read() failed [err %d] ",errno);
		perror(msgerror);
		fflush(stdout);
		return(1);
	}
	if (n == 0){
		printf("Connection closed\n");
		close_conn = 1;
	} else {
		//a new package was exchanged
		s_msg = (server_mes*)buffer;

		close_conn = unpack_package(s_msg); //read content of the package received from server
#if VERBOSE
		for(j = 0, i = last_package.indeces[0]; j < n_ants; i++, j++) {

			printf("Received from server (%d,%d)\n",  c_msg.pos[i].x, c_msg.pos[i].y);
		}
#endif

		if(n_pkgs == 0) { //if this is the first package, set all variables according to what client received
			printf("number of n_threads is: %d\n", s_msg->n_threads);
			printf("number of n_ants is: %d\n", s_msg->n_ants);
			n_threads = s_msg->n_threads;
			n_ants= s_msg->n_ants;
			if(n_threads > 1)
				sync_max = n_threads;
			else
				sync_max = 1;
			if(n_threads == 0)
				end_client = 1;

			n_client = s_msg->n_client;
			main_threads = new pthread_t[n_threads];
			pipes_in = new int*[n_threads];
			pipes_out = new int*[n_threads];
			pipes_pfd_in = new struct pollfd[n_threads];
			pipes_pfd_out = new struct pollfd[n_threads];
			ants_per_thread = new int*[n_threads];
			random_ants_per_thread = random_threads_number(n_ants);
			tmp = 0;
			for(i = 0; i < n_threads; i++) {
				pipes_in[i] = new int[2];//two file descriptors per thread
				pipes_out[i] = new int[2];//two file descriptors per thread
				ants_per_thread[i] = new int[2];
				ants_per_thread[i][0] = tmp;

				tmp += random_ants_per_thread[i]; //simple formula to assign indeces of array basing on the number of ants per thread i
				ants_per_thread[i][1] = tmp - 1 ;
				status = socketpair(AF_UNIX,SOCK_STREAM,0,pipes_in[i]);
				if (status < 0) {
					/* an error occurred */
					perror("socketpair failed");
					exit(1);
				}
				status = socketpair(AF_UNIX,SOCK_STREAM,0,pipes_out[i]);
				if (status < 0) {
					/* an error occurred */
					perror("socketpair failed");
					exit(1);
				}
				pipes_pfd_in[i].fd = pipes_in[i][writesocket];
				pipes_pfd_out[i].fd = pipes_out[i][readsocket];
				pipes_pfd_in[i].events = POLLOUT;
				pipes_pfd_out[i].events = POLLIN;
			}

			free(random_ants_per_thread);
			create_thread(); //now, client can create threads

		}

		n_pkgs++;


		usleep(USLEEP); //use a pause to make it slower
	}

	return close_conn;	
}  /* End of existing connection is readable  */


int
Client::main() {
	struct sockaddr_in Local, Serv;
	int socketfd, res;
	int rc, i;

	struct pollfd client_fds;

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd == SOCKET_ERROR) {
		printf ("socket() failed, Err: %d \"%s\"\n", errno,strerror(errno));
		exit(1);
	}

	memset ( &Local, 0, sizeof(Local) );
	Local.sin_family		=	AF_INET;
	Local.sin_addr.s_addr	=	htonl(INADDR_ANY);
	Local.sin_port	=	htons(0);

	res = bind(socketfd, (struct sockaddr*) &Local, sizeof(Local));
	if (res == SOCKET_ERROR)  {
		printf ("bind() failed, Err: %d \"%s\"\n",errno,strerror(errno));
		exit(1);
	}

	memset ( &Serv, 0, sizeof(Serv) );
	Serv.sin_family	 =	AF_INET;
	Serv.sin_addr.s_addr  =	inet_addr(string_remote_ip_address);
	Serv.sin_port		 =	htons(remote_port_number);

	res = connect(socketfd, (struct sockaddr*) &Serv, sizeof(Serv));
	if (res == SOCKET_ERROR)  {
		printf ("connect() failed, Err: %d \"%s\"\n",errno,strerror(errno));
		exit(1);
	}

	client_fds.fd = socketfd;
	client_fds.events = POLLOUT;

	do { //until server does not say to die..
		printf("Waiting on poll()...\n");
		rc = poll(&client_fds, 1, -1);

		/* Check to see if the poll call failed.                   */
		if (rc < 0) {
			perror("poll() failed");
			exit(0);

		} else {

			if(client_fds.revents & POLLOUT) {
				res = write_to_server(socketfd);
				if(res == 1)
					end_client = 1;
			}
		}
	} while(!end_client);

	close(socketfd);
	//free all memory allocated
	if(n_pkgs > 0) { //it means that memory was allocated
		for(i = 0; i < n_threads; i++){
			close(pipes_in[i][0]);
			close(pipes_in[i][1]);
			close(pipes_out[i][1]);
			close(pipes_out[i][1]);
		}
		delete[] pipes_pfd_in;
		delete[] pipes_pfd_out;
		delete_thread();
		for(int i = 0; i < n_threads; i++) { //two file descriptors per thread
			delete[] pipes_in[i];
			delete[] pipes_out[i];
			delete[] ants_per_thread[i];
		}
		delete[] pipes_in;
		delete[] pipes_out;
		delete[] ants_per_thread;
	}
	return(0);

}

//main for client
int
main() {
	Client *cl = new Client();
	cl->main();
}
