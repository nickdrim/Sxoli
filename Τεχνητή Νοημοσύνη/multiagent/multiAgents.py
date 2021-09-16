# multiAgents.py
# --------------
# Licensing Information:  You are free to use or extend these projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to UC Berkeley, including a link to http://ai.berkeley.edu.
# 
# Attribution Information: The Pacman AI projects were developed at UC Berkeley.
# The core projects and autograders were primarily created by John DeNero
# (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# Student side autograding was added by Brad Miller, Nick Hay, and
# Pieter Abbeel (pabbeel@cs.berkeley.edu).


from util import manhattanDistance
from game import Directions
import random, util

from game import Agent

class ReflexAgent(Agent):
    """
    A reflex agent chooses an action at each choice point by examining
    its alternatives via a state evaluation function.

    The code below is provided as a guide.  You are welcome to change
    it in any way you see fit, so long as you don't touch our method
    headers.
    """


    def getAction(self, gameState):
        """
        You do not need to change this method, but you're welcome to.

        getAction chooses among the best options according to the evaluation function.

        Just like in the previous project, getAction takes a GameState and returns
        some Directions.X for some X in the set {NORTH, SOUTH, WEST, EAST, STOP}
        """
        # Collect legal moves and successor states
        legalMoves = gameState.getLegalActions()

        # Choose one of the best actions
        scores = [self.evaluationFunction(gameState, action) for action in legalMoves]
        bestScore = max(scores)
        bestIndices = [index for index in range(len(scores)) if scores[index] == bestScore]
        chosenIndex = random.choice(bestIndices) # Pick randomly among the best

        "Add more of your code here if you want to"

        return legalMoves[chosenIndex]

    def evaluationFunction(self, currentGameState, action):
        """
        Design a better evaluation function here.

        The evaluation function takes in the current and proposed successor
        GameStates (pacman.py) and returns a number, where higher numbers are better.

        The code below extracts some useful information from the state, like the
        remaining food (newFood) and Pacman position after moving (newPos).
        newScaredTimes holds the number of moves that each ghost will remain
        scared because of Pacman having eaten a power pellet.

        Print out these variables to see what you're getting, then combine them
        to create a masterful evaluation function.
        """
        # Useful information you can extract from a GameState (pacman.py)
        successorGameState = currentGameState.generatePacmanSuccessor(action)
        newPos = successorGameState.getPacmanPosition()
        newFood = successorGameState.getFood().asList()
        newGhostStates = successorGameState.getGhostStates()
        newScaredTimes = [ghostState.scaredTimer for ghostState in newGhostStates]
        capsulePos = currentGameState.getCapsules()
        
        "*** YOUR CODE HERE ***"

        minCap = float("inf")
        for position in capsulePos:
            minCap = min(minCap, util.manhattanDistance(newPos,position))

        if minCap != float("inf"):
            for ghost in successorGameState.getGhostPositions():
                if util.manhattanDistance(newPos, ghost) > 2:
                    if(minCap == 1):
                        return 10000
                    if minCap == 0:
                        return float("inf")

        minFood = float("inf")
        for food in newFood:
            minFood = min(minFood, util.manhattanDistance(newPos,food))
        
        ghostpoints = float("inf")
        for ghost in successorGameState.getGhostPositions():
            if util.manhattanDistance(newPos, ghost) < 3:
                ghostpoints = min(ghostpoints,util.manhattanDistance(newPos, ghost))
                for timer in newScaredTimes:
                    if timer < 2:
                        return -float("inf")
                    else:
                        return successorGameState.getScore() + 1/ghostpoints
        
        successorScore = successorGameState.getScore() + 1/minFood
        
        return successorScore

def scoreEvaluationFunction(currentGameState):
    """
    This default evaluation function just returns the score of the state.
    The score is the same one displayed in the Pacman GUI.

    This evaluation function is meant for use with adversarial search agents
    (not reflex agents).
    """
    return currentGameState.getScore()

