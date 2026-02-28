import time
import math

class SliderPuzzle:
    def __init__(self, board, n, goal_zero_idx):
        self.n = n
        self.size = int(math.sqrt(n + 1))
        self.board = board
        self.goal_zero_idx = goal_zero_idx if goal_zero_idx != -1 else n
        self.path = []

    def get_manhattan_distance(self, board):
        distance = 0
        for i in range(len(board)):
            val = board[i]
            if val == 0:
                continue
            
            target_idx = val - 1 if val <= self.goal_zero_idx else val
            
            curr_row, curr_col = divmod(i, self.size)
            target_row, target_col = divmod(target_idx, self.size)
            
            distance += abs(curr_row - target_row) + abs(curr_col - target_col)
        return distance

    def is_solvable(self):
        inversions = 0
        flat_board = [x for x in self.board if x != 0]
        for i in range(len(flat_board)):
            for j in range(i + 1, len(flat_board)):
                if flat_board[i] > flat_board[j]:
                    inversions += 1
        
        zero_row = self.board.index(0) // self.size
        goal_zero_row = self.goal_zero_idx // self.size
        
        if self.size % 2 != 0:
            return inversions % 2 == 0
        else:
            return (inversions + abs(zero_row - goal_zero_row)) % 2 == 0

    def get_neighbors(self, pos):
        neighbors = []
        r, c = divmod(pos, self.size)
        moves = [
            (r, c + 1, 'left'),   
            (r, c - 1, 'right'),  
            (r + 1, c, 'up'),     
            (r - 1, c, 'down')    
        ]
        
        for nr, nc, move in moves:
            if 0 <= nr < self.size and 0 <= nc < self.size:
                neighbors.append((nr * self.size + nc, move))
        return neighbors

    def solve(self):
        if not self.is_solvable():
            return -1

        threshold = self.get_manhattan_distance(self.board)
        while True:
            result, t = self.search(self.board, 0, threshold, -1)
            if result == "FOUND":
                return len(self.path), self.path
            if t == float('inf'):
                return -1
            threshold = t

    def search(self, board, g, threshold, last_pos):
        f = g + self.get_manhattan_distance(board)
        if f > threshold:
            return "NOT_FOUND", f
        if f == g and self.get_manhattan_distance(board) == 0:
            return "FOUND", threshold

        min_val = float('inf')
        zero_pos = board.index(0)
        
        for next_pos, move in self.get_neighbors(zero_pos):
            if next_pos == last_pos:
                continue
            
            # Swap
            new_board = list(board)
            new_board[zero_pos], new_board[next_pos] = new_board[next_pos], new_board[zero_pos]
            
            self.path.append(move)
            res, t = self.search(new_board, g + 1, threshold, zero_pos)
            if res == "FOUND":
                return "FOUND", threshold
            if t < min_val:
                min_val = t
            self.path.pop()
            
        return "NOT_FOUND", min_val

def main():
    try:
        n = int(input())
        goal_idx = int(input())
        board = []
        size = int(math.sqrt(n + 1))
        for _ in range(size):
            board.extend(map(int, input().split()))

        start_time = time.time()
        puzzle = SliderPuzzle(board, n, goal_idx)
        solution = puzzle.solve()

        if solution == -1:
            print("-1")
        else:
            steps_count, steps = solution
            print(steps_count)
            for step in steps:
                print(step)
        
        # print(f"Time: {time.time() - start_time:.4f}s") # Опция за време

    except EOFError:
        pass

if __name__ == "__main__":
    main()