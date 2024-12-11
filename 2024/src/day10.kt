import java.io.File

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

    fun neighbors(position: Vector2): List<Vector2> {
        val all = listOf(
            Vector2(position.x, position.y - 1),
            Vector2(position.x + 1, position.y),
            Vector2(position.x, position.y + 1),
            Vector2(position.x - 1, position.y)
        )

        return all.filter {
            inBounds(it)
        }
    }

}


fun main() {
    fun parseGrid(content: String): Grid<Int> {
        return Grid(content.trim().lines().map { it.map(Char::digitToInt) })
    }

    fun countReachableNines(grid: Grid<Int>, from: Vector2): Int {
        val reachableNines = mutableSetOf<Vector2>()

        val toVisit = mutableListOf<Vector2>(from)

        while (toVisit.isNotEmpty()) {
            val visiting = toVisit.removeLast()
            val height = grid[visiting]
            if (height == 9) {
                reachableNines.add(visiting)
                continue
            }

            toVisit.addAll(grid.neighbors(visiting).filter { grid[it] == height + 1 })
        }

        return reachableNines.size
    }

    fun part1(content: String): Int {
        val grid = parseGrid(content)
        return grid.positionsOf(0).asSequence().sumOf { countReachableNines(grid, it) }
    }

    fun sumRatings(grid: Grid<Int>, from: Vector2): Int {
        fun rec(position: Vector2): Int {
            val height = grid[position]
            if (height == 9) {
                return 1
            }
            return grid.neighbors(position).filter { grid[it] == height + 1 }.sumOf(::rec)
        }
        return rec(from)
    }

    fun part2(content: String): Int {
        val grid = parseGrid(content)
        return grid.positionsOf(0).asSequence().sumOf { sumRatings(grid, it) }
    }

    val contentTest = File("src", "day10_test.txt").readText()
    val content = File("src", "day10.txt").readText()

    assertEq(part1(contentTest), 36)
    assertEq(part1(content), 517)

    assertEq(part2(contentTest), 81)
    assertEq(part2(content), 1116)
}
