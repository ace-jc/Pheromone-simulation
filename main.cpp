#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <Windows.h>

#define array_width 50
#define array_height 50
#define ant_hill_rock 10 // 1 part per ant_hill_rock is rock in ant hill
#define food_amt 500 // 1 part per food_amt is food

using namespace std;

/*
    This is an abstract data type that will handle all types of ant
    variables.
*/
class Abstract_ants{
protected:
    //Variables
    int internal_hungry; // to help create more minima and media workers
    int internal_pheromone; // what this ant releases
    int scared_level; // to create more soldiers
    int external_pheromone; // what this ant is reading to find food
    int horizontal_pos;
    int vertical_pos;

public:
    Abstract_ants(){
        internal_hungry = 50 + (rand() % 10);
        internal_pheromone = 0;
        scared_level = 20 + (rand() % 10);
        external_pheromone = 0;
        horizontal_pos = 0;
        vertical_pos = 0;
    }

    void set_position(int hori, int vert){
        this->horizontal_pos = hori;
        this->vertical_pos = vert;
    }

    int get_horizontal(){
        return this->horizontal_pos;
    }

    int get_vertical(){
        return this->vertical_pos;
    }
};

/*
    This is a specific type of ant. It is a worker ant.
*/
class Worker: public Abstract_ants{
    bool finding_food; // to stop reading pheromone and release its own
    bool leaf_in_radius;

public:
    Worker(){
        finding_food = true;
        leaf_in_radius = false;
    }

    char ant_letter(){
        return 'W';
    }

    Worker return_itself(){
        return *this;
    }
};

/*
    The containers class will hold the information pertaining to the array
    grid. Each grid block contains a container which contains several different
    piece of information about the simulation.
*/
class Containers{
public:
    char state; //for display on the board
    int pheromone_food; // food pheromone level of the block at given moment
    int pheromone_home;// home pheromone level of the block at given moment
    Worker *worker; // will be a pointer to a worker in the given container
    int food_block;

    Containers(){
        // Constructor
        this->state = '.';
        this->pheromone_food = 0;
        this->pheromone_home = 0;
        this->food_block = 0;
        this->worker = NULL;
    }

    void set_food(int incoming_food){
        this->food_block = incoming_food;
    }

    void get_food(){
        this->food_block--;
        if(!this->food_block){ // food is zero
            this->set_state('.');
        }
    }

    void set_state(char incoming_char){
        // method to help set the state of the container when ants are removed
        this->state = incoming_char;
    }

    void set_worker(Worker incoming_worker){
        // sets the worker address to the given worker
        this->worker = &incoming_worker;
        this->state = incoming_worker.ant_letter();
    }

    void remove_worker(){
        // removes the worker and sets it to null
        this->worker = NULL;
        this->state = '.';
    }

    int current_pheromone_food(){
        // increases the pheromone level by one when called
        return this->pheromone_food;
    }

    void increase_pheromone_food(){
        // increases the pheromone level by one when called
        pheromone_food++;
    }

    void decrease_pheromone_food(){
        // decreases the pheromone level by one when called
        if(this->pheromone_food){
            this->pheromone_food--;
        }
    }

    void increase_pheromone_home(){
        // increases the pheromone level by one when called
        this->pheromone_home++;
    }

    void decrease_pheromone_home(){
        // increases the pheromone level by one when called
        if(pheromone_home){
            this->pheromone_home--;
        }
    }

    char current_state(){
        // returns the state of the given container
        return this->state;
    }
};


/*
    This is the world which will contain all of the board and will display
    the current world.
*/
class World{
    //variables
    Containers container_world[array_width][array_height];
    vector<Worker> workers_list; //  will contain all of the active workers


public:
    //constructor
    World(){
        cout << "\t\t\tA 70x70 Ant World" << endl << endl;
        create_ant_hill();
    }

    void print(){
        for(int i=0; i<array_width; i++){
            for(int j=0; j<array_height; j++){
                cout << container_world[i][j].current_state();
            }
            cout << endl;
        }
    }

    void create_ant_hill(){
        for(int i=0; i<array_width; i++){
            for(int j=0; j<array_height; j++){
                if(j == (array_width/4*3) && !(i == (array_width/2) || i == ((array_width/2)+1))){
                    // creating the barrier to the exit and the entrance
                    // set to 3/4 with and entrance in near middle
                    container_world[i][j].set_state('R');
                }
                if(j > (array_width/4*3)){
                    int tmp = rand()%ant_hill_rock;
                    if(tmp == 0){
                        container_world[i][j].set_state('R');
                    }
                }
                if(j < (array_width/4*3)){
                    int tmp = rand()%food_amt; // food amount in part per food_amt defined above
                    if(tmp == 0){
                        container_world[i][j].set_food((rand()%50)+1); // set food between 1 and 50
                        container_world[i][j].set_state('F');
                    }
                }
            }
        }
    }

