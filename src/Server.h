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
#include "Config.h"
#include "utils.h"
#include "GUI.h"
#include "Observer.h"
#include <pthread.h>

class Server{

	public:

		Server(GUI *gui_, Observer *obs_, int n_clients_ = -1, int port_ = DEFAULT_SERVER_PORT):
		gui(gui_),
		obs(obs_),
		server_sock(-1),
		n_clients(n_clients_),
		n_ants(N_ANTS),
		port(port_),
		end_server(0),
		has_finished(0),
		assigned_threads(NULL),
		assigned_ants(NULL),
		server_threads(SERVER_THREADS),
		client_threads(CLIENT_THREADS)
		{}

		GUI *gui; //gui
		Observer *obs; //observer
		int server_sock; //socket of the thread used for listen
		std::vector<int> sockets; //clients sockets array that liste has opened
		int n_clients;
		int n_ants;
		int port;
		int end_server;
		int has_finished;
		int *assigned_threads; //array of assigned threads for client i
		int *assigned_ants; //array of ants assigned for client i
		int **indeces_per_client; //indeces of ants assigned for client i
		pthread_t main_threads[SERVER_THREADS];


		int server_threads; //number of threads for the server
		int client_threads; //number of total client threads`


		int main();
		int create_thread();
		int delete_thread();


		static void* thread_functions(void *thread_arg);
		int read_from_client(int filedes, int client);
		void launch_gui(int ind);
		int* random_threads_number(int what);
		int make_package(struct server_mes *s_msg, int client);
		void unpack_packet(struct client_mes* c_msg);
		static void check_finish(Server *s);
		bool check_ants_per_client();
		int return_client_index(int sock);
};
