#pragma once
#include "game.hpp"


class MatrixAnimation : public GameComponent {
public:
	enum AnimationType {
		NONE,
		ROTATE,
		TRANSLATE,
		SHAPE
	};

protected:
	AnimationType type;
	double animation_speed;
	double percentage;
	double old_percentage;

	void rotation();

	void translation();

	void shape();

public:
	MatrixAnimation() = delete;

	MatrixAnimation(SpatialPawn* s);

	MatrixAnimation(SpatialPawn* s, AnimationType type);

	void setAnimation(AnimationType newType);

	AnimationType getAnimation() const;

	void onUpdate(Context c) override;

	void onFixedUpdate(FixedContext c) override;

	void onConnected() override;

	InputResult onEvent(const InputEvent& event) override;
};
