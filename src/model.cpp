#include <model.hpp>
#include <utils.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <memory>
#include <cassert>
#include <string>

namespace std {
template <>
struct hash<engine::Model::Vertex> {
    size_t operator()(engine::Model::Vertex const &vertex) const {
        size_t seed = 0;
        engine::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
        return seed;
    }
};
}  // namespace std

namespace engine {

Model::Model(Device& deviceRef, const Model::Builder &builder) : device{deviceRef} {
    createVertexBuffers(builder.vertices);
    createIndexBuffers(builder.indices);
}

Model::~Model() {}

void Model::Builder::loadModel(const std::string& filepath) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
        throw std::runtime_error(warn + err);
    }

    vertices.clear();
    indices.clear();

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};
    for (const tinyobj::shape_t &shape : shapes) {
        for (const tinyobj::index_t &index : shape.mesh.indices) {
        Vertex vertex{};

        if (index.vertex_index >= 0) {
            vertex.position = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2],
            };

            vertex.color = {
                attrib.colors[3 * index.vertex_index + 0],
                attrib.colors[3 * index.vertex_index + 1],
                attrib.colors[3 * index.vertex_index + 2],
            };
        }

        if (index.normal_index >= 0) {
            vertex.normal = {
                attrib.normals[3 * index.normal_index + 0],
                attrib.normals[3 * index.normal_index + 1],
                attrib.normals[3 * index.normal_index + 2],
            };
        }

        if (index.texcoord_index >= 0) {
            vertex.uv = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                attrib.texcoords[2 * index.texcoord_index + 1],
            };
        }

        if (uniqueVertices.count(vertex) == 0) {
            uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
            vertices.push_back(vertex);
        }
        indices.push_back(uniqueVertices[vertex]);
        }
    }
}

std::unique_ptr<Model> Model::createModelFromFile(Device &deviceRef, const std::string &filepath) {
    Builder builder{};
    builder.loadModel(filepath);
    return std::make_unique<Model>(deviceRef, builder);
}

void Model::bind(VkCommandBuffer commandBuffer) {
    VkBuffer buffers[] {vertexBuffer->getBuffer()};
    VkDeviceSize offsets[] {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

    if (hasIndexBuffer) {
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
    }
}

void Model::draw(VkCommandBuffer commandBuffer) {
    if (hasIndexBuffer) {
        vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
    }
    else {
        vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
    }
}

void Model::createVertexBuffers(const std::vector<Vertex> &vertices) {
    vertexCount = static_cast<uint32_t>(vertices.size());
    assert(vertexCount >= 3 && "Vertex count must be at least 3!");
    uint32_t vertexSize = sizeof(vertices[0]);
    VkDeviceSize bufferSize = vertexSize * vertexCount;

    Buffer stagingBuffer{
        device,
        vertexSize,
        vertexCount,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer(static_cast<void*>(const_cast<engine::Model::Vertex *>(vertices.data()))); // equivalent to (void *)vertices.data()

    vertexBuffer = std::make_unique<Buffer>(
        device,
        vertexSize,
        vertexCount,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    device.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
}

void Model::createIndexBuffers(const std::vector<uint32_t>& indices) {
    indexCount = static_cast<uint32_t>(indices.size());
    hasIndexBuffer = indexCount > 0;

    if (!hasIndexBuffer) {
        return;
    }

    uint32_t indexSize = sizeof(indices[0]);
    VkDeviceSize bufferSize = indexSize * indexCount;

    Buffer stagingBuffer{
    device,
    indexSize,
    indexCount,
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer(static_cast<void *>(const_cast<uint32_t *>(indices.data()))); // equivalent to (void *)indices.data()

    indexBuffer = std::make_unique<Buffer>(
    device,
    indexSize,
    indexCount,
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    device.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
}

std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescriptions() {
    std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
    bindingDescriptions[0].binding      = 0;
    bindingDescriptions[0].stride       = sizeof(Vertex);
    bindingDescriptions[0].inputRate    = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions() {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(4);
    attributeDescriptions[0].location   = 0;
    attributeDescriptions[0].binding    = 0;
    attributeDescriptions[0].format     = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset     = offsetof(Vertex, position);

    attributeDescriptions[1].location   = 1;
    attributeDescriptions[1].binding    = 0;
    attributeDescriptions[1].format     = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset     = offsetof(Vertex, color);

    attributeDescriptions[2].location   = 2;
    attributeDescriptions[2].binding    = 0;
    attributeDescriptions[2].format     = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[2].offset     = offsetof(Vertex, normal);

    attributeDescriptions[3].location   = 3;
    attributeDescriptions[3].binding    = 0;
    attributeDescriptions[3].format     = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[3].offset     = offsetof(Vertex, uv);

    return attributeDescriptions;
}

} // namespace engine