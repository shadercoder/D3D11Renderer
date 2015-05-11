#pragma once

struct HWND__;
typedef HWND__* HWND;

namespace SampleFramework {
	class SampleBase {
	public:
		SampleBase() = default;
		virtual ~SampleBase() = default;

		virtual bool initialize(HWND hwnd) = 0;
		virtual void terminate() = 0;

		virtual void onResize() = 0;
		virtual void update() = 0;
		virtual void render() = 0;

	protected:
		float getDeltaTime() const;
		float getRunningTime() const;
		int getWidth() const;
		int getHeight() const;
		float getAspect() const;

	private:
		friend class Framework;
		float mDeltaTime;
		float mRunningTime;
		int mWidth;
		int mHeight;
		float mAspect;
	};

	inline float SampleBase::getDeltaTime() const {
		return mDeltaTime;
	}

	inline float SampleBase::getRunningTime() const {
		return mRunningTime;
	}

	inline int SampleBase::getWidth() const {
		return mWidth;
	}

	inline int SampleBase::getHeight() const {
		return mHeight;
	}

	inline float SampleBase::getAspect() const {
		return mAspect;
	}
}