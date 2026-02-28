import sys

def terminal_state(board):
    # Проверка за ред или колона
    for i in range(3):
        if board[i][0] != '_' and board[i][0] == board[i][1] == board[i][2]:
            return board[i][0]
        if board[0][i] != '_' and board[0][i] == board[1][i] == board[2][i]:
            return board[0][i]
    
    # Диагонали
    if board[0][0] != '_' and board[0][0] == board[1][1] == board[2][2]:
        return board[0][0]
    if board[0][2] != '_' and board[0][2] == board[1][1] == board[2][0]:
        return board[0][2]
    
    if any('_' in row for row in board):
        return None
    return 'DRAW'

def minimax(board, depth, is_maximizing, alpha, beta):
    res = terminal_state(board)
    if res == 'O': return 10 - depth
    if res == 'X': return depth - 10
    if res == 'DRAW': return 0

    if is_maximizing: # Ред е на O
        best_score = -float('inf')
        for r in range(3):
            for c in range(3):
                if board[r][c] == '_':
                    board[r][c] = 'O'
                    score = minimax(board, depth + 1, False, alpha, beta)
                    board[r][c] = '_'
                    best_score = max(score, best_score)
                    alpha = max(alpha, score)
                    if best_score >= beta: break
        return best_score
    else: # Ред е на X
        best_score = float('inf')
        for r in range(3):
            for c in range(3):
                if board[r][c] == '_':
                    board[r][c] = 'X'
                    score = minimax(board, depth + 1, True, alpha, beta)
                    board[r][c] = '_'
                    best_score = min(score, best_score)
                    beta = min(beta, score)
                    if best_score <= alpha: break
        return best_score

def get_best_move(board, player):
    best_val = -float('inf') if player == 'O' else float('inf')
    move = (-1, -1)
    for r in range(3):
        for c in range(3):
            if board[r][c] == '_':
                board[r][c] = player
                score = minimax(board, 0, player == 'X', -float('inf'), float('inf'))
                board[r][c] = '_'
                if (player == 'O' and score > best_val) or (player == 'X' and score < best_val):
                    best_val = score
                    move = (r + 1, c + 1)
    return move

def print_board(board):
    for row in board:
        print("+---+---+---+")
        print(f"| {' | '.join(row)} |")
    print("+---+---+---+")


def read_board():
    board = []
    count = 0
    while count < 7:
        line = sys.stdin.readline()
        if not line: break # Защита срещу празен вход
        line = line.strip()
        if '|' in line:
            # Премахваме празни низове и взимаме само символите X, O или _
            parts = line.split('|')
            row = []
            for p in parts:
                clean_p = p.strip()
                if clean_p in ['X', 'O', '_']:
                    row.append(clean_p)
            if row:
                board.append(row)
        count += 1
    return board

def main():
    mode = sys.stdin.readline().strip()
    if "JUDGE" in mode:
        turn_line = sys.stdin.readline().strip()
        player = 'O' if 'O' in turn_line else 'X'
        board = read_board()
        if terminal_state(board):
            print("-1")
        else:
            r, c = get_best_move(board, player)
            print(f"{r} {c}")

    elif "GAME" in mode:
        first = 'O' if 'O' in sys.stdin.readline() else 'X'
        human = 'O' if 'O' in sys.stdin.readline() else 'X'
        board = [['_' for _ in range(3)] for _ in range(3)]
        read_board() # Изчистваме началната празна дъска от входа
        
        current = first
        while not terminal_state(board):
            print_board(board)
            if current == human:
                r, c = map(int, sys.stdin.readline().split())
                board[r-1][c-1] = human
            else:
                r, c = get_best_move(board, current)
                board[r-1][c-1] = current
            current = 'O' if current == 'X' else 'X'
        
        print_board(board)
        res = terminal_state(board)
        if res == 'DRAW': print("DRAW")
        else: print(f"WINNER: {res}")

if __name__ == "__main__":
    main()
