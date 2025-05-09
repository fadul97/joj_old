#ifndef _JOJ_GLTF_IMPORTER_H
#define _JOJ_GLTF_IMPORTER_H

#include "joj/core/defines.h"

#include "joj/core/error_code.h"
#include <string>
#include <vector>
#include "joj/core/math/vector3.h"
#include "gltf_animation_sampler.h"
#include "gltf_buffer_view.h"
#include "gltf_accessor.h"
#include "gltf_mesh.h"
#include "gltf_animation.h"
#include "gltf_skin.h"
#include "gltf_scene_data.h"
#include "gltf_node.h"
#include "gltf_model.h"
#include "gltf_scene.h"
#include "joj/resources/buffer.h"
#include "joj/resources/animation.h"
#include "joj/utils/json_value.h"
#include "joj/resources/mesh.h"

// TODO: Use Namespace Memory
#include <string.h>

namespace joj
{
    struct GLTFVertex
    {
        Vector3 pos;
        Vector4 color;
        Vector3 normal;
    };

    constexpr i32 BUFFER_VIEW_TARGET_ARRAY_BUFFER = 34962;
    constexpr i32 BUFFER_VIEW_TARGET_ELEMENT_ARRAY_BUFFER = 34963;

    class JOJ_API GLTFImporter
    {
    public:
        GLTFImporter();
        ~GLTFImporter();

        ErrorCode load(const char* file_path);

        void get_vertices(std::vector<GLTFVertex>& vertices);
        void get_indices(std::vector<u16>& indices);
        void get_vertices_and_indices(std::vector<GLTFVertex>& vertices, std::vector<u16>& indices);

        const GLTFModel* get_model() const;
        const GLTFScene* get_scene() const;

        void get_meshes(std::vector<GLTFMesh>& meshes);

        std::vector<GLTFAccessor>& get_accessors();
        const GLTFAccessor& get_accessor(const u32 index) const;

        const Buffer& get_buffer(const GLTFAccessor& accessor) const;
        const GLTFBufferView& get_buffer_view(const GLTFAccessor& accessor) const;

        void setup_mesh(GLTFMesh& gltf_mesh, Mesh& mesh);
        void setup_meshes(std::vector<GLTFMesh>& gltf_meshes, Mesh& mesh);
        void setup_aggregated_mesh(const GLTFNode& node, Mesh& mesh);
        void build_aggregated_meshes();
        void setup_aggregated_meshes(Mesh& mesh);

        template <typename T>
        std::vector<T> read_buffer(const u32 accessor_index) const
        {
            const auto& accessor = get_accessor(accessor_index);
            const auto& bufferView = get_buffer_view(accessor);
            const auto& buffer = get_buffer(accessor);

            return read_buffer_internal<T>(buffer, accessor, bufferView);
        }

    private:
        std::string m_gltf_filename;
        std::string m_bin_filename;

        std::vector<Vector3> m_positions;
        std::vector<Vector3> m_normals;
        std::vector<u16> m_indices;
        std::vector<Vector3> m_translations;
        std::vector<Vector4> m_rotations;
        std::vector<Vector3> m_scales;
        std::vector<GLTFAnimationSampler> m_samplers;
        std::vector<GLTFNode> m_nodes;
        std::vector<GLTFAccessor> m_accessors;
        std::vector<GLTFBufferView> m_buffer_views;
        std::vector<Buffer> m_buffers;
        std::vector<GLTFMesh> m_meshes;
        std::vector<GLTFAnimation> m_animations;
        std::vector<GLTFSkin> m_skins;
        std::vector<GLTFSceneData> m_scenes;

        i32 m_positions_byte_offset;
        i32 m_normals_byte_offset;
        i32 m_indices_byte_offset;
        i32 m_animations_byte_offset;
        i32 m_translation_byte_offset;
        i32 m_rotation_byte_offset;
        i32 m_scale_byte_offset;

