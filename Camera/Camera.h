#ifndef CAMERA_H_
#define CAMERA_H_

#define _camera Camera::GetInstance()

class Camera
{
public:
	Camera();
	virtual ~Camera();


	static Camera* GetInstance();			/* Returns the instance of the camera, should be only 1 */

	void positionCamera();

	void ProcessCoordinates(short dot1x, short dot1y, short dot2x, short dot2y);

	void SetCameraLocation(float X, float Y, float Z, float newCenterX, float newCenterY, float newCenterZ);

	void ShakeCamera(float intensity);

	void GetPosition(float* buf);

	void FreezeCamera();
	void UnFreezeCamera();
	void Rotate(float speed);
	void StopRotate();
	bool IsRotating() { return m_rotate; };

	void NextFrame();

	void ModifyRotationRadius(float amount);

private:

        void CalcRotation();

	float eyeX, eyeY, eyeZ;
	float centerX, centerY, centerZ;
	float upX, upY, upZ;
	float confCenterX, confCenterY, confCenterZ;
	float shakeCounter;
	bool m_cameraFreeze;
	float m_rotateSpeed;
	float m_rotateAngle;
	bool m_rotate;
	float m_radius;
	bool m_hasRotated;


};

#endif /*CAMERA_H_*/
