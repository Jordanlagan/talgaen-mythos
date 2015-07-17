#pragma once

#include <vector>
#include "Cmn.h"
#include "IDynamic.h"
#include "Math/Vector3.h"
#include "Math/Matrix4x4.h"
#include "AssetManager.h"
#include "Rectangle.h"

namespace talga
{
	struct Rect;
	class Game;
	class ARenderableObj;
	class Animation;
	class PhysSprite;
	class AssetManager;
	class LuaEngine;

	class Camera : public IDynamic
	{
	public:
		Camera(I32 width, I32 height, vec3 position = vec3(0.0f, 0.0f, 1.0f));
		Camera(const Camera& c);
		~Camera();
		static void LUA_REGISTER(LuaEngine* engine);
		virtual void update(F32 dt) override;

		const mat4& getCameraMat() const;

        void setW(I32 w);
        void setH(I32 h);

		Rectangle& getBox() { return *mBox; }
		const Rectangle& getBox() const { return *mBox; }
		vec3 screenToWorld(const vec3& pos);

		int getij() const { return ij; }

		void printC();

		I32 getPoop() const { return mPoop; }
		void setPoop(I32 poop) { mPoop = poop; }
	protected:
		I32 mPoop = 0;
		Rectangle* mBox;
		int ij;

		mat4 mCameraMat;
		mat4 mCameraScaleMat;
		mat4 mCameraRotMat;
		mat4 mCameraTransMat;

		void updateCameraScaleMat();
		void updateCameraRotMat();
		void updateCameraTransMat();
		void updateCameraMat();
	};
}
