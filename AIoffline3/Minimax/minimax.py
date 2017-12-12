
def draw_board(board):
    print(end='  ')
    #print the row heading
    for i in range(8):
        print(i+1, end=' ')
    print()
    for i in range(8):
        print(i+1, end=' ')
        for j in range(8):
            print(board[i][j], end=' ')
        print()


def init_board():
    board = [['-']*8 for i in range(8)]
    board[3][3], board[3][4], board[4][3], board[4][4] = 'w', 'b', 'b', 'w'
    return board


def main():
    print('Hello world')
    board = init_board()
    draw_board(board)


if __name__ == "__main__":
    main()