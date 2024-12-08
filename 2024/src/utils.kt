fun <T> assertEq(got: T, expected: T) {
    if (got != expected) {
        throw AssertionError("Expected $expected, got $got")
    }
}

data class Vector2(val x: Int, val y: Int) {
    operator fun plus(v: Vector2) = Vector2(x + v.x, y + v.y)
    operator fun minus(v: Vector2) = Vector2(x - v.x, y - v.y)
    operator fun unaryMinus() = Vector2(-x, -y)
    operator fun times(factor: Int) = Vector2(x * factor, y * factor)
    operator fun div(factor: Int) = Vector2(x / factor, y / factor)
}