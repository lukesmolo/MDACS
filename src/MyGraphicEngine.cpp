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

#include <stdio.h>
#include <vector>

#include "MyGraphicEngine.h"
#include "Ant.h"
#include "BLine.h"
#include "Heap.h"

void MyGraphicEngine::Draw(){
	std::vector<BLine *> *Blines;
	std::vector<Ant *> *ants;
	std::vector<Object*> *objs;
	Blines = gui->Blines;
	ants = gui->ants;
	objs = gui->objs;
	//draw all the lines
	for (int i = 0; i < (int)Blines->size(); i++) {
		(*Blines)[i]->draw();
	}

	//draw all the ants
	if(!ants->empty()) {
		for (int i = 0; i < (int) ants->size(); i++) {
			(*ants)[i]->draw();
		}
	}
	//draw all the objects
	
	if(!objs->empty()) {
		for(int j = 0; j < (int)(*objs).size(); j++) {
			(*objs)[j]->draw();
		}
	}
	//draw the writing
	gui->draw();

}

