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

#include "Observer.h"


//macro for controls
#define cell_is_free (gui->grid[*x_to_move][*y_to_move].full_ant == 0)
#define same_position (x == *x_to_move && y == *y_to_move)
#define out_position (*x_to_move < 0 || *x_to_move >= N_LINES ||*y_to_move <0 || *y_to_move >= N_CELLS && *x_to_move != POS_NOT_SET && *y_to_move != POS_NOT_SET)

#define position_problem (*x_to_move == what && *y_to_move == what)
#define valid_movement ((x == *x_to_move+1 ||x == *x_to_move || x == *x_to_move-1) && (y == *y_to_move+1 || y == *y_to_move || y == *y_to_move-1) && !flying)
#define flying_valid_movement ((x == *x_to_move+1 ||x == *x_to_move || x == *x_to_move-1 || 0 == *x_to_move || N_LINES-1 == *x_to_move) && (y == *y_to_move+1 || y == *y_to_move || y == *y_to_move-1 || 0 == *y_to_move || N_CELLS-1 == *y_to_move) && flying)


void
Observer::check_ant_positions(int start_index, int n_ants){
	std::vector<Ant *> *ants;
	std::vector<Heap *> *heaps;
	std::vector<Object*> *objs;
	int x, y;
	int *x_to_move, *y_to_move, *flag, *flying;
	int what, ant_id;
	int tmp, j;
	Object * tmp_obj;
	Heap* tmp_heap;


	ants = gui->ants;
	heaps = gui->heaps;
	objs = gui->objs;

	if(!ants->empty()) {
		for (int i = start_index; i < start_index+n_ants; i++) {
			x = (*ants)[i]->pos.nLine;
			y =  (*ants)[i]->pos.ncell;
			x_to_move = &((*ants)[i]->move_to_cell[0]);
			y_to_move = &((*ants)[i]->move_to_cell[1]);
			flag = &((*ants)[i]->problem);
			ant_id = (*ants)[i]->id_number;
			flying = &(*ants)[i]->flying;

			if(out_position) { //check if the position sent by the client is out of the grid
				if(*x_to_move < 0) {
					*x_to_move = N_LINES-1 ;
				} else if(*x_to_move >= N_LINES) {
					*x_to_move = 0;
				}
				if(*y_to_move < 0) {
					*y_to_move = N_CELLS-1;
				} else if(*y_to_move >= N_CELLS) {
					*y_to_move = 0;
				}
				*flying = 1; //set the "special" flying movement
			}

			what = POS_NOT_SET;
			if(same_position || position_problem) { //client has not received yet information or it is the same position. Just continue
				continue;
			}
			if(!valid_movement && !flying_valid_movement) { //if the movement is not valid, let server and client synchronize again
				*flag = NOT_VALID;
				continue;
			}
			if(!cell_is_free) { //the cell is busy, just wait and send to client that position was rejected
				*flag = POS_REJECTED;
				//printf("cell full for ant %d\n", i);
				continue;
			}

			//ok, let's move ant (and maybe also object..)
			*flag = 0; //indication for client that everything is ok
			gui->set_grid_cell_ant(x, y, ant_id, 0); //leave position free

			if((*ants)[i]->object > -1) { //if the ant has an object..

				tmp_obj = gui->return_object((*ants)[i]->object);

				if(gui->check_grid_cell_for_heap(x, y)) { //if there is already an heap here
					tmp_heap = gui->return_heap(gui->grid[x][y].heap);	
					if(tmp_heap->type == -1 || tmp_heap->type == tmp_obj->type) { //no problem, heap is mixed or of the same type

						(*ants)[i]->steps = ANT_STEPS; //reset the number of steps
						tmp_obj->order = tmp_heap->objs.size(); //the new order of the object is the size of the array
						tmp_heap->objs.push_back(tmp_obj); //insert the ant into the heap
						(*ants)[i]->object = -1; //ant releases object
						tmp_obj->moving = 0; //the object is not moving
						gui->check_heap_type(tmp_heap->id_number);
					} else {
						if((*ants)[i]->steps < 0) { //recharge steps for ant
							(*ants)[i]->steps = ANT_STEPS; //reset the number of steps
						}
						gui->move_object((*ants)[i]->object, &gui->grid[*x_to_move][*y_to_move]); //move object
						(*ants)[i]->steps = (*ants)[i]->steps - 1; //decrease steps left

					}
				} else {
					if((*ants)[i]->steps > 0) {
						gui->move_object((*ants)[i]->object, &gui->grid[*x_to_move][*y_to_move]); //move object
						(*ants)[i]->steps = (*ants)[i]->steps - 1; //decrease steps left
					} else { //create a new heap here
						(*ants)[i]->steps = ANT_STEPS; //reset the number of steps
						tmp = -1;
						for(j = 0; j < (int) heaps->size(); j++) { //take the maximum id for heaps
							if(tmp < (*heaps)[j]->id_number) {
								tmp = (*heaps)[j]->id_number;
							}
						}
						tmp++; //new id has to be bigger than the maximum found
						tmp_heap = new Heap(&gui->grid[x][y], 0, tmp, -1); //take the last, tho one just now added
						heaps->push_back(tmp_heap); //mixed at the beginning

						(heaps->back())->objs.push_back(tmp_obj); //insert the ant into the heap
						tmp_obj->order = 0;
						gui->set_grid_cell_heap(x,y,tmp,1); //set cell contains heap
						gui->check_heap_type(tmp_heap->id_number); //see if the left objects make the heap uniform
						(*ants)[i]->object = -1; //ant releases object
						tmp_obj->moving = 0; //the object is not moving

					}
				}

			} else if(gui->grid[x][y].heap > -1) { //if the cell has an heap, i.e., at least an object..
				tmp_heap = gui->return_heap(gui->grid[x][y].heap);
				if(tmp_heap->type == -1) { //move one object only if the heap is not homogeneous
					tmp_obj = tmp_heap->objs[0];
					(*ants)[i]->object = tmp_obj->id_number; //take an object
					tmp_heap->objs.erase(tmp_heap->objs.begin()); //remove the object from the i-th heap
					gui->fix_objects_order(tmp_heap->id_number); //fix the indices in the heap
					gui->delete_free_heaps(); //check if it is possible removing some empty heaps

					gui->check_heap_type(tmp_heap->id_number); //see if the left objects make the heap uniform
					gui->move_object((*ants)[i]->object, &gui->grid[*x_to_move][*y_to_move]); //move the object
				}
			}

			(*ants)[i]->pos = gui->grid[*x_to_move][*y_to_move];
			*flying = 0;
			gui->set_grid_cell_ant(*x_to_move, *y_to_move, ant_id, 1); //set the grid cell busy

		}

	}
	gui->to_die = gui->has_finished(); //check if the ant algorithm has finished

}


