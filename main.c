#include <ncurses.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>


#define HEIGHT 20
#define WIDTH 20
#define W 119
#define A 97
#define S 115
#define D 100


//Do you think God stays in heaven because, he too, lives in fear of formatting
//ASCII output?

#define LOGO " ________  ________   ________  ___  __    _______      \n"\
"|\\   ____\\|\\   ___  \\|\\   __  \\|\\  \\|\\  \\ |\\  ___ \\     \n"\
"\\ \\  \\___|\\ \\  \\\\ \\  \\ \\  \\|\\  \\ \\  \\/  /|\\ \\   __/|    \n"\
" \\ \\  \\    \\ \\  \\\\ \\  \\ \\   __  \\ \\   ___  \\ \\  \\_|/__  \n"\
"  \\ \\  \\____\\ \\  \\\\ \\  \\ \\  \\ \\  \\ \\  \\\\ \\  \\ \\  \\_|\\ \\ \n"\
"   \\ \\_______\\ \\__\\\\ \\__\\ \\__\\ \\__\\ \\__\\\\ \\__\\ \\_______\\\n"\
"    \\|_______|\\|__| \\|__|\\|__|\\|__|\\|__| \\|__|\\|_______|\n"

char strbuf[100];
int buffer = 0;
int done = 0;
int max_x;
int max_y;

int bottomOffset = 3;

unsigned int applesEaten = 0;

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

SNAKE head = {'&', 4, 4, 0, 0, NULL};

typedef struct apple
{
  int x;
  int y;
  char symbol;
} APPLE;

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
  time.tv_nsec = 100000000;

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

  if(head.x == -1 || head.y == -1 || head.x == max_x || head.y == max_y)
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

void addTail()
{
  
  SNAKE *tail = &head;
  
  while(tail -> next != NULL)
    {
      tail = tail -> next;
    }

  tail -> next = malloc(sizeof(struct snake));
  (tail -> next) -> x = tail -> prevx;
  (tail -> next) -> y = tail -> prevy;
  (tail -> next) -> prevy = -1;
  (tail -> next) -> prevx = -1;
  (tail -> next) -> symbol = '+';
}


void moveApple()
{
  SNAKE *tail = &head;
  int valid = 0;
  int i;

  applesEaten++;

  for(i = 0; i < max_x / 25 + 1; i++)
    {
      addTail();
    }

  while(!valid)
    {
      next.x = rand() % max_x;
      next.y = rand() % max_y;

      valid = 1;

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

  clear();

  mvprintw(next.y, next.x, "O");
  mvprintw(head.y, head.x, "&");

  for(i = 0; i < max_x; i++)
    {
      mvprintw(max_y, i, "=");
    }

  sprintf(strbuf, "Score: %07u", applesEaten * 100);
  mvprintw(max_y + 1, max_x / 2  - strlen(strbuf)/2, strbuf);
  mvprintw(max_y + 2, max_x / 2  - strlen("WASD to Move")/2, "WASD to Move");

  SNAKE* part = head.next;

  while(part != NULL)
    {
      mvprintw(part -> y, part -> x, "+");
      part = part -> next;
    }

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
  char* splitString;

  getmaxyx(stdscr, max_y, max_x);
  max_y -= bottomOffset;

  noecho();
  curs_set(FALSE);
 
  pthread_t characterGetter;
  pthread_t inputReader;


  int offset = -3;
  char logo[] = LOGO;

  splitString = strtok(logo, "\n");

  while(splitString != NULL)
    {
      mvprintw(max_y / 2 + offset, max_x / 2 - strlen(splitString) / 2, splitString);
      splitString = strtok(NULL, "\n");
      offset++; 
    }

  offset++;

  mvprintw(max_y / 2 + offset, max_x / 2 - strlen("Coded in glorious C by Alex Ruth") / 2, "Coded in glorious C by Alex Ruth");
  offset+=2;
  mvprintw(max_y / 2 + offset, max_x / 2 - strlen("Press any button to start") / 2, "Press any button to start\n");

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
