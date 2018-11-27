#pragma once
#define M_PI		3.14159265358979323846f
#define M_RADPI		57.295779513082f
#define M_PI_F		((float)(M_PI))	// Shouldn't collide with anything.
#define RAD2DEG( x  )  ( (float)(x) * (float)(180.f / M_PI_F) )
#define DEG2RAD( x  )  ( (float)(x) * (float)(M_PI_F / 180.f) )
#define square( x ) ( x * x )

#include "Vars.h"

typedef float(*RandomFloat_t)(float, float);
typedef void(*RandomSeed_t)(UINT);

namespace Math
{
	inline float sseSqrt(float x)
	{
		//return sqrtf(x);

		float root = 0.0f;

		__asm
		{
			sqrtss xmm0, x
			movss root, xmm0
		}

		return root;
	}

	inline void SinCos(float radians, float *sine, float *cosine)
	{
		*sine = sin(radians);
		*cosine = cos(radians);
	}

	inline void VectorTransform(const Vector in1, matrix3x4_t in2, Vector &out)
	{
		out[0] = in1.Dot(in2[0]) + in2[0][3];
		out[1] = in1.Dot(in2[1]) + in2[1][3];
		out[2] = in1.Dot(in2[2]) + in2[2][3];
	}

	inline void VectorAngles(Vector forward, QAngle& angles)
	{
		//Assert(s_bMathlibInitialized);
		float yaw, pitch;

		if (forward[1] == 0 && forward[0] == 0)
		{
			yaw = 0;
			if (forward[2] > 0)
				pitch = 270;
			else
				pitch = 90;
		}
		else
		{
			yaw = (atan2(forward[1], forward[0]) * 180 / M_PI);
			if (yaw < 0)
				yaw += 360;

			float tmp = sseSqrt(forward[0] * forward[0] + forward[1] * forward[1]);
			pitch = (atan2(-forward[2], tmp) * 180 / M_PI);
			if (pitch < 0)
				pitch += 360;
		}

		angles[0] = pitch;
		angles[1] = yaw;
		angles[2] = 0;
	}

	inline double avg(std::vector<float> const& v) {
		int n = 0;
		double mean = 0.0;
		for (auto x : v) {
			double delta = x - mean;
			mean += delta / ++n;
		}
		return mean;
	}

	inline void AngleVectors(const QAngle &angles, Vector *forward)
	{
		float sp, sy, cp, cy;

		SinCos(DEG2RAD(angles.y), &sy, &cy);
		SinCos(DEG2RAD(angles.x), &sp, &cp);

		forward->x = cp*cy;
		forward->y = cp*sy;
		forward->z = -sp;
	}

	inline float RandomFloat(float fMin, float fMax) {

		static RandomFloat_t m_RandomFloat;

		if (!m_RandomFloat)
			m_RandomFloat = (RandomFloat_t)GetProcAddress(GetModuleHandle(L"vstdlib.dll"), "RandomFloat");

		return m_RandomFloat(fMin, fMax);
	}

	inline void RandomSeed(UINT Seed) {

		static RandomSeed_t m_RandomSeed;

		if (!m_RandomSeed)
			m_RandomSeed = (RandomSeed_t)GetProcAddress(GetModuleHandle(L"vstdlib.dll"), "RandomSeed");

		m_RandomSeed(Seed);
	}

	inline void AngleVectors(const QAngle &angles, Vector& forward, Vector& right, Vector& up)
	{
		float sr, sp, sy, cr, cp, cy;

		SinCos(DEG2RAD(angles[1]), &sy, &cy);
		SinCos(DEG2RAD(angles[0]), &sp, &cp);
		SinCos(DEG2RAD(angles[2]), &sr, &cr);

		forward.x = (cp * cy);
		forward.y = (cp * sy);
		forward.z = (-sp);
		right.x = (-1 * sr * sp * cy + -1 * cr * -sy);
		right.y = (-1 * sr * sp * sy + -1 * cr *  cy);
		right.z = (-1 * sr * cp);
		up.x = (cr * sp * cy + -sr * -sy);
		up.y = (cr * sp * sy + -sr * cy);
		up.z = (cr * cp);
	}

