#ifndef JUMPY_CONTAINER
#define JUMPY_CONTAINER
#ifdef WIN32
#include <windows.h>
#endif

#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/classes/container.hpp>
#include <godot_cpp/classes/tween.hpp>
#include <godot_cpp/classes/method_tweener.hpp>
#include <godot_cpp/classes/curve.hpp>
using namespace godot;

class JumpyContainer : public Container
{
	GDCLASS(JumpyContainer, Container)

		JumpyContainer();
	~JumpyContainer();

public:
	enum JumpOrigin
	{
		JUMP_ORIGIN_CENTER = 0,
		JUMP_ORIGIN_LEFT = 1,
		JUMP_ORIGIN_RIGHT = 2,
		JUMP_ORIGIN_TOP = 3,
		JUMP_ORIGIN_BOTTOM = 4
	};
protected:
	static void _bind_methods();
	void _notification(int p_what);

private:
	bool still_inside;

	Ref<Curve> rotation_curve;
	Ref<Curve> position_curve_x;
	Ref<Curve> position_curve_y;
	Ref<Curve> scale_curve_x;
	Ref<Curve> scale_curve_y;
	float resting_size;

public:
	virtual Vector2 _get_minimum_size() const override;
	virtual PackedInt32Array _get_allowed_size_flags_horizontal() const override;
	virtual PackedInt32Array _get_allowed_size_flags_vertical() const override;

	void jump();
	void jump_child(double amm);


	Ref<Curve> get_rotation_curve() const;
	void set_rotation_curve(const Ref<Curve> cve);
	Ref<Curve> get_position_curve_x() const;
	void set_position_curve_x(const Ref<Curve> cve);
	Ref<Curve> get_position_curve_y() const;
	void set_position_curve_y(const Ref<Curve> cve);
	Ref<Curve> get_scale_curve_x() const;
	void set_scale_curve_x(const Ref<Curve> cve);
	Ref<Curve> get_scale_curve_y() const;
	void set_scale_curve_y(const Ref<Curve> cve);

	float get_resting_size() const;
	void set_resting_size(const float size);
};
#endif