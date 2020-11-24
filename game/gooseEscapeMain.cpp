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
	Actor player(PLAYER_CHAR, 10,10,1);  // you probably don't want to start in the same place each time
	
	//make the monster
	Actor goose(MONSTER_CHAR, 70,20,2);
	
	// Declare the array that will hold the game board "map"
    int map[MAX_BOARD_X][MAX_BOARD_Y] = {EMPTY};
  	
  	
  	
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
    //place 5 powerups to slow the goose down and 5 to ___
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
/*
    All key presses start with "TK_" then the character.  So "TK_A" is the "a"
    key being pressed.
*/
    int keyEntered = TK_A;  // can be any valid value that is not ESCAPE or CLOSE
    bool has_not_won = true;
    
    
    while(keyEntered != TK_ESCAPE && keyEntered != TK_CLOSE 
                    && !captured(player,goose) && has_not_won)
	{
	    // get player key press
	    keyEntered = terminal_read();

        if (keyEntered != TK_ESCAPE && keyEntered != TK_CLOSE)
        {
            // move the player, you can modify this function
    	    movePlayer(keyEntered,player,map);
    	    
    	    if(map[player.get_x()][player.get_y()] == WINNER)
    	    {
    	    	has_not_won = false;	
			}

            // call the goose's chase function
            gooseMove(player, goose, map);
            // call other functions to do stuff?	    
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
    
        // output why:  did the goose get us, or did we get to the win location?
	
    	// Wait until user closes the window
        while (terminal_read() != TK_CLOSE);
    }

	//game is done, close it  
    terminal_close();
}
