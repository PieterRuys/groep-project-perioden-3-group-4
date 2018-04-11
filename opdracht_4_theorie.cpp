#include <iostream>
#include <iomanip>

using namespace std;

const int board_width = 5;
const int board_heigth = 5;

char board[board_width][board_heigth];

struct Pos {
    int x;
    int y;
};

void init_board() {

    for ( int x = 0; x < board_width; x++ ) {
        for ( int y = 0; y < board_heigth; y++ ) {
            board[x][y] = '~';
        }
    }

    switch ( 2 ) {
    case 0:
        board[3][0] = 'X';
        board[2][1] = 'X';
        board[2][3] = 'X';
        break;
    case 1:
        board[3][0] = 'X';
        board[2][1] = 'X';
        board[1][2] = 'X';
        board[3][3] = 'X';
        board[3][4] = 'X';
        break;
    case 2:
        board[1][1] = 'X';
        board[2][2] = 'X';
        board[3][2] = 'X';
        board[4][2] = 'X';
        break;
    }
}

bool pos_move_one_step(struct Pos &pos, int direction) {
    struct Pos new_pos = pos;

    switch ( direction) {
    case 0: // east
        new_pos.x++;
        break;
    case 1: // south
        new_pos.y++;
        break;
    case 2: // west
        new_pos.x--;
        break;
    case 3: // north
        new_pos.y--;
        break;
    }

    // Are we still on the board?
    if ( new_pos.x < 0 || new_pos.x >= board_width ) return false;
    if ( new_pos.y < 0 || new_pos.y >= board_heigth ) return false;

    // Is the position not already done?
    if ( board[new_pos.x][new_pos.y] != '~' ) return false;

    pos = new_pos;

    return true;
}

void print_board(struct Pos rob_pos, int direction) {
    for ( int y = 0; y < board_heigth; y++ ) {
        for ( int x = 0; x < board_width; x++ ) {
            if ( x == rob_pos.x && y == rob_pos.y ) {
                switch ( direction) {
                case 0: // east
                    cout << setw(4) << '>';
                    break;
                case 1: // south
                    cout << setw(4) << 'V';
                    break;
                case 2: // west
                    cout << setw(4) << '<';
                    break;
                case 3: // north
                    cout << setw(4) << '^';
                    break;
                }
            }
            else {
                cout << setw(4) << board[x][y];
            }
        }
        cout << endl;
    }
    cout << endl;
}

bool run(struct Pos rob_pos, int direction) {

    board[rob_pos.x][rob_pos.y] = '*'; // Mark 'been here'

    print_board(rob_pos, direction);

    if ( rob_pos.x == board_width - 1 && rob_pos.y == board_heigth - 1 ) {
        cout << "Hurray!" << endl;
        return true;
    }

    if ( pos_move_one_step(rob_pos, direction) ) {
        for ( int d = 0; d < 4; d++ ) {
            if ( run(rob_pos, d) ) {
                return true;
            }
        }
    }

    board[rob_pos.x][rob_pos.y] = 'X'; // Give up this space

    return false;
}

int main() {

    init_board();
    struct Pos rob_pos = {0, 0};

    for ( int direction = 0; direction < 4 ; direction++ ) {
        if ( run(rob_pos, direction) ) {
            break;
        }
    }

    return 0;
}
