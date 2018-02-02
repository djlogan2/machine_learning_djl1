__author__ = 'david'


class State:
    def __init__(self, left, right, boat):
        self._left = left
        self._right = right
        self._boat = boat
        pass

    @property
    def str(self):
        return repr(self._left) + '/' + repr(self._right) + '/' + repr(self._boat)

    @property
    def legal_moves(self):
        legalmoves = []
        possibles = [[1, 0], [2, 0], [1, 1], [0, 1], [0, 2]]
        for possible in possibles:
            if self._boat == 0:
                left = [self._left[0] - possible[0], self._left[1] - possible[1]]
                right = [self._right[0] + possible[0], self._right[1] + possible[1]]
            else:
                left = [self._left[0] + possible[0], self._left[1] + possible[1]]
                right = [self._right[0] - possible[0], self._right[1] - possible[1]]
            if 0 <= left[0] <= 3 and 0 <= left[1] <= 3 and \
                0 <= right[0] <= 3 and 0 <= right[1] <= 3 and \
                    (left[0] == 0 or left[0] >= left[1]) and (right[0] == 0 or right[0] >= right[1]):
                legalmoves.append(possible)
        return legalmoves

    def makemove(self, move):
            if self._boat == 0:
                left = [self._left[0] - move[0], self._left[1] - move[1]]
                right = [self._right[0] + move[0], self._right[1] + move[1]]
            else:
                left = [self._left[0] + move[0], self._left[1] + move[1]]
                right = [self._right[0] - move[0], self._right[1] - move[1]]
            return State(left, right, abs(self._boat - 1))

    @property
    def won(self):
        return self._left[0] == 0 and self._left[1] == 0 and self._right[0] == 3 and self._right[1] == 3


class Node:
    def __init__(self, parent, move, state):
        self._parent = parent
        self._state = state
        self._move = move

    @property
    def children(self):
        legalmoves = self._state.legal_moves
        children = []
        if len(legalmoves) ==0:
            return []
        for move in legalmoves:
            children.append(Node(self, move, self._state.makemove(move)))
        return children

    @property
    def won(self):
        return self._state.won

def show_solution(node):
    if node._parent is not None:
        show_solution(node._parent)
    print("Move " + repr(node._move))

def run():
    explored = {}
    frontier = {}
    queue = []
    starting_position = State([3, 3], [0, 0], 0)
    starting_node = Node(None, None, starting_position)
    queue.append(starting_node)
    frontier[starting_position.str] = 1
    while len(queue) != 0:
        node = queue.pop(0)
        frontier.pop(node._state.str)
        explored[node._state.str] = 1
        if node.won:
            show_solution(node)
            return
        children = node.children
        for child in children:
            try:
                frontier[child._state.str]
            except KeyError:
                try:
                    explored[child._state.str]
                except KeyError:
                    queue.append(child)
                    frontier[child._state.str] = 1

run()
