boardDim = 8


def initLetterToIndex():
    LTX = {}
    for i,l in enumerate("abcdefgh"):
        LTX[l] = i
    return LTX
letterToIndex = initLetterToIndex()
UNICODE_PIECES = {
  'r': u'♜', 'n': u'♞', 'b': u'♝', 'q': u'♛',
  'k': u'♚', 'p': u'♟', 'R': u'♖', 'N': u'♘',
  'B': u'♗', 'Q': u'♕', 'K': u'♔', 'P': u'♙',
  'X': u'X',
  None: ' '
}
def printBoard(board):
    
    for i in range(boardDim):
        #print(board[i])
        row = ""
        for j in range(boardDim):
            row += UNICODE_PIECES[board[i][j]]
        print(row)

points = {"K":100,"Q":10,"R":5,"B":3,"N":3,"P":1,"k":100,"q":10,"r":5,"b":3,"n":3,"p":1}


def availableMoves(piece,board):
    return

def initializeBoard():
    board = []
    for i in range(boardDim):
        l = [""]*boardDim
        board.append(l)
    #print(board)
    t1bottomRow = "RNBQKBNR"
    t2topRow = "rnbqkbnR"
    for i in range(boardDim):
        board[0][i] = t1bottomRow[i]
        board[7][i] = t2topRow[i]
        board[6][i] = "p"
        board[1][i] = "P"
    for i in range(2,6):
        for j in range(boardDim):
            board[i][j] = 'X'
    #print(board)
    return board
BOARD = initializeBoard()
printBoard(BOARD)