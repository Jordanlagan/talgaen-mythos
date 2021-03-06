#include "AnimSet.h"
#include <iostream>
#include "Rect.h"
#include "Texture.h"
#include "Math/Operations.h"

namespace talga
{
	AnimationSet::AnimationSet(cpTex tex, const std::string& name)
		: mTex(tex)
		, mAnims({AnimSetPair("default", UVAnimation{ WHOLE_TEXTURE }) })
		, mWidth(-1)
		, mHeight(-1)
	{
		mName = name;
	}

	const UVAnimation* AnimationSet::getAnim(const std::string& name) const
	{
		for (auto iter = mAnims.begin(); iter != mAnims.end(); ++iter)
			if ((*iter).first == name)
				return &(*iter).second;

		std::cout << "Could not find animation: " << name << std::endl;
		return &mAnims.back().second;
	}

	/* Takes a list of Rectangles using pixel coordinates and converts them
	to UV cooridnates and stores them in the Animation set*/

	void AnimationSet::addAnim(std::string name, const RectList& frames)
	{
		UVAnimation anim;
		for (const auto& rectFrame : frames)
		{
			if (mTex)
			{
				UVFrame frame;

				frame[0] = GET_UV(rectFrame.x, mTex->w(), rectFrame.y, mTex->h());
				frame[1] = GET_UV(rectFrame.x + rectFrame.w, mTex->w(), rectFrame.y, mTex->h());
				frame[2] = GET_UV(rectFrame.x + rectFrame.w, mTex->w(), rectFrame.y + rectFrame.h, mTex->h());
				frame[3] = GET_UV(rectFrame.x, mTex->w(), rectFrame.y + rectFrame.h, mTex->h());

				anim.push_back(frame);
			}
			else
			{
				anim.push_back(WHOLE_TEXTURE);
			}

			mWidth = rectFrame.w;
			mHeight = rectFrame.h;
		}

		mAnims.push_back(AnimSetPair(name, anim));
	}

	AnimationSet::~AnimationSet()
	{

	}
}