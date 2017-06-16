#include "CollisionLibrary.h"
#include <math.h>
#include <algorithm>

using namespace std;
// ComputePlane
//
// Calculate the plane normal and plane offset from the input points
void ComputePlane(Plane &plane, const vec3f& pointA, const vec3f& pointB, const vec3f &pointC)
{
	cross_product(plane.normal, pointB - pointA, pointC - pointB);
	plane.normal = plane.normal.normalize();
	plane.offset = dot_product(plane.normal.normalize(), pointA);
}

// ClassifyPointToPlane
//
// Perform a half-space test. Returns 1 if the point is on or in front of the plane.
// Returns 2 if the point is behind the plane.
int ClassifyPointToPlane(const Plane& plane, const vec3f& point)
{
	float distance = (dot_product(point, plane.normal) - plane.offset);
	if (distance > 0)
	{
		return 1;
	}
	else
	{
		return 2;
	}
}

// ClassifySphereToPlane
//
// Perform a sphere-to-plane test. 
// Returns 1 if the sphere is in front of the plane.
// Returns 2 if the sphere is behind the plane.
// Returns 3 if the sphere straddles the plane.
int ClassifySphereToPlane(const Plane& plane, const Sphere& sphere)
{
	float distance = (dot_product(sphere.m_Center, plane.normal) - plane.offset);
	if (distance > sphere.m_Radius)
	{
		return 1;
	}
	else if (distance < -sphere.m_Radius)
	{
		return 2;
	}

	return 3;
}

// ClassifyAabbToPlane
//
// Performs a AABB-to-plane test.
// Returns 1 if the aabb is in front of the plane.
// Returns 2 if the aabb is behind the plane.
// Returns 3 if the aabb straddles the plane.
int ClassifyAabbToPlane(const Plane& plane, const AABB& aabb)
{
	vec3f E;
	vec3f center;
	center = (aabb.min + aabb.max) * 0.5f;

	E = aabb.max - center;

	float distance = dot_product(center, plane.normal) - plane.offset;
	float radius = E.x * abs(plane.normal.x) + E.y * abs(plane.normal.y) + E.z * abs(plane.normal.z);

	if (distance > radius)
	{
		return 1;
	}
	else if (distance < -radius)
	{
		return 2;
	}

	return 3;
}

// ClassifyCapsuleToPlane
//
// Performs a Capsule-to-plane test.
// Returns 1 if the aabb is in front of the plane.
// Returns 2 if the aabb is behind the plane.
// Returns 3 if the aabb straddles the plane.
int ClassifyCapsuleToPlane(const Plane& plane, const Capsule& capsule)
{
	float distance = (dot_product(capsule.m_Segment.m_Start, plane.normal) - plane.offset);
	float distance_2 = (dot_product(capsule.m_Segment.m_End, plane.normal) - plane.offset);

	if (distance > capsule.m_Radius && distance_2 > capsule.m_Radius)
	{
		return 1;
	}
	else if (distance < -capsule.m_Radius && distance_2 < -capsule.m_Radius)
	{
		return 2;
	}

	return 3;
}

// BuildFrustum
//
// Calculates the corner points and planes of the frustum based upon input values.
// Should call ComputePlane.
void BuildFrustum( Frustum& frustum, float fov, float nearDist, float farDist, float ratio, const matrix4f& camXform )
{
	// TO DO:
	// Calculate the 8 corner points of the frustum and store them in the frustum.corners[] array.
	// Use the FrustumCorners enum in CollisionLibrary.h to index into the corners array.
	vec3f nc = camXform.axis_pos - camXform.axis_z * nearDist;
	vec3f fc = camXform.axis_pos - camXform.axis_z * farDist;

	float Hnear = 2 * tan(fov / 2) * nearDist;
	float Hfar = 2 * tan(fov / 2) *  farDist;
	float Wnear = Hnear * ratio;
	float Wfar = Hfar * ratio;

	vec3f up = { 0, 1, 0 };
	vec3f right = { 1, 0, 0 };

	frustum.corners[FTL] = fc + camXform.axis_y * (Hfar*0.5f) - camXform.axis_x * (Wfar*0.5f);
	frustum.corners[FTR] = fc + camXform.axis_y * (Hfar*0.5f) + camXform.axis_x * (Wfar*0.5f);
	frustum.corners[FBL] = fc - camXform.axis_y * (Hfar*0.5f) - camXform.axis_x * (Wfar*0.5f);
	frustum.corners[FBR] = fc - camXform.axis_y * (Hfar*0.5f) + camXform.axis_x * (Wfar*0.5f);

	frustum.corners[NTL] = nc + camXform.axis_y * (Hnear*0.5f) - camXform.axis_x * (Wnear*0.5f);
	frustum.corners[NTR] = nc + camXform.axis_y * (Hnear*0.5f) + camXform.axis_x * (Wnear*0.5f);
	frustum.corners[NBL] = nc - camXform.axis_y * (Hnear*0.5f) - camXform.axis_x * (Wnear*0.5f);
	frustum.corners[NBR] = nc - camXform.axis_y * (Hnear*0.5f) + camXform.axis_x * (Wnear*0.5f);

	// Use the corner points to calculate the frustum planes.
	// This step is completed for you.
	ComputePlane(frustum.planes[NEAR_PLANE], frustum.corners[NBR], frustum.corners[NBL], frustum.corners[NTL]);
	ComputePlane(frustum.planes[FAR_PLANE], frustum.corners[FBL], frustum.corners[FBR], frustum.corners[FTR]);
	ComputePlane(frustum.planes[LEFT_PLANE], frustum.corners[NBL], frustum.corners[FBL], frustum.corners[FTL]);
	ComputePlane(frustum.planes[RIGHT_PLANE], frustum.corners[FBR], frustum.corners[NBR], frustum.corners[NTR]);
	ComputePlane(frustum.planes[TOP_PLANE], frustum.corners[NTR], frustum.corners[NTL], frustum.corners[FTL]);
	ComputePlane(frustum.planes[BOTTOM_PLANE], frustum.corners[NBL], frustum.corners[NBR], frustum.corners[FBR]);
}

