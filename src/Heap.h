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
#include "Config.h"
#include "utils.h"
#include "Config.h"
#include "Object.h"
#include "GraphicPrimitives.h"
#include <vector>



using namespace std;

class Heap{

	public:
		Heap(struct point *pos_, int n_objects_ = 0, int id_number_ = 0, int type_= 0):
			n_objects(n_objects_),
			id_number(id_number_),
			type(type_) {
			if(pos_ != NULL)
			memcpy(&pos, pos_, sizeof(struct point));

			}

		struct point pos;
		int n_objects;
		int id_number;
		int type; //if -1 it is mixed
		std::vector<Object*> objs; //objects that heap contains

};
