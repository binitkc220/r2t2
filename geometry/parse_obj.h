#ifndef GEOMETRY_PARSE_OBJ_HH
#define GEOMETRY_PARSE_OBJ_HH

#define TINYOBJLOADER_USE_DOUBLE
#include "../external/tiny_obj_loader.h"
#include <vector>
#include "../renderer/rt.h"
#include "../renderer/vec3.h"
#include "../geometry/bvh.h"
#include "../geometry/hittable_list.h"
#include "../geometry/triangle.h"
#include "../material/material.h"
#include "../material/perlin.h"
#include "../material/texture.h"

static inline hittable_list mesh(std::string objFile, std::string textureFile)
{
    hittable_list objs, world;
    auto perlin = make_shared<noise_texture>();
    auto perlin_lambert = make_shared<lambertian>(perlin);
    auto material = make_shared<lambertian>(color(1, 0, 0));
    auto image = make_shared<image_texture>(textureFile);
    auto car = make_shared<lambertian>(image);
    // auto car = make_shared<metal>(color(1,0,0),0.1);

    std::string inputfile = objFile;
    tinyobj::ObjReaderConfig reader_config;

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(inputfile, reader_config))
    {
        if (!reader.Error().empty())
        {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty())
    {
        std::cerr << "TinyObjReader: " << reader.Warning();
    }

    auto &attrib = reader.GetAttrib();
    auto &shapes = reader.GetShapes();
    std::vector<vec3> vert;
    std::vector<vec3> text;

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++)
    {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++)
            {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                size_t index = 3 * size_t(idx.vertex_index);
                tinyobj::real_t vx = attrib.vertices[index + 0];
                tinyobj::real_t vy = attrib.vertices[index + 1];
                tinyobj::real_t vz = attrib.vertices[index + 2];
                vec3 vxyz(vx, vy, vz);
                vert.push_back(vxyz);
                // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                if (idx.texcoord_index >= 0)
                {
                    tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                    tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                    vec3 t(tx, ty, 0);
                    text.push_back(t);
                }

                // Check if `normal_index` is zero or positive. negative = no normal data
                //   if (idx.normal_index >= 0) {
                //     tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
                //     tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
                //     tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];
                //   }
            }
            objs.add(make_shared<triangle>(vert.at(0), vert.at(1), vert.at(2), text.at(0), text.at(1), text.at(2), car));
            vert.clear();
            text.clear();
            index_offset += fv;
        }
    }
    world.add(make_shared<bvh_node>(objs));
    return world;
}

#endif
