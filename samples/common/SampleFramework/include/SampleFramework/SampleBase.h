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

		virtual void onResize(int width, int height, float aspect) = 0;
		virtual void update() = 0;
		virtual void render() = 0;

		float getDeltaTime() const;
		float getRunningTime() const;

	private:
		friend class Framework;
		float mDeltaTime;
		float mRunningTime;
	};

	inline float SampleBase::getDeltaTime() const {
		return mDeltaTime;
	}

	inline float SampleBase::getRunningTime() const {
		return mRunningTime;
	}
}