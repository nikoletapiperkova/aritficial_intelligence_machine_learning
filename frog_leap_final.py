def start_state(n):
    return ">"*n+"_"+"<"*n

def goal_state(n):
    return "<"*n+"_"+">"*n

def print_path(path):
    for row in path:
        print(row)

def neighbours_research(state):

    s = list(state)
    b = s.index("_")
    result = []

    if b-2 >= 0 and s[b-2] == ">" and s[b-1] in "<>":
        t = s[:]
        t[b], t[b - 2] = t[b-2], t[b]
        result.append("".join(t))

    if b+2 < len(s) and s[b+2] == "<" and s[b+1] in "<>":
        t = s[:]
        t[b], t[b+2] = t[b+2], t[b]
        result.append("".join(t))

    if b-1 >= 0 and s[b-1] == ">":
        t = s[:]
        t[b], t[b-1] = t[b-1], t[b]
        result.append("".join(t))

    if b+1 < len(s) and s[b+1] == "<":
        t = s[:]
        t[b], t[b+1] = t[b+1], t[b]
        result.append("".join(t))
        
    return result



def solver(n):
    start = start_state(n)
    goal = goal_state(n)
    visited = set()
    path = []
        
    def dfs(state: str) -> bool:
        if state in visited:
            return False
        visited.add(state)
        path.append(state)
        if state == goal:
            return True

        for nxt in neighbours_research(state):            
            if dfs(nxt):
                return True
        path.pop()
        return False

    dfs(start)
    return path


      
print("Enter the number of frogs n:")
n = int(input())  


path = solver(n)
for step in path:
   print(step) 