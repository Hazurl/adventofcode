import java.io.File

fun part1(content: String): Int {
    return content.lines().sumOf { it.toInt() }
}

fun part2(content: String): Int {
    return 0
}

fun main() {
    val contentTest = File("src", "day01_test.txt").readText()
    val content = File("src", "day01.txt").readText()

    assertEq(part1(contentTest), 6)
    assertEq(part1(content),  15)

    val part2 = part2(contentTest)
}
