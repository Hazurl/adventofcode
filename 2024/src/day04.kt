import java.io.File


fun main() {
    fun<T> horizontalWindowed(table: List<String>, len: Int, func: (String) -> T): List<T> {
        return table.flatMap{ it.windowed(len).map(func) }
    }

    fun<T> verticalWindowed(table: List<String>, len: Int, func: (String) -> T): List<T> {
        return table[0].indices.flatMap {
                col ->
            table.joinToString(separator = "") {
                it[col].toString()
            }.windowed(len).map(func)
        }
    }

    fun extractLine(table: List<String>, from: Pair<Int, Int>, steps: Pair<Int, Int>): String {
        var str = ""
        try {
            var indices = from

            while (true) {
                str += table[indices.second][indices.first]
                indices = Pair(indices.first + steps.first, indices.second + steps.second)
            }
        } catch (e: IndexOutOfBoundsException) {
            return str
        }
    }

    fun<T> positiveDiagonalWindowed(table: List<String>, len: Int, func: (String) -> T): List<T> {
        var startingPoints = mutableListOf<Pair<Int, Int>>()
        var width = table[0].length
        var height = table.size

        for (x in 1..<width) {
            startingPoints.add(Pair(x, height - 1))
        }
        for (y in 0..<height) {
            startingPoints.add(Pair(0, y))
        }

        return startingPoints.flatMap {
                startingPoint ->
            extractLine(table, startingPoint, Pair(1, -1)).windowed(len).map(func)
        }
    }

    fun<T> negativeDiagonalWindowed(table: List<String>, len: Int, func: (String) -> T): List<T> {
        var startingPoints = mutableListOf<Pair<Int, Int>>()
        var width = table[0].length
        var height = table.size

        for (x in 0..<width) {
            startingPoints.add(Pair(x, 0))
        }
        for (y in 1..<height) {
            startingPoints.add(Pair(0, y))
        }

        return startingPoints.flatMap {
                startingPoint ->
            extractLine(table, startingPoint, Pair(1, 1)).windowed(len).map(func)
        }
    }

    fun part1(content: String): Int {
        var table = content.lines().filter { it.isNotBlank() }
        var word = "XMAS"

        return horizontalWindowed(table, word.length) {
            it == word || it == word.reversed()
        }.count{ it } + verticalWindowed(table, word.length) {
            it == word || it == word.reversed()
        }.count{ it } + positiveDiagonalWindowed(table, word.length) {
            it == word || it == word.reversed()
        }.count{ it } + negativeDiagonalWindowed(table, word.length) {
            it == word || it == word.reversed()
        }.count{ it }
    }

    fun findInnerPositionsOf(table: List<String>, ch: Char): List<Pair<Int, Int>> {
        var positions = mutableListOf<Pair<Int, Int>>()
        var width = table[0].length
        var height = table.size

        for (y in 1..<height - 1) {
            for (x in 1..<width - 1) {
                if (table[y][x] == ch) {
                    positions.add(Pair(x, y))
                }
            }
        }

        return positions
    }

    fun isXMASCenter(table: List<String>, position: Pair<Int, Int>): Boolean {
        var tl = table[position.second - 1][position.first - 1]
        var tr = table[position.second - 1][position.first + 1]
        var bl = table[position.second + 1][position.first - 1]
        var br = table[position.second + 1][position.first + 1]

        if (tl == tr && bl == br) return (tl == 'M' && bl == 'S') || (tl == 'S' && bl == 'M')
        if (tl == bl && tr == br) return (tl == 'M' && tr == 'S') || (tl == 'S' && tr == 'M')
        return false
    }


    fun part2(content: String): Int {
        var table = content.lines().filter { it.isNotBlank() }
        return findInnerPositionsOf(table, 'A').count{ isXMASCenter(table, it) }
    }

    val contentTest = File("src", "day04_test.txt").readText()
    val content = File("src", "day04.txt").readText()

    assertEq(part1(contentTest), 18)
    assertEq(part1(content),  2483)

    assertEq(part2(contentTest), 9)
    assertEq(part2(content),  1925)
}
