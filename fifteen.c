/**
 * fifteen.c
 *
 * Computer Science 50
 * Problem Set 3
 *
 * Implements the Game of Fifteen (generalized to d x d).
 *
 * Usage: ./fifteen d
 *
 * whereby the board's dimensions are to be d x d,
 * where d must be in [MIN,MAX]
 *
 * Note that usleep is obsolete, but it offers more granularity than
 * sleep and is simpler to use than nanosleep; `man usleep` for more.
 */
 
#define _XOPEN_SOURCE 500

#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// board's minimal dimension
#define MIN 3

// board's maximal dimension
#define MAX 9

// board, whereby board[i][j] represents row i and column j
int board[MAX][MAX];
int win_board[MAX][MAX];

// board's dimension
int d;

// prototypes
void clear(void);
void greet(void);
void init(void);
void draw(void);
bool move(int tile);
bool won(void);
void save(void);

// blank position variables
string blank_char = ":op";
int blank = -1;
int bx = 0;
int by = 0;
// tile position variables
int tx = 0;
int ty = 0;

int main(int argc, string argv[])
{
    // greet player
    greet();

    // ensure proper usage
    if (argc != 2)
    {
        printf("Usage: ./fifteen d\n");
        return 1;
    }

    // ensure valid dimensions
    d = atoi(argv[1]);
    if (d < MIN || d > MAX)
    {
        printf("Board must be between %i x %i and %i x %i, inclusive.\n",
            MIN, MIN, MAX, MAX);
        return 2;
    }

    // initialize the board
    init();

    // accept moves until game is won
    while (true)
    {
        // clear the screen
        clear();

        // draw the current state of the board
        draw();

        // saves the current state of the board (for testing)
        save();

        // check for win
        if (won())
        {
            printf("ftw!\n");
            break;
        }

        // prompt for move
        printf("Tile to move: ");
        int tile = GetInt();

        // move if possible, else report illegality
        if (!move(tile))
        {
            printf("\nIllegal move.\n");
            usleep(600000);
        }

        // sleep for animation's sake
        usleep(50000);
    }

    // that's all folks
    return 0;
}

/**
 * Clears screen using ANSI escape sequences.
 */
void clear(void)
{
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);
}

/**
 * Greets player.
 */
void greet(void)
{
    clear();
    printf("GAME OF FIFTEEN\n");
    usleep(600000);
}

/**
 * Initializes the game's board with tiles numbered 1 through d*d - 1,
 * (i.e., fills board with values but does not actually print them),
 * whereby board[i][j] represents row i and column j.
 */
void init(void)
{
    // init game board - using d, i, j - odd -> 1,2 , even -> 2,1
    int count=d*d-1;
    for(int i=0; i<d; i++){
        for(int j=0; j<d; j++){
            board[i][j] = count;
            count--;    
        }
    }
    if((d*d)%2==0){
        board[d-1][d-3] = 1;
        board[d-1][d-2] = 2;
    }
    board[d-1][d-1] = blank;
    bx = by = d-1;
    
    // init win_board
    count = 1;
    for(int i=0; i<d; i++){
        for(int j=0; j<d; j++){
            win_board[i][j] = count;
            count++;    
        }
    }
    win_board[d-1][d-1] = blank;
}

/**
 * Prints the board in its current state.
 */
void draw(void)
{
    printf("\n");
    for(int i=0; i<d; i++){
        for(int j=0; j<d; j++){
            if(board[i][j]==-1){
                printf("  %s", blank_char);
            } else if(board[i][j]>=10){
                printf("   %d", board[i][j]);
            } else{
                printf("   0%d", board[i][j]);
            }
        }
        printf("\n\n");
    }
}

/**
 * If tile borders empty space, moves tile and returns true, else
 * returns false. 
 */
bool move(int tile)
{
    bool moved = false;
    if(tile <= 0 || tile > (d*d)-1){
        return false;
    }
    
    //Find tile's current position
    bool found_tile = false;
    while(found_tile == false){
        for(int i=0; i<d; i++){
            for(int j=0; j<d; j++){
                if(tile == board[i][j]){
                    ty = i;
                    tx = j;
                    found_tile = true;
                    break;
                }    
            }
        }
    }
    
    //test if blank_pos is next to tile_pos
    bool found_b_pos = false;
    
    // up
    if(board[ty-1][tx] == blank){
        found_b_pos = true;
    }
    //down
    if(board[ty+1][tx] == blank){
        found_b_pos = true;
    }
    // left
    if(board[ty][tx-1] == blank){
        found_b_pos = true;
    }
    //right
    if(board[ty][tx+1] == blank){
        found_b_pos = true;
    }
    
    //if tile is next to blank, switch pos
    if(found_b_pos == true){
        board[by][bx] = board[ty][tx];
        by = ty;
        bx = tx;
        board[ty][tx] = blank;
        moved = true;
    }
    return moved;
}

/**
 * Returns true if game is won (i.e., board is in winning configuration), 
 * else false.
 */
bool won(void)
{
    bool won = false;
    bool winning = false;
    int win_count=1;
    
    for(int i=0; i<d; i++){
        for(int j=0; j<d; j++){
            if(board[i][j] == win_board[i][j]){
                winning = true;
            } else{
                winning = false;
                return won;
            }
            win_count++; 
        }
    }
    won = winning;
    return won;
}

/**
 * Saves the current state of the board to disk (for testing).
 */
void save(void)
{
    // log
    const string log = "log.txt";

    // delete existing log, if any, before first save
    static bool saved = false;
    if (!saved)
    {
        unlink(log);
        saved = true;
    }

    // open log
    FILE* p = fopen(log, "a");
    if (p == NULL)
    {
        return;
    }

    // log board
    fprintf(p, "{");
    for (int i = 0; i < d; i++)
    {
        fprintf(p, "{");
        for (int j = 0; j < d; j++)
        {
            fprintf(p, "%i", board[i][j]);
            if (j < d - 1)
            {
                fprintf(p, ",");
            }
        }
        fprintf(p, "}");
        if (i < d - 1)
        {
            fprintf(p, ",");
        }
    }
    fprintf(p, "}\n");

    // close log
    fclose(p);
}
