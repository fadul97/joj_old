#include "joj/resources/submesh.h"

joj::Submesh::Submesh()
    : name("")
    , vertex_start(-1)
    , vertex_count(-1)
    , index_start(-1)
    , index_count(-1)
{
    local_transform = Matrix4x4::identity();
}

joj::Submesh::~Submesh()
{
}