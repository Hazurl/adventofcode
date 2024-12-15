import java.io.File
import javax.print.Doc


data class Document(val grid: Grid<Document.Tile>, val robot: Vector2I, val moves: List<Direction>) {
    enum class Tile {
        Empty,
        Wall,
        BoxLeft,
        BoxRight
    }
}

fun main() {
    fun parseDocument(content: String): Document {
        val (map, rawMoves) = content.trim().split("\r\n\r\n")

        val grid = Grid<Document.Tile>(
            map.lines().map { row ->
                row.map {
                    when (it) {
                        '#' -> Document.Tile.Wall
                        'O' -> Document.Tile.BoxLeft
                        '@' -> Document.Tile.Empty
                        '.' -> Document.Tile.Empty
                        else -> throw RuntimeException("Unexpected tile: $it")
                    }
                }
            }
        )

        fun findRobot(): Vector2I {
            for ((y, row) in map.lines().withIndex()) {
                for ((x, ch) in row.withIndex()) {
                    if (ch == '@') {
                        return Vector2I(x, y)
                    }
                }
            }
            throw RuntimeException("No robot found")
        }

        val robot = findRobot()

        val moves = rawMoves.lines().flatMap {
            it.map { ch ->
                when (ch) {
                    '>' -> Direction.RIGHT
                    '<' -> Direction.LEFT
                    '^' -> Direction.UP
                    'v' -> Direction.DOWN
                    else -> throw RuntimeException("Unexpected direction: $it")
                }
            }
        }

        return Document(grid, robot, moves)
    }

    fun widen(robot: Vector2I, grid: Grid<Document.Tile>): Pair<Vector2I, Grid<Document.Tile>> {
        return Pair(
            Vector2I(robot.x * 2, robot.y), Grid(
                grid.cells.map { row ->
                    row.flatMap {
                        when (it) {
                            Document.Tile.Wall -> listOf(Document.Tile.Wall, Document.Tile.Wall)
                            Document.Tile.BoxLeft -> listOf(Document.Tile.BoxLeft, Document.Tile.BoxRight)
                            Document.Tile.Empty -> listOf(Document.Tile.Empty, Document.Tile.Empty)
                            else -> throw RuntimeException("Unexpected tile: $it")
                        }
                    }
                }
            ))
    }

    fun printGrid(robot: Vector2I, grid: Grid<Document.Tile>, wide: Boolean = false) {
        println(grid.mapWithPosition { position, tile ->
            if (position == robot) {
                '@'
            } else {
                when (tile) {
                    Document.Tile.Wall -> '#'
                    Document.Tile.BoxLeft -> if (wide) '[' else 'O'
                    Document.Tile.BoxRight -> ']'
                    Document.Tile.Empty -> '.'
                }
            }
        })
    }

    fun simpleMoveRobot(robot: Vector2I, grid: Grid<Document.Tile>, move: Direction): Vector2I {
        val offset = when (move) {
            Direction.UP -> Vector2I(0, -1)
            Direction.DOWN -> Vector2I(0, 1)
            Direction.LEFT -> Vector2I(-1, 0)
            Direction.RIGHT -> Vector2I(1, 0)
        }

        fun moveNext(position: Vector2I): Boolean {
            val nextPosition = position + offset
            val nextTile = grid[nextPosition]
            if (nextTile == Document.Tile.Wall || (nextTile == Document.Tile.BoxLeft || nextTile == Document.Tile.BoxRight) && !moveNext(nextPosition)) {
                return false
            } else {
                grid[nextPosition] = grid[position]
                return true
            }
        }

        if (moveNext(robot)) {
            return robot + offset
        }
        return robot
    }

    fun part1(content: String): Int {
        val document = parseDocument(content)
        var robot = document.robot
        val grid = document.grid

        for (move in document.moves) {
            robot = simpleMoveRobot(robot, grid, move)
        }

        var total = 0
        for ((position, tile) in grid.positions()) {
            if (tile == Document.Tile.BoxLeft) {
                total += position.x + position.y * 100
            }
        }

        return total
    }

    fun wideMoveRobot(robot: Vector2I, grid: Grid<Document.Tile>, move: Direction): Vector2I {
        if (move == Direction.LEFT || move == Direction.RIGHT) {
            return simpleMoveRobot(robot, grid, move)
        }

        val offset = when (move) {
            Direction.UP -> Vector2I(0, -1)
            Direction.DOWN -> Vector2I(0, 1)
            else -> throw RuntimeException("Unexpected direction: $move")
        }

        fun collectAllBoxes(position: Vector2I): Set<Vector2I>? {
            val nextPosition = position + offset
            val nextTile = grid[nextPosition]

            when (nextTile) {
                Document.Tile.Wall -> return null
                Document.Tile.BoxLeft -> {
                    val nextPositionRight = nextPosition + Vector2I(1, 0)
                    val rightBoxes = collectAllBoxes(nextPositionRight)
                    val leftBoxes = collectAllBoxes(nextPosition)
                    if (rightBoxes != null && leftBoxes != null) {
                        return rightBoxes.union(leftBoxes).union(setOf(nextPositionRight, nextPosition))
                    }
                    return null
                }
                Document.Tile.BoxRight -> {
                    val nextPositionLeft = nextPosition + Vector2I(-1, 0)
                    val rightBoxes = collectAllBoxes(nextPosition)
                    val leftBoxes = collectAllBoxes(nextPositionLeft)
                    if (rightBoxes != null && leftBoxes != null) {
                        return rightBoxes.union(leftBoxes).union(setOf(nextPositionLeft, nextPosition))
                    }
                    return null
                }
                else -> return setOf()
            }
        }

        val boxes = collectAllBoxes(robot) ?: return robot

        val newTiles = boxes.map {
            Pair(it + offset, grid[it])
        }

        for (box in boxes) {
            grid[box] = Document.Tile.Empty
        }
        for ((position, tile) in newTiles) {
            grid[position] = tile
        }
        return robot + offset
    }

    fun part2(content: String): Int {
        val document = parseDocument(content)
        var (robot, grid) = widen(document.robot, document.grid)
        printGrid(robot, grid, wide=true)

        for (move in document.moves) {
            robot = wideMoveRobot(robot, grid, move)
        }

        var total = 0
        for ((position, tile) in grid.positions()) {
            if (tile == Document.Tile.BoxLeft) {
                total += position.x + position.y * 100
            }
        }

        return total
    }

    val contentTest = File("src", "day15_test.txt").readText()
    val content = File("src", "day15.txt").readText()

    assertEq(part1(contentTest), 10092)
    assertEq(part1(content), 1294459)

    assertEq(part2(contentTest), 9021)
    assertEq(part2(content), 1319212)
}

