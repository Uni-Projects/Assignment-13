#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <vector>

using namespace std;

enum Cell {empty=0, wall, destination, worker, box,worker_on_dest,box_on_dest};
enum Direction {north=0, south, east,west};

const int HEIGHT = 15;
const int WIDTH = 15;

int a_height;
int a_width;

Cell world [HEIGHT][WIDTH];

struct Pos
{
    int row;
    int col;
};

struct State
{
    Cell layout [HEIGHT][WIDTH];
    Pos player;
    vector <Pos> boxes;
};

struct Node
{
    State config ;
    int parent ;
};

bool open_file (fstream& file, string file_name)
{
   file.open(file_name.c_str());
   return file.is_open();
}

fstream& operator>> (fstream& file, Cell world[HEIGHT][WIDTH])
{// Pre-condition:
    assert (true) ;
/*  Post-condition:
    a value for length has been obtained from in: first minutes, followed by ':', followed by seconds.
*/  char c;

    for (int i = 0 ; i < HEIGHT ; i++)
    {
        for(int j = 0 ; j < WIDTH ; j++)
        {
            file.get(c);

            if (j == 0 && c != '*')
            {
                a_height = i;
                j = WIDTH;
                i = HEIGHT;
            }
            if (c == '\n')
            {
                 if (i == 0)
                    a_width = j;
                 j = WIDTH;
            }
            if (c == '*')
                world[i][j] = wall;
            if (c == ' ')
                world[i][j] = empty;
            if (c == '.')
                world[i][j] = destination;
            if (c == 'w')
                world[i][j] = worker;
            if (c == 'b')
                world[i][j] = box;
            if (c == 'W')
                world[i][j] = worker_on_dest;
            if (c == 'B')
                world[i][j] = box_on_dest;
        }
    }

}
bool operator== (Pos p1, Pos p2)
{
    return (p1.col == p2.col && p1.row == p2.row) ;
}
bool operator== (vector<Pos> b1, vector<Pos> b2)
{
    for (int i = 0 ; i < b1.size() ; i ++)
    {
        if (!(b1[i] == b2[i]) )
            return false;
    }
    return true;
}
bool operator== (State& s1, State& s2)
{
    return (s1.player == s2.player && s1.boxes == s2.boxes);
}

void show_world (Cell world[HEIGHT][WIDTH])
{// Pre-condition:
    assert (true) ;

    for (int i = 0 ; i < a_height ; i++)
    {
        for(int j = 0 ; j < a_width ; j++)
        {
            if (world[i][j] == wall)
                cout << "*";
            if ( world[i][j] == empty)
                cout << " ";
            if (world[i][j] == destination)
                cout << ".";
            if (world[i][j] == worker)
                cout << "w";
            if (world[i][j] == box)
                cout << "b";
            if (world[i][j] == worker_on_dest)
                cout << "W";
            if (world[i][j] == box_on_dest)
                cout << "B";
        }
        cout << endl ;
    }
}

State set_up (Cell w [HEIGHT][WIDTH])
{
    State config;

    for (int i = 0 ; i < a_height ; i++)
    {
        for(int j = 0 ; j < a_width ; j++)
        {
            Pos temp;
            config.layout[i][j] = w [i][j];

            if (config.layout[i][j] == worker || config.layout[i][j] == worker_on_dest )
            {
                config.player.row = i ;
                config.player.col = j ;
            }

            if (config.layout[i][j] == box || config.layout[i][j] == box_on_dest )
            {
                temp.row = i ;
                temp.col = j ;
                config.boxes.push_back(temp) ;
            }
        }
    }
    return config ;
}
/*
void find_corners (State& start)
{
    for (int i = 1 ; i < a_height-1 ; i++)
    {
        for (int j = 1 ; j < a_width-1 ; j++)
        {
            if (start.layout[i][j] == empty)
            {
               if (start.layout[i+1][j] == wall || start.layout[i-1][j] == wall)
                {
                  if (start.layout[i][j+1] == wall || start.layout[i][j-1] == wall)
                    start.layout[i][j] = corner;
                }
            }
        }
    }
}

*/
bool facing_box (State s, Direction d)
{
    switch (d)
        {

            case north: return ((s.layout[s.player.row - 1] [s.player.col] == box
                              || s.layout[s.player.row - 1] [s.player.col] == box_on_dest));

            case south: return ((s.layout[s.player.row + 1] [s.player.col] == box
                              || s.layout[s.player.row + 1] [s.player.col] == box_on_dest));

            case east:  return ((s.layout[s.player.row] [s.player.col + 1] == box
                              || s.layout[s.player.row] [s.player.col + 1] == box_on_dest));

            case west:  return ((s.layout[s.player.row] [s.player.col - 1] == box
                              || s.layout[s.player.row] [s.player.col - 1] == box_on_dest));
        }

}
bool is_movable (State s, Direction d)
{
    switch (d)
    {
        case north:
            return (s.layout[s.player.row - 2][s.player.col] == empty || s.layout[s.player.row - 2][s.player.col] == destination);
            break;
        case south:
            return (s.layout[s.player.row + 2][s.player.col] == empty || s.layout[s.player.row + 2][s.player.col] == destination);
            break;
        case east:
            return (s.layout[s.player.row][s.player.col + 2] == empty || s.layout[s.player.row][s.player.col + 2] == destination);
            break;
        case west:
            return (s.layout[s.player.row][s.player.col - 2] == empty || s.layout[s.player.row][s.player.col - 2] == destination);
            break;
    }
}

