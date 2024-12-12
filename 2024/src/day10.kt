import java.io.File


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

            toVisit.addAll(grid.neighbors(visiting).map { it.second }.filter { grid[it] == height + 1 })
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
            return grid.neighbors(position).map { it.second }.filter { grid[it] == height + 1 }.sumOf(::rec)
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