	inline vec_t VectorNormalize(Vector& v)
	{
		vec_t l = v.Length();

		if (l != 0.0f)
		{
			v /= l;
		}
		else
		{
			v.x = v.y = 0.0f; v.z = 1.0f;
		}

		return l;
	}

	inline float GetFov(const QAngle& viewAngle, const QAngle& aimAngle)
	{
		Vector ang, aim;

		AngleVectors(viewAngle, &aim);
		AngleVectors(aimAngle, &ang);

		return RAD2DEG(acos(aim.Dot(ang) / aim.LengthSqr()));
	}

	inline void GUWOPNormalize(QAngle &flAngle)
	{

		float flRevolutionsY = flAngle.y / 360;
		float flRevolutionsX = flAngle.x / 180;

		if (flAngle.y > 180 || flAngle.y < -180)
		{
			if (flRevolutionsY < 0)
			{
				flRevolutionsY = -flRevolutionsY;
			}

			flRevolutionsY = round(flRevolutionsY);

			if (flAngle.y < 0)
			{
				flAngle.y = (flAngle.y + 360 * flRevolutionsY);
			}
			else
			{
				flAngle.y = (flAngle.y - 360 * flRevolutionsY);
			}
		}

		if (flAngle.x > 89 || flAngle.x < -89)
		{
			if (flRevolutionsX < 0)
			{
				flRevolutionsX = -flRevolutionsX;
			}

			flRevolutionsX = round(flRevolutionsX);

			if (flAngle.x < 0)
			{
				flAngle.x = (flAngle.x + 180 * flRevolutionsX);
			}
			else
			{
				flAngle.x = (flAngle.x - 180 * flRevolutionsX);
			}
		}

		flAngle.z = 0;

	}

	inline void GUWOPNormalizeYaw(float &flAngle)
	{

		float flRevolutionsY = flAngle / 360;

		if (flAngle > 180 || flAngle < -180)
		{
			if (flRevolutionsY < 0)
			{
				flRevolutionsY = -flRevolutionsY;
			}

			flRevolutionsY = round(flRevolutionsY);

			if (flAngle < 0)
			{
				flAngle = (flAngle + 360 * flRevolutionsY);
			}
			else
			{
				flAngle = (flAngle - 360 * flRevolutionsY);
			}
		}

	}

	inline void GUWOPNormalizeYaw360(float &flAngle)
	{

		float flRevolutionsY = flAngle / 360;

		if (flAngle > 360 || flAngle < 0)
		{
			if (flRevolutionsY < 0)
			{
				flRevolutionsY = -flRevolutionsY;
			}

			flRevolutionsY = round(flRevolutionsY);

			if (flAngle < 0)
			{
				flAngle = (flAngle + 360 * flRevolutionsY);
			}
			else
			{
				flAngle = (flAngle - 360 * flRevolutionsY);
			}
		}

	}

	inline bool ClampMovement(CUserCmd* cmd)
	{
		if (cmd->sidemove > 450.f) cmd->sidemove = 450.f;
		if (cmd->sidemove < -450.f) cmd->sidemove = -450.f;
		if (cmd->forwardmove > 450.f) cmd->forwardmove = 450.f;
		if (cmd->forwardmove < -450.f) cmd->forwardmove = -450.f;

		return true;
	}

	template< class T, class Y >
	T Clamp2(T const &val, Y const &minVal, Y const &maxVal)
	{
		if (val < minVal)
			return minVal;
		if (val > maxVal)
			return maxVal;
		return val;
	}

	inline void ClampAngles(QAngle &angles)
	{
		if (angles.x > 89.0) angles.x = 89;
		if (angles.x < -89.0) angles.x = -89;
		if (angles.y < -180) angles.y = -179.999;
		if (angles.y > 180) angles.y = 179.999;

		angles.z = 0;
	}

	inline bool ClampUserCmd(CUserCmd* cmd)
	{
		ClampMovement(cmd);
		ClampAngles(cmd->viewangles);
		
		return true;
	}