bool is_solved (State s)
{
    for (int i = 0 ; i < a_height ; i++)
    {
        for(int j = 0 ; j < a_width ; j++)
        {
           if (s.layout[i][j] == box)
            return false ;
        }
    }
    return true;
}

bool can_go_north(State s)
{
     return (s.layout[s.player.row - 1][s.player.col] == empty || s.layout[s.player.row - 1][s.player.col] == destination);
}

bool can_go_south(State s)
{
    return (s.layout[s.player.row + 1][s.player.col] == empty || s.layout[s.player.row + 1][s.player.col] == destination);
}

bool can_go_east(State s)
{
    return (s.layout[s.player.row][s.player.col + 1] == empty || s.layout[s.player.row][s.player.col + 1] == destination);
}

bool can_go_west(State s)
{
    return (s.layout[s.player.row][s.player.col - 1] == empty || s.layout[s.player.row][s.player.col - 1] == destination);
}

bool new_config (vector<Node>& n, State s)
{
    for(int j = 0 ; j < n.size(); j++)
    {
        if (n[j].config == s)
          return false;
    }
    return true;
}

void move_player (vector<Node>& n, int i, Direction d, State s)
{
    State newState = s;
    Node newNode;

    switch (d)
     {
            case north:
                if(newState.layout[newState.player.row][newState.player.col] == worker)
                {
                        if(newState.layout[newState.player.row - 1][newState.player.col] == empty)
                        {
                            newState.layout[newState.player.row - 1][newState.player.col] = worker;
                            newState.layout[newState.player.row][newState.player.col] = empty ;
                        }
                        if(newState.layout[newState.player.row - 1][newState.player.col] == destination)
                        {
                            newState.layout[newState.player.row - 1][newState.player.col] = worker_on_dest;
                            newState.layout[newState.player.row][newState.player.col] = empty ;
                        }
                }
                if(newState.layout[newState.player.row][newState.player.col] == worker_on_dest)
                {
                        if(newState.layout[newState.player.row - 1][newState.player.col] == empty)
                        {
                            newState.layout[newState.player.row - 1][newState.player.col] = worker;
                            newState.layout[newState.player.row][newState.player.col] = destination;
                        }
                        if(newState.layout[newState.player.row - 1][newState.player.col] == destination)
                        {
                            newState.layout[newState.player.row - 1][newState.player.col] = worker_on_dest;
                            newState.layout[newState.player.row][newState.player.col] = destination;
                        }
                }
                break;

            case south:
                if(newState.layout[newState.player.row][newState.player.col] == worker)
                {
                        if(newState.layout[newState.player.row + 1][newState.player.col] == empty)
                        {
                            newState.layout[newState.player.row + 1][newState.player.col] = worker;
                            newState.layout[newState.player.row][newState.player.col] = empty ;
                        }
                        if(newState.layout[newState.player.row + 1][newState.player.col] == destination)
                        {
                            newState.layout[newState.player.row + 1][newState.player.col] = worker_on_dest;
                            newState.layout[newState.player.row][newState.player.col] = empty ;
                        }
                }
                if(newState.layout[newState.player.row][newState.player.col] == worker_on_dest)
                {
                        if(newState.layout[newState.player.row + 1][newState.player.col] == empty)
                        {
                            newState.layout[newState.player.row + 1][newState.player.col] = worker;
                            newState.layout[newState.player.row][newState.player.col] = destination;
                        }
                        if(newState.layout[newState.player.row + 1][newState.player.col] == destination)
                        {
                            newState.layout[newState.player.row + 1][newState.player.col] = worker_on_dest;
                            newState.layout[newState.player.row][newState.player.col] = destination;
                        }
                }
                break;

            case east:
                if(newState.layout[newState.player.row][newState.player.col] == worker)
                {
                        if(newState.layout[newState.player.row][newState.player.col + 1] == empty)
                        {
                            newState.layout[newState.player.row][newState.player.col + 1] = worker;
                            newState.layout[newState.player.row][newState.player.col] = empty ;
                        }
                        if(newState.layout[newState.player.row][newState.player.col + 1] == destination)
                        {
                            newState.layout[newState.player.row][newState.player.col + 1] = worker_on_dest;
                            newState.layout[newState.player.row][newState.player.col] = empty ;
                        }
                }
                if(newState.layout[newState.player.row][newState.player.col] == worker_on_dest)
                {
                        if(newState.layout[newState.player.row][newState.player.col + 1] == empty)
                        {
                            newState.layout[newState.player.row][newState.player.col + 1] = worker;
                            newState.layout[newState.player.row][newState.player.col] = destination;
                        }
                        if(newState.layout[newState.player.row][newState.player.col + 1] == destination)
                        {
                            newState.layout[newState.player.row][newState.player.col + 1] = worker_on_dest;
                            newState.layout[newState.player.row][newState.player.col] = destination;
                        }
                }
                break;

            case west:
                if(newState.layout[newState.player.row][newState.player.col] == worker)
                {
                        if(newState.layout[newState.player.row][newState.player.col - 1] == empty)
                        {
                            newState.layout[newState.player.row][newState.player.col - 1] = worker;
                            newState.layout[newState.player.row][newState.player.col] = empty ;
                        }
                        if(newState.layout[newState.player.row][newState.player.col - 1] == destination)
                        {
                            newState.layout[newState.player.row][newState.player.col - 1] = worker_on_dest;
                            newState.layout[newState.player.row][newState.player.col] = empty ;
                        }
                }
                if(newState.layout[newState.player.row][newState.player.col] == worker_on_dest)
                {
                        if(newState.layout[newState.player.row][newState.player.col - 1] == empty)
                        {
                            newState.layout[newState.player.row][newState.player.col - 1] = worker;
                            newState.layout[newState.player.row][newState.player.col] = destination;
                        }
                        if(newState.layout[newState.player.row][newState.player.col - 1] == destination)
                        {
                            newState.layout[newState.player.row][newState.player.col - 1] = worker_on_dest;
                            newState.layout[newState.player.row][newState.player.col] = destination;
                        }
                }
                break;
     }

    newState = set_up(newState.layout);

    if (new_config(n, newState))
    {
        newNode.config = newState;
        newNode.parent = i;
        n.push_back(newNode) ;
    }
}

