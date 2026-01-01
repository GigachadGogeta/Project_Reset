#include <buffer.hpp>

#include <cassert>

namespace engine{

VkDeviceSize Buffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
    if (minOffsetAlignment > 0) {
        return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
    }
    return instanceSize;
}

Buffer::Buffer(
    Device &deviceRef,
    VkDeviceSize instanceSize,
    uint32_t instanceCount,
    VkBufferUsageFlags usageFlags,
    VkMemoryPropertyFlags memoryPropertyFlags,
    VkDeviceSize minOffsetAlignment
    ) : device{deviceRef},
        instanceSize{instanceSize},
        instanceCount{instanceCount},
        usageFlags{usageFlags},
        memoryPropertyFlags{memoryPropertyFlags} {
    alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
    bufferSize = alignmentSize * instanceCount;
    device.createBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer, memory);
}

Buffer::~Buffer() {
    unmap();
    vkDestroyBuffer(device.getLogicalDevice(), buffer, nullptr);
    vkFreeMemory(device.getLogicalDevice(), memory, nullptr);
}

VkResult Buffer::map(VkDeviceSize size, VkDeviceSize offset) {
    assert(buffer && memory && "Called map on buffer before create");
    return vkMapMemory(device.getLogicalDevice(), memory, offset, size, 0, &mapped);
}

void Buffer::unmap() {
    if (mapped) {
        vkUnmapMemory(device.getLogicalDevice(), memory);
        mapped = nullptr;
    }
}

void Buffer::writeToBuffer(void *data, VkDeviceSize size, VkDeviceSize offset) {
    assert(mapped && "Cannot copy to unmapped buffer");

    if (size == VK_WHOLE_SIZE) {
        memcpy(mapped, data, bufferSize);
    } else {
        char *memOffset = static_cast<char *>(mapped);
        memOffset += offset;
        memcpy(memOffset, data, size);
    }
}

VkResult Buffer::flush(VkDeviceSize size, VkDeviceSize offset) {
    VkMappedMemoryRange mappedRange{};
    mappedRange.sType   = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory  = memory;
    mappedRange.offset  = offset;
    mappedRange.size    = size;
    return vkFlushMappedMemoryRanges(device.getLogicalDevice(), 1, &mappedRange);
}

VkResult Buffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
    VkMappedMemoryRange mappedRange {};
    mappedRange.sType   = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory  = memory;
    mappedRange.offset  = offset;
    mappedRange.size    = size;
    return vkInvalidateMappedMemoryRanges(device.getLogicalDevice(), 1, &mappedRange);
}

VkDescriptorBufferInfo Buffer::createDescriptorBufferInfo(VkDeviceSize size, VkDeviceSize offset) {
    return VkDescriptorBufferInfo{
        buffer,
        offset,
        size,
    };
}

void Buffer::writeToIndex(void *data, int index) {
    writeToBuffer(data, instanceSize, index * alignmentSize);
}

VkResult Buffer::flushIndex(int index) { return flush(alignmentSize, index * alignmentSize); }

VkDescriptorBufferInfo Buffer::createDescriptorBufferInfoForIndex(int index) {
    return createDescriptorBufferInfo(alignmentSize, index * alignmentSize);
}

VkResult Buffer::invalidateAtIndex(int index) {
    return invalidate(alignmentSize, index * alignmentSize);
}

} // namespace engine