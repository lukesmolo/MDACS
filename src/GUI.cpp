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
#include "GUI.h"
#include "MyGraphicEngine.h"
#include "Engine.h"
#include "MyGameEngine.h"
#include "MyControlEngine.h"
#include "BLine.h"
#include "Observer.h"

int
GUI::main() {
	int i = 0;
	int j = 0;
	int x, y;
	int tmp;
	int tmp1;

	Object *tmp_obj = NULL;
	float objects_colors[N_TYPES_OBJECTS][4];

//	srand((unsigned)time(NULL));

	for(i = 0; i < N_TYPES_OBJECTS; i++) { //decide colors for object types
		for(j = 0; j < 4; j++) {
			objects_colors[i][j] = return_random(0.1, 1.0);

		}

	}

	for(i = 0; i < N_HEAPS; i++) {

		do { //decide randomly position for the heap
			x = rand() % N_LINES;
			y = rand() % N_CELLS;
		} while(check_grid_cell_for_heap(x, y)); //check if there is already another heap

		heaps->push_back(new Heap(&grid[x][y], 0, i, -1)); //by default, heap is mixed and it is empty

		set_grid_cell_heap(x,y,i,1); //set cell contains heap
	}

	for(i = 0; i < N_OBJECTS; i++) { //decide randmoly type of the object and which heap to belong to

		tmp = rand() % N_HEAPS;
		tmp1 = rand() % N_TYPES_OBJECTS;
		tmp_obj = new Object(&((*heaps)[tmp]->pos), i, (*heaps)[tmp]->objs.size(), tmp1);
		objs->push_back(tmp_obj); //add object
		(*heaps)[tmp]->objs.push_back(tmp_obj); //we add to heap-nth the last object added in objs array
		memcpy(&(tmp_obj->rgba), &objects_colors[tmp1], sizeof(objects_colors[tmp1]));
		n_objects_per_type[tmp1]++;
	}
	
	delete_free_heaps(); //delete heaps that do not contain any object
	

	for(i = 0; i < N_ANTS; i++) {

		do {
			x = rand() % N_LINES;
			y = rand() % N_CELLS;
		} while(check_grid_cell_for_ant(x, y) || check_grid_cell_for_heap(x, y)); //set ant position only if cell is not already busy
		ants->push_back(new Ant(&grid[x][y], i)); //add ant

		set_grid_cell_ant(x,y,i,1);
	}

	for(i = 0; i< N_LINES; i++) {
		Blines->push_back(new BLine(N_CELLS, window_width/(Blines->size()+1), window_height/N_CELLS, Blines->size())); //add lines for the grid
	}

	//settings for the graphics library

	GraphicEngine * ge = new MyGraphicEngine(this);
	GameEngine * gme = new MyGameEngine(this);
	ControlEngine * ce = new MyControlEngine(this);

	Engine e(0, NULL, window_width, window_height);
	e.setGraphicEngine(ge);
	e.setGameEngine(gme);
	e.setControlEngine(ce);

	//start GUI
	e.start();
	return 1;

}


float
GUI::return_random(float x, float x1) {
	//return a random value between x and x1;	
	float tmp = x + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/((x1)-(x))));
	return tmp;
}


void
GUI::return_o_lines(point *p, float rh_cell) {
	float tmp = -1.0f;

	while((tmp + rh_cell) < p->y) {
		tmp += rh_cell;
	}
	p->o_lines[0] = tmp;
	p->o_lines[1] = tmp + rh_cell;
}



void
GUI::return_v_lines(point *p, float rw_cell) {
	float tmp = -1.0f;

	while((tmp + rw_cell) < p->x) {
		tmp += rw_cell;
	}
	p->v_lines[0] = tmp;
	p->v_lines[1] = tmp + rw_cell;
}


//utility function for debugging positions
void
GUI::print_pos(point *pos) {
	printf("x position is: %f\n", pos->x);
	printf("y position is: %f\n", pos->y);
	printf("nLine is: %d\n", pos->nLine);
	printf("ncell is: %d\n", pos->ncell);
	printf("vertical lines are: %f %f\n", pos->v_lines[0], pos->v_lines[1]);
	printf("orizontal lines are: %f %f\n", pos->o_lines[0], pos->o_lines[1]);
}

//check if there is already an ant in the cell
bool
GUI::check_grid_cell_for_ant(int x, int y) {
	if(grid[x][y].full_ant == 1)
		return true;
	else
		return false;
}


//check if there is already an heap in the cell
bool
GUI::check_grid_cell_for_heap(int x, int y) {
	if(grid[x][y].full_heap == 1)
		return true;
	else
		return false;
}


void
GUI::fps(int useconds) {
	usleep(useconds);
}


//set a cell busy/free because of an ant
void
GUI::set_grid_cell_ant(int x, int y, int ant, int what) {

	grid[x][y].full_ant = what;
	if(what) {
		grid[x][y].ant = ant;
	} else {
		grid[x][y].ant = -1;
	}
}

