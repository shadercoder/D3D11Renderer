#pragma once

namespace SampleFramework {
	template<typename T, size_t numSamples>
	class SmoothValue {
	public:
		static_assert(numSamples > 1, "SmoothValue's numSamples must be > 1");
		SmoothValue() : mNumSamples {numSamples} {
		}

		/*explicit*/ SmoothValue(const T& value)
			: mValue {value}
			, mNumSamples {numSamples} {
		}

		SmoothValue& operator=(const T& b) {
			set(b);
			return *this;
		}

		void set(const T& b) {
			const T total = static_cast<T>(mNumSamples - 1) * mValue;
			mValue = (total + b) / static_cast<T>(mNumSamples);
		}

		T get() {
			return mValue;
		}

	private:
		T mValue {};
		size_t mNumSamples {0};
	};
}