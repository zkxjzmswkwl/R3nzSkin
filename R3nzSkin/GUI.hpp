#pragma once

class GUI {
public:
	void render() noexcept;

	// why is this public?
	bool is_open{ false };
	// and this..?
	std::once_flag set_font_scale;

private:
	char str_buffer[256];
};
