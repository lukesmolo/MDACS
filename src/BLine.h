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
#include "GraphicPrimitives.h"
#include <vector>

using namespace std;

class BLine {

	public:

		BLine(int n_cells_ = N_CELLS, float _h_cell = 0, float _w_cell = 0, int n_Bline_ = 0,  int n_olines_ = 2):
			n_cells(n_cells_),
			h_cell(_h_cell),
			w_cell(_w_cell),
			n_Bline(n_Bline_),
			rw_cell(RW_CELL),
			rh_cell(RH_CELL),
			n_vlines(n_cells+1),
			n_olines(n_olines_){
			}

		int n_cells;	//number of cells
		float h_cell;	//high of cell
		float w_cell;	//width of cell
		int n_Bline;	//number of BLine from 0
		float rw_cell;	//relative width of cell
		float rh_cell;	//relative width of cell
		int n_vlines;	//number of vertical lines: n_cells+1
		int n_olines;	//number of orizontal lines

		void draw();
};
