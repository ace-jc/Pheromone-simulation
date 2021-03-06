/** @file World.h
 *  @brief Definition of World class
 */

#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include "Containers.h"
#include "Worker.h"

#define print_pheromones true

/*
    This is the world which will contain all of the board and will display
    the current world.
*/
class World{
    private:

	public:
		// variables temporarily public
	    Containers container_world[array_width][array_height];
		// vector<Worker> workers_list; 
		// will contain all of the active workers
	    vector<Worker *> workers_list;
	    int frame_count;

	    //constructor
	    World();
	    void create_ant_hill();
	    void print();
	    void statistics();
	    void add_workers(int num);
	    void tick();
	    void one_block_from(vector<Worker *>::iterator iter, char food_or_home);
};

#endif