import java.io.File


fun main() {

    // Regex for `mul(%d,%d)`

    fun part1(content: String): Int {
        val reg = Regex("""mul\((\d{1,3}),(\d{1,3})\)""")
        return reg.findAll(content).sumOf { it.destructured.toList().map(String::toInt).reduce(Int::times) }
    }

    fun part2(content: String): Int {
        val reg = Regex("""mul\((\d{1,3}),(\d{1,3})\)|do(n't)?""")
        var mulEnabled = true
        var acc = 0
        for (line in reg.findAll(content)) {
            if (line.groupValues[0].startsWith("mul")) {
                if (mulEnabled) {
                    acc += line.destructured.toList().filter{ it.isNotBlank() }.map(String::toInt).reduce(Int::times)
                }
            } else {
                mulEnabled = line.groupValues[0] != "don't"
            }
        }
        return acc
    }

    val contentTest = File("src", "day03_test.txt").readText()
    val content = File("src", "day03.txt").readText()

    assertEq(part1(contentTest), 161)
    assertEq(part1(content),  184511516)

    assertEq(part2(contentTest), 48)
    assertEq(part2(content),  90044227)
}
