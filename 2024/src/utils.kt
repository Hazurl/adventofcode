fun <T> assertEq(got: T, expected: T) {
    if (got != expected) {
        throw AssertionError("Expected $got, got $expected")
    }
}