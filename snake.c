#include <curses.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>


#define HEIGHT 20
#define WIDTH 20
#define W 119
#define A 97
#define S 115
#define D 100

int buffer = 0;
int done = 0;

char board[HEIGHT][WIDTH + 1];

void updateBoard();
void moveApple();
void updateTail();
void displayLose();

typedef struct snake
{
  char symbol;
  int x;
  int y;
  int prevx;
  int prevy;
  struct snake *next;
} SNAKE;

typedef struct apple
{
  int x;
  int y;
  char symbol;
} APPLE;

SNAKE head = {'&', 4, 4, 0, 0, NULL};
APPLE next = {5, 5, 'O'};

void* getChars()
{
  int temp;

  while(!done)
    {
      temp = getch();
      if(temp == W || temp == A || temp == S || temp == D)
	{
	  if(!(temp == W && buffer == S && head.next != NULL)
	     && !(temp == A && buffer == D && head.next != NULL)
	     && !(temp == S && buffer == W && head.next != NULL)
	     && !(temp == D && buffer == A && head.next != NULL))
	    {
	      buffer = temp;
	    }
	}
    }
  return NULL;
}

void* readInput()
{

  struct timespec time, time2;
  
  time.tv_sec = 0;
  time.tv_nsec = 150000000;

  while(!done)
    {
      nanosleep(&time, &time2);
      if(buffer == W)
	{
	  head.prevy = head.y;
	  head.prevx = head.x;
	  head.y -= 1;
	}
      if(buffer == A)
	{
	  head.prevy = head.y;
	  head.prevx = head.x;
	  head.x -= 1;
	}
      if(buffer == S)
	{
	  head.prevy = head.y;
	  head.prevx = head.x;
	  head.y += 1;
	}
      if(buffer == D)
	{
	  head.prevx = head.x;
	  head.prevy = head.y;
	  head.x += 1;
	}

      if(head.x == next.x && head.y == next.y)
	{
	  moveApple(); 
	}

      updateTail();
      if(!done)
	updateBoard();
      else
	displayLose();
    }

  return NULL;
}

void updateTail()
{
  SNAKE *part = &head;

  if(head.x == -1 || head.y == -1 || head.x == WIDTH || head.y == HEIGHT)
    {
      done = 1;
      return;
    }

  
  while(part -> next != NULL)
    {
      if((head.x == part -> next -> x && head.y == part -> next -> y))
	{
	  done = 1;
	  return;	  
	}
      else
	{
	  part -> next -> prevx = part -> next -> x;
	  part -> next -> x = part -> prevx;
	  part -> next -> prevy = part -> next -> y;
	  part -> next -> y = part -> prevy;

	  part = part -> next;
	}
    }
}

void moveApple()
{
  SNAKE *tail = &head;
  int valid = 0;

  while(tail->next != NULL)
    {
      tail = tail->next;
    }

  tail -> next = malloc(sizeof(struct snake));
  (tail -> next) -> x = tail -> prevx;
  (tail -> next) -> y = tail -> prevy;
  (tail -> next) -> symbol = '+';

  while(!valid)
    {
      next.x = rand() % WIDTH;
      next.y = rand() % HEIGHT;

      valid = 1;

      tail = &head;

      while(tail->next != NULL)
	{
	  if(tail -> x == next.x && tail -> y == next.y)
	    {
	      valid = 0;
	    }
	  tail = tail -> next;
	}
    }

}


void updateBoard()
{
  int i;
  int j;

  for(i = 0; i < HEIGHT; i++)
    {
      for(j = 0; j < WIDTH; j++)
	{
	  board[i][j] = ' ';
	}
      board[i][WIDTH] = '\0';
    }

  board[next.y][next.x] = next.symbol;
  board[head.y][head.x] = head.symbol;

  SNAKE* part = head.next;

  while(part != NULL)
    {
      board[part -> y][part -> x] = part -> symbol;
      part = part -> next;
    }


  clear();
  
  printw("|====================|\n");
  for(i = 0; i < HEIGHT; i++)
    {
      printw("|%s|\n", &board[i][0]);
    }
	 
  printw("|====================|\n");


  printw("y = %d", head.y);
  printw("buffer = %d", buffer);
  printw("prevx = %d", head.prevx);
  printw("prevy = %d", head.prevy);
  refresh();
}

void displayLose()
{
  clear();
  printw("\n\n\n        YOU LOSE     \nPress any button to quit\n\n");
  refresh();
}


int main()
{
  WINDOW *stdscr = initscr();
 
  pthread_t characterGetter;
  pthread_t inputReader;

  printw("  _____             _    \n");
  printw(" / ____|           | |       \n");
  printw("| |     _ __   __ _| | _____ \n");
  printw("| |    | '_ \\ / _` | |/ / _ \\\n");
  printw("| |____| | | | (_| |   <  __/\n");
  printw(" \\_____|_| |_|\\__,_|_|\\_\\___|\n");
  printw("\nCoded in glorious C by Alex Ruth\n");
                              
                              


  printw("  Press any button to start\n");
  refresh();

  getch();

  updateBoard();
  
  pthread_create(&characterGetter, NULL, getChars, NULL);
  pthread_create(&inputReader, NULL, readInput, NULL);

  pthread_join(characterGetter,NULL);
  pthread_join(inputReader,NULL);
  
  endwin();

  printf("Donezo\n");
  return 0;
}
