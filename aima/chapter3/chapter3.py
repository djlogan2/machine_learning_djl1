__author__ = 'david'

import numpy as np

class State:
    def __init__(self, board, parent=None):
        self._board = board
        self._transitions = []
        self._zero = np.concatenate(np.where(board._board == 0))
        self._parent = parent
        self._children = None

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
        self._removeDuplicateStates()
        p = self if self._parent != None else None
        self._children = []
        for transition in self._transitions:
            s = State(self._board.newBoard(transition[0], transition[1], self._zero[0], self._zero[1]), parent=p)
            self._children.append(s)
        return self._children

    def _removeDuplicateStates(self):
        if self._parent == None:
            return
        newTransitions = []
        for transition in self._transitions:
            newBoard = self._board.newBoard(transition[0], transition[1], self._zero[0], self._zero[1])
            p = self._parent
            iss = False
            while p != None and p != True:
                if np.array_equal(p._board._board, newBoard._board):
                    iss = True
                    p = None
                else:
                    p = p._parent
            if not iss:
                newTransitions.append(transition)
        self._transitions = newTransitions

class Board:
    @staticmethod
    def initialboard(side):
        board = Board()
        board._side = side
        board._board = np.arange(side*side)
        np.random.shuffle(board._board)
        board._board = board._board.reshape((side, side))
        return board

    def newBoard(self, fy, fx, ty, tx):
        if self._board[ty][tx] != 0:
            raise Exception('To square must be zero')
        board = np.array(self._board)
        board[ty][tx] = board[fy][fx]
        board[fy][fx] = 0
        newboard = Board()
        newboard._side = self._side
        newboard._board = board
        return newboard


board = Board.initialboard(4)
state = State(board)
children = state.children
while len(children) != 0:
    children = children[0].children
print('done')