    void add_workers(int num){
        // adding workers to array and map
        for(int i=0; i<num; i++){
            int vertical_pos = rand()%array_width;
            int horizontal_pos = rand()%array_height;
            while(container_world[vertical_pos][horizontal_pos].current_state() != '.'){
                // ensuring the workers are only added to empty areas of the map
                vertical_pos = rand()%array_width;
                horizontal_pos = rand()%array_height;
            };

            Worker worker;
            // set the position of the worker internally to each worker
            worker.set_position(horizontal_pos, vertical_pos);
            // adding worker to the list of workers maintained in the world
            workers_list.push_back(worker);

//            cout << horizontal_pos << " " << vertical_pos << endl;

            // set pointer to worker and the letter on the map
            container_world[vertical_pos][horizontal_pos].set_worker(worker);
        }
    }

    void tick(){
        // will cause the board to move forward one tick for all participants
        vector<Worker>::iterator iter;
        for(iter = workers_list.begin(); iter!=workers_list.end(); iter++){
            // iteration of worker list
            int saved_horizontal = iter->get_horizontal();
            int saved_vertical = iter->get_vertical();
            int temp_horizontal_position = iter->get_horizontal();
            int temp_vertical_position = iter->get_vertical();
            int upper_temp_horizontal = 0;
            int upper_temp_vertical = 0;
            int best_pheromone = 0;
            int best_pheromone_horizontal = 0;
            int best_pheromone_vertical = 0;

//            cout << "saved_horizontal: " << saved_horizontal << endl;
//            cout << "saved_vertical: " << saved_vertical << endl;

            // moving the center of the ant to the upper right hand side of square for analysis
            // also creating an upper bound to check for corner cases
            temp_horizontal_position -= 2;
            temp_vertical_position -= 2;
            upper_temp_horizontal += 2;
            upper_temp_vertical += 2;
            //checking for out of bounds
            if(temp_horizontal_position < 0){ // horizontally
                temp_horizontal_position = 0;
            }
            if(temp_vertical_position < 0){ // and vertically
                temp_vertical_position = 0;
            }
            if(upper_temp_horizontal >= array_width){
                upper_temp_horizontal = (array_width-1);
            }
            if(upper_temp_vertical >= array_height){
                upper_temp_vertical = (array_height-1);
            }

            for(int j=temp_horizontal_position; j<upper_temp_horizontal; j++){
                for(int k=temp_vertical_position; k<upper_temp_vertical; k++){
                    int temp_pheromone = container_world[j][k].current_pheromone_food();
                    if(temp_pheromone > best_pheromone){
                        best_pheromone = temp_pheromone; // setting the best pheromone to highest amount
                        best_pheromone_horizontal = j; //
                        best_pheromone_vertical = k;
                    }
                }
            }

            if((best_pheromone_horizontal == 0) && (best_pheromone_vertical == 0)){
                // there was no pheromone around the ant
                int temptemp = ((rand()%3)-1);
                int temptemp2 = ((rand()%3)-1);
//                cout << "temptemp: " << temptemp << endl;
//                cout << "temptemp2: " << temptemp2 << endl;
                saved_horizontal += temptemp;
                saved_vertical += temptemp2;
                // check for out of bounds again
                // checking for out of bounds
                if(saved_horizontal < 0){ // horizontally
                    saved_horizontal = 0;
                }
                if(saved_vertical < 0){ // and vertically
                    saved_vertical = 0;
                }
                if(saved_horizontal >= array_width){
                    saved_horizontal = (array_width-1);
                }
                if(saved_vertical >= array_height){
                    saved_vertical = (array_height-1);
                }
            }
            else{
                // there was a pheromone around the ant!
            }

//            cout << "saved_horizontal: " << saved_horizontal << endl;
//            cout << "saved_vertical: " << saved_vertical << endl;
//
//            cout << "iter->get_horizontal(): " << iter->get_horizontal() << endl;
//            cout << "iter->get_vertical(): " << iter->get_vertical() << endl;


            if(container_world[saved_vertical][saved_horizontal].current_state() == '.'){
//                cout << "inside the critical function if" << endl;
                container_world[iter->get_vertical()][iter->get_horizontal()].remove_worker();
                container_world[saved_vertical][saved_horizontal].set_worker(iter->return_itself());
            }




        }

        // will decrease the pheromone amounts across the board
        // essentially a type of decay
        for(int i=0; i<array_width; i++){
            for(int j=0; j<array_height; j++){
                container_world[i][j].decrease_pheromone_food();
                container_world[i][j].decrease_pheromone_home();
            }
        }
    }
};




/*
    This is the main program for the simulation
*/
int main(){
    srand(time(NULL)); // sets up rand
    World the_world; // creates the world
    the_world.print(); // prints the world
    the_world.add_workers(20); // adds num workers to the world

    cout << "Workers added" << endl;
    the_world.print(); // prints the world

    the_world.tick();
    the_world.tick();

    cout << "After tick" << endl;
    the_world.print(); // prints the world

    return 0;
}

