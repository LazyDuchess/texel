#include "dynamicmeshloader.h"
#include "texeltypes.h"
#include <cstdio>
#include <gccore.h>
#include "platform/renderer.h"

namespace DynamicMeshLoader{
    bool LoadFromFile(Mesh* mesh, const char* path){
        FILE* file = fopen(path, "rb");
        if (!file){
            SYS_Report("dynamic mesh loader couldn't load %s!\n", path);
            return false;
        }
        ui16 count;
        fread(&count, sizeof(count), 1, file);
        for(int i=0;i<count;i++){
            float x;
            float y;
            float z;
            fread(&x, sizeof(x), 1, file);
            fread(&y, sizeof(y), 1, file);
            fread(&z, sizeof(z), 1, file);
            mesh->verts.push_back(glm::vec3(x,y,z));
        }
        fread(&count, sizeof(count), 1, file);
        for(int i=0;i<count;i++){
            float u;
            float v;
            fread(&u, sizeof(u), 1, file);
            fread(&v, sizeof(v), 1, file);
            mesh->uvs.push_back(glm::vec2(u,v));
        }
        fread(&count, sizeof(count), 1, file);
        for(int i=0;i<count;i++){
            float x;
            float y;
            float z;
            fread(&x, sizeof(x), 1, file);
            fread(&y, sizeof(y), 1, file);
            fread(&z, sizeof(z), 1, file);
            mesh->normals.push_back(glm::vec3(x,y,z));
        }
        fread(&count, sizeof(count), 1, file);
        for(int i=0;i<count;i++){
            ui16 v1;
            ui16 v2;
            ui16 v3;

            ui16 n1;
            ui16 n2;
            ui16 n3;

            ui16 u1;
            ui16 u2;
            ui16 u3;

            fread(&v1, sizeof(v1), 1, file);
            fread(&v2, sizeof(v2), 1, file);
            fread(&v3, sizeof(v3), 1, file);

            fread(&n1, sizeof(n1), 1, file);
            fread(&n2, sizeof(n2), 1, file);
            fread(&n3, sizeof(n3), 1, file);

            fread(&u1, sizeof(u1), 1, file);
            fread(&u2, sizeof(u2), 1, file);
            fread(&u3, sizeof(u3), 1, file);

            mesh->indices.push_back(v1);
            mesh->indices.push_back(n1);
            mesh->indices.push_back(u1);

            mesh->indices.push_back(v2);
            mesh->indices.push_back(n2);
            mesh->indices.push_back(u2);

            mesh->indices.push_back(v3);
            mesh->indices.push_back(n3);
            mesh->indices.push_back(u3);
        }
        fclose(file);
        Renderer::ProcessMesh(mesh);
        return true;
    }
}