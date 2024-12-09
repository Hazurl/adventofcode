import java.io.File

data class Block(var id: Int?, var size: Int) {
    fun split(at: Int): Pair<Block, Block?> {
        return if (size <= at) {
            Pair(this, null)
        } else {
            Pair(Block(id, at), Block(id, size - at))
        }
    }

    fun isEmpty(): Boolean {
        return id == null
    }
}

class Node(var previous: Node?, var next: Node?, var block: Block) {
    fun insertNext(node: Node) {
        check(node.next == null && node.previous == null)
        if (next != null) {
            next!!.previous = node
        }
        node.next = next
        node.previous = this
        next = node
    }

    fun remove(): Node {
        if (next != null) {
            next!!.previous = previous
            next = null
        }
        if (previous != null) {
            previous!!.next = next
            previous = null
        }
        return this
    }
}

fun main() {
    fun parseFilesystem(content: String): MutableList<Block> {
        val blocks = mutableListOf<Block>()

        var isEmpty = false
        var blockId = 0
        for (size in content.trim().map { "$it".toInt() }) {
            if (isEmpty) {
                blocks.add(Block(null, size))
                isEmpty = false
            } else {
                blocks.add(Block(blockId, size))
                blockId++
                isEmpty = true
            }
        }

        return blocks
    }

    fun asNodes(blocks: List<Block>): Pair<Node, Node> {
        check(blocks.isNotEmpty())
        val firstNode = Node(null, null, blocks[0])
        var lastNode = firstNode
        for ((index, block) in blocks.withIndex()) {
            if (index == 0) {
                continue
            }
            lastNode.insertNext(Node(null, null, block))
            lastNode = lastNode.next!!
        }

        return Pair(firstNode, lastNode)
    }

    fun part1(content: String): Long {
        val blocks = parseFilesystem(content)

        var checksum = 0L
        var checksumIndex = 0L

        fun addToChecksum(block: Block) {
            for (i in 0..<block.size) {
                checksum += checksumIndex * block.id!!
                ++checksumIndex
            }
        }

        while (blocks.isNotEmpty()) {
            val block = blocks.first()
            if (block.isEmpty()) {
                val endBlock = blocks.removeLast()
                check(!endBlock.isEmpty())
                val (fit, notFit) = endBlock.split(block.size)
                block.size -= fit.size
                if (block.size <= 0) {
                    blocks.removeFirst()
                }
                addToChecksum(fit)
                if (notFit != null) {
                    blocks.add(notFit)
                } else {
                    check(blocks.removeLast().isEmpty())
                }
            } else {
                addToChecksum(block)
                blocks.removeFirst()
            }
        }


        return checksum
    }

    fun part2(content: String): Long {
        val blocks = parseFilesystem(content)
        val (first, last) = asNodes(blocks)

        fun getLeftMostNodeWithSpace(notPastNode: Node, size: Int): Node? {
            var leftMostBlock = first
            while (leftMostBlock != notPastNode) {
                if (leftMostBlock.block.isEmpty() && leftMostBlock.block.size >= size) {
                    return leftMostBlock
                } else {
                    leftMostBlock = leftMostBlock.next!! //  Should never be null otherwise it must have crossed `blockToCheck`
                }
            }
            return null
        }

        fun moveInto(node: Node, into: Node) {
            val remainingEmptySpace = into.block.size - node.block.size

            if (remainingEmptySpace > 0) {
                into.insertNext(Node(null, null, Block(null, remainingEmptySpace)))
            }
            into.block = node.block
            node.block = Block(null, node.block.size)
        }

        var nodeToMove: Node? = last

        val blockIdsSeen = mutableSetOf<Int>()

        while (nodeToMove != null) {
            if (nodeToMove.block.isEmpty() || nodeToMove.block.id in blockIdsSeen) {
                nodeToMove = nodeToMove.previous
                continue
            }

            var into = getLeftMostNodeWithSpace(nodeToMove, nodeToMove.block.size)
            blockIdsSeen.add(nodeToMove.block.id!!)
            if (into != null) {
                moveInto(nodeToMove, into)
            }
            nodeToMove = nodeToMove.previous
        }

        var checksum = 0L
        var checksumIndex = 0L

        fun addToChecksum(block: Block) {
            if(block.isEmpty()) {
                checksumIndex += block.size
                return
            }

            for (i in 0..<block.size) {
                checksum += checksumIndex * block.id!!
                ++checksumIndex
            }
        }

        var node: Node? = first
        while(node != null) {
            addToChecksum(node.block)
            node = node.next
        }

        return checksum
    }

    val contentTest = File("src", "day09_test.txt").readText()
    val content = File("src", "day09.txt").readText()

    assertEq(part1(contentTest), 1928)
    assertEq(part1(content), 6288707484810)

    assertEq(part2(contentTest), 2858)
    assertEq(part2(content), 6311837662089)
}
