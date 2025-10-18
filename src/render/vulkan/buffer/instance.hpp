#pragma once

#include "external.hpp"
#include "render/api/reusable.hpp"

class RenderObject;

/**
 * Manages all objects (instances) present in the scene
 */
class InstanceManager {

protected:

    size_t capacity = 0;
    size_t freed = 0;

    ReusableBuffer instance_buffer;
    ReusableBuffer attachment_buffer;
    std::vector<std::shared_ptr<RenderObject>> delegates;

    virtual void write(const RenderObject& delegate) = 0;
    void trim();

public:

    InstanceManager(VkBufferUsageFlags instance_flags, VkBufferUsageFlags attachment_flags);
    virtual ~InstanceManager();

    /// Create new delegate
    virtual std::shared_ptr<RenderObject> create() = 0;

    /// Write all delegates to the staging buffer
    void flush(CommandRecorder& recorder);

    /// Get the buffer object used for instance array storage
    const ReusableBuffer& getInstanceBuffer() const;

    /// Get the buffer object used for object data storage
    const ReusableBuffer& getAttachmentBuffer() const;

    /// Get the number of instances
    uint32_t count() const;

    std::vector<std::shared_ptr<RenderObject>> getDelegates() const;

};
