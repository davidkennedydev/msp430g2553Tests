#pragma once

namespace carInMaze {
	struct PinPort {
		const uint8_t PIN;
		volatile uint8_t & PORT;
	};
}
