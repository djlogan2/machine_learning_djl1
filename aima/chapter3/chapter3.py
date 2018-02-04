__author__ = 'david'

import numpy as np
import math
import bisect
import time

class State:
    def __init__(self, board, action, current_cost, parent):
        self._board = board
        self._action = action
        self._transitions = []
        self._zero = np.concatenate(np.where(board._board == 0))
        self._parent = parent
        self._children = None
        self._current_cost = current_cost
        self._cost = None

    def __lt__(self, other):
        return self.cost < other.cost

    @property
    def cost(self):
        if self._action is None:
            return 999999999
        if self._cost is not None:
            return self._cost
        self._cost = self._current_cost
        newboard = self._board.newBoard(self._action[0], self._action[1], self._zero[0], self._zero[1])
        for n in range(1, board._side ** 2):
            final = int((n-1) / board._side)
            final = [final, ((n-1) % board._side)]
            l = np.where(newboard._board == n)
            this_cost = abs(final[0] - l[0][0]) + abs(final[1] - l[1][0])
            self._cost += this_cost
        return self._cost

    @property
    def depth(self):
        if self._parent == None:
            return 0
        else:
            return self._parent.depth + 1

    def same(self, node):
        b1 = node.board.ravel()
        b2 = self.board.ravel()
        w = b1.shape[0]
        for x in range(w):
            if b1[x] != b2[x]:
                return False
        return True

    @property
    def won(self):
        return self._board.won

    @property
    def board(self):
        return self._board._board

    @property
    def children(self):
        if self._children != None:
            return self._children
        if self._zero[0] != 0:
            self._transitions.append((self._zero[0]-1, self._zero[1]))
        if self._zero[0] < self._board._side - 1:
            self._transitions.append((self._zero[0]+1, self._zero[1]))
        if self._zero[1] != 0:
            self._transitions.append((self._zero[0], self._zero[1]-1))
        if self._zero[1] < self._board._side - 1:
            self._transitions.append((self._zero[0], self._zero[1]+1))
        self._children = []
        for transition in self._transitions:
            s = State(self._board.newBoard(transition[0], transition[1], self._zero[0], self._zero[1]), transition, self.cost, self)
            self._children.append(s)
        return self._children

class Board:
    @staticmethod
    def initialboard(side):
        board = Board()
        board._side = side
        while True:
            board._board = np.arange(side*side)
            np.random.shuffle(board._board)
            board._board = board._board.reshape((side, side))
            if board.solvable:
                return board

    @staticmethod
    def setboard(array):
        board = Board()
        board._board = np.array(array)
        board._side = board._board.shape[0]
        return board

    def newBoard(self, fy, fx, ty, tx):
        board = np.array(self._board)
        board[ty][tx] = board[fy][fx]
        board[fy][fx] = 0
        newboard = Board()
        newboard._side = self._side
        newboard._board = board
        return newboard

    @property
    def board(self):
        return self._board

    @property
    def solvable(self):
        inversions = 0
        r = self._board.ravel()
        for x in range(r.shape[0]):
            for y in range(x + 1, r.shape[0]):
                if r[y] != 0 and r[x] > r[y]:
                    inversions += 1

        if self._side % 2 == 1 and inversions % 2 == 0:
            return True

        if self._side % 2 == 0:
            zero = np.concatenate(np.where(self._board == 0))
            return (self._side - zero[0]) % 2 != inversions % 2

        return False

    @property
    def won(self):
        n = 1
        for y in range(self._side):
            for x in range(self._side):
                if self._board[y,x] != 0 and self._board[y,x] != n:
                    return False
                elif self._board[y,x] != 0:
                    n = n + 1 # We already know we have a matching number from above
        return True

explored = {}
frontier_dict = {}
frontier = []
solutions = []


def push_frontier(node):
    frontier.append(node)
    frontier_dict[str(node.board)] = 1


def pop_frontier():
    node = frontier.pop(0)
    del frontier_dict[str(node.board)]
    return node


def insert_frontier(node):
    bisect.insort_left(frontier, node)
    frontier_dict[str(node.board)] = 1


def in_frontier(node):
    return str(node.board) in frontier_dict


def in_explored(node):
    return str(node.board) in explored


def show_solution(child):
    if child._parent != None:
        show_solution(child._parent)
    elif child._parent == None:
        print('Initial board: ' + repr(child.board))
        return
    print('Move ' + repr(child._action) + ' giving new board ' + repr(child.board))

