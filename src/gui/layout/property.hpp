#pragma once

#include "external.hpp"
#include "state.hpp"

struct StyleContext {

	int width = 0;
	int height = 0;

};

template <typename T>
struct StyleProperty {

	public:

		using Factory = std::function<T(const StyleContext&, ElementState state)>;

	private:

		Factory factory;

	public:

		StyleProperty(const T& constant) {
			this->operator=(constant);
		}

		StyleProperty(const Factory& factory) {
			this->operator=(factory);
		}

		StyleProperty& operator =(T constant) {
			this->factory = [constant] (const StyleContext&, ElementState state) noexcept -> T {
				return constant;
			};

			return *this;
		}

		StyleProperty& operator =(const Factory& factory) {
			this->factory = factory;

			return *this;
		}

		/// Invoke the property factory given the styling context
		T get(const StyleContext& context, ElementState state = ElementState::LAYOUT) const {
			return this->factory(context, state);
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