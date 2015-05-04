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

#include<string>
#include<cstring>
#include<sstream>
#include <stdio.h>
#include <stdlib.h>
#include "Object.h"

using namespace std;

void Object::draw(){
	float tmp_x = width/2;
	float tmp_y = height/2;
	if(moving == 0) {
		if(order < 4) { //if it can be shown
			switch(order) {
				case 0:
					tmp_x = pos.x-width-tmp_x;
					tmp_y = pos.y+height;
					break;
				case 1:
					tmp_x = pos.x+tmp_x;
					tmp_y = pos.y+height;
					break;
				case 2:
					tmp_x = pos.x-width-tmp_x;
					tmp_y = pos.y-2*height;
					break;
				case 3:
					tmp_x = pos.x+tmp_x;
					tmp_y = pos.y-2*height;
					break;

				default: break;

			}
			GraphicPrimitives::drawFillRect2D(tmp_x,tmp_y,width,height,rgba[0],rgba[1],rgba[2], rgba[3]);
		}
	} else { //if the object is moving, draw it together with the ant
		GraphicPrimitives::drawFillRect2D(pos.x-tmp_x, pos.y-tmp_y, width,height,rgba[0],rgba[1],rgba[2], rgba[3]);
	}


}

