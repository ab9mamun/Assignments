
def draw_board(board):
    global turn
    dup_board = get_board_with_valid_moves(board, turn)
    print(end='  ')
    #print the row heading
    for i in range(8):
        print(i, end=' ')
    print()
    for i in range(8):
        print(i, end=' ')
        for j in range(8):
            print(dup_board[i][j], end=' ')
        print()


def init_board():
    board = [['-']*8 for i in range(8)]
    board[3][3], board[3][4], board[4][3], board[4][4] = 'w', 'b', 'b', 'w'
    return board

#returns true if the coordinates are on the board
def is_on_board(x, y):
    return x>=0 and x<8 and y>=0 and y<8


#returns false if a move is not valid on this place
#else return the positions where cells will get flipped
def is_valid_move(board, color, x, y):

    if board[x][y] != '-' or not is_on_board(x,y):
        return False
    if color == 'w':
        other_color = 'b'
    else:
        other_color = 'w'

    #now check for the 8 directions..........
    #by shifting horizontally -1,0, 1
    cells_to_flip = []
    directions = [] #holds all the directions
    for x_d in range(-1,2):
        for y_d in range(-1,2):
            directions.append((x_d, y_d))
    #this will add an additional 0, 0.. which we can ignore
    for x_d, y_d in directions:
        i, j = x, y
        #get the position of the adjacent cell
        i+= x_d
        j+= y_d
        if is_on_board(i,j) and board[i][j] == other_color:
            #the adjacent cell is other color, so possibility
            i+= x_d
            j+= y_d
            if not is_on_board(i,j):
                continue
            while board[i][j] == other_color:
                i+= x_d
                j+= y_d
                if not is_on_board(i,j): # out of the board
                    break
            if not is_on_board(i,j):
                continue
            if board[i][j] == color:
                #found my color in a cell... all the intermediate cells will be replaced
                while True:
                    i-= x_d
                    j-= y_d
                    if i== x and j == y:
                        break
                    cells_to_flip.append((i,j))
    #check if any cell is to be flipped, if no, move is not valid here
    if len(cells_to_flip) == 0:
        return False
    return cells_to_flip


def get_child_board(board, color, x, y):
    dup_board = get_board_copy(board)
    cells_to_flip = is_valid_move(dup_board, color, x, y)
    if cells_to_flip:
        dup_board[x][y] = color
        for i, j in cells_to_flip:
            dup_board[i][j] = color  # flip the cell
        return dup_board
    return False


def make_move(board, color, x, y):
    global turn
    cells_to_flip = is_valid_move(board, color, x, y)
    if cells_to_flip:
        board[x][y] = color
        for i, j in cells_to_flip:
            board[i][j] = color #flip the cell

        #toggle the turn
        if turn == 'b':
            turn = 'w'
        else:
            turn = 'b'
        return True
    return False


def get_valid_moves(board, color):
    valid_moves = []
    for x in range(8):
        for y in range(8):
            if is_valid_move(board, color, x, y):
                valid_moves.append((x,y))
    return valid_moves


def get_board_copy(board):
    dup_board = init_board()
    for x in range(8):
        for y in range(8):
            dup_board[x][y] = board[x][y]
    return dup_board


#returns a new board with # marking the valid moves
def get_board_with_valid_moves(board, color):
    dup_board = get_board_copy(board)
    for x,y in get_valid_moves(dup_board, color):
        dup_board[x][y] = '#'
    return dup_board


def ask_for_move(board, human=False):
    global turn
    x, y = 0, 0
    if human:
        print('Turn for '+str(turn)+': ', end='')
        string = input()
        x = int(string[0])
        y = int(string[1])
    make_move(board, turn, x, y)


def run_game(board):
    global turn
    draw_board(board)
    while True:
        ask_for_move(board, True)
        draw_board(board)


#----------------------
# -global variables
turn = 'b'
#----------------------


def main():
    global turn
    print('Hello world')
    board = init_board()
    run_game(board)


if __name__ == "__main__":
    main()