#pragma once
#include "gameComponent.hpp"


class MatrixAnimation : public GameComponent {
public:
	enum AnimationType{
		NONE,
		ROTATE,
		TRANSLATE,
		SHAPE
	};
protected:
	AnimationType type;
	double animationSpeed;
	double percentage;
	double old_percentage;

	void rotation();
	void translation();
	void shape();

public:


	MatrixAnimation();

	MatrixAnimation(AnimationType type);

	void setAnimation(AnimationType newType);

	AnimationType getAnimation() const;

	void onUpdate(Context c) override;

	void onFixedUpdate(FixedContext c) override;

	void onConnected() override;

	InputResult onEvent(const InputEvent& event) override;

};

