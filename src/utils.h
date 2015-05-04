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
#ifndef UTILS_H

#include <vector>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <ctime>
#include "Config.h"



#define UTILS_H
typedef struct point { //structure of a cell of the grid
	float x;
	float y;
	int nLine;
	int ncell;
	float v_lines[2];
	float o_lines[2];
	int full_ant; //flag to indicate the presence of an ant
	int full_heap; //flag to indicate the presence of an heap
	int ant; //id of the ant contained in this cell
	int heap; //id of the heap contained in this cell
} point;

typedef struct ant_msg{ //structure for ant in clients-server message passing
	int flag;
	int x; //line of cell
	int y; //column of cell
} ant_msg;

typedef struct socketpair_arg {
	int n_ants;
	struct ant_msg pos[N_ANTS];
	int indeces[2]; //indices for buffer reading for client
} socketpair_arg ;


typedef struct client_mes { //message for server by client
	int n_ants;
	int n_client;
	int indeces[2]; //indices for buffer reading for client
	struct ant_msg pos[N_ANTS];
} client_mes;

typedef struct server_mes { //message for client by server
	int n_client;
	int n_threads;
	int n_ants;
	int to_die;
	int indeces[2];
	struct ant_msg pos[N_ANTS];
} server_mes;

typedef struct  thread_struct {  //structure for thread by main
	void *cl;
	int a;
} thread_struct;



#endif
