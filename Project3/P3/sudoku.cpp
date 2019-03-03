#include <unistd.h>
#include <stdlib.h>
#include <curses.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <gdbm.h>

using namespace std;

void displayNumbers(char nums[81]);
void generateAndDisplayBoard();
void generateNumbers();
void determineSubMatrix(int row, int col);
void displayGameInfo(int x, int y);
void hideNumbers();

int realBrdHt = 19;
int realBrdWdth = 37;
int brdHt = 9;
int brdWdth = 9;

int realSctnLngth = 7;
int realSctnWdth = 7;
int sctnLngth = 3;
int sctnWdth = 3;

char Gnums[81];

int rowPos, colPos;

/*
 Sudoku Rules:
 
 1. In all 9 sub matrices 3×3 the elements should be 1-9, without repetition.
 2. In all rows there should be elements between 1-9 , without repetition.
 3. In all columns there should be elements between 1-9 , without repetition.
 */



int main()
{
    initscr();
    clear();
    noecho();
    cbreak();
    
    keypad(stdscr, true);
    start_color();
    
    int x,y;
    getmaxyx(stdscr, x, y);
    
    attron(A_BOLD);
    char title[] = "Simple Sudoku";
    mvprintw(1, (y - strlen(title))/2, "%s", title);
    attroff(A_BOLD);
    
    displayGameInfo(x,y);
    generateAndDisplayBoard();
    nodelay(stdscr, true);
    
    generateNumbers(); //Display numbers is nested inside
    hideNumbers();
    time_t start = time(NULL);
    int min = 0, tens = 0, sec;
    int input;
    int init_y = y;
    y = 1;
    x = 2;
    
    while (input != 'Q')
    {
        input = getch();
        
        sec = (time(NULL) - start);
        tens = sec / 10;
        min = sec / 60;
        mvprintw(7, (init_y - 10)/2, "%2d:%d%d", min, tens%6, sec%10);
        
        move(1, 2);
        if(isdigit(input))
        {
            if (input <= '9' && input >= '1')
                mvprintw(y, x, "%c", input);
        }
        else
        {
            switch (input)
            {
                case KEY_UP:
                    if(y - 1> 1)
                        y -= 2;
                    break;
                case KEY_DOWN:
                    if(y + 2 < 19)
                        y += 2;
                    break;
                case KEY_LEFT:
                    if(x - 2 > 2)
                        x -= 4;
                    break;
                case KEY_RIGHT:
                    if(x + 4< 37)
                        x += 4;
                    break;
                case('N'):
                    generateNumbers();
                    hideNumbers();
                    break;
                case('S'):
                    displayNumbers(Gnums);
                    break;
                default:
                    break;
            }
        }
        
        move(y,x);
        refresh();
        
        refresh();
        usleep(10);
        
        
    }
    getch();
    endwin();
    exit(EXIT_SUCCESS);
    return 0;
}
void generateAndDisplayBoard()
{
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    init_pair(4, COLOR_WHITE, COLOR_BLACK);
    
    for(int row = 0; row < realBrdHt; row++)
    {
        for(int col = 0; col < realBrdWdth; col++)
        {
            attron(COLOR_PAIR(2));
            if(row % 2 == 0 && col % 4 != 0)
            {
                if(row == 0 || row == realBrdHt - 1)
                    attron(COLOR_PAIR(1));
                else if(row == 6 || row == 12)
                    attron(COLOR_PAIR(2));
                else
                    attron(COLOR_PAIR(3));
                mvhline(row, col, ACS_HLINE, 1);
                
                attroff(COLOR_PAIR(1));
                attroff(COLOR_PAIR(2));
                attroff(COLOR_PAIR(3));
            }
            if(row % 2 != 0 && col % 4 == 0)
            {
                if(col == 0 || col == realBrdWdth - 1)
                    attron(COLOR_PAIR(1));
                else if(col == 12 || col == 24)
                    attron(COLOR_PAIR(2));
                else
                    attron(COLOR_PAIR(3));
                mvvline(row, col, ACS_VLINE, 1);
                attroff(COLOR_PAIR(1));
                attroff(COLOR_PAIR(2));
                attroff(COLOR_PAIR(3));
            }
            attroff(COLOR_PAIR(2));
            
            attron(COLOR_PAIR(1));
            if(row == 0 && col % 4 == 0 && col > 0 && col < realBrdWdth - 1) //top inside
                mvhline(row, col, ACS_TTEE, 1);
            if(row % 2 == 0 && row > 0 && row < realBrdHt - 1 && col == 0) //left inside
                mvvline(row, col, ACS_LTEE, 1);
            if(row == realBrdHt - 1 && col % 4 == 0 && col > 0 && col < realBrdWdth - 1) //bottom inside
                mvhline(row, col, ACS_BTEE, 1);
            if(row % 2 == 0 && row > 0 && row < realBrdHt - 1 && col == realBrdWdth - 1) //right inside
                mvvline(row, col, ACS_RTEE, 1);
            
            if(row == 0 && col == 0) //top left corner
                mvvline(row, col, ACS_ULCORNER, 1);
            if(row == 0 && col == realBrdWdth - 1) //top right corner
                mvvline(row, col, ACS_URCORNER, 1);
            if(row == realBrdHt - 1 && col == 0) //bottom left corner
                mvvline(row, col, ACS_LLCORNER, 1);
            if(row == realBrdHt - 1 && col == realBrdWdth - 1) //bottom right corner
                mvvline(row, col, ACS_LRCORNER, 1);
            attroff(COLOR_PAIR(1));
            
            //Plus signs
            if (row > 0 && row < realBrdHt - 1 && row % 2 == 0 && col > 0 && col < realBrdWdth - 1 && col % 4 == 0)
            {
                if(row == 6 || row == 12 || col == 12 || col == 24)
                    attron(COLOR_PAIR(2));
                else
                    attron(COLOR_PAIR(3));
                mvvline(row, col, ACS_PLUS, 1);
                
                attroff(COLOR_PAIR(2));
                attroff(COLOR_PAIR(3));
            }
        }
    }
}

