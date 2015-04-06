#pragma once

namespace Clair {
	typedef unsigned RenderPass;
}

#define CLAIR_RENDER_PASS(a) static_cast<Clair::RenderPass>(a)
#define CLAIR_RENDER_PASS_ENUM(a) enum class a : Clair::RenderPass