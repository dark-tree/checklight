#pragma once

#include <optional>

#include "gui/widget.hpp"
#include "render/draw/text.hpp"
#include "shared/unicode.hpp"

class TextCursor {

	private:

		int master = 0;
		int slave = 0;

		void eraseSelection(utf8::UnicodeVector& text);
		void clampSelection(const utf8::UnicodeVector& text);

	public:

		uint32_t hasSelection() const;
		void insert(utf8::UnicodeVector& text, uint32_t unicode);
		void seek(const utf8::UnicodeVector& text, int position, bool drag);
		void move(const utf8::UnicodeVector& text, int offset, bool select);
		void eraseForward(utf8::UnicodeVector& text);
		void eraseBackward(utf8::UnicodeVector& text);

		int begin() const;
		int end() const;

};

class InputWidget : public Widget {

	protected:

		static constexpr int OPTION_NONE = -2;
		static constexpr int OPTION_HEADER = -1;

		bool enabled;
		bool hovered;
		bool pressed;

		TextCursor cursor;
		std::optional<BakedText> baked {};
		utf8::UnicodeVector text;

		float getCursorOffset(int glyph) const;
		void drawCursorSelection(ImmediateRenderer& immediate);

	public:

		InputWidget(const std::function<void()>& callback);

		void draw(ImmediateRenderer& immediate) override;
		bool handle(const InputEvent &event) override;

};