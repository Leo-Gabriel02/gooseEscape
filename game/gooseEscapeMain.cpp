#include <BearLibTerminal.h>
#include <cmath>
#include <iostream>
#include <ctime>
using namespace std;
#include "gooseEscapeUtil.hpp"
#include "gooseEscapeActors.hpp"
#include "gooseEscapeConsole.hpp"
#include "gooseEscapeGamePlay.hpp"

//set up the console.   Don't modify this line!
Console out;

int main()
{	
	//Set up the window.  Don't edit these two lines
    terminal_open();
  	terminal_set(SETUP_MESSAGE);
  	
  	//set random seed for number generator
  	srand((unsigned) time(0));

/*
    The code below provides a skeleton of the game play.  You will need to
    write code for setting up the game board, and playing the game itself.
    You can modify the code given as needed.
    
    Call the functions that you have written in the game play file, and that
    you have added to the Actor class.
*/
 	
    //make the player
    const int HALF_BOARD_X = MAX_BOARD_X / 2;
    const int PLAYER_SPEED_INITIAL = 1;
	Actor player(PLAYER_CHAR, rand() % HALF_BOARD_X, rand() % MAX_BOARD_Y, PLAYER_SPEED_INITIAL);  // you probably don't want to start in the same place each time
	
	//make the monster
	const int GOOSE_SPEED_INITIAL = 2;
	Actor goose(MONSTER_CHAR, rand() % HALF_BOARD_X + HALF_BOARD_X, rand() % MAX_BOARD_Y, GOOSE_SPEED_INITIAL);
	
	// Declare the array that will hold the game board "map"
    int map[MAX_BOARD_X][MAX_BOARD_Y] = {EMPTY};
  	
  	const int PORTAL_OFFSET = 1;
  	//portal between top left and bottom right
    int portal1[2][2] = {PORTAL_OFFSET, PORTAL_OFFSET, MAX_BOARD_X-PORTAL_OFFSET, MAX_BOARD_Y-PORTAL_OFFSET};
    map[PORTAL_OFFSET][PORTAL_OFFSET] = PORTAL;
    map[MAX_BOARD_X-PORTAL_OFFSET][MAX_BOARD_Y-PORTAL_OFFSET] = PORTAL;
    //portal between bottom left and top right
    int portal2[2][2] = {PORTAL_OFFSET, MAX_BOARD_Y-PORTAL_OFFSET, MAX_BOARD_X-PORTAL_OFFSET, PORTAL_OFFSET};
  	map[PORTAL_OFFSET][MAX_BOARD_Y-PORTAL_OFFSET] = PORTAL;
    map[MAX_BOARD_X-PORTAL_OFFSET][PORTAL_OFFSET] = PORTAL;
    
  	//amount of straight wall segments, and the initial length
  	int const STRAIGHT_WALL_AMOUNT = 6;
  	int length = 3;
  	
  	for(int count = 0; count < STRAIGHT_WALL_AMOUNT; count++)
  	{
  		straightWalls(map, length);
  		length += 2;
	}
  	
  	
    //declare amount of walls that are randomly distributed
	int const WALL_AMOUNT = 40;
	int const POWERUP_AMOUNT = 5;
	//place walls in random locations
	for(int count = 0; count < WALL_AMOUNT; count++)
	{
		randomObjectPlacement(map, SHALL_NOT_PASS);
	}
    //place 5 powerups to slow the goose
	for(int count = 0; count < POWERUP_AMOUNT; count++)
	{
		randomObjectPlacement(map, SLOW_GOOSE);
	}    
    
    //place objective %
	randomObjectPlacement(map, WINNER);
	
    // Call the function to print the game board
  	game_world_print(map);
  	
	// Printing the instructions
    out.writeLine("Escape the Goose! " + goose.get_location_string());
	out.writeLine("Use the arrow keys to move");
	out.writeLine("If the goose catches you, you lose!");
	out.writeLine("Be careful! Sometimes the goose can jump through walls!");

/*
    This is the main game loop.  It continues to let the player give input
    as long as they do not press escape or close, they are not captured by
    the goose, and they didn't reach the win tile
*/

    int keyEntered = TK_A;  // can be any valid value that is not ESCAPE or CLOSE
    bool has_not_won = true;
    int goose_slow_counter = 0;	//counts how long the goose has been slowed
    const int GOOSE_SLOW_TICKS = 15; //amount of time the goose slows when powerup
    
    
    
    bool teleported1 = false, teleported2 = false;
    
    while(keyEntered != TK_ESCAPE && keyEntered != TK_CLOSE 
			      && !captured(player,goose) && has_not_won)
	{
	    keyEntered = terminal_read(); // get player key press


        if (keyEntered != TK_ESCAPE && keyEntered != TK_CLOSE)
        {
            // move the player, you can modify this function
    	    movePlayer(keyEntered,player,map);
			if(tileOnMap(map, player) == PORTAL){
				if(!teleported1)
				{
			    	//portal 1 corner 1
					if(player.get_x() == portal1[0][0] && player.get_y() == portal1[0][1]){
						teleported1 = true;	
		  		    	player.teleport(portal1[1][0], portal1[1][1]); //teleport to portal 1 corner 2
					}
					//portal 1 corner 2
					else if(player.get_x() == portal1[1][0] && player.get_y() == portal1[1][1]){
    			    	teleported1 = true;
			        	player.teleport(portal1[0][0], portal1[0][1]); //teleport to portal 1 corner 2
					}
				}
				if(!teleported2)
				{
			    	//portal 2 corner 1
			    	if(player.get_x() == portal2[0][0] && player.get_y() == portal2[0][1]){
						teleported2 = true;	
	  		        	player.teleport(portal2[1][0], portal2[1][1]); //teleport to portal 2 corner 2
	  		    	}
			    	//portal 2 corner 2
			    	else if(player.get_x() == portal2[1][0] && player.get_y() == portal2[1][1]){
						teleported2 = true;	
   			 	    	player.teleport(portal2[0][0], portal2[0][1]); //teleport to portal 2 corner 2
   			 		}
				}
			}


    	    if(tileOnMap(map, player) == WINNER)
    	    	has_not_won = false;	
			
			
			if(goose_slow_counter == 1)	//change goose speed back to 1 on last tick
	  		    goose.speed_up();
			goose_slow_counter--;	//decrement counter each tick
			
			if(tileOnMap(map, player) == SLOW_GOOSE) //when get powerup
    	    {
 	 		    player.get_powerup(map);
 	 		    if(goose_slow_counter <= 0) //no active powerup
	  			    goose.slow_down();	//only slow down the goose if it isnt slow already
  			    goose_slow_counter = GOOSE_SLOW_TICKS;	//always set the counter to 10
			}

            // call the goose's chase function
            gooseMove(player, goose, map);
        }
  	}

    if (keyEntered != TK_CLOSE)
    {
      	//once we're out of the loop, the game is over
        if(has_not_won == false)
        {
        	out.writeLine("Congratulations! You have won!");
		}
		else
		{
			out.writeLine("You have been captured by the goose, and have lost.");
		}
			
		out.writeLine("The game has ended.");
        out.writeLine("Please close the console now.");
    	
    	// Wait until user closes the window
        while (terminal_read() != TK_CLOSE);
    }

	//game is done, close it  
    terminal_close();
}
