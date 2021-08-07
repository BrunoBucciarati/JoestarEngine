#include "Scene.h"
#include <vector>

//test sphere
Mesh* GenUVSphere()
{
    Mesh* mesh = new Mesh();
    std::vector<Vertex> vertices;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    const float PI = 3.14159265359;
    for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
    {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                Vertex v;
                v.Position = glm::vec3(xPos, yPos, zPos);
                v.TexCoords = glm::vec2(xSegment, ySegment);
                v.Normal = glm::vec3(xPos, yPos, zPos);
                vertices.push_back(v);
            }
    }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        //indexCount = indices.size();
        mesh->indices_ = indices;
        mesh->vertices_ = vertices;

        return mesh;
    }


Scene::Scene() {
	//for test
	GameObject* go = new GameObject;
	Renderer* render = new Renderer;
    render->mesh_ = GenUVSphere();
    render->mat_ = new Material;
    //render->mat_->SetDefault();
    go->render = render;
}

void Scene::RenderScene() {
	for (std::vector<GameObject>::const_iterator iter = gameObjects.begin(); iter != gameObjects.end(); iter++) {
		if (iter->render) {
			iter->render->Render(camera);
		}
	}
}