	inline bool Clamp(QAngle &angles)
	{
		GUWOPNormalize(angles);
		ClampAngles(angles);

		if (angles.x > 89.f)
			angles.x = 89.f;
		if (angles.x < -89.f)
			angles.x = -89.f;
		if (angles.y > 180.f)
			angles.y = 179.5f;
		if (angles.y < -180.f)
			angles.y = -179.5f;
		if (angles.z != 0.f)
			angles.z = 0.f;

		return true;
	}

	inline QAngle CalcAngle(const Vector& src, const Vector& dst)
	{
		Vector delta = dst - src;
		QAngle angles;
		VectorAngles(delta, angles);

		return angles;
	}

	inline QAngle SmoothAngleBezierCurve(QAngle &From, QAngle &To, float PercentX, float PercentY)
	{
		Vector out;
		Vector Delta = (To - From);

		Clamp(Delta);

		float CubicStepX = (1 - cos(PercentX * M_PI)) / 2;
		float CubicStepY = (1 - cos(PercentY * M_PI)) / 2;

		out.x = (From.x + Delta.x * CubicStepX);
		out.y = (From.y + Delta.y * CubicStepY);

		Clamp(out);
		return out;
	}

	inline QAngle SmoothAngle(QAngle &From, QAngle &To, float Percent)
	{
		QAngle VecDelta = From - To;

		Clamp(VecDelta);

		VecDelta.x *= Percent;
		VecDelta.y *= Percent;

		QAngle temp = From - VecDelta;

		Clamp(temp);
		return temp;
	}

	inline bool ScreenTransform(const Vector& point, Vector& screen)
	{
		const VMatrix& w2sMatrix = ENGINE->WorldToScreenMatrix();
		screen.x = w2sMatrix.m[0][0] * point.x + w2sMatrix.m[0][1] * point.y + w2sMatrix.m[0][2] * point.z + w2sMatrix.m[0][3];
		screen.y = w2sMatrix.m[1][0] * point.x + w2sMatrix.m[1][1] * point.y + w2sMatrix.m[1][2] * point.z + w2sMatrix.m[1][3];
		screen.z = 0.0f;

		float w = w2sMatrix.m[3][0] * point.x + w2sMatrix.m[3][1] * point.y + w2sMatrix.m[3][2] * point.z + w2sMatrix.m[3][3];

		if (w < 0.001f) {
			screen.x *= 100000;
			screen.y *= 100000;
			return true;
		}

		float invw = 1.0f / w;
		screen.x *= invw;
		screen.y *= invw;

		return false;
	}

	inline bool WorldToScreen(const Vector &origin, Vector &screen)
	{
		if (!ScreenTransform(origin, screen))
		{
			int iScreenWidth, iScreenHeight;
			ENGINE->GetScreenSize(iScreenWidth, iScreenHeight);

			screen.x = (iScreenWidth / 2.0f) + (screen.x * iScreenWidth) / 2;
			screen.y = (iScreenHeight / 2.0f) - (screen.y * iScreenHeight) / 2;

			return true;
		}
		return false;
	}

	inline float Get3dDistance(Vector myCoords, Vector enemyCoords)
	{
		return sseSqrt(
			pow(double(enemyCoords.x - myCoords.x), 2.0) +
			pow(double(enemyCoords.y - myCoords.y), 2.0) +
			pow(double(enemyCoords.z - myCoords.z), 2.0));
	}

	inline Vector VectorSubtract(Vector& pOut, Vector& pV1, Vector& pV2)
	{
		pOut = pV1 - pV2;

		return pOut;
	}

	inline Vector VectorAdd(Vector& pOut, Vector& pV1, Vector& pV2)
	{
		pOut = pV1 + pV2;

		return pOut;
	}

	template< class T >
	T clamp(T const &val, T const &minVal, T const &maxVal)
	{
		if (maxVal < minVal)
			return maxVal;
		if (val < minVal)
			return minVal;
		if (val > maxVal)
			return maxVal;
		return val;
	}

