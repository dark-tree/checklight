
#include "field.hpp"

#include "input/input.hpp"
#include "render/immediate.hpp"
#include "gui/context.hpp"

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

	if (begin() < (int) text.size()) {
		text.erase(text.begin() + begin());
	}
}

void TextCursor::eraseBackward(utf8::UnicodeVector& text) {
	if (hasSelection()) {
		eraseSelection(text);
		return;
	}

	if (begin() <= (int) text.size() && begin() > 0) {
		text.erase(text.begin() + begin() - 1);
	}

	if (begin() > 0) {
		master --;
		slave = master;
	}
}

void TextCursor::select(int start, int end) {
	master = start;
	slave = end;
}

int TextCursor::begin() const {
	return std::min(master, slave);
}

int TextCursor::end() const {
	return std::max(master, slave);
}

/*
 * FieldWidget
 */

FieldWidget::FieldWidget()
: InputWidget() {
	min_height = Unit::px(30);
	min_width = Unit::px(100);
	background = Color {100, 100, 100};
	radius = Unit::px(10);
	vertical = VerticalAlignment::CENTER;
	horizontal = HorizontalAlignment::LEFT;
}

float FieldWidget::getCursorOffset(int glyph) const {
	const auto& glyphs = baked->getQuads();
	float ox = content.x;

	const int end = std::min((int) glyphs.size(), glyph);
	for (int i = 0; i < end; i++) {
		ox += glyphs[i].advance;
	}

	return ox;
}

void FieldWidget::drawCursorSelection(ImmediateRenderer& immediate) {
	float start = getCursorOffset(cursor.begin());
	float end = getCursorOffset(cursor.end());

	float height = 20; // TODO
	float baseline = (content.h * 0.5f) + content.y - height / 2;
	bool show = shouldDrawCursor();

	if (show) {
		immediate.setFill(0, 0, 100);
		immediate.drawLine2D(start, baseline, start, baseline + height);
	}

	if (cursor.hasSelection()) {
		immediate.drawLine2D(end, baseline, end, baseline + height);

		immediate.setFill(100, 100, 200, 150);
		immediate.setRectRadius(0);
		immediate.drawRect2D(start, baseline, end - start, height);
	}
}

utf8::UnicodeVector FieldWidget::getDisplayUnicodes() {
	if (kind == PASSWORD) {
		utf8::UnicodeVector result;
		result.reserve(text.size());

		for ([[maybe_unused]] uint32_t unicode : text) {
			result.push_back('*');
		}

		return result;
	}

	if (text.empty()) {
		return utf8::toCodePoints(placeholder.c_str());
	}

	return text;
}

bool FieldWidget::shouldDrawCursor() {
	const double time = glfwGetTime();
	return cursor.hasSelection() || sin(time * 6) < 0 || (cooldown + 0.4 > time);
}


bool FieldWidget::applyCursorSelection(const PositionedEvent* event, bool drag) {
	if (!pressed) {
		return false;
	}

	// check each character in string
	for (int i = 0; i <= (int) text.size(); i++) {
		float target = getCursorOffset(i);

		// make the selection box be start the middles of two
		// neighbouring characters not just on the gap itself
		float start = (target + getCursorOffset(i - 1)) * 0.5f;
		float end = (target + getCursorOffset(i + 1)) * 0.5f;

		// seek to gap n success
		if (event->isWithinBox(start, content.y, end - start, content.h)) {
			cursor.seek(text, i, drag);
			return true;
		}
	}

	return false;
}


void FieldWidget::draw(ImmediateRenderer& immediate, ElementState state) {

	state = computeWidgetState();
	drawBasicPanel(immediate, state);

	immediate.setWrapping(false);
	immediate.setTextAlignment(vertical.fetch(state));
	immediate.setTextAlignment(horizontal.fetch(state));
	immediate.setFont(font.fetch(state));
	immediate.setFontSize(size.fetch(state));
	immediate.setTextBox(content.w, content.h);

	if (text.empty()) {
		immediate.setFill(placeholder_color.fetch(state));
	} else {
		immediate.setFill(color.fetch(state));
	}

	baked = immediate.bakeUnicode(content.x, content.y, getDisplayUnicodes());
	immediate.drawText2D(0, 0, *baked);

	if (isFocused()) {
		immediate.setLineWidth(1);
		drawCursorSelection(immediate);
	}

}

bool FieldWidget::event(WidgetContext& context, const InputEvent& any) {

	// some of the functions we invoke here expect the text to be baked
	if (!enabled || !baked.has_value()) {
		return false;
	}

	if (auto* event = any.as<FrameEvent>()) {
		if (hovered) {
			event->cursor(CursorIcon::TEXT);
			return true;
		}

		return false;
	}

	bool used = InputWidget::event(context, any);

	// handle dragging (make selection)
	if (const auto* button = any.as<MouseEvent>()) {
		used |= applyCursorSelection(button, true);
	}

	// handle click (begin selection) + shift-click (make selection)
	if (const auto* button = any.as<ButtonEvent>()) {
		used |= applyCursorSelection(button, button->isShiftPressed());
		cooldown = glfwGetTime();
	}

	if (!isFocused()) {
		return false;
	}

	if (const auto* unicode = any.as<UnicodeEvent>()) {
		uint32_t codepoint = unicode->unicode;

		if (kind == INTEGER) {
			if (codepoint < '0') return true;
			if (codepoint > '9') return true;
		}

		cursor.insert(text, codepoint);
		cursor.move(text, +1, false);
		cooldown = glfwGetTime();
		return true;
	}

	if (const auto* keyboard = any.as<KeyboardEvent>()) {
		if (keyboard->isCtrlPressed() && keyboard->wasPressed(GLFW_KEY_A)) {
			cursor.select(0, text.size());
			return true;
		}

		if (keyboard->isTypedEvent()) {
			if (keyboard->keycode == GLFW_KEY_BACKSPACE) {
				cursor.eraseBackward(text);
				cooldown = glfwGetTime();
				return true;
			}

			if (keyboard->keycode == GLFW_KEY_DELETE) {
				cursor.eraseForward(text);
				cooldown = glfwGetTime();
				return true;
			}

			if (keyboard->keycode == GLFW_KEY_LEFT) {
				cursor.move(text, -1, keyboard->isShiftPressed());
				cooldown = glfwGetTime();
				return true;
			}

			if (keyboard->keycode == GLFW_KEY_RIGHT) {
				cursor.move(text, +1, keyboard->isShiftPressed());
				cooldown = glfwGetTime();
				return true;
			}
		}
	}

	return used;
}