// FrustumToSphere
//
// Perform a Sphere-to-Frustum check. Returns true if the sphere is inside. False if not.
bool FrustumToSphere(const Frustum& frustum, const Sphere& sphere)
{
	for (int i = 0; i < 6; i++)
	{
		int retVal = ClassifySphereToPlane(frustum.planes[i], sphere);
		if (retVal == 2)
		{
			return false;
		}
	}

	return true;
}

// FrustumToAABB
//
// Perform a Aabb-to-Frustum check. Returns true if the aabb is inside. False if not.
bool FrustumToAABB(const Frustum& frustum, const AABB& aabb)
{
	for (int i = 0; i < 6; i++)
	{
		int retVal = ClassifyAabbToPlane(frustum.planes[i], aabb);
		if (retVal == 2)
		{
			return false;
		}
	}

	return true;
}

// FrustumToCapsule
//
// Perform a Capsule-to-Frustum check. Returns true if the Capsule is inside. False if not.
bool FrustumToCapsule(const Frustum& frustum, const Capsule& capsule)
{
	for (int i = 0; i < 6; i++)
	{
		int retVal = ClassifyCapsuleToPlane(frustum.planes[i], capsule);
		if (retVal == 2)
		{
			return false;
		}
	}

	return true;
}

// AABBtoAABB
//
// Returns true if the AABBs collide. False if not.
bool AABBtoAABB(const AABB& lhs, const AABB& rhs)
{
	if (lhs.max.x > rhs.min.x && lhs.min.x < rhs.max.x &&
		lhs.max.y > rhs.min.y && lhs.min.y < rhs.max.y &&
		lhs.max.z > rhs.min.z && lhs.min.z < rhs.max.z
		)
	{
		return true;
	}
	return false;
}

// SphereToSphere
//
// Returns true if the Spheres collide. False if not.
bool SphereToSphere(const Sphere& lhs, const Sphere& rhs)
{
	vec3f vec_d = (rhs.m_Center - lhs.m_Center);
	float distance = dot_product(vec_d, vec_d);
	float radius = (lhs.m_Radius + rhs.m_Radius);

	radius = radius * radius;
	if (distance <= radius)
	{
		return true;
	}
	return false;
}

// SphereToAABB
//
// Returns true if the sphere collides with the AABB. False if not.
bool SphereToAABB(const Sphere& lhs, const AABB& rhs)
{
	float radius = lhs.m_Radius * lhs.m_Radius;
	float x, y, z;
	x = max(rhs.min.x, min(lhs.m_Center.x, rhs.max.x));
	y = max(rhs.min.y, min(lhs.m_Center.y, rhs.max.y));
	z = max(rhs.min.z, min(lhs.m_Center.z, rhs.max.z));

	float distance = sqrt((x - lhs.m_Center.x) * (x - lhs.m_Center.x) +
		(y - lhs.m_Center.y) * (y - lhs.m_Center.y) +
		(z - lhs.m_Center.z) * (z - lhs.m_Center.z)
	);

	if (distance < lhs.m_Radius)
	{
		return true;
	}

	return false;
}

// CapsuleToSphere
//
// Returns true if the capsule collides with the sphere. False if not.
bool CapsuleToSphere(const Capsule& capsule, const Sphere& sphere)
{
	vec3f center = capsule.m_Segment.m_End - capsule.m_Segment.m_Start;
	float cap_d = dot_product(sphere.m_Center - capsule.m_Segment.m_Start, center) / dot_product(center, center);

	if (cap_d < 0.0f) { cap_d = 0.0f; }
	if (cap_d > 1.0f) { cap_d = 1.0f; }

	vec3f closest = capsule.m_Segment.m_Start + (center * cap_d);
	vec3f col = sphere.m_Center - closest;
	float dis = col.magnitude();

	if (dis <= (sphere.m_Radius + capsule.m_Radius))
	{
		return true;
	}

	return false;
}