void move_box (vector<Node>& n, int i, Direction d)
{
    State newState = n[i].config;
    switch (d)
     {
            case north:
                if(newState.layout[newState.player.row - 1][newState.player.col] == box)
                {
                        if(newState.layout[newState.player.row - 2][newState.player.col] == empty)
                        {
                            newState.layout[newState.player.row - 2][newState.player.col] = box;
                            newState.layout[newState.player.row - 1][newState.player.col] = empty ;
                        }
                        if(newState.layout[newState.player.row - 2][newState.player.col] == destination)
                        {
                            newState.layout[newState.player.row - 2][newState.player.col] = box_on_dest;
                            newState.layout[newState.player.row - 1][newState.player.col] = empty ;
                        }
                }
                if(newState.layout[newState.player.row - 1][newState.player.col] == box_on_dest)
                {
                        if(newState.layout[newState.player.row - 2][newState.player.col] == empty)
                        {
                            newState.layout[newState.player.row - 2][newState.player.col] = box;
                            newState.layout[newState.player.row - 1][newState.player.col] = destination;
                        }
                        if(newState.layout[newState.player.row - 2][newState.player.col] == destination)
                        {
                            newState.layout[newState.player.row - 2][newState.player.col] = box_on_dest;
                            newState.layout[newState.player.row][newState.player.col] = destination;
                        }
                }
                break;

            case south:
                if(newState.layout[newState.player.row + 1][newState.player.col] == box)
                {
                        if(newState.layout[newState.player.row + 2][newState.player.col] == empty)
                        {
                            newState.layout[newState.player.row + 2][newState.player.col] = box;
                            newState.layout[newState.player.row + 1][newState.player.col] = empty ;
                        }
                        if(newState.layout[newState.player.row + 2][newState.player.col] == destination)
                        {
                            newState.layout[newState.player.row + 2][newState.player.col] = box_on_dest;
                            newState.layout[newState.player.row + 1][newState.player.col] = empty ;
                        }
                }
                if(newState.layout[newState.player.row + 1][newState.player.col] == box_on_dest)
                {
                        if(newState.layout[newState.player.row + 2][newState.player.col] == empty)
                        {
                            newState.layout[newState.player.row + 2][newState.player.col] = box;
                            newState.layout[newState.player.row + 1][newState.player.col] = destination;
                        }
                        if(newState.layout[newState.player.row + 2][newState.player.col] == destination)
                        {
                            newState.layout[newState.player.row + 2][newState.player.col] = box_on_dest;
                            newState.layout[newState.player.row + 1][newState.player.col] = destination;
                        }
                }
                break;

            case east:
                if(newState.layout[newState.player.row][newState.player.col + 1] == box)
                {
                        if(newState.layout[newState.player.row][newState.player.col + 2] == empty)
                        {
                            newState.layout[newState.player.row][newState.player.col + 2] = box;
                            newState.layout[newState.player.row][newState.player.col + 1] = empty ;
                        }
                        if(newState.layout[newState.player.row][newState.player.col + 2] == destination)
                        {
                            newState.layout[newState.player.row][newState.player.col + 2] = box_on_dest;
                            newState.layout[newState.player.row][newState.player.col + 1] = empty ;
                        }
                }
                if(newState.layout[newState.player.row][newState.player.col + 1] == box_on_dest)
                {
                        if(newState.layout[newState.player.row][newState.player.col + 2] == empty)
                        {
                            newState.layout[newState.player.row][newState.player.col + 2] = box;
                            newState.layout[newState.player.row][newState.player.col + 1] = destination;
                        }
                        if(newState.layout[newState.player.row][newState.player.col + 2] == destination)
                        {
                            newState.layout[newState.player.row][newState.player.col + 2] = box_on_dest;
                            newState.layout[newState.player.row][newState.player.col + 1] = destination;
                        }
                }
                break;

            case west:
                if(newState.layout[newState.player.row][newState.player.col - 1] == box)
                {
                        if(newState.layout[newState.player.row][newState.player.col - 2] == empty)
                        {
                            newState.layout[newState.player.row][newState.player.col - 2] = box;
                            newState.layout[newState.player.row][newState.player.col - 1] = empty ;
                        }
                        if(newState.layout[newState.player.row][newState.player.col - 2] == destination)
                        {
                            newState.layout[newState.player.row][newState.player.col - 2] = box_on_dest;
                            newState.layout[newState.player.row][newState.player.col - 1] = empty ;
                        }
                }
                if(newState.layout[newState.player.row][newState.player.col - 1] == box_on_dest)
                {
                        if(newState.layout[newState.player.row][newState.player.col - 2] == empty)
                        {
                            newState.layout[newState.player.row][newState.player.col - 2] = box;
                            newState.layout[newState.player.row][newState.player.col - 1] = destination;
                        }
                        if(newState.layout[newState.player.row][newState.player.col - 2] == destination)
                        {
                            newState.layout[newState.player.row][newState.player.col - 2] = box_on_dest;
                            newState.layout[newState.player.row][newState.player.col - 1] = destination;
                        }
                }
                break;
     }

    newState = set_up(newState.layout);
    move_player(n,i,d, newState);
}

