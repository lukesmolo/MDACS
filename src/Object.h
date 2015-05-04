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
#include "GraphicPrimitives.h"

#include "utils.h"

class Object{

	public:

		Object( struct point *pos_ = NULL , int id_number_ = 0, int order_ = 0, int type_= 0):
			id_number(id_number_),
			order(order_),
			type(type_),
			width(OBJECT_WIDTH),
			height(OBJECT_HEIGHT),
			moving(0)
	{
		if(pos_ != NULL)
			memcpy(&pos, pos_, sizeof(struct point));
		for(int i = 0; i < 4; i++) {
			rgba[i] = 0.8;
		}


	}


		struct point pos;
		int id_number;
		int order; //order in the heap
		int type; //type of object
		float width;
		float height;
		int moving; //flag if ant is moving it
		float rgba[4];
		void draw();


};
