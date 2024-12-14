import java.io.File
import kotlin.math.round

data class MachineSettings(val buttonA: Vector2L, val buttonB: Vector2L, val prize: Vector2L)

fun main() {

    fun parseMachineSettings(content: String): MachineSettings{
        val reg = Regex("""Button A: X\+(\d+), Y\+(\d+)\r\nButton B: X\+(\d+), Y\+(\d+)\r\nPrize: X=(\d+), Y=(\d+)""")
        val result = reg.find(content)!!
        val (ax, ay, bx, by, px, py) = result.destructured
        return MachineSettings(
            Vector2L(ax.toLong(), ay.toLong()),
            Vector2L(bx.toLong(), by.toLong()),
            Vector2L(px.toLong(), py.toLong())
        )
    }

    fun parseAllMachineSettings(content: String): List<MachineSettings> {
        return content.split("\r\n\r\n").map { parseMachineSettings(it) }
    }

    fun part1(content: String): Long {
        val machines = parseAllMachineSettings(content)

        var tokens = 0L

        for (machine in machines) {
            val a = machine.buttonA
            val b = machine.buttonB
            val p = machine.prize

            val ay_ax = a.y.toDouble()/a.x.toDouble()
            val y_dx_x = p.y.toDouble() - ay_ax*p.x.toDouble()
            val by_dx_bx = b.y.toDouble()-ay_ax*b.x.toDouble()

            val b_count = round(y_dx_x / by_dx_bx).toLong()
            val a_count = round((p.x.toDouble() - b_count*b.x.toDouble())/a.x.toDouble()).toLong()

            val isSolved = p == a * a_count + b*b_count
            if (isSolved && a_count <= 100 && b_count <= 100) {
                tokens += a_count * 3 + b_count
            }
        }
        return tokens
    }

    fun part2(content: String): Long {
        val machines = parseAllMachineSettings(content)

        var tokens = 0L

        for (machine in machines) {
            val a = machine.buttonA
            val b = machine.buttonB
            val p = machine.prize + Vector2L(10000000000000L, 10000000000000L)

            val ay_ax = a.y.toDouble()/a.x.toDouble()
            val y_dx_x = p.y.toDouble() - ay_ax*p.x.toDouble()
            val by_dx_bx = b.y.toDouble()-ay_ax*b.x.toDouble()

            val b_count = round(y_dx_x / by_dx_bx).toLong()
            val a_count = round((p.x.toDouble() - b_count*b.x.toDouble())/a.x.toDouble()).toLong()

            val isSolved = p == a * a_count + b*b_count
            if (isSolved) {
                tokens += a_count * 3 + b_count
            }
        }
        return tokens
    }

    val contentTest = File("src", "day13_test.txt").readText()
    val content = File("src", "day13.txt").readText()

    assertEq(part1(contentTest), 480)
    assertEq(part1(content), 27105)

    assertEq(part2(contentTest), 875318608908)
    assertEq(part2(content), 101726882250942)
}
