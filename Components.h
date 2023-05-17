#pragma once

#include <cmath>
#include <vector>
#include <gl/freeglut.h>
#include "Geometry.h"
#include "Graphics.h"
#include "Loader.h"
#include "LidarViewer.h"

/**
* Класс, отвечающий за камеру
*/
class Camera
{
private:
	Vector3 position, target, axis;
	float perspectiveFov, perspectiveRatio;
	float orthoHalfWidth, orthoHalfHeight;
	float clipNear, clipFar;
	bool isOrtho;

public:
	void SetupPerspective(float FOV, float ScreenRatio, float NearClip, float FarClip)
	{
		perspectiveFov = FOV;
		perspectiveRatio = ScreenRatio;
		clipNear = NearClip;
		clipFar = FarClip;
	}
	void SetupOrtho(float planeWidth, float planeHeight, float NearClip, float FarClip)
	{
		orthoHalfHeight = planeHeight / 2.0f;
		orthoHalfWidth = planeWidth / 2.0f;
		clipNear = NearClip;
		clipFar = FarClip;
	}
	void SetupDefault(void)
	{
		SetupPerspective(60, 1.777f, 0.1f, 50.0f), SetupOrtho(12, 6.75f, 0.1f, 50.0f);
		isOrtho = false;
	}

	Camera()
	{
		position = Vector3();
		target = Vector3(1, 0, 0);
		axis = Vector3(0, 1, 0);
		SetupDefault();
	}
	Camera(Vector3 CameraPosition)
	{
		position = CameraPosition;
		target = Vector3(1, 0, 0);
		axis = Vector3(0, 1, 0);
		SetupDefault();
	}
	Camera(Vector3 CameraPosition, Vector3 TargetPosition)
	{
		position = CameraPosition;
		target = TargetPosition;
		axis = Vector3(0, 1, 0);
		SetupDefault();
	}
	Camera(Vector3 CameraPosition, Vector3 TargetPosition, Vector3 AxisDirection)
	{
		position = CameraPosition;
		target = TargetPosition;
		axis = AxisDirection;
		SetupDefault();
	}

	void UpdatePosition(void) const
	{
		gluLookAt
		(
			position.x, position.y, position.z,
			target.x, target.y, target.z,
			axis.x, axis.y, axis.z
		);
	}
	float GetFarClip(void) const
	{
		return clipFar;
	}
	Vector3 GetAxis(void) const
	{
		return axis;
	}
	Vector3 GetCameraPosition(void) const
	{
		return position;
	}
	Vector3 GetTargetPosition(void) const
	{
		return target;
	}

	Vector3 Normal(void) const
	{
		return (Vector3(target) - Vector3(position)).Normal();
	}

	void SetAxis(Vector3 newAxis)
	{
		axis = newAxis;
	}

	void SetCameraPosition(Vector3 newPosition)
	{
		position = newPosition;
	}

	void SetTargetPosition(Vector3 newPosition)
	{
		target = newPosition;
	}

	void SetClipDistance(float NearClip, float FarClip)
	{
		clipNear = NearClip;
		clipFar = FarClip;
	}

	void SetPerspective(void)
	{
		gluPerspective(perspectiveFov, perspectiveRatio, clipNear, clipFar);
		isOrtho = false;
	}

	void SetOrtho(void)
	{
		glOrtho(-orthoHalfWidth, orthoHalfWidth, -orthoHalfHeight, orthoHalfHeight, clipNear, clipFar);
		isOrtho = true;
	}

	void SetAvailable(void)
	{
		if (isOrtho)
		{
			SetOrtho();
		}
		else
		{
			SetPerspective();
		}
	}
};

/**
* Класс, отвечающий за рисование
*/
class Renderer
{
public:
	Camera camera;

private:
	float clamp(float val, const float& min, const float& max) const
	{
		return val < min ? min : val > max ? max : val;
	}
	float diffusePoint(float angle, float roughness) const
	{
		roughness = 1 - roughness;
		angle = (roughness * fabsf(angle) - roughness);
		return 1 - angle * angle;
	}
	float realisticPoint(float angle, float distance, float roughness) const
	{
		return fabsf(angle) * diffusePoint(angle, roughness) * (2 * roughness - clamp(distance / camera.GetFarClip(), 0, 1));
	}

public:
	Renderer(Camera cameraToUse)
	{
		camera = cameraToUse;
	}

