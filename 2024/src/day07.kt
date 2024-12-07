import java.io.File

data class Equation(val result: Long, val operands: List<Long>)

fun parseEquation(equation: String): Equation {
    val parts = equation.split(": ")
    val operands = parts[1].split(" ").map(String::toLong)
    return Equation(parts[0].toLong(), operands)
}

fun findAllResults(operands: List<Long>, operations: List<(Long, Long) -> Long>): List<Long> {
    fun foo(index: Int): List<Long> {
        val operand = operands[index]
        if (index == 0) {
            return listOf(operand)
        }

        val next = foo(index - 1)
        return operations.flatMap { operation -> next.map { operation(it, operand) } }
    }

    return foo(operands.size - 1)
}

fun main() {
    fun part1(content: String): Long {
        val equations = content.trim().lines().map { parseEquation(it) }
        return equations.filter {
            it.result in findAllResults(it.operands, listOf(
                { a, b -> a + b },
                { a, b -> a * b },
            ))
        }.sumOf { it.result }
    }


    fun part2(content: String): Long {
        val equations = content.trim().lines().map { parseEquation(it) }
        return equations.filter {
            it.result in findAllResults(it.operands, listOf(
                { a, b -> a + b },
                { a, b -> a * b },
                { a, b -> (a.toString() + b.toString()).toLong() },
            ))
        }.sumOf { it.result }
    }

    val contentTest = File("src", "day07_test.txt").readText()
    val content = File("src", "day07.txt").readText()

    assertEq(part1(contentTest), 3749)
    assertEq(part1(content), 20665830408335)


    assertEq(part2(contentTest), 11387)
    assertEq(part2(content), 354060705047464)
}