#board = Board.initialboard(4)
#board = Board.setboard([[7,3,0,1],[8,11,14,6],[9,15,2,12],[4,5,10,13]])
board = Board.setboard([[6,5,8],[1,0,4],[7,2,3]])
#board =Board.setboard([[10,3,14,1],[8,5,4,15],[2,6,0,11],[12,9,13,7]])

def breadth_first():
    max_depth = 0
    node_count = 0
    if not board.solvable:
        print('No point. Board is not solvable')
        return

    push_frontier(State(board, None, 0, None))

    m_ekl = 0
    m_fl = 0
    m_sl = 0

    while len(frontier) != 0:
        node = pop_frontier()
        if node.depth > max_depth:
            max_depth = node.depth
            print('Evaluated ' + repr(node_count) + ' nodes, starting to evaluate depth ' + repr(max_depth) + ', and there are ' + repr(len(frontier)) + ' nodes on the frontier')
            node_count = 0
        node_count += 1
        explored[repr(node.board)] = 1
        ekl = len(explored.keys())
        fl = len(frontier)
        sl = len(solutions)
        if m_ekl < ekl:
            m_ekl = ekl
        if m_fl < fl:
            m_fl = fl
        if m_sl < sl:
            m_sl = sl
        for child in node.children:
            if child.won:
                solutions.append(child)
                show_solution(child)
                print('-------------')
                print(repr(m_ekl) + ' states explored, ' + repr(m_fl) + ' on the frontier, ' + repr(m_sl) + ' solutions found')
                return
            elif not in_explored(child) and not in_frontier(child):
                push_frontier(child)

depth = 0
max_depth = 0
xxx_max_depth = 0
min_cost = 999999999

def state_already_seen(check_node, current_node):
    if np.array_equal(check_node.board, current_node.board):
        return True
    if current_node._parent == None:
        return False
    return state_already_seen(check_node, current_node._parent)


def find_depth_answer(node):
    global depth
    global max_depth
    global xxx_max_depth
    depth += 1

    if depth > max_depth:
        max_depth = depth
        print('Max depth=' + repr(max_depth))

    if node.won:
        solutions.append(node)
        show_solution(node)
        print('-------------')
        depth -= 1
        return
    if len(node.children) == 0:
        print('Ran out of moves at depth ' + repr(depth))
        depth -= 1
        return # No solution
    if xxx_max_depth != 0 and depth >= xxx_max_depth:
        depth -= 1
        return
    for child in node.children:
        if not state_already_seen(child, child._parent):
            find_depth_answer(child)
    depth -= 1
    return

def depth_first():
    global xxx_max_depth
    if not board.solvable:
        print('No point. Board is not solvable')
        return

    while len(solutions) == 0:
        if xxx_max_depth != 0:
            xxx_max_depth += 1
        print('Max depth=' + repr(xxx_max_depth))
        root = State(board, None, 0, None)
        find_depth_answer(root)
    print(repr(len(solutions)) + ' solutions found, max depth is ' + repr(max_depth))

def a_star():
    max_depth = 0
    node_count = 0
    if not board.solvable:
        print('No point. Board is not solvable')
        return

    insert_frontier(State(board, None, 0, None))

    m_ekl = 0
    m_fl = 0
    m_sl = 0

    while len(frontier) != 0:
        node = pop_frontier()
        if node.depth > max_depth:
            max_depth = node.depth
            print('Evaluated ' + repr(node_count) + ' nodes, starting to evaluate depth ' + repr(max_depth) + ', and there are ' + repr(len(frontier)) + ' nodes on the frontier')
            node_count = 0
        node_count += 1
        explored[repr(node.board)] = 1
        ekl = len(explored.keys())
        fl = len(frontier)
        sl = len(solutions)
        if m_ekl < ekl:
            m_ekl = ekl
        if m_fl < fl:
            m_fl = fl
        if m_sl < sl:
            m_sl = sl
        for child in node.children:
            if child.won:
                solutions.append(child)
                show_solution(child)
                print('-------------')
                print(repr(m_ekl) + ' states explored, ' + repr(m_fl) + ' on the frontier, ' + repr(m_sl) + ' solutions found')
                return
            elif not in_explored(child) and not in_frontier(child):
                insert_frontier(child)
start = time.clock()
#depth_first()
#breadth_first()
a_star()
print(repr(time.clock()-start) + ' seconds')