	static void SendVertex(const Vertex3& vertex)
	{
		glColor3ub(vertex.color.r, vertex.color.g, vertex.color.b);
		glVertex3f(vertex.position.x, vertex.position.y, vertex.position.z);
	}
	static void SendVertex(const Vertex3& vertex, const Color& color)
	{
		glColor3ub(color.r, color.g, color.b);
		glVertex3f(vertex.position.x, vertex.position.y, vertex.position.z);
	}

	void init(void)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		camera.SetAvailable();
		glMatrixMode(GL_MODELVIEW);
		glPointSize(5);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE); //glDisable(GL_CULL_FACE);
	}

	void RenderPoint(float x, float y, float z, const Color& color)
	{
		glBegin(GL_POINTS);
		glColor3ub(color.r, color.g, color.b);
		glVertex3f(x, y, z);
		glEnd();
	}

	void RenderPicture(std::vector<Vector3> v, long z_max, long z_min, const Color& color)
	{
		for (auto vec : v)
		{
			//RenderPoint(vec.x, vec.y, vec.z, Color(255, 255, 255));
			Vector3 xyz_color = GradientColorY(vec.y, z_max, z_min);
			RenderPoint(vec.x, vec.y, vec.z, Color(xyz_color.x, xyz_color.y, xyz_color.z));
		}
	}

	void RenderPoints(const std::vector<Vector3>& points, const Color& color)
	{
		size_t pSize = points.size();
		glBegin(GL_POINTS);
		glColor3ub(color.r, color.g, color.b);
		while (pSize--)
		{
			glVertex3f(points[pSize].x, points[pSize].y, points[pSize].z);
		}
		glEnd();
	}

	void RenderVector(const Vector3 vector, const Color& color)
	{
		glBegin(GL_LINES);
		glColor3ub(color.r, color.g, color.b);
		glVertex3f(0, 0, 0);
		glVertex3f(vector.x, vector.y, vector.z);
		glEnd();
	}

	void RenderVector(const Vector3& vector, const Vector3& startPoint, const Color& color)
	{
		glBegin(GL_LINES);
		glColor3ub(color.r, color.g, color.b);
		glVertex3f(startPoint.x, startPoint.y, startPoint.z);
		glVertex3f(startPoint.x + vector.x, startPoint.y + vector.y, startPoint.z + vector.z);
		glEnd();
	}

	void RenderCootrdinates()
	{
		glBegin(GL_LINES);
		glColor3ub(255, 0, 0);
		glVertex3f(-200, 0, 0);
		glVertex3f(200, 0, 0);

		glColor3ub(0, 255, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 200, 0);

		glColor3ub(0, 0, 255);
		glVertex3f(0, 0, -200);
		glVertex3f(0, 0, 200);
		glEnd();
	}

	void RenderGrid(float startX, float endX, unsigned amountX, float startZ, float endZ, unsigned amountZ, float height, const Color& color)
	{
		glBegin(GL_LINES);
		glColor3ub(color.r, color.g, color.b);

		float dx = (endX - startX) / amountX, cx = startX;
		unsigned x = 0;

		for (x; x < amountX; x++)
		{
			if ((cx - dx) != 0)
			{
				glVertex3f(cx - dx, height, startZ - dx);
				glVertex3f(cx - dx, height, endZ - dx);
				cx += dx;
			}
			else
				cx += dx;
		}

		float dz = (endZ - startZ) / amountZ, cz = startZ;
		unsigned z = 0;


		for (z; z < amountZ; z++)
		{
			if ((cz - dz) != 0)
			{
				glVertex3f(startX - dz, height, cz - dz);
				glVertex3f(endX - dz, height, cz - dz);
			}
			cz += dz;
		}
		glEnd();
	}

	void BeginFrame(void)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		camera.UpdatePosition();
	}
	void EndFrame(void)
	{
		glFlush();
	}
};
