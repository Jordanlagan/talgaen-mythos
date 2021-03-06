#include <algorithm>

#include "luadebugprompt.h"
#include "Renderer.h"
#include "font.h"
#include "Rect.h"
#include "LuaEngine.h"

namespace talga
{
	const int YOFFSET = 10;
	Rect TEXT_ENTER_BOX;

	LuaDebugPrompt::LuaDebugPrompt(cpTex tex, cpFont font)
		: Sprite(tex)
		, mFont(font)
		, mStartAt()
		, mMaxVisibleLines(0)
		, mMaxLinePixelWidth(0)
		, mCurrentCommand()
	{
		mMaxVisibleLines = ((box().getH() - LUA_DEBUG_BORDER) / font->getMaxGlyphHeight()) - 4;
		mMaxLinePixelWidth = (box().getW() - LUA_DEBUG_BORDER * 2);

		TEXT_ENTER_BOX = Rect{ 0, YOFFSET, (I32)box().getW(), font->getMaxGlyphHeight() * 2 };
	}

	void LuaDebugPrompt::pushCToCmd(char c)
	{
		mCurrentCommand.push_back(c);
	}

	void LuaDebugPrompt::popCmd()
	{
		if (mCurrentCommand.size() > 0)
			mCurrentCommand.pop_back();
	}

	void LuaDebugPrompt::doCmd()
	{
		LuaEngine::instance()->ExecuteStr(mCurrentCommand);
		mCurrentCommand.clear();
		return;
	}

	void LuaDebugPrompt::print(const std::string& line)
	{
		std::string proxyStr;
		std::string proxyWord;
		F32 wordWidth = 0.0f;
		F32 totalStrWidth = 0.0f;
		I32 previousWidth = 0;

		std::vector<std::string> tempVec;
		for (int i = 0; i < line.size(); ++i)
		{
			char c = line[i];

			if (c == '\n')
			{
				totalStrWidth += wordWidth;
				proxyStr.push_back('\n');
				tempVec.push_back(proxyStr);
				proxyWord.clear();
				proxyStr.clear();
				proxyStr.append(proxyWord);

				wordWidth = 0.0f;
				totalStrWidth = 0.0f;
				previousWidth = 0;;
				continue;
			}
			else if (c == '\t')
			{
				proxyStr.push_back('\t');
				totalStrWidth += mFont->getTabWidth();

				continue;
			}
			auto glyph = mFont->getGlyph(c);

			if (c == ' ')
			{
				I32 widthBeforeWord = totalStrWidth / mMaxLinePixelWidth;
				I32 widthAfterWord = (totalStrWidth + wordWidth + glyph.xadvance) / (F32)mMaxLinePixelWidth;
				
				
				if (widthAfterWord > widthAfterWord || widthBeforeWord > previousWidth)
				{
					totalStrWidth += wordWidth;
					proxyStr.push_back('\n');
					tempVec.push_back(proxyStr);
					proxyWord.clear();
					proxyStr.clear();
					proxyStr.append(proxyWord);

					wordWidth = 0.0f;
					totalStrWidth = 0.0f;
					previousWidth = 0;
				}
				else
				{
					proxyStr.append(proxyWord);
					totalStrWidth += wordWidth + glyph.xadvance;
					proxyWord.clear();
					proxyStr.push_back(' ');
					wordWidth = glyph.xadvance;
				}
				previousWidth = widthBeforeWord;
			}
			else
			{
				proxyWord.push_back(c);
				wordWidth += glyph.xadvance;
			}

		}

		tempVec.push_back(proxyStr + proxyWord + "\n");

		for (auto it = tempVec.rbegin(); it != tempVec.rend(); ++it)
			mTextLines.push_front(*it);

		

		while (mTextLines.size() > LUA_DEBUG_MAX_LINES)
			mTextLines.pop_back();

		proxyStr.push_back('\n');
	}

	void LuaDebugPrompt::render(Renderer* renderer, const Camera* camera) const
	{
		Sprite::render(renderer, camera);
		renderer->tStackPush(box().getTransformationMatrix());
		std::string rdrStr = ">" + mCurrentCommand + "\n";
		I32 numNewLines = 0;
		for (auto it = mTextLines.begin(); it != mTextLines.end() && numNewLines <= mMaxVisibleLines; ++it)
		{
			numNewLines += std::count(it->begin(), it->end(), '\n');
			rdrStr += *it;
		}
		vec3 tempPos(box().getW() * -0.5f + LUA_DEBUG_BORDER, (box().getH() * -0.5f + mFont->getMaxGlyphHeight() + LUA_DEBUG_BORDER * 2) + YOFFSET);

		renderer->submit(rdrStr, mFont, tempPos,vec4(1.0f, 1.0f, 1.0f, 1.0f));
		renderer->tStackPop();
	}

	bool LuaDebugPrompt::wasSelected(I32 mx, I32 my) const
	{
		int imgBoxTLX = (box().getX() - box().getW() * 0.5f);
		int imgBoxTLY = (box().getY() - box().getH() * 0.5f);
		if (mx >= TEXT_ENTER_BOX.x + imgBoxTLX)
		{
			if (mx <= TEXT_ENTER_BOX.x + TEXT_ENTER_BOX.w + imgBoxTLX)
			{
				if (my >= TEXT_ENTER_BOX.y + imgBoxTLY)
				{
					if (my <= TEXT_ENTER_BOX.y + TEXT_ENTER_BOX.h + imgBoxTLY)
					{
						return true;
					}
				}
			}
		}

		return false;
	}

	LuaDebugPrompt::~LuaDebugPrompt()
	{
	}
}