	inline void AngleNormalize(float& angle)
	{
		while (angle > 180.f)
			angle -= 360.f;
		while (angle < -180.f)
			angle += 360.f;
	}

	inline float AngleNormalize(float angle)
	{
		while (angle > 180.f)
			angle -= 360.f;
		while (angle < -180.f)
			angle += 360.f;
		return angle;
	}

	inline void AngleNormalize(Vector& angles)
	{
		for (int i = 0; i < 3; i++)
		{
			while (angles[i] > 180.f)
				angles[i] -= 360.f;
			while (angles[i] < -180.f)
				angles[i] += 360.f;
		}

		if (angles[1] == -180.f) angles[1] = 180.f;
		angles.z = 0.f;
	}

	inline void AngleIMatrix(const RadianEuler& angles, matrix3x4_t& matrix)
	{
		QAngle quakeEuler(RAD2DEG(angles.y), RAD2DEG(angles.z), RAD2DEG(angles.x));

		AngleIMatrix(quakeEuler, matrix);
	}

	inline void AngleMatrix(const Vector angles, matrix3x4_t& matrix)
	{
		float sr, sp, sy, cr, cp, cy;

		sy = sin(DEG2RAD(angles[1]));
		cy = cos(DEG2RAD(angles[1]));

		sp = sin(DEG2RAD(angles[0]));
		cp = cos(DEG2RAD(angles[0]));

		sr = sin(DEG2RAD(angles[2]));
		cr = cos(DEG2RAD(angles[2]));

		//matrix = (YAW * PITCH) * ROLL
		matrix[0][0] = cp * cy;
		matrix[1][0] = cp * sy;
		matrix[2][0] = -sp;

		float crcy = cr * cy;
		float crsy = cr * sy;
		float srcy = sr * cy;
		float srsy = sr * sy;

		matrix[0][1] = sp * srcy - crsy;
		matrix[1][1] = sp * srsy + crcy;
		matrix[2][1] = sr * cp;

		matrix[0][2] = (sp * crcy + srsy);
		matrix[1][2] = (sp * crsy - srcy);
		matrix[2][2] = cr * cp;

		matrix[0][3] = 0.0f;
		matrix[1][3] = 0.0f;
		matrix[2][3] = 0.0f;
	}

	inline void MatrixSetColumn(const Vector &in, int column, matrix3x4_t& out)
	{
		out[0][column] = in.x;
		out[1][column] = in.y;
		out[2][column] = in.z;
	}

	inline void AngleMatrix(const QAngle &angles, const Vector &position, matrix3x4_t& matrix_out)
	{
		AngleMatrix(angles, matrix_out);
		MatrixSetColumn(position, 3, matrix_out);
	}

