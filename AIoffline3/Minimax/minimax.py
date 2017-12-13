
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
    sb, sw = get_score(dup_board)
    print('Black:',sb,'White:',sw)
    print('--------------------------------------')


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

    cells_to_flip = []
    if board[x][y] != '-' or not is_on_board(x,y):
        return cells_to_flip
    if color == 'w':
        other_color = 'b'
    else:
        other_color = 'w'

    #now check for the 8 directions..........
    #by shifting horizontally -1,0, 1

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
    return cells_to_flip


def get_child_board(board, color, x, y):
    dup_board = get_board_copy(board)
    cells_to_flip = is_valid_move(dup_board, color, x, y)
    if len(cells_to_flip) > 0:
        dup_board[x][y] = color
        for i, j in cells_to_flip:
            dup_board[i][j] = color  # flip the cell
        return dup_board
    return dup_board


def make_move(board, color, x, y):
    global turn
    cells_to_flip = is_valid_move(board, color, x, y)
    if len(cells_to_flip) > 0:
        board[x][y] = color
        for i, j in cells_to_flip:
            board[i][j] = color #flip the cell

        #toggle the turn
        toggle_turn()
        return True
    return False


def get_valid_moves(board, color):
    valid_moves = []
    for x in range(8):
        for y in range(8):
            if len(is_valid_move(board, color, x, y)) > 0:
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


#---basic functionalities end here--

def ask_for_move(board, human=False):
    global turn, turn_failed
    if len(get_valid_moves(board, turn)) == 0:
        toggle_turn()
        turn_failed += 1
        return

    x, y = 0, 0
    if human:
        print('Turn for '+str(turn)+': ', end='')
        string = input()
        x = int(string[0])
        y = int(string[1])
    else:
        x, y = get_ai_move(board, turn)
    make_move(board, turn, x, y)
    turn_failed = 0


#positional heuristic
def heuristic1(board, color):
    weight = [[4,-3,2,2,2,2,-3,4],
              [-3,-4,-1,-1,-1,-1,-4,-3],
              [2,-1,1,0,0,1,-1,2],
              [2,-1,0,1,1,0,-1,2],
              [2,-1,0,1,1,0,-1,2],
              [2,-1,1,0,0,1,-1,2],
              [-3,-4,-1,-1,-1,-4,-3],
              [4,-3,2,2,2,2,-3,4]]

    value = 0
    if color == 'b':
        other_color = 'w'
    else:
        other_color = 'b'

    for x in range(8):
        for y in range(8):
            if board[x][y] == color:
                value += weight[x][y]
            elif board[x][y] == other_color:
                value -= weight[x][y]

    return value


#mobility heuristic
def heuristic2(board, color):

    if color == 'b':
        other_color = 'w'
    else:
        other_color = 'b'

    w1 = 10
    w2 = 1

    my_mob = len(get_valid_moves(board, color))
    other_mob = len(get_valid_moves(board, other_color))

    my_c = 0
    other_c = 0
    for x, y in [(0,0), (0,7), (7,0),(7,7)]:
        if board[x][y] == color:
            my_c+= 1
        elif board[x][y] == other_color:
            other_c+= 1

    value = w1*(my_c-other_c) + w2*(my_mob-other_mob)*1.0/(my_mob+other_mob)
    return value


#absolute count heuristic
def heuristic3(board, color):
    sb, sw = get_score(board)
    if color == 'b':
        return sb-sw
    else:
        return sw-sb


def heuristic(board, color):
    global heuristic_num
    if heuristic_num == 1:
        return heuristic1(board,color)
    elif heuristic_num == 2:
        return heuristic2(board,color)
    #3 or default
    return heuristic3(board,color)


def get_score(board):
    b_score = 0
    w_score = 0
    for x in range(8):
        for y in range(8):
            if board[x][y] == 'b':
                b_score+= 1
            elif board[x][y] == 'w':
                w_score+= 1
    return b_score, w_score


def get_ai_move(board, color):
    if color == 'b':
        max_depth = depth1
        name = 'Black'
    else:
        max_depth = depth2
        name = 'White'

    next_move =  minimax(board, color, 0, True, -INFINITY, INFINITY, max_depth)
    print(name, 'selected ',next_move)
    return next_move


def is_leaf(node, color, depth, max_depth):
    return len(get_valid_moves(node, color)) == 0 or depth >= max_depth


def minimax(node, color, depth, isMaximizingPlayer, alpha, beta, max_depth):
    next_move = (0,0)

    if is_leaf(node, color, depth, max_depth):
        return heuristic(node, color)

    if color == 'b':
        other_color = 'w'
    else:
        other_color = 'b'

    if isMaximizingPlayer:
        bestVal = -INFINITY
        for x,y in get_valid_moves(node, color):
            node = get_child_board(node, color, x, y)
            value = minimax(node, other_color, depth + 1, False, alpha, beta, max_depth)
            prev_best = bestVal
            bestVal = max(bestVal, value)
           #for the root, we need the child for which we got the best value-------------------------
            if depth == 0 and value > prev_best:
                next_move = (x,y)
            alpha = max(alpha, bestVal)
            if beta <= alpha:
                break
        #for the root------------------
        if depth==0:
            return next_move
        return bestVal

    else:
        bestVal = +INFINITY
        for x, y in get_valid_moves(node, color):
            node = get_child_board(node, color, x, y)
            value = minimax(node, color, depth + 1, True, alpha, beta, max_depth)
            bestVal = min(bestVal, value)
            beta = min(beta, bestVal)
            if beta <= alpha:
                break
        return bestVal


def run_game(board):
    global turn, turn_failed
    draw_board(board)
    while turn_failed < 2:
        ask_for_move(board)
        if turn_failed==0:
            draw_board(board)


#----------------------
# -global variables
turn = 'b'
turn_failed = 0
heuristic_num = 3
depth1 = 6
depth2 = 6
INFINITY = 99999
#----------------------





def toggle_turn():
    global turn
    if turn == 'b':
        turn = 'w'
    else:
        turn = 'b'


def init_globals():
    global turn, turn_failed, depth1, depth2
    turn = 'b'
    turn_failed = 0

    depth1 = 5
    depth2 = 2

    heuristic_num = 1


def main():
    global turn
    print('Welcome to Reversi')
    init_globals()
    board = init_board()
    run_game(board)
    #print(get_score(board))
    # make_move(board, turn, 3,2)
    # print(heuristic1(board, turn))


if __name__ == "__main__":
    main()