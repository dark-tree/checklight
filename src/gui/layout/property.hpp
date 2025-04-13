#pragma once

#include "external.hpp"
#include "state.hpp"
#include "lerps.hpp"
#include "shared/ease.hpp"

/*
 * Yee who enter need not be afraid of
 * dead ends, cuz they are plentiful here;
 * slithering along the paths and passes of this
 * ever-changing wasteland. Abstract concepts and
 * arcane constructs fallen by the waysides, like
 * tossed stones guiding onto paths most devious.
 *
 * Beware!
 */

struct StyleContext {

	int width = 0;
	int height = 0;

};

// Check if the given property value type can be interpolated
template <typename T>
concept HasInterpolator = requires (T a, T b, float delta) {
	{ interpolate(a, b, delta) } -> std::same_as<T>;
};

// Extra state kept for interpolatable properties
template <typename T, template<typename Ty> class C>
struct StyleInterpolator {

	protected:

		using clock = std::chrono::steady_clock;
		using timestamp = std::chrono::time_point<clock>;
		using easing = float(*)(float);
		using interpolated = std::true_type;

		// interpolation timing
		std::chrono::milliseconds duration = 0ms;
		mutable timestamp stamp {};

		// interpolation endpoints
		mutable T begin {};
		mutable T end {};

		// easing function
		mutable bool initial = true;
		easing ease = nullptr;

		/// Get the interpolation delta with the easing function applied
		constexpr double getEasedDelta(timestamp now) const {
			using float_cast = std::chrono::duration<float, std::chrono::seconds::period>;

			const float delta = float_cast(now - stamp).count();
			const float limit = float_cast(duration).count();

			return ease == nullptr ? 1 : ease((std::clamp(delta, -limit, 0.0f) + limit) / limit);
		}

		constexpr timestamp getClockNow() const {
			return clock::now();
		}

	public:

		/// Enable interpolation
		template <typename R, typename P>
		C<T>& transition(const std::chrono::duration<R, P>& duration, easing ease = ease::ofLinear) {
			this->duration = duration;
			this->ease = ease;
			return *static_cast<C<T>*>(this);
		}

		/// Disable interpolation
		C<T>& transition(Disabled disabled) {
			this->duration = 0ms;
			this->ease = nullptr;
			return *static_cast<C<T>*>(this);
		}

};

template <typename T>
struct StyleProperty : std::conditional_t<HasInterpolator<T>, StyleInterpolator<T, StyleProperty>, std::monostate> {

	public:

		using Factory = std::move_only_function<T(const StyleContext&, ElementState state) const noexcept>;

	private:

		Factory factory;

		/// Pass the given value through the interpolator
		T filter(const T& next) const {

			// check if the interpolation mark is present
			if constexpr (HasInterpolator<T>) {

				const auto now = this->getClockNow();

				// init the interpolator
				if (this->initial) {
					this->begin = next;
					this->end = next;
					this->stamp = now + this->duration;
					this->initial = false;
				}

				const T current = interpolate(this->begin, this->end, this->getEasedDelta(now));

				// end point changed, update interpolation range
				if (this->end != next) {
					this->begin = current;
					this->end = next;
					this->stamp = now + this->duration;
					return this->begin;
				}

				return current;
			}

			return next;
		}

	public:

		StyleProperty(const T& constant) {
			this->operator=(constant);
		}

		StyleProperty(Factory&& factory) {
			this->operator=(factory);
		}

		StyleProperty& operator =(T constant) {
			this->factory = [constant] (const StyleContext&, ElementState state) noexcept -> T {
				return constant;
			};

			return *this;
		}

		StyleProperty& operator =(Factory&& factory) {
			this->factory = std::move(factory);

			return *this;
		}

		/// Invoke the property factory given the styling context
		T get(const StyleContext& context, ElementState state = ElementState::ofLayout()) const {
			return filter(this->factory(context, state));
		}

		/// Unwrap property using user specified unwrapper
		template <auto Unwrapper> requires requires (T t, StyleContext context) { (&t->*Unwrapper)(context); }
		constexpr auto unwrap(const StyleContext& context) const -> std::invoke_result_t<decltype(Unwrapper), T, StyleContext> {
			return (get(context).*Unwrapper)(context);
		}

		/// Unwrap property using the type-defined unwrapper
		constexpr auto unwrap(const StyleContext& context) const
		requires requires { T::unwrapper; } {
			return unwrap<T::unwrapper>(context);
		}

};

// non-interpolated StyleProperties should be no larger than the internal factory itself
static_assert(sizeof(StyleProperty<std::string>) == sizeof(StyleProperty<std::string>::Factory));

// interpolated StyleProperties should be bigger by the size of the interpolation struct
static_assert(sizeof(StyleProperty<int>) >= sizeof(StyleProperty<int>::Factory) + sizeof(StyleInterpolator<int, StyleProperty>));
