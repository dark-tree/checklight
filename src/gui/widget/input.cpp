
#include "input.hpp"

#include <input/event/button.hpp>
#include <input/event/frame.hpp>
#include <input/event/keyboard.hpp>
#include <input/event/mouse.hpp>
#include <input/event/unicode.hpp>
#include <render/immediate.hpp>

/*
 * TextCursor
 */

uint32_t TextCursor::hasSelection() const {
	return master != slave;
}

void TextCursor::insert(utf8::UnicodeVector& text, uint32_t unicode) {
	if (hasSelection()) {
		eraseSelection(text);
	}

	text.insert(text.begin() + begin(), unicode);
}

void TextCursor::seek(const utf8::UnicodeVector& text, int position, bool drag) {
	if (!drag) {
		master = position;
	}

	slave = position;
	clampSelection(text);
}

void TextCursor::move(const utf8::UnicodeVector& text, int offset, bool select) {

	// in select mode master will stay anchored (won't move)
	if (select) {
		slave += offset;
	} else {
		const int anchor = offset < 0 ? begin() : end();

		slave = anchor + offset;
		master = anchor + offset;
	}

	clampSelection(text);
}

void TextCursor::eraseSelection(utf8::UnicodeVector& text) {
	text.erase(text.begin() + begin(), text.begin() + end());

	master = begin();
	slave = master;
}

void TextCursor::clampSelection(const utf8::UnicodeVector& text) {
	slave = std::clamp(slave, 0, (int) text.size());
	master = std::clamp(master, 0, (int) text.size());
}

void TextCursor::eraseForward(utf8::UnicodeVector& text) {
	if (hasSelection()) {
		eraseSelection(text);
		return;
	}

	if (begin() < text.size()) {
		text.erase(text.begin() + begin());
	}
}

void TextCursor::eraseBackward(utf8::UnicodeVector& text) {
	if (hasSelection()) {
		eraseSelection(text);
		return;
	}

	if (begin() <= text.size() && begin() > 0) {
		text.erase(text.begin() + begin() - 1);
	}

	if (begin() > 0) {
		master --;
		slave = master;
	}
}

int TextCursor::begin() const {
	return std::min(master, slave);
}

int TextCursor::end() const {
	return std::max(master, slave);
}

/*
 * InputWidget
 */

InputWidget::InputWidget(const std::function<void()>& callback)
: Widget() {}

float InputWidget::getCursorOffset(int glyph) const {
	const auto& glyphs = baked->getQuads();
	float ox = x;

	const int end = std::min((int) glyphs.size(), glyph);
	for (int i = 0; i < end; i++) {
		ox += glyphs[i].advance;
	}

	return ox;
}

void InputWidget::drawCursorSelection(ImmediateRenderer& immediate) {
	float start = getCursorOffset(cursor.begin());
	float end = getCursorOffset(cursor.end());

	float height = 20; // TODO
	float baseline = (h / 2) + y - height / 2;

	immediate.setColor(0, 0, 100);
	immediate.drawLine2D(start, baseline, start, baseline + height);

	if (cursor.hasSelection()) {
		immediate.drawLine2D(end, baseline, end, baseline + height);

		immediate.setColor(100, 100, 200, 150);
		immediate.setRectRadius(0);
		immediate.drawRect2D(start, baseline, end - start, height);
	}
}

void InputWidget::draw(ImmediateRenderer& immediate) {

	if (enabled) {
		if (hovered) {
			if (pressed) {
				immediate.setColor(255, 100, 100);
			} else {
				immediate.setColor(255, 80, 80);
			}
		} else {
			immediate.setColor(255, 0, 0);
		}
	} else {
		immediate.setColor(70, 70, 70);
	}

	immediate.setRectRadius(10);
	immediate.drawRect2D(x, y, w, h);

	immediate.setTextAlignment(HorizontalAlignment::LEFT);
	immediate.setFont("assets/font/OpenSans-Variable.ttf");
	immediate.setTextBox(w, h);
	baked = immediate.bakeUnicode(x, y, text);

	if (text.empty()) {
		immediate.setColor(50, 50, 50, 200);
		immediate.drawString2D(x, y, placeholder);
	} else {
		immediate.setColor(0, 0, 0);
		immediate.drawText2D(0, 0, *baked);
	}

	immediate.setLineWidth(1);
	drawCursorSelection(immediate);

}

bool InputWidget::handle(const InputEvent& any) {

	// some of the functions we invoke here expect the text to be baked
	if (!enabled || !baked.has_value()) {
		return false;
	}

	Widget::handle(any);

	if (const auto* unicode = any.as<UnicodeEvent>()) {
		cursor.insert(text, unicode->unicode);
		cursor.move(text, +1, false);
	}

	if (const auto* button = any.as<ButtonEvent>()) {
		if (button->isPressEvent()) {
			if (button->isWithinBox(x, y, w, h)) {
				for (int i = 0; i <= text.size(); i++) {
					float target = getCursorOffset(i);

					float start = (target + getCursorOffset(i - 1)) * 0.5f;
					float end = (target + getCursorOffset(i + 1)) * 0.5f;

					if (button->isWithinBox(start, y, end - start, h)) {
						cursor.seek(text, i, button->isShiftPressed());
						break;
					}
				}
			}
		}
	}

	if (const auto* keyboard = any.as<KeyboardEvent>()) {
		if (keyboard->isPressEvent()) {

			if (keyboard->keycode == GLFW_KEY_BACKSPACE) {
				cursor.eraseBackward(text);
			}

			if (keyboard->keycode == GLFW_KEY_DELETE) {
				cursor.eraseForward(text);
			}

			if (keyboard->keycode == GLFW_KEY_LEFT) {
				cursor.move(text, -1, keyboard->isShiftPressed());
			}

			if (keyboard->keycode == GLFW_KEY_RIGHT) {
				cursor.move(text, +1, keyboard->isShiftPressed());
			}
		}
	}

	return false;
}