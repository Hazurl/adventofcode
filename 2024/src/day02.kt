import java.io.File
import kotlin.math.absoluteValue


fun main() {
    fun isReportSafe(report: List<Int>): Boolean {
        val diffs = report.zipWithNext().map { it.second - it.first }
        val increasing  = diffs.all { it >= 0 }
        val decreasing  = diffs.all { it <= 0 }
        val safe = diffs.all { it.absoluteValue in 1..3 }
        return (increasing || decreasing) && safe
    }

    fun alterReport(report: List<Int>, index: Int): List<Int> {
        return report.subList(0, index) + report.subList(index + 1, report.size)
    }

    fun part1(content: String): Int {
        var reports = content.lines().filterNot { it.isBlank() }.map { it.split(' ').map(String::toInt) }
        return reports.count { isReportSafe(it) }
    }

    fun part2(content: String): Int {
        var reports = content.lines().filterNot { it.isBlank() }.map { it.split(' ').map(String::toInt) }
        return reports.count { report ->
            isReportSafe(report) || (0..<report.count()).any {
                isReportSafe(alterReport(report, it))
            }
        }
    }

    val contentTest = File("src", "day02_test.txt").readText()
    val content = File("src", "day02.txt").readText()

    assertEq(part1(contentTest), 2)
    assertEq(part1(content),  472)

    assertEq(part2(contentTest), 4)
    assertEq(part2(content),  520)
}
