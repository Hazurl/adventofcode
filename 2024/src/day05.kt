import java.io.File

typealias Page = Int
typealias Update = List<Page>

data class OrderingRule(var before: Int, var after: Int) {
    fun respectedIn(update: Update): Boolean {
        var seenAfter = false
        for (page in update) {
            if (page == after) {
                seenAfter = true
            } else if (page == before && seenAfter) {
                return false
            }
        }
        return true
    }
}

fun parseRules(content: String): List<OrderingRule> {
    return content.lines().filter { it.isNotBlank() }.map {
        val parts = it.split("|").map(String::toInt)
        return@map OrderingRule(parts[0], parts[1])
    }
}

fun parseUpdates(content: String): List<Update> {
    return content.lines().filter { it.isNotBlank() }.map {
        it.split(",").map(String::toInt)
    }
}

fun parseRulesAndUpdates(content: String): Pair<List<OrderingRule>, List<Update>> {
    val parts = content.split("\r\n\r\n")
    return Pair(
        parseRules(parts[0]),
        parseUpdates(parts[1])
    )
}


fun main() {
    fun isUpdateCorrect(update: Update, rules: List<OrderingRule>): Boolean {
        return rules.all { it.respectedIn(update) }
    }

    fun part1(content: String): Int {
        val (rules, updates) = parseRulesAndUpdates(content)
        return updates.filter { isUpdateCorrect(it, rules) }.sumOf { it[it.size / 2] }
    }

    fun reorderUpdate(update: Update, rules: List<OrderingRule>): Update {
        val pagesInUpdate = update.toMutableSet()
        val pagesBefore = mutableMapOf<Page, MutableSet<Page>>()
        for (rule in rules) {
            if (pagesInUpdate.contains(rule.before) && pagesInUpdate.contains(rule.after)) {
                if (!pagesBefore.contains(rule.after)) {
                    pagesBefore[rule.after] = mutableSetOf()
                }
                pagesBefore[rule.after]!!.add(rule.before)
            }
        }

        val newUpdate = mutableListOf<Page>()
        val pagesAdded = mutableSetOf<Page>()
        while (pagesInUpdate.isNotEmpty()) {
            val page = pagesInUpdate.first { (pagesBefore.getOrDefault(it, setOf())- pagesAdded).isEmpty() }
            newUpdate.add(page)
            pagesAdded.add(page)
            pagesInUpdate.remove(page)
        }
        return newUpdate
    }

    fun part2(content: String): Int {
        val (rules, updates) = parseRulesAndUpdates(content)
        val incorrectUpdates = updates.filter { !isUpdateCorrect(it, rules) }
        return incorrectUpdates.map{ reorderUpdate(it, rules) }.sumOf { it[it.size / 2] }
    }

    val contentTest = File("src", "day05_test.txt").readText()
    val content = File("src", "day05.txt").readText()

    assertEq(part1(contentTest), 143)
    assertEq(part1(content),  5329)

    assertEq(part2(contentTest), 123)
    assertEq(part2(content),  5833)
}
