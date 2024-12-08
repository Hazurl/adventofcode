import java.io.File

data class Antenna(val position: Vector2, val frequency: Char)
data class Map(val width: Int, val height: Int, val antennas: List<Antenna>) {
    fun contains(position: Vector2): Boolean {
        return position.x in 0..<width && position.y in 0..<height
    }
}


fun main() {
    fun parseMap(content: String): Map {
        val rows = content.trim().lines()
        val antennas = mutableListOf<Antenna>()
        val width = rows[0].length
        val height = rows.size

        for (y in 0..<height) {
            for (x in 0..<width) {
                val c = rows[y][x]
                if (c != '.') {
                    antennas.add(Antenna(Vector2(x, y), c))
                }
            }
        }

        return Map(width, height, antennas)
    }

    fun part1(content: String): Int {
        val map = parseMap(content)
        val antennasPerFreq = map.antennas.groupBy { it.frequency }
        val antiNodes = mutableSetOf<Vector2>()

        for ((freq, antennas) in antennasPerFreq) {
            for (lhs in antennas) {
                for (rhs in antennas) {
                    if (lhs == rhs) {
                        continue
                    }

                    val diff = rhs.position - lhs.position
                    antiNodes.add(rhs.position + diff)
                    antiNodes.add(lhs.position - diff)
                }
            }
        }

        return antiNodes.count { map.contains(it) }
    }


    fun part2(content: String): Int {
        val map = parseMap(content)
        val antennasPerFreq = map.antennas.groupBy { it.frequency }
        val antiNodes = mutableSetOf<Vector2>()

        for ((freq, antennas) in antennasPerFreq) {
            for (lhs in antennas) {
                for (rhs in antennas) {
                    if (lhs == rhs) {
                        continue
                    }

                    val diff = rhs.position - lhs.position

                    for(k in generateSequence(0, Int::inc)) {
                        val antiNode = rhs.position + (diff * k)
                        if (map.contains(antiNode)) {
                            antiNodes.add(antiNode)
                        } else {
                            break
                        }
                    }
                    for(k in generateSequence(0, Int::inc)) {
                        val antiNode = lhs.position - (diff * k)
                        if (map.contains(antiNode)) {
                            antiNodes.add(antiNode)
                        } else {
                            break
                        }
                    }
                }
            }
        }

        return antiNodes.size
    }

    val contentTest = File("src", "day08_test.txt").readText()
    val content = File("src", "day08.txt").readText()

    assertEq(part1(contentTest), 14)
    assertEq(part1(content), 379)


    assertEq(part2(contentTest), 34)
    assertEq(part2(content), 1339)
}
