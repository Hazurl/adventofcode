import java.awt.image.BufferedImage
import java.io.File
import javax.imageio.ImageIO

data class Robot(var position: Vector2I, val velocity: Vector2I) {
    fun moveStep(mapSize: Vector2I) {
        position = (position + velocity).wrap(mapSize)
    }
}

fun main() {

    fun parseRobot(content: String): Robot {
        val reg = Regex("""p=(\d+),(\d+) v=(-?\d+),(-?\d+)""")
        val result = reg.find(content)!!
        val (px, py, vx, vy) = result.destructured
        return Robot(
            Vector2I(px.toInt(), py.toInt()),
            Vector2I(vx.toInt(), vy.toInt()),
        )
    }

    fun parseRobots(content: String): List<Robot> {
        return content.trim().lines().map { parseRobot(it) }
    }

    fun generateImage(robots: List<Robot>, mapSize: Vector2I): BufferedImage {
        val img = BufferedImage(mapSize.x, mapSize.y, BufferedImage.TYPE_INT_RGB)
        val positionsWithRobot = robots.map { it.position }.toSet()

        for (x in 0..<mapSize.x) {
            for (y in 0..<mapSize.y) {
                val position = Vector2I(x, y)
                img.setRGB(
                    x, y, if (positionsWithRobot.contains(position)) {
                        0x000000
                    } else {
                        0xffffff
                    }
                )
            }
        }

        return img
    }

    fun printGrid(robots: List<Robot>, mapSize: Vector2I) {
        val positionsWithRobot = robots.map { it.position }.toSet()

        val grid = StringBuilder()
        for (y in 0..<mapSize.y) {
            for (x in 0..<mapSize.x) {
                val position = Vector2I(x, y)
                grid.append(if (positionsWithRobot.contains(position)) {
                    '#'
                } else {
                    ' '
                })
            }
            grid.append("\n")
        }

        println(grid)
    }

    fun part1(content: String, mapSize: Vector2I): Long {
        val robots = parseRobots(content)

        var tlQuadrant = 0L
        var trQuadrant = 0L
        var blQuadrant = 0L
        var brQuadrant = 0L

        for (robot in robots) {
            for (i in 0..<100) {
                robot.moveStep(mapSize)
            }

            val tlDistance = robot.position.manhattanDistance(Vector2I(0, 0))
            val trDistance = robot.position.manhattanDistance(Vector2I(mapSize.x - 1, 0))
            val blDistance = robot.position.manhattanDistance(Vector2I(0, mapSize.y - 1))
            val brDistance = robot.position.manhattanDistance(Vector2I(mapSize.x - 1, mapSize.y - 1))

            if (listOf(trDistance, blDistance, brDistance).all { it > tlDistance }) {
                tlQuadrant++
            } else if (listOf(tlDistance, blDistance, brDistance).all { it > trDistance }) {
                trQuadrant++
            } else if (listOf(tlDistance, trDistance, brDistance).all { it > blDistance }) {
                blQuadrant++
            } else if (listOf(tlDistance, trDistance, blDistance).all { it > brDistance }) {
                brQuadrant++
            }
        }
        return tlQuadrant * trQuadrant * blQuadrant * brQuadrant
    }

    fun part2(content: String, mapSize: Vector2I): Int {
        val robots = parseRobots(content)

        var steps = 6285

        for (step in 0..<steps) {
            for (robot in robots) {
                robot.moveStep(mapSize)
            }

        }

        printGrid(robots, mapSize)

        return steps
    }

    val contentTest = File("src", "day14_test.txt").readText()
    val content = File("src", "day14.txt").readText()

    val testMapSize = Vector2I(11, 7)
    val mapSize = Vector2I(101, 103)

    assertEq(part1(contentTest, testMapSize), 12)
    assertEq(part1(content, mapSize), 215476074)

    assertEq(part2(content, mapSize), 6285)
}