	inline void MatrixCopy(const matrix3x4_t& source, matrix3x4_t& target)
	{
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++) {
				target[i][j] = source[i][j];
			}
		}
	}

	inline void MatrixMultiply(matrix3x4_t& in1, const matrix3x4_t& in2)
	{
		matrix3x4_t out;
		if (&in1 == &out)
		{
			matrix3x4_t in1b;
			MatrixCopy(in1, in1b);
			MatrixMultiply(in1b, in2);
			return;
		}
		if (&in2 == &out)
		{
			matrix3x4_t in2b;
			MatrixCopy(in2, in2b);
			MatrixMultiply(in1, in2b);
			return;
		}

		out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
			in1[0][2] * in2[2][0];
		out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
			in1[0][2] * in2[2][1];
		out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
			in1[0][2] * in2[2][2];
		out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] +
			in1[0][2] * in2[2][3] + in1[0][3];
		out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
			in1[1][2] * in2[2][0];
		out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
			in1[1][2] * in2[2][1];
		out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
			in1[1][2] * in2[2][2];
		out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] +
			in1[1][2] * in2[2][3] + in1[1][3];
		out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
			in1[2][2] * in2[2][0];
		out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
			in1[2][2] * in2[2][1];
		out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
			in1[2][2] * in2[2][2];
		out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] +
			in1[2][2] * in2[2][3] + in1[2][3];

		in1 = out;
	}

	inline vec_t DotProduct(const float * a, const float * b)
	{
		return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
	}

	inline void VectorRotate(const float *in1, const matrix3x4_t& in2, float *out)
	{
		out[0] = DotProduct(in1, in2[0]);
		out[1] = DotProduct(in1, in2[1]);
		out[2] = DotProduct(in1, in2[2]);
	}

	inline void VectorRotate(const Vector& in1, const matrix3x4_t &in2, Vector &out)
	{
		VectorRotate(&in1.x, in2, &out.x);
	}

	inline void VectorRotate(const Vector &in1, const QAngle &in2, Vector &out)
	{
		matrix3x4_t matRotate;
		AngleMatrix(in2, matRotate);
		VectorRotate(in1, matRotate, out);
	}

	inline float DotProductT(const float* a, const float* b)
	{
		return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
	}

	inline void VectorTransformA(const float *in1, const matrix3x4_t& in2, float *out)
	{

		out[0] = DotProductT(in1, in2[0]) + in2[0][3];
		out[1] = DotProductT(in1, in2[1]) + in2[1][3];
		out[2] = DotProductT(in1, in2[2]) + in2[2][3];
	}

	inline void VectorTransformZ(const Vector& in1, const matrix3x4_t &in2, Vector &out)
	{
		VectorTransformA(&in1.x, in2, &out.x);
	}

	void MatrixAngles(const matrix3x4_t & matrix, float *angles);

	inline void MatrixAngles(const matrix3x4_t &matrix, QAngle &angles)
	{
		MatrixAngles(matrix, &angles.x);
	}

	inline void MatrixAngles(const matrix3x4_t &matrix, QAngle &angles, Vector &position)
	{
		MatrixAngles(matrix, angles);
		MatrixPosition(matrix, position);
	}

	inline void MatrixAngles(const matrix3x4_t &matrix, RadianEuler &angles)
	{
		MatrixAngles(matrix, &angles.x);

		angles.Init(DEG2RAD(angles.z), DEG2RAD(angles.x), DEG2RAD(angles.y));
	}

	inline void MatrixAngles(const matrix3x4_t& matrix, float *angles)
	{
		float forward[3];
		float left[3];
		float up[3];

		//
		// Extract the basis vectors from the matrix. Since we only need the Z
		// component of the up vector, we don't get X and Y.
		//
		forward[0] = matrix[0][0];
		forward[1] = matrix[1][0];
		forward[2] = matrix[2][0];
		left[0] = matrix[0][1];
		left[1] = matrix[1][1];
		left[2] = matrix[2][1];
		up[2] = matrix[2][2];

		float xyDist = sseSqrt(forward[0] * forward[0] + forward[1] * forward[1]);

		// enough here to get angles?
		if (xyDist > 0.001f)
		{
			// (yaw)	y = ATAN( forward.y, forward.x );		-- in our space, forward is the X axis
			angles[1] = RAD2DEG(atan2f(forward[1], forward[0]));

			// (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
			angles[0] = RAD2DEG(atan2f(-forward[2], xyDist));

			// (roll)	z = ATAN( left.z, up.z );
			angles[2] = RAD2DEG(atan2f(left[2], up[2]));
		}
		else	// forward is mostly Z, gimbal lock-
		{
			// (yaw)	y = ATAN( -left.x, left.y );			-- forward is mostly z, so use right for yaw
			angles[1] = RAD2DEG(atan2f(-left[0], left[1]));

			// (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
			angles[0] = RAD2DEG(atan2f(-forward[2], xyDist));

			// Assume no roll in this case as one degree of freedom has been lost (i.e. yaw == roll)
			angles[2] = 0;
		}
	}

	inline void NormalizeVector(Vector& vec)
	{
		for (auto i = 0; i < 3; i++) {
			while (vec[i] < -180.0f) vec[i] += 360.0f;
			while (vec[i] >  180.0f) vec[i] -= 360.0f;
		}
		vec[2] = 0.f;
	}
}
