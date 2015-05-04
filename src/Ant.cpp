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
#include <unistd.h>
#include <stdlib.h>
#include "Ant.h"

using namespace std;


void Ant::draw(){
	float r, g, b, a;
	r = rgba[0];
	g = rgba[1];
	b = rgba[2];
	a = rgba[3];
		//draw set of lines that define an hash
		GraphicPrimitives::drawLine2D(pos.x-width/4, pos.y,pos.x+width/4, pos.y, r, g, b, a);
		GraphicPrimitives::drawLine2D(pos.x-width/4, pos.y+height/2,pos.x+width/4, pos.y-height/2, r, g, b, a);
		GraphicPrimitives::drawLine2D(pos.x, pos.y+height/2,pos.x, pos.y-height/2, r, g, b, a);

		GraphicPrimitives::drawLine2D(pos.x+width/4, pos.y+height/2,pos.x-height/4, pos.y-height/2, r, g, b, a);
		GraphicPrimitives::drawLine2D(pos.x+width/4, pos.y+height/4,pos.x-height/4, pos.y-height/4, r, g, b, a);
		GraphicPrimitives::drawLine2D(pos.x-width/4, pos.y+height/4,pos.x+height/4, pos.y-height/4, r, g, b, a);
}
