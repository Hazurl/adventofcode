import java.io.File
import kotlin.math.absoluteValue


fun main() {
    fun part1(content: String): Int {
        val pairs = content.lines().filterNot { it.isEmpty() }.map { line ->
            line.split(' ').filterNot { it.isEmpty() }.map {
                it.toInt()
            }
        }

        val leftList = pairs.map { it[0] }.sorted()
        val rightList = pairs.map { it[1] }.sorted()

        return leftList.zip(rightList).sumOf { (it.first - it.second).absoluteValue }
    }

    fun part2(content: String): Int {
        val pairs = content.lines().filterNot { it.isEmpty() }.map { line ->
            line.split(' ').filterNot { it.isEmpty() }.map {
                it.toInt()
            }
        }

        val leftList = pairs.map { it[0] }
        val rightList = pairs.map { it[1] }.groupingBy { it }.eachCount()

        return leftList.sumOf { rightList.getOrDefault(it, 0) * it }
    }

    val contentTest = File("src", "day01_test.txt").readText()
    val content = File("src", "day01.txt").readText()

    assertEq(part1(contentTest), 11)
    assertEq(part1(content),  2057374)

    assertEq(part2(contentTest), 31)
    assertEq(part2(content),  23177084)
}
