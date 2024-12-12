import java.io.File

fun main() {

    fun parseGarden(content: String): Grid<Char> {
        return Grid(content.trim().lines().map { it.map { ch -> ch } })
    }

    fun part1(content: String): Long {
        val garden = parseGarden(content)
        val positionsSeen = HashSet<Vector2>()

        fun computePriceOfRegion(initial: Vector2): Long {
            val toVisit = mutableListOf(initial)
            var area = 0L
            var fences = 0L

            while (toVisit.isNotEmpty()) {
                val visiting = toVisit.removeLast()
                if (visiting in positionsSeen) {
                    continue
                }
                positionsSeen.add(visiting)
                val neighbors = garden.neighbors(visiting).map { it.second }
                area += 1
                fences += neighbors.count { garden[it] != garden[visiting] } + garden.borders(visiting).size
                toVisit.addAll(neighbors.filter { garden[it] == garden[visiting] })
            }

            return area * fences
        }

        return garden.positions().asSequence().sumOf { computePriceOfRegion(it.first) }
    }

    fun part2(content: String): Long {
        val garden = parseGarden(content)
        val positionsSeen = HashSet<Vector2>()

        fun computePriceOfRegion(initial: Vector2): Long {
            val toVisit = mutableListOf(initial)
            val fencesSeen = HashSet<Pair<Vector2, Direction>>()

            var area = 0L
            var fences = 0L


            while (toVisit.isNotEmpty()) {
                toVisit.sortByDescending { it }
                val visiting = toVisit.removeLast()
                if (visiting in positionsSeen) {
                    continue
                }
                positionsSeen.add(visiting)

                val neighbors = garden.neighbors(visiting)
                area += 1

                val upDownDelta = listOf(Vector2(0, -1), Vector2(0, 1))
                val leftRightDelta = listOf(Vector2(-1, 0), Vector2(1, 0))

                val directions = mutableSetOf<Direction>()
                directions.addAll(neighbors.filter { garden[it.second] != garden[visiting] }.map { it.first })
                directions.addAll(garden.borders(visiting))

                for (direction in directions) {
                    val perpendicularNeighborsDelta = if (direction == Direction.UP || direction == Direction.DOWN) {
                        leftRightDelta
                    } else {
                        upDownDelta
                    }
                    val perpendicularNeighbors = perpendicularNeighborsDelta.map { it + visiting }
                    if (perpendicularNeighbors.map { Pair(it, direction) }.all { !fencesSeen.contains(it) }) {
                        fences += 1
                    }
                    fencesSeen.add(Pair(visiting, direction))
                }

                toVisit.addAll(neighbors.map { it.second }.filter { garden[it] == garden[visiting] })
            }

            return area * fences
        }

        return garden.positions().asSequence().sumOf { computePriceOfRegion(it.first) }
    }

    val contentTest = File("src", "day12_test.txt").readText()
    val content = File("src", "day12.txt").readText()

    assertEq(part1(contentTest), 1930)
    assertEq(part1(content), 1375476)

    assertEq(part2(contentTest), 1206)
    assertEq(part2(content), 821372)
}
