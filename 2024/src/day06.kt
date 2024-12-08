import java.io.File

enum class Tile {
    Wall,
    Empty
}

enum class Direction {
    UP, DOWN, LEFT, RIGHT;

    fun rotateRight(): Direction {
        return when (this) {
            Direction.UP -> Direction.RIGHT
            Direction.RIGHT -> Direction.DOWN
            Direction.DOWN -> Direction.LEFT
            Direction.LEFT -> Direction.UP
        }
    }

    fun asCharacter(): Char {
        return when (this) {
            Direction.UP -> '^'
            Direction.DOWN -> 'v'
            Direction.LEFT -> '<'
            Direction.RIGHT -> '>'
        }
    }
}

open class GuardLeftMappedAreaException() : Exception()
open class GuardStuckInLoopException() : Exception()

data class State(
    val grid: List<List<Tile>>,
    val guardPosition: Vector2,
    val direction: Direction,
    val positionsSeen: Set<Vector2>,
    val guardSeen: Set<Pair<Vector2, Direction>>
) {
    private fun getTileAt(position: Vector2): Tile? {
        return try {
            grid[position.y][position.x]
        } catch (e: IndexOutOfBoundsException) {
            null
        }
    }

    private fun getNextPredictedGuardPosition(): Vector2 {
        return when (direction) {
            Direction.UP -> guardPosition.copy(y = guardPosition.y - 1)
            Direction.DOWN -> guardPosition.copy(y = guardPosition.y + 1)
            Direction.LEFT -> guardPosition.copy(x = guardPosition.x - 1)
            Direction.RIGHT -> guardPosition.copy(x = guardPosition.x + 1)
        }
    }

    private fun positionAsCharacter(position: Vector2): Char {
        return if (position == guardPosition) {
            direction.asCharacter()
        } else if (positionsSeen.contains(position)) {
            'X'
        } else when (getTileAt(position)) {
            Tile.Wall -> '#'
            else -> '.'
        }
    }

    fun print() {
        val output = StringBuilder()
        for (y in grid.indices) {
            for (x in grid[y].indices) {
                val position = Vector2(x, y)
                output.append(positionAsCharacter(position))
            }
            output.append('\n')
        }
        println(output.toString())
    }

    private fun stateWithNewGuardPositionAndDirection(newGuardPosition: Vector2, newDirection: Direction): State {
        val guardState = Pair(newGuardPosition, newDirection)
        if (guardState in guardSeen) {
            throw GuardStuckInLoopException()
        }
        return copy(
            guardPosition = newGuardPosition,
            direction = newDirection,
            positionsSeen = positionsSeen union setOf(newGuardPosition),
            guardSeen = guardSeen union setOf(guardState)
        )
    }

    fun nextState(): State {
        val nextGuardPosition = getNextPredictedGuardPosition()

        return when (getTileAt(nextGuardPosition)) {
            null -> throw GuardLeftMappedAreaException()
            Tile.Wall -> stateWithNewGuardPositionAndDirection(guardPosition, direction.rotateRight())
            Tile.Empty -> stateWithNewGuardPositionAndDirection(nextGuardPosition, direction)
        }
    }

    fun withWallAt(position: Vector2): State {
        return copy(
            grid = grid.mapIndexed { y, row ->
                row.mapIndexed { x, c ->
                    if (Vector2(x, y) == position) Tile.Wall else c
                }
            }
        )
    }

    fun getSeenPositions(): Set<Vector2> {
        var newState = this
        while (true) {
            try {
                newState = newState.nextState()
            } catch (e: GuardLeftMappedAreaException) {
                return newState.positionsSeen
            }
        }
    }
}

fun parseInitialState(content: String): State {
    val grid = mutableListOf<MutableList<Tile>>()
    var guardPosition: Vector2? = null

    content.lines().filter { it.isNotBlank() }.forEachIndexed { y, line ->
        grid.add(mutableListOf())
        val row = grid.last()
        line.forEachIndexed { x, c ->
            when (c) {
                '^' -> {
                    guardPosition = Vector2(x, y)
                    row.add(Tile.Empty)
                }

                '#' -> row.add(Tile.Wall)
                '.' -> row.add(Tile.Empty)
                else -> throw IllegalArgumentException()
            }
        }
    }

    return State(
        grid,
        guardPosition!!,
        Direction.UP,
        setOf(guardPosition!!),
        setOf(Pair(guardPosition!!, Direction.UP))
    )
}

fun main() {
    fun part1(content: String): Int {
        var state: State = parseInitialState(content)
        return state.getSeenPositions().size
    }


    fun part2(content: String): Int {
        val initialState = parseInitialState(content)

        val emptyPositions = (initialState.getSeenPositions() - initialState.guardPosition).toList()
        var stuckInLoopCount = 0

        for ((index, emptyPosition) in emptyPositions.withIndex()) {
            println("${index + 1}/${emptyPositions.size}")
            
            var state = initialState.withWallAt(emptyPosition)
            while (true) {
                try {
                    state = state.nextState()
                } catch (e: GuardLeftMappedAreaException) {
                    break
                } catch (e: GuardStuckInLoopException) {
                    stuckInLoopCount++
                    break
                }
            }
        }
        return stuckInLoopCount
    }

    val contentTest = File("src", "day06_test.txt").readText()
    val content = File("src", "day06.txt").readText()

    assertEq(part1(contentTest), 41)
    assertEq(part1(content), 4826)


    assertEq(part2(contentTest), 6)
    assertEq(part2(content), 1721)
}
