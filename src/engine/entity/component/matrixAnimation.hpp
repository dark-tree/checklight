#pragma once
#include "gameComponent.hpp"


class MatrixAnimation : public GameComponent {
	enum AnimationType{
		NONE,
		ROTATE,
		TRANSLATE,
		SHAPE
	};

	AnimationType type;
	int animationSpeed;
	float percentage;

	void rotation();
	void translation();
	void shape();

public:
	MatrixAnimation();

	void setAnimation(AnimationType newType);

	AnimationType getAnimation() const;

	void onUpdate(Context c) override;

	void onFixedUpdate(FixedContext c) override;

	void onConnected() override;

	InputResult onEvent(const InputEvent& event) override;

};

