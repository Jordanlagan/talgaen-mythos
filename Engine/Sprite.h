#pragma once

#include "Rectangle.h"
#include "Math/Vector2.h"
#include "IRenderable.h"
#include "IDynamic.h"
#include "AnimSet.h"
#include "Cmn.h"

namespace talga
{
	const UVFrame DEFAULT_SPRITE_UV = { { vec2(0, 1), vec2(1, 1), vec2(1, 0), vec2(0, 0) } };
	class Sprite : public IRenderable, public IDynamic
	{
	public:
		Sprite(cpTex tex, UVFrame quords = DEFAULT_SPRITE_UV, F32 transScale = 1.0f);
		virtual ~Sprite() { TALGA_MSG("Sprite destructor called"); }

		virtual void render(Renderer* renderer, const Camera* camera) const override;
		virtual void update(F32 dt) override;

		cpTex getTexure() const { return mTex; }   
	protected:
		cpTex mTex;
		UVFrame mUVCurrentFrame;
		F32 mTransparencyScale;
	};

	class AnimSprite : public IRenderable, public IDynamic
	{
	public:
		AnimSprite(cpAnimSet anims, I32 width = -1, I32 height = -1);
		virtual ~AnimSprite() { TALGA_MSG("AnimSprite destructor called"); };

		virtual void render(Renderer* renderer, const Camera* camera) const override;
		virtual void update(F32 dt) override;

		/*speed in ms*/
		void playAnimation(const std::string& animName, I32 speed, bool loop);
		void playDefault();

	protected:
		void setUVFrame();
		UVFrame mUVCurrentFrame;

		// Animation specific members

		cpAnimSet mAnims;
		const UVAnimation* mCurrentAnimation;
		bool isAnimated;
		bool isLoop;
		F32 mTimeSince;
		F32 mFrameSpeed;
		I32 mCurrentFrame;
	};
}