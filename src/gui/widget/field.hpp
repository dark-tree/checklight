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

		/// Check if the cursor has an area selected
		uint32_t hasSelection() const;

		/// Apply a character insertion to the selection
		void insert(utf8::UnicodeVector& text, uint32_t unicode);

		/// Move the cursor to a specific offset, the drag flag can be used to move only one end of the selection
		void seek(const utf8::UnicodeVector& text, int position, bool drag);

		/// Move the cursor by a set number of spaces, the select flag can be used to move only one end of the selection
		void move(const utf8::UnicodeVector& text, int offset, bool select);

		/// Apply the delete key to the selection
		void eraseForward(utf8::UnicodeVector& text);

		/// Apply the backspace key to the selection
		void eraseBackward(utf8::UnicodeVector& text);

		/// Select the specific text range, assumes the range is correct, but can be empty
		void select(int start, int end);

		/// Get leftmost extend of the selection, in range [0, text.size()]
		int begin() const;

		/// Get rightmost extend of the selection, in range [0, text.size()]
		int end() const;

};

/**
 * Simple Unicode text field with several options
 */
class FieldWidget : public InputWidget {

	public:

		using Callback = std::function<void(const utf8::UnicodeVector&)>;

		enum Kind {
			INTEGER,
			TEXT,
			PASSWORD,
		};

	private:

		double cooldown = 0;

		Kind kind = TEXT;
		std::string placeholder = "Text Input...";
		TextCursor cursor;
		std::optional<BakedText> baked {};
		utf8::UnicodeVector text;
		Callback callback;

		/// Get the cursor offset, in pixels, given a position in text
		float getCursorOffset(int glyph) const;

		/// Draw the cursor, and selection
		void drawCursorSelection(ImmediateRenderer& immediate);

		/// Get a string to show as the field's value
		utf8::UnicodeVector getDisplayUnicodes();

		/// Handles placing and dragging (selecting) the cursor
		bool applyCursorSelection(const PositionedEvent* event, bool drag);

		/// Used for cursor blink animation
		bool shouldDrawCursor();

	public:

		/// Font size
		StyleProperty<int> size = 20;

		/// Text color
		StyleProperty<Color> color = Color {0, 0, 0};

		/// Placeholder color
		StyleProperty<Color> placeholder_color = Color {40, 40, 40};

		/// Font filepath
		StyleProperty<std::string> font = std::string {"assets/font/OpenSans-Variable.ttf"};

	public:

		FieldWidget();

		void draw(ImmediateRenderer& immediate, ElementState state) override;
		bool event(WidgetContext& context, const InputEvent &event) override;

		const utf8::UnicodeVector& getUnicodeValue() const;
		void setValue(const std::string& value);

		void onChange(const Callback& callback);

};