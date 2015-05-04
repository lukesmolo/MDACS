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
#include "GraphicPrimitives.h"
#include "Config.h"
#include "utils.h"


class Ant{

	public:

		Ant( struct point *pos_ = NULL , int id_number_ = 0):
			id_number(id_number_),
			object(-1),
			steps(ANT_STEPS),
			problem(0),
			width(ANT_WIDTH),
			height(ANT_HEIGHT),

			flying(0) {
				if(pos_ != NULL)
					memcpy(&pos, pos_, sizeof(struct point));
				move_to_cell[0] = pos.nLine;
				move_to_cell[1] = pos.ncell;
				for(int i = 0; i < 4; i++) {
					rgba[i] = 0.8; //at the beginning ants are grey;
				}

			}



		struct point pos; //position of the ant in the grid
		int id_number; //id of ant
		int object; //id of the object ant has
		int steps; //number of steps left
		int problem; //flag in case of problems

		float width,height; //dimensions of ant
		float rgba[4]; //colours of ant
		int flying; //flag for special movement, when ant goes outside of the grid
		int move_to_cell[2]; //next movement


		void draw();


};
