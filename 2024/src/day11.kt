import java.io.File

typealias Stone = Long

fun main() {

    fun parseStones(content: String): List<Stone> {
        return content.trim().split(" ").map(String::toLong)
    }

    fun stonesForStone(stone: Stone) = iterator<Stone> {
        if (stone == 0L) {
            yield(1L)
        } else {
            val str = stone.toString()
            if (str.length % 2 == 0) {
                yield(str.substring(0, str.length / 2).toLong())
                yield(str.substring(str.length / 2).toLong())
            } else {
                yield(stone * 2024L)
            }
        }
    }

    fun stoneCountAfterNBlink(cache: MutableMap<Pair<Stone, Int>, Long>, stone: Stone, count: Int): Long {
        if (count == 0) {
            return 1
        }
        return cache.getOrPut(Pair(stone, count)) {
            stonesForStone(stone).asSequence().sumOf { s -> stoneCountAfterNBlink(cache, s, count - 1) }
        }
    }

    fun part1(content: String): Long {
        val stones = parseStones(content)
        return stones.sumOf { stone -> stoneCountAfterNBlink(HashMap(), stone, 25) }
    }

    fun part2(content: String): Long {
        var stones = parseStones(content)

        return stones.sumOf { stone -> stoneCountAfterNBlink(HashMap(), stone, 75) }
    }

    val contentTest = File("src", "day11_test.txt").readText()
    val content = File("src", "day11.txt").readText()

    assertEq(part1(contentTest), 55312)
    assertEq(part1(content), 183620)

    assertEq(part2(content), 220377651399268)
}
