fun <T> assertEq(got: T, expected: T) {
    if (got != expected) {
        throw AssertionError("Expected $expected, got $got")
    }
}

data class Vector2(val x: Int, val y: Int): Comparable<Vector2> {
    operator fun plus(v: Vector2) = Vector2(x + v.x, y + v.y)
    operator fun minus(v: Vector2) = Vector2(x - v.x, y - v.y)
    operator fun unaryMinus() = Vector2(-x, -y)
    operator fun times(factor: Int) = Vector2(x * factor, y * factor)
    operator fun div(factor: Int) = Vector2(x / factor, y / factor)

    override fun compareTo(other: Vector2): Int {
        return if (y == other.y) {
            x compareTo other.x
        } else {
            y compareTo other.y
        }
    }
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


class Grid<T> constructor(cells: List<List<T>>) {
    val cells: List<List<T>> = cells

    val width: Int
        get() = cells[0].size

    val height: Int
        get() = cells.size

    fun positionsOf(toFind: T) = iterator<Vector2> {
        for ((position, item) in positions()) {
            if (item == toFind) {
                yield(position)
            }
        }
    }

    fun positions() = iterator<Pair<Vector2, T>> {
        for (y in 0..<height) {
            for (x in 0..<width) {
                val position = Vector2(x, y)
                yield(Pair(position, get(position)))
            }
        }
    }

    operator fun get(position: Vector2): T {
        return cells[position.y][position.x]
    }

    fun inBounds(position: Vector2): Boolean {
        return position.x in 0..<width && position.y in 0..<height
    }

    fun neighbors(position: Vector2): List<Pair<Direction, Vector2>> {
        val all = listOf(
            Pair(Direction.UP, Vector2(position.x, position.y - 1)),
            Pair(Direction.RIGHT, Vector2(position.x + 1, position.y)),
            Pair(Direction.DOWN, Vector2(position.x, position.y + 1)),
            Pair(Direction.LEFT, Vector2(position.x - 1, position.y))
        )

        return all.filter {
            inBounds(it.second)
        }
    }

    fun borders(position: Vector2): List<Direction> {
        val all = listOf(
            Pair(Direction.UP, Vector2(position.x, position.y - 1)),
            Pair(Direction.RIGHT, Vector2(position.x + 1, position.y)),
            Pair(Direction.DOWN, Vector2(position.x, position.y + 1)),
            Pair(Direction.LEFT, Vector2(position.x - 1, position.y))
        )

        return all.filter {
            !inBounds(it.second)
        }.map { it.first }
    }

}


