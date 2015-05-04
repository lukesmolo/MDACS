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
#include "Engine.h"
#include "Config.h"
#include "BLine.h"
#include "Ant.h"
#include "Heap.h"
#include "Object.h"

class GUI{

	public:
		GUI(std::vector<BLine *> *Blines_, std::vector<Ant* > * ants_, std::vector<Object* > * objs_, std::vector<Heap* > * heaps_):
			Blines(Blines_),
			objs(objs_),
			ants(ants_),
			heaps(heaps_),
			window_width(WINDOW_WIDTH),
			window_height(WINDOW_HEIGHT),
			to_die(0),
			n_ants(N_ANTS),
			n_objects_left(N_OBJECTS),
			n_objects(N_OBJECTS)
	{
		int i = 0;
		int j = 0;

		float tmp_x = -1 + RW_CELL/2;
		float tmp_y = 1 - RH_CELL/2;

		//initialize grid with the center of a cell
		for(i = 0; i < N_LINES; i++) {
			for(j = 0; j < N_CELLS; j++) {
				grid[i][j].x = tmp_x + j*(RW_CELL);
				grid[i][j].y = tmp_y - i*(RH_CELL);
				grid[i][j].nLine = i;
				grid[i][j].ncell = j;
				return_v_lines(&grid[i][j], RW_CELL);
				return_o_lines(&grid[i][j], RH_CELL);
				grid[i][j].full_ant = 0;
				grid[i][j].full_heap = 0;
				grid[i][j].ant = -1;
				grid[i][j].heap= -1;
			}
		}

		for(i = 0; i < N_TYPES_OBJECTS; i++) {
			n_objects_per_type[i] = 0;
		}

		str_ants= new char[20];
	}
		std::vector<BLine *> *Blines; //array of lines for the grid
		std::vector<Object* > *objs; //array of Objects
		std::vector<Ant* > *ants; //array of ants
		std::vector<Heap* > *heaps; //array of heaps

		int window_width;
		int window_height;
		int to_die;

		int n_ants;	//counter for the number of ants
		int n_objects_left;	//counter for the number of ants to order
		int n_objects;  //counter for the number of objects
		char *str_ants;

		struct point grid[N_LINES][N_CELLS]; //grid
		int n_objects_per_type[N_TYPES_OBJECTS]; //hash table indicating how many objects for type i

		int main();
		void draw();

		//utils
		float return_random(float x, float x1);
		void return_v_lines(point *pos, float rw_cell);
		void return_o_lines(point *pos, float rh_cell);
		void print_pos(point *pos);
		bool check_grid_cell_for_ant(int x, int y);
		bool check_grid_cell_for_heap(int x, int y);
		void check_point(point *p, float width, float heigth);
		void fps(int useconds);
		void set_grid_cell_ant(int x, int y, int ant, int what);
		void set_grid_cell_heap(int x, int y, int heap, int what);
		void move_ant(int ant, int x, int y);
		void move_object(int id_obj, struct point *pos);
		void assign_ant_color(int n_clients, int **indeces_per_client);
		void delete_free_heaps();
		void fix_objects_order(int id_heap);
		void check_heap_type(int id_heap);
		Heap* return_heap(int id_heap);
		Object* return_object(int id_object);
		int has_finished();

};