//set a cell busy/free because of an heap
void
GUI::set_grid_cell_heap(int x, int y, int heap, int what) {

	grid[x][y].full_heap = what;
	if(what) {
		grid[x][y].heap = heap;
	} else {
		grid[x][y].heap= -1;
	}
}


//change next position of an ant
void
GUI::move_ant(int ant, int x, int y) {
	(*ants)[ant]->move_to_cell[0] = x;
	(*ants)[ant]->move_to_cell[1] = y;
}

//assign colours to ants according to which client they belong to
void
GUI::assign_ant_color(int n_clients, int **indeces_per_client) {
	int i = 0;
	int j = 0;
	int t = 0;
	int tmp = 0;
	float rgba[4];


	//srand((unsigned)time(NULL));
	for(i = 0; i < n_clients; i++) {
		if(indeces_per_client[i][0] != -1 && indeces_per_client[i][0] != -1) {
			tmp = indeces_per_client[i][1]-indeces_per_client[i][0]+1;
			for(int t = 0; t < 4; t++) {
				rgba[t] = return_random(0.1, 1.0);
				printf("color %f\n",rgba[t]);
			}
			rgba[i % 4] = 1.0;


			for(j = indeces_per_client[i][0], t = 0; t < tmp; j++, t++) {
				memcpy(&(*ants)[j]->rgba, &rgba, sizeof(rgba));
			}
		}

	}
}

//check if it possibile delete free heaps in the grid
void
GUI::delete_free_heaps(){
	int i = 0;
	while(i < (int) heaps->size()) { //delete empty heaps
		if((*heaps)[i]->objs.empty()) {

			set_grid_cell_heap((*heaps)[i]->pos.nLine, (*heaps)[i]->pos.ncell, (*heaps)[i]->id_number, 0); //empty cell
			heaps->erase(heaps->begin()+i);
		} else
			i++;
	}
}

//change position of object
void
GUI::move_object(int id_obj, struct point *pos) {
	int i = 0;
	while(id_obj != (*objs)[i]->id_number) {
		i++;
	}
	memcpy(&(*objs)[i]->pos, pos, sizeof(*pos)); //copy positions of cell
	(*objs)[i]->order = -1; //order for moving
	(*objs)[i]->moving = 1;
}

//fix the new order of objects in the heap
void
GUI::fix_objects_order(int id_heap) {
	int i = 0;
	Object *tmp_obj;
	Heap *heap = return_heap(id_heap);
	if(!heap->objs.empty()) {
		for(i = 0; i < (int)(heap->objs.size()); i++) {
			tmp_obj = heap->objs[i];
			tmp_obj->order = tmp_obj->order-1; //decrease the order in the heap

		}

	}
}

//return an object according to its id
Object*
GUI::return_object(int id_object) {
	int j = 0;
	while(id_object != (*objs)[j]->id_number && j < (int)(*objs).size()) //search the object with the id that ant had
		j++;
	return (*objs)[j];
}



//return an heap according to its id
Heap*
GUI::return_heap(int id_heap) {
	int j = 0;
	while(id_heap != (*heaps)[j]->id_number && j < (int)(*heaps).size()) //search the heap with the id that cell has
		j++;
	return (*heaps)[j];
}


//check if an heap is uniform and in that case change flag values
void
GUI::check_heap_type(int id_heap) {
	Heap* heap = return_heap(id_heap);
	int i = 0;
	int uniform, tmp;
	if(heap->objs.size() > 1) {
		uniform = heap->objs[0]->type;
		for(i = 0; i < (int)heap->objs.size(); i++) {
			tmp = heap->objs[i]->type;
			if(uniform != tmp) {
				break;
			}
		}
		if(uniform == tmp) { //the heap is uniform
			heap->type = uniform;
		}
	} else if(heap->objs.size() == 1 && n_objects_per_type[heap->objs[0]->type] == 1){
		heap->type = heap->objs[0]->type;

	}
}

//check if the entire algorithm has finished. Every heap has to be uniform and the number of objects in heaps has to be the number of all objects in the grid and update the number of objects left to be ordered
int
GUI::has_finished() {
	int i = 0;
	bool ret = 1;
	int sum, tmp;
	tmp = 0;
	sum = objs->size();
	n_objects_left = 0;
	

	for(i = 0; i < (int)heaps->size(); i++) {
		//check_heap_type((*heaps)[i]->id_number);
		tmp += (*heaps)[i]->objs.size();
		if((*heaps)[i]->type == -1 ) {
			ret = 0;
			n_objects_left += (*heaps)[i]->objs.size();
		}
	}
	for(i = 0; i < (int)(objs)->size(); i++) {
		if((*objs)[i]->moving == 1) {
			n_objects_left++;
		}
	}

	if(tmp != sum)
		ret = 0;
	return ret;
}

//draw the writing of objects to order left
void
GUI::draw() {

	sprintf(str_ants, "%s%d/%d", "Objects left: ", n_objects_left, n_objects);
	GraphicPrimitives::drawText2D(str_ants, -1, -1, 1, 1, 0);
}
