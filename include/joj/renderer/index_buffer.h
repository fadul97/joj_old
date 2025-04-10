#ifndef _JOJ_INDEX_BUFFER_H
#define _JOJ_INDEX_BUFFER_H

#include "joj/core/defines.h"

#include "joj/core/error_code.h"
#include "joj/renderer/buffer_usage.h"
#include "joj/renderer/cpu_access_type.h"
#include "joj/renderer/data_format.h"

namespace joj
{
    /**
     * @brief GraphicsDevice is a struct that holds the graphics device.
     * It is API dependent, so each API will have its own implementation.
     *  
     * Forward declaration to avoid increasing compile time.
     */
    struct GraphicsDevice;

    /**
     * @brief CommandList is a struct that holds the command list.
     * It is API dependent, so each API will have its own implementation.
     * 
     * Forward declaration to avoid increasing compile time.
     * 
     */
    struct CommandList;

    /**
     * @brief IndexBufferData is a struct that holds the index buffer data.
     * It is API dependent, so each API will have its own implementation.
     * 
     * Forward declaration to avoid increasing compile time.
     */
    struct IndexBufferData;

    class JOJ_API IIndexBuffer
    {
    public:
        /**
         * @brief Construct a new IIndexBuffer object.
         */
        IIndexBuffer();

        /**
         * @brief Construct a new IIndexBuffer object.
         * 
         * @param device The graphics device.
         * @param cmd_list The command list.
         * 
         * @note The device and the command list are optional.
         * It is up to the API to decide whether it is necessary or not.
         * 
         * @note The memory of the device and the command list will not be managed by the IIndexBuffer class.
         */
        IIndexBuffer(GraphicsDevice* device, CommandList* cmd_list);

        /**
         * @brief Destroy the IIndexBuffer object.
         */
        virtual ~IIndexBuffer();

        /**
         * @brief Create the Index Buffer.
         * 
         * @param usage Type of usage for the Index Buffer.
         * @param cpu_access_flags Access type that the CPU has on the Index Buffer.
         * @param byte_width The size of the buffer in bytes.
         * @param data The data that will be copied to the buffer.
         * @return ErrorCode The error code that will be returned if the buffer
         * creation fails. If the buffer creation is successful, it will return
         * ErrorCode::OK.
         */
        virtual ErrorCode create(const BufferUsage usage, const CPUAccessType cpu_access_flags,
            const u32 byte_width, const void* data) = 0;

        /**
         * @brief Bind the Index Buffer to the pipeline.
         * 
         * @param format The format of the data.
         * @param offset The offset of the data.
         */
        virtual void bind(const DataFormat format, const u32 offset) = 0;

        /**
         * @brief Unbind the Index Buffer from the pipeline.
         */
        virtual void unbind() = 0;

        /**
         * @brief Update the Index Buffer.
         * 
         * @tparam T The type of data that will be updated.
         * @param cmd_list The command list.
         * @param data The data that will be updated.
         * @return ErrorCode The error code that will be returned if the buffer
         * update fails. If the buffer update is successful, it will return
         * ErrorCode::OK.
         * 
         * @note This method will call the update_internal method with
         * more specific parameters.
         */
        template <typename T>
        ErrorCode update(T& data);

        /**
         * @brief Return if the Index Buffer is filled.
         * 
         * @return b8 True if the Index Buffer is filled, false otherwise.
         */
        b8 is_filled() const;

        /**
         * @brief Cleanup the Index Buffer.
         * It will release the resources that were created in the create method.
         * 
         * This method is called in the destructor of the IIndexBuffer class.
         */
        virtual void cleanup() = 0;

    protected:

        /**
         * @brief Update the Index Buffer.
         * 
         * @param data The data that will be copied to the buffer.
         * @param sizeof_data The size of the data in bytes.
         * @return ErrorCode The error code that will be returned if the buffer
         * update fails. If the buffer update is successful, it will return
         * ErrorCode::OK.
         */
        virtual ErrorCode update_internal(const void* data, const u32 sizeof_data) = 0;
    };

    template <typename T>
    inline ErrorCode IIndexBuffer::update(T& data)
    {
        // static_assert(sizeof(T) <= max_buffer_size, "Data size exceeds constant buffer limits.");
        return update_internal(&data, sizeof(T));
    }
}

#endif // _JOJ_INDEX_BUFFER_H