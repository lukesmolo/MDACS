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

#pragma once
#include <vector>
#include <cstring>
#include "Config.h"
#include "utils.h"
#include <pthread.h>

class Client{

	public:

		Client():
			remote_port_number(DEFAULT_SERVER_PORT),
			n_ants(0),
			n_client(-1),
			n_pkgs(0),
			end_client(0),
			sync_max(0),
			sync_count(0),
			pipes_in(NULL),
			pipes_out(NULL),
			pipes_pfd_in(NULL),
			pipes_pfd_out(NULL),
			ants_per_thread(NULL)

	{
		char string_remote_ip_address_[] = DEFAULT_SERVER_ADDRESS; //address of server
		strncpy(string_remote_ip_address, string_remote_ip_address_, strlen(string_remote_ip_address_));
	}



		short int remote_port_number;
		char string_remote_ip_address[32];

		int n_threads;
		int n_ants;
		int n_client;
		int n_pkgs;
		int end_client;
		int sync_max;
		int sync_count;
		pthread_mutex_t  sync_lock; //thread m.e. variable
		pthread_cond_t   sync_cond; //thread synchronizig variable
		struct server_mes last_package;
		int **pipes_in; //pipes for threads
		int **pipes_out; //pipes for threads
		struct pollfd* pipes_pfd_in; //poll struct for reading into pipes
		struct pollfd* pipes_pfd_out; //poll struct for writing into pipes
		int **ants_per_thread; //index of ants per thread in the ants array
		pthread_t *main_threads;


		int main();
		static void* thread_functions(void *thread_arg);
		int create_thread();
		int delete_thread(); //natural death of threads
		void thread(int ind);
		int *random_threads_number(int what);
		int write_to_server(int filedes);


		void make_package(struct client_mes* c_msg);
		void ant_algorithm(struct ant_msg *positions,  struct socketpair_arg *buf);
		int unpack_package(struct server_mes* s_msg);
		void Sync();
		void close_thread(int index); //force useless thread to die

		void move_ant(int *x_move, int* y_move) {
			int pos;
			pos = rand() % 7; //random movement of an ant are 6
			//printf("random move is %d\n", pos);
			switch(pos){
				case 0:
					  (*x_move)--;
						  break;
				case 1:
					 (*x_move)--;
						 (*y_move)++;
						 break;
				case 2:
					  (*y_move)++;
						  break;
				case 3:
					  (*x_move)++;
						  y_move++;
						  break;
				case 4:
					  (*x_move)++;
						  break;
				case 5:
						  (*x_move)++;
						  (*x_move)--;
						  break;
				case 6:
						  (*y_move)--;
						  break;
				case 7:
						  (*x_move)--;
						  (*y_move)--;
						  break;
				default: break;

			}

		}

};