void generateNumbers()
{
    int i = 0;
    char nums[81];
    vector<int> tried;
    //bool full = false;
    bool used = false;
    bool incr = false;
    vector<int>::iterator it;
    int board [9][9] = { 0 };
    int random;
    int j = 1;
    for(int row = 0; row < 9; row++)
    {
        for(int col = 0; col < 9; col++)
        {
            incr = false;
            while(!tried.empty())
            {
                tried.pop_back();
            }
            while(incr == false) //the code is meant to run with the commented lines, but including them results in an endless loop (inr is always true)
            {
                used = false;
                random = rand() % 9 + 1;
                it = find (tried.begin(), tried.end(), random);
                if(it == tried.end())
                {
                    tried.push_back(random);
                }
                //else
                //continue;
                for(int h = 0; h < 9; h++)
                {
                    
                    j++;
                    
                    if(board[h][col] == random || board[row][h] == random) //not in the same collumn/row
                    {
                        //used = true;
                        //break;
                    }
                }
                //if (used == true)
                //continue;
                
                determineSubMatrix(row, col);
                for(int f = rowPos; f < (3 + rowPos); f++) //not in the same 3x3 matrix; THE PROBLEM
                {
                    for(int g = colPos; g < (3 + colPos); g++)
                    {
                        if(board[f][g] == random)
                        {
                            //used = true;
                            //break;
                        }
                    }
                    //if(used)
                    //break;
                }
                
                if(used == false)
                {
                    
                    board[row][col] = random;
                    incr = true;
                    nums[i] = '0' + board[row][col];
                    Gnums[i] = nums[i];
                    /*if(i == 81)
                     {
                     break;
                     }
                     else
                     {
                     i++;
                     }*/
                    i++;
                }
            }
        }
    }
    displayNumbers(nums);
}

void displayNumbers(char nums[81])
{
    int i = 0;
    
    for(int row = 0; row < realBrdHt; row++)
    {
        for(int col = 0; col < realBrdWdth; col++)
        {
            attron(COLOR_PAIR(4));
            
            if(((row - 1) % 2 == 0) && ((col - 2) % 4 == 0))
            {
                mvprintw(row, col, "%c", nums[i]);
                i++;
                //cout << nums[i] << " ";
            }
            
            attroff(COLOR_PAIR(4));
        }
    }
    //cout << endl;
}

void determineSubMatrix(int row, int col) //finds the starting indexes for the loop above
{
    if(row < 3 && col < 3)
    {
        rowPos = 0;
        colPos = 0;
    }
    else if(row > 2 && row < 6 && col < 3)
    {
        rowPos = 3;
        colPos = 0;
    }
    else if(row > 5 && col < 3)
    {
        rowPos = 6;
        colPos = 0;
    }
    else if(row < 3 && col > 2 && col < 6)
    {
        rowPos = 0;
        colPos = 3;
    }
    else if(row < 3 && col > 5) //IT ALWAYS ENDS UP HERE!!!
    {
        rowPos = 0;
        colPos = 6;
    }
    else if(row > 2 && row < 6 && col > 2 && col < 6)
    {
        rowPos = 3;
        colPos = 3;
    }
    else if(row > 5 && col > 2 && col < 6)
    {
        rowPos = 6;
        colPos = 3;
    }
    else if(row > 2 && row < 6 && col > 5)
    {
        rowPos = 3;
        colPos = 6;
    }
    else if(row > 5 && col > 5)
    {
        rowPos = 6;
        colPos = 6;
    }
    /*else
     {
     rowPos = 0;
     colPos = 0;
     }*/
}

void displayGameInfo(int x, int y)
{
    mvprintw(3, (y - 10)/2, "%s", " Q - quit");
    mvprintw(4, (y - 10)/2, "%s", " N - new game");
    mvprintw(5, (y - 10)/2, "%s", " S - solve board");
}

void hideNumbers()
{
    int random;
    
    for(int row = 0; row < realBrdHt; row++)
    {
        for(int col = 0; col < realBrdWdth; col++)
        {
            attron(COLOR_PAIR(4));
            random = rand() % 9 + 1;
            if(((row - 1) % 2 == 0) && ((col - 2) % 4 == 0))
            {
                if(random < 7)
                    mvprintw(row, col, "%c", ' ');
            }
            
            attroff(COLOR_PAIR(4));
        }
    }
}

/*
 Implementation of GDBM would have been a record of lowest times for completion.
 This would have included a CHECK feature, to see if your solution is correct without solving it.
 */