        i32 m_positions_count;
        i32 m_normals_count;
        i32 m_indices_count;
        i32 m_animations_count;
        i32 m_translation_count;
        i32 m_rotation_count;
        i32 m_scale_count;

        JsonValue m_root;
        
        GLTFModel m_model;
        GLTFScene m_scene;

        b8 parse_json();

        Buffer load_binary_file(const char* filename);
        void load_data_buffer(const size_t byte_offset, const size_t count);

        b8 load_buffers();
        void print_buffers();
        void write_buffers_to_file(const char* filename) const;

        b8 load_buffer_views();
        void print_buffer_views();
        void write_buffer_views_to_file(const char* filename) const;

        b8 load_accessors();
        void print_accessors();
        void write_accessors_to_file(const char* filename) const;

        b8 load_nodes();
        void print_nodes();
        void write_nodes_to_file(const char* filename) const;

        b8 load_meshes();
        void print_meshes();
        void write_meshes_to_file(const char* filename) const;

        b8 load_animations();
        void print_animations();
        void write_animations_to_file(const char* filename) const;

        b8 load_skins();
        void print_skins();
        void write_skins_to_file(const char* filename) const;

        b8 load_scenes();
        void print_scenes();
        void write_scenes_to_file(const char* filename) const;

        void write_vec3_vector_to_file(const char* filename, const std::vector<Vector3>& data, const char* top_line) const;
        void write_vec4_vector_to_file(const char* filename, const std::vector<Vector4>& data, const char* top_line) const;
        void write_u16_vector_to_file(const char* filename, const std::vector<u16>& data, const char* top_line) const;

        void build_model();
        void build_model_new();

        void build_scene();
        void process_root_node(const u32 node_index);
        void process_mesh(const u32 mesh_index);
        void process_primitive(const GLTFPrimitive& primitive, const std::string& mesh_name);

        template <typename T>
        std::vector<T> process_accessor(const i32 accessor_index)
        {
            std::vector<T> result;

            // If the accessor index is invalid, return an empty vector
            if (accessor_index == -1)
            {
                return result;
            }

            // Get the accessor from the index
            const GLTFAccessor& accessor = m_accessors[accessor_index];

            // Get the buffer view from the accessor
            const GLTFBufferView& buffer_view = m_buffer_views[accessor.buffer_view];

            // Get the buffer from the buffer view
            const Buffer& buffer = m_buffers[buffer_view.buffer];

            // Read the buffer data
            result = read_buffer_internal<T>(buffer, accessor, buffer_view);
            
            return result;
        }

        template <typename T>
        std::vector<T> read_buffer_internal(const Buffer& buffer, const GLTFAccessor& accessor, const GLTFBufferView& bufferView) const
        {
            std::vector<T> data;
            size_t element_size = sizeof(T);
            size_t count = accessor.count;
            size_t stride = (bufferView.byte_stride > 0) ? bufferView.byte_stride : element_size;
            size_t start_offset = bufferView.byte_offset + accessor.byte_offset;

            /*
            std::cout << "    Reading buffer... " << std::endl;
            std::cout << "        Element Size: " << element_size << " bytes" << std::endl;
            std::cout << "        Count: " << count << std::endl;
            std::cout << "        Stride: " << stride << std::endl;
            std::cout << "        Start Offset: " << start_offset << std::endl;
            std::cout << "        Buffer View Size: " << bufferView.byte_length << std::endl;
            std::cout << "        Buffer Size: " << buffer.data.size() << std::endl;
            */

            data.resize(count);
            for (size_t i = 0; i < count; i++)
            {
                size_t offset = start_offset + (i * stride);

                if (offset + element_size > buffer.data.size())
                {
                    std::cerr << "ERROR: Offset " << offset << " exceeds buffer size " << buffer.data.size() << std::endl;
                    break;
                }

                memcpy(&data[i], &buffer.data[offset], element_size);
            }

            return data;
        }
    };
}

#endif // _JOJ_GLTF_IMPORTER_H