class MultiAgentSearchAgent(Agent):
    """
    This class provides some common elements to all of your
    multi-agent searchers.  Any methods defined here will be available
    to the MinimaxPacmanAgent, AlphaBetaPacmanAgent & ExpectimaxPacmanAgent.

    You *do not* need to make any changes here, but you can if you want to
    add functionality to all your adversarial search agents.  Please do not
    remove anything, however.

    Note: this is an abstract class: one that should not be instantiated.  It's
    only partially specified, and designed to be extended.  Agent (game.py)
    is another abstract class.
    """

    def __init__(self, evalFn = 'scoreEvaluationFunction', depth = '2'):
        self.index = 0 # Pacman is always agent index 0
        self.evaluationFunction = util.lookup(evalFn, globals())
        self.depth = int(depth)

class MinimaxAgent(MultiAgentSearchAgent):
    """
    Your minimax agent (question 2)
    """

    def getAction(self, gameState):
        """
        Returns the minimax action from the current gameState using self.depth
        and self.evaluationFunction.

        Here are some method calls that might be useful when implementing minimax.

        gameState.getLegalActions(agentIndex):
        Returns a list of legal actions for an agent
        agentIndex=0 means Pacman, ghosts are >= 1

        gameState.generateSuccessor(agentIndex, action):
        Returns the successor game state after an agent takes an action

        gameState.getNumAgents():
        Returns the total number of agents in the game

        gameState.isWin():
        Returns whether or not the game state is a winning state

        gameState.isLose():
        Returns whether or not the game state is a losing state
        """
        "*** YOUR CODE HERE ***"

        return self.maxvalue(gameState, 0, self.index)[0]
        
        util.raiseNotDefined()

    def minimax(self, gameState, depth, index):
        if gameState.isWin() or gameState.isLose() or depth==self.depth * gameState.getNumAgents():
            return self.evaluationFunction(gameState)
        
        if index == self.index:
            return self.maxvalue(gameState, depth, index)[1]
        else:
            return self.minvalue(gameState, depth, index)[1]


    def maxvalue(self, gameState, depth, index):
        maxEval = []
        for action in gameState.getLegalActions(index):
            maxEval.append((action, self.minimax(gameState.generateSuccessor(index,action), depth+1, (depth+1)%gameState.getNumAgents())))

        return max(maxEval, key=lambda x:x[1])

    def minvalue(self, gameState, depth, index):
        minEval = []
        for action in gameState.getLegalActions(index):
            minEval.append((action, self.minimax(gameState.generateSuccessor(index,action), depth+1, (depth+1)%gameState.getNumAgents())))

        return min(minEval,key=lambda x:x[1])


class AlphaBetaAgent(MultiAgentSearchAgent):
    """
    Your minimax agent with alpha-beta pruning (question 3)
    """

    def getAction(self, gameState):
        """
        Returns the minimax action using self.depth and self.evaluationFunction
        """
        "*** YOUR CODE HERE ***"
        return self.maxvalue(gameState, 0, self.index, -float("inf"), float("inf"))[0]
        util.raiseNotDefined()

    def alphabeta(self, gameState, depth, index, a, b):
        if gameState.isWin() or gameState.isLose() or depth==self.depth * gameState.getNumAgents():
            return self.evaluationFunction(gameState)
        
        if index == self.index:
            return self.maxvalue(gameState, depth, index, a, b)[1]
        else:
            return self.minvalue(gameState, depth, index, a, b)[1]

    def maxvalue(self, gameState, depth, index, a, b):
        maxVal = []
        for action in gameState.getLegalActions(index):
            maxVal.append((action, self.alphabeta(gameState.generateSuccessor(index,action), depth+1, (depth+1)%gameState.getNumAgents(), a, b)))

            if max(maxVal,key=lambda x:x[1])[1] > b:
                return max(maxVal,key=lambda x:x[1])
           
            a = max(max(maxVal,key=lambda x:x[1])[1],a)

        return max(maxVal,key=lambda x:x[1])

    def minvalue(self, gameState, depth, index, a, b):
        minVal = []
        for action in gameState.getLegalActions(index):
            minVal.append((action, self.alphabeta(gameState.generateSuccessor(index,action), depth+1, (depth+1)%gameState.getNumAgents(), a, b)))

            if min(minVal,key=lambda x:x[1])[1] < a:
                return min(minVal,key=lambda x:x[1])
            
            b = min(min(minVal,key=lambda x:x[1])[1],b)

        return min(minVal,key=lambda x:x[1])
            

