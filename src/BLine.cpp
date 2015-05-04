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
#include "BLine.h"
#include "Config.h"

using namespace std;


void BLine::draw(){

	int i = 0;
	float x1, y1, x2, y2;
	x2 = y1 = y2 = 0;
	//draw all the vertical lines, according to the number of cells
	for(i = 0; i < n_vlines; i++){
		x1 = -1;
		x1 += rw_cell*i;
		x2 = x1;
		y1 = 1-(rh_cell*n_Bline);
		y2 =y1-rh_cell ;
		GraphicPrimitives::drawLine2D(x1,y1,x2,y2, 0.3, 0.3, 0.3, 0);
	}
	x1 = x2 = y2 = 0;

	//draw all the orizontal lines, according to the number of lines we want for each line
	for(i = 0; i < n_olines; i++) {
		y1 = -1;
		y1 += rh_cell*i*n_Bline;
		y2 = y1;
		x1 = 1;
		x2 = -x1;
		GraphicPrimitives::drawLine2D(x1,y1,x2,y2, 0.3, 0.3, 0.3, 0);
	}
}