void show_path (vector <Node>& n ,int i)
{
    if (i >= 0)
    {
        show_world(n[i].config.layout);
        cout << endl ;
        show_path(n, n[i].parent);
    }

}

void solve (State start)
{
    vector<Node> n = {{start,-1}} ;
    int i = 0 ;
    while(i < n.size() && !is_solved(n[i].config))
    {
        State s = n[i].config;
        if (facing_box(s,north) && is_movable(s,north))
        {
            move_box(n,i,north);
        }
        if (can_go_north(s))
            move_player(n,i,north,n[i].config);

        if (facing_box(s,south) && is_movable(s,south))
        {
            move_box(n,i,south);
        }
        if (can_go_south(s))
            move_player(n,i,south,n[i].config);

        if (facing_box(s,east) && is_movable(s,east))
        {
            move_box(n,i,east);
        }
        if (can_go_east(s))
            move_player(n,i,east,n[i].config);

        if (facing_box(s,west) && is_movable(s,west))
        {
            move_box(n,i,west);
        }
        if (can_go_west(s))
            move_player(n,i,west,n[i].config);

        //show_world(n[i].config.layout);
        //cout << endl;

        i++ ;

     }

      if(i < n.size())
        show_path(n, i) ;
}

int main()
{
    fstream file;
    string file_name;

    cout << "insert file name: ";

    getline(cin,file_name);

    if (open_file(file,file_name))
        cout << "File loaded correctly" << endl ;
    else cout << "File not loaded!" << endl ;

    file >> world ;

    file.close();

    State start = set_up(world);
    //find_corners(start);
    cout << endl ;
    show_world(start.layout);
    cout << endl ;
    cout << "Player position: " << endl ;
    cout << start.player.col << "," << start.player.row << endl;
    cout << "Boxes positions: " << endl ;

    for (int i = 0 ; i < start.boxes.size(); i++)
    {
        cout << start.boxes[i].col << "," << start.boxes[i].row << endl;
    }

    solve(start);

    return 0;
}
