#ifndef STRETCHY_CONTAINER
#define STRETCHY_CONTAINER
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

class StretchyContainer : public Container
{
	GDCLASS(StretchyContainer, Container)

	StretchyContainer();
	~StretchyContainer();

public:
	enum StretchOrigin
	{
		STRETCH_ORIGIN_CENTER = 0,
		STRETCH_ORIGIN_LEFT = 1,
		STRETCH_ORIGIN_RIGHT = 2,
		STRETCH_ORIGIN_TOP = 3,
		STRETCH_ORIGIN_BOTTOM = 4
	};
protected:
	static void _bind_methods();
	void _notification(int p_what);
private:
	bool still_inside;
	bool enabled;
	float duration = 0.2f;
	StretchOrigin stretch_origin;

	Ref<Curve> stretch_curve_x;
	Ref<Curve> stretch_curve_y;
	Ref<Tween> pop_tw;
	Ref<Tween> unpop_tw;
	bool resize_x = false;
	bool resize_y = false;

	Point2 (*_resize_child_by)(const Vector2& size, const Vector2& child_size);
	static Point2 offset_from_center(const Vector2& size, const Vector2& child_size);
	static Point2 offset_from_left(const Vector2& size, const Vector2& child_size);
	static Point2 offset_from_right(const Vector2& size, const Vector2& child_size);
	static Point2 offset_from_top(const Vector2& size, const Vector2& child_size);
	static Point2 offset_from_bottom(const Vector2& size, const Vector2& child_size);

	void resize_child_by(const float amm);
	void pop_box();
	void unpop_box();
	void reset_function();
	void reset_tweens();

public:
	virtual Vector2 _get_minimum_size() const override;
	virtual PackedInt32Array _get_allowed_size_flags_horizontal() const override;
	virtual PackedInt32Array _get_allowed_size_flags_vertical() const override;
	void set_enabled(const bool enb);
	bool get_enabled() const;
	void set_duration(const float dur);
	float get_duration() const;
	
	Ref<Curve> get_stretch_curve_x() const;
	void set_stretch_curve_x(const Ref<Curve> cve);
	Ref<Curve> get_stretch_curve_y() const;
	void set_stretch_curve_y(const Ref<Curve> cve);

	uint32_t  get_stretch_origin() const;
	void set_stretch_origin(uint32_t  str);
};
#endif