class ExpectimaxAgent(MultiAgentSearchAgent):
    """
      Your expectimax agent (question 4)
    """

    def getAction(self, gameState):
        """
        Returns the expectimax action using self.depth and self.evaluationFunction

        All ghosts should be modeled as choosing uniformly at random from their
        legal moves.
        """
        "*** YOUR CODE HERE ***"
        return self.maxvalue(gameState, 0, self.index)[0]
        util.raiseNotDefined()

    def expectimax(self, gameState, depth, index):
        if gameState.isWin() or gameState.isLose() or depth==self.depth * gameState.getNumAgents():
            return self.evaluationFunction(gameState)

        if index == self.index:
            return self.maxvalue(gameState, depth, index)[1]
        else:
            return self.minvalue(gameState, depth, index)[1]

    def maxvalue(self, gameState, depth, index):
        maxEval = []
        for action in gameState.getLegalActions(index):
            maxEval.append((action, self.expectimax(gameState.generateSuccessor(index,action), depth+1, (depth+1)%gameState.getNumAgents())))

        return max(maxEval, key=lambda x:x[1])

    def minvalue(self, gameState, depth, index):
        minEval = []
        v = 0
        for action in gameState.getLegalActions(index):
            minEval.append((action, self.expectimax(gameState.generateSuccessor(index,action), depth+1, (depth+1)%gameState.getNumAgents())))

        for evaluation in minEval:
            v += evaluation[1] * (1/len(minEval))

        ranVal = (random.choice(minEval[0]),v)

        return ranVal

def betterEvaluationFunction(currentGameState):
    """
    Your extreme ghost-hunting, pellet-nabbing, food-gobbling, unstoppable
    evaluation function (question 5).

    DESCRIPTION: <write something here so we know what you did>

    CapsuleScore: Eating a capsule and then eating a ghost gives the most points
    at the game.So a good position for pacman is when he is near one.

    ghostpoints: Ghosts kill pacman so being in a position that is far from them is
    a good position for pacman

    foodScore: Last we also calculate the score for food. The further pacman is from
    food the smaller this score is. When he approaches food it gets better.

    """
    "*** YOUR CODE HERE ***"

    currentPos = currentGameState.getPacmanPosition()
    newFood = currentGameState.getFood().asList()
    newGhostStates = currentGameState.getGhostStates()
    newScaredTimes = [ghostState.scaredTimer for ghostState in newGhostStates]
    capsulePos = currentGameState.getCapsules()


    capsuleScore = 0
    for position in capsulePos:
        capsuleScore += 1/util.manhattanDistance(currentPos,position)

    ghostpoints = 0
    for ghost in currentGameState.getGhostPositions():
        if util.manhattanDistance(currentPos, ghost) < 3:
            for timer in newScaredTimes:
                if timer < 2:
                    return -float("inf")
                else:
                    ghostpoints += util.manhattanDistance(currentPos,ghost)
    
    foodScore = 0
    for food in newFood:
        foodScore += 1/util.manhattanDistance(currentPos,food)

    return currentGameState.getScore() + foodScore + ghostpoints + capsuleScore
    

    util.raiseNotDefined()

# Abbreviation
better = betterEvaluationFunction
