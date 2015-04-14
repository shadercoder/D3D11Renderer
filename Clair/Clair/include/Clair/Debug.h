#pragma once
#include <string>
#include <sstream>
#include <functional>

namespace Clair {
	#ifdef NDEBUG
		#define CLAIR_ASSERT(condition, message)
	#else
		#define CLAIR_ASSERT(condition, message)\
		if(!(condition)) {\
			CLAIR_LOG("CLAIR ASSERTION FAILED:");\
			std::stringstream ss {};\
			ss << "- File: " << __FILE__;\
			CLAIR_LOG(ss.str());\
			ss = std::stringstream {};\
			ss << "- Function: " << __FUNCTION__;\
			CLAIR_LOG(ss.str());\
			ss = std::stringstream {};\
			ss << "- Line: " << __LINE__;\
			CLAIR_LOG(ss.str());\
			ss = std::stringstream {};\
			ss << "- Condition: " << #condition;\
			CLAIR_LOG(ss.str());\
			ss = std::stringstream {};\
			ss << "- Message: " << message;\
			CLAIR_LOG(ss.str());\
			abort();\
		}
	#endif

	typedef std::function<void(const std::string&)> LogCallback;

	class Log {
	public:
		static void log(const std::string& message) {
			msCallback(message);
		}

	private:
		friend class Renderer;
		static LogCallback msCallback;
	};


	#define CLAIR_LOG(msg)\
		Clair::Log::log(msg)
}