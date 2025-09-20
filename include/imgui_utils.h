#pragma once

namespace slavetats_ui {
	void show_confirmation_popups(bool* confirmed, bool* success_shown, bool* error_shown);

	inline constexpr uint32_t ABGRtoARGB(uint32_t abgr) {
		uint32_t rgb = abgr & 0xff;
		rgb = (rgb << 8) | ((abgr >> 8) & 0xff);
		rgb = (rgb << 8) | ((abgr >> 16) & 0xff);
		return rgb | (abgr & 0xff000000);
	}

	inline constexpr uint32_t ARGBtoABGR(uint32_t argb) {
		return ABGRtoARGB(argb); // same algorithm
	}

}

