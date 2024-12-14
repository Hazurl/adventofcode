fun <T> assertEq(got: T, expected: T) {
    if (got != expected) {
        throw AssertionError("Expected $expected, got $got")
    }
}

data class Vector2I(val x: Int, val y: Int): Comparable<Vector2I> {
    operator fun plus(v: Vector2I) = Vector2I(x + v.x, y + v.y)
    operator fun minus(v: Vector2I) = Vector2I(x - v.x, y - v.y)
    operator fun unaryMinus() = Vector2I(-x, -y)
    operator fun times(factor: Int) = Vector2I(x * factor, y * factor)
    operator fun div(factor: Int) = Vector2I(x / factor, y / factor)

    fun toVVector2L() = Vector2L(x.toLong(), y.toLong())

    override fun compareTo(other: Vector2I): Int {
        return if (y == other.y) {
            x compareTo other.x
        } else {
            y compareTo other.y
        }
    }
}

data class Vector2L(val x: Long, val y: Long): Comparable<Vector2L> {
    operator fun plus(v: Vector2L) = Vector2L(x + v.x, y + v.y)
    operator fun minus(v: Vector2L) = Vector2L(x - v.x, y - v.y)
    operator fun unaryMinus() = Vector2L(-x, -y)
    operator fun times(factor: Long) = Vector2L(x * factor, y * factor)
    operator fun div(factor: Long) = Vector2L(x / factor, y / factor)

    fun toVector2I() = Vector2I(x.toInt(), y.toInt())

    override fun compareTo(other: Vector2L): Int {
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

    fun positionsOf(toFind: T) = iterator {
        for ((position, item) in positions()) {
            if (item == toFind) {
                yield(position)
            }
        }
    }

    fun positions() = iterator<Pair<Vector2I, T>> {
        for (y in 0..<height) {
            for (x in 0..<width) {
                val position = Vector2I(x, y)
                yield(Pair(position, get(position)))
            }
        }
    }

    operator fun get(position: Vector2I): T {
        return cells[position.y][position.x]
    }

    fun inBounds(position: Vector2I): Boolean {
        return position.x in 0..<width && position.y in 0..<height
    }

    fun neighbors(position: Vector2I): List<Pair<Direction, Vector2I>> {
        val all = listOf(
            Pair(Direction.UP, Vector2I(position.x, position.y - 1)),
            Pair(Direction.RIGHT, Vector2I(position.x + 1, position.y)),
            Pair(Direction.DOWN, Vector2I(position.x, position.y + 1)),
            Pair(Direction.LEFT, Vector2I(position.x - 1, position.y))
        )

        return all.filter {
            inBounds(it.second)
        }
    }

    fun borders(position: Vector2I): List<Direction> {
        val all = listOf(
            Pair(Direction.UP, Vector2I(position.x, position.y - 1)),
            Pair(Direction.RIGHT, Vector2I(position.x + 1, position.y)),
            Pair(Direction.DOWN, Vector2I(position.x, position.y + 1)),
            Pair(Direction.LEFT, Vector2I(position.x - 1, position.y))
        )

        return all.filter {
            !inBounds(it.second)
        }.map { it.first }
    }

}


