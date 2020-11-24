#include <iostream>
#include <cmath>
#include <ctime>
using namespace std;
#include <BearLibTerminal.h>
#include "gooseEscapeUtil.hpp"
#include "gooseEscapeActors.hpp"
#include "gooseEscapeConsole.hpp"
#include "gooseEscapeGamePlay.hpp"

extern Console out;
/*
With graphics, screens are given an x,y coordinate system with the origin
in the upper left corner.  So it means the coordinate axes are:
---------------->   x direction
|
|
|
|
|
V

y direction 
*/
	
/*
    Print the game world
    
    The functions should draw characters to present features of the game
    board, e.g. win location, obstacles, power ups
*/



// gets random x and y co-ordinates that are currently empty
void randomObjectPlacement(int map[MAX_BOARD_X][MAX_BOARD_Y], int tile)
{
	//Get random values for x and y that are on the board
	int x = rand() % MAX_BOARD_X;
	int y = rand() % MAX_BOARD_Y;
	
	//Ensure that selected location is empty
	while(map[x][y] != EMPTY)
	{
		x = rand() % MAX_BOARD_X;
		y = rand() % MAX_BOARD_Y;
	}
	
	map[x][y] = tile;
}


//creates continuous straight wall segments
//Note: overlaps are allowed and can create corners or even longer wall segments
void straightWalls(int map[MAX_BOARD_X][MAX_BOARD_Y], int length)
{
	//Get random values for x and y that are on the board
	int x = rand() % (MAX_BOARD_X - length);
	int y = rand() % (MAX_BOARD_Y - length);
	
	// 50/50 for if walls are vertical or horizontal
	int odd_or_even = rand() % 2;
	
	for(int count = 0; count < length; count++)
	{
		if(odd_or_even == 0) // horizontal walls
		{
			map[x + count][y] = SHALL_NOT_PASS;
		}
		else // vertical walls
		{
			map[x][y + count] = SHALL_NOT_PASS;
		}
	}
}



// print the game board function
void game_world_print(int map[MAX_BOARD_X][MAX_BOARD_Y])
{
	for(int y_location = 0; y_location < MAX_BOARD_Y; y_location++)
	{
		for(int x_location = 0; x_location < MAX_BOARD_X; x_location++)
		{
			if(map[x_location][y_location] == SHALL_NOT_PASS)
			{
				terminal_put(x_location, y_location, WALL_CHAR);
			}
			else if(map[x_location][y_location] == WINNER)
			{
				terminal_put(x_location, y_location, WIN_CHAR);
			}
			else if(map[x_location][y_location] == SLOW_GOOSE)
			{
				terminal_put(x_location, y_location, SLOW_GOOSE_CHAR);
			}
		}
	}
	terminal_refresh();
}

/*
    Do something when the goose captures the player
    
    If you want to attack or something else, this is the function you 
    need to change.  For example, maybe the two touch each other and
    then fight.  You could add a health to the Actor class that is
    updated.  Run, use weapons, it's up to you!
*/

bool captured(Actor const & player, Actor const & monster)
{
    return (player.get_x() == monster.get_x() 
         && player.get_y() == monster.get_y());
}

/*
    Move the player to a new location based on the user input.  You may want
    to modify this if there are extra controls you want to add.
    
    All key presses start with "TK_" then the character.  So "TK_A" is the a
    key being pressed.
    
    A look-up table might be useful.
    You could decide to learn about switch statements and use them here.
*/

void movePlayer(int key, Actor & player, int map[MAX_BOARD_X][MAX_BOARD_Y])
{
    int yMove = 0, xMove = 0;
    int speed = player.get_speed();
	if (key == TK_UP)
        yMove = -speed;
    else if (key == TK_DOWN)
        yMove = speed;
    else if (key == TK_LEFT)
        xMove = -speed;
    else if (key == TK_RIGHT)
        xMove = speed;
        
    if (player.can_move(xMove, yMove) 
      && map[player.get_x() + xMove][player.get_y() + yMove] != SHALL_NOT_PASS)
        player.update_location(xMove, yMove);
}

void gooseMove(Actor & player, Actor & goose, int map[MAX_BOARD_X][MAX_BOARD_Y])	//goose will move 2 places per game tick
{
	 int xMove = 0, yMove = 0;
	 int deltaX = player.get_x() - goose.get_x();
	 int deltaY = player.get_y() - goose.get_y();
	 int gooseSpeed = goose.get_speed();
	 if(abs(deltaX) > abs(deltaY))	//further in x direction so move in x direction
	 {
		if(deltaX < -1)	//goose is to the right(+x) of player so must move left(-x)
 		    xMove = -gooseSpeed;
		else if(deltaX > 1) //goose is to the right(+x) of player so must move left(-x)
			xMove = gooseSpeed;
		else 
			xMove = deltaX; //goose is adjacent so just move one to catch player
	}
	else	//more distance to travel in y direction so choose to move that way
	{
		if(deltaY < -1)	//goose is further down(+y) of player so must move up(-y)
 		    yMove = -gooseSpeed;
		else if(deltaY > 1) //goose is higher(-y) than player so must move left(+sy)
			yMove = gooseSpeed;
		else 
			yMove = deltaY; //goose is adjacent so just move one to catch player
	}
    if (goose.can_move(xMove, yMove))
	{
		if(map[goose.get_x() + xMove][goose.get_y() + yMove] != SHALL_NOT_PASS)
		{
			goose.update_location(xMove, yMove);
		}
		else //you've run into a wall there will be at least 1 side with no wall so check each side and move into empty
		{
			if(!map[goose.get_x() + xMove + 1][goose.get_y() + yMove] && goose.can_move(xMove + 1, yMove))
	  		    goose.update_location(xMove + 1, yMove);
			else if(!map[goose.get_x() + xMove - 1][goose.get_y() + yMove] && goose.can_move(xMove - 1, yMove))
	  		    goose.update_location(xMove - 1, yMove);
			else if(!map[goose.get_x() + xMove][goose.get_y() + yMove + 1] && goose.can_move(xMove, yMove + 1))
	  		    goose.update_location(xMove, yMove + 1);
			else if(!map[goose.get_x() + xMove][goose.get_y() + yMove - 1] && goose.can_move(xMove, yMove - 1))
	  		    goose.update_location(xMove, yMove - 1);
		}
	}
}





/*
    What other functions do you need to make the game work?  What can you
    add to the basic functionality to make it more fun to play?
*/
