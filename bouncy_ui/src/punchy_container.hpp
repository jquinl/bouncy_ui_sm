#ifndef PUNCHY_CONTAINER
#define PUNCHY_CONTAINER
#ifdef WIN32
#include <windows.h>
#endif

#include <godot_cpp/core/binder_common.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/core/binder_common.hpp>

#include <godot_cpp/classes/container.hpp>
#include <godot_cpp/classes/tween.hpp>
#include <godot_cpp/classes/method_tweener.hpp>
#include <godot_cpp/classes/callback_tweener.hpp>
#include <godot_cpp/classes/curve.hpp>
using namespace godot;

class PunchyContainer : public Container
{
	GDCLASS(PunchyContainer, Container)

	PunchyContainer();
	~PunchyContainer();

public:
	enum PunchOrigin
	{
		PUNCH_ORIGIN_CENTER = 0,
		PUNCH_ORIGIN_LEFT = 1,
		PUNCH_ORIGIN_RIGHT = 2,
		PUNCH_ORIGIN_TOP = 3,
		PUNCH_ORIGIN_BOTTOM = 4
	};
protected:
	static void _bind_methods();
	void _notification(int p_what);
private:
	Point2(*_resize_child_by)(const Vector2& size, const Vector2& child_size);
	static Point2 offset_from_center(const Vector2& size, const Vector2& child_size);
	static Point2 offset_from_left(const Vector2& size, const Vector2& child_size);
	static Point2 offset_from_right(const Vector2& size, const Vector2& child_size);
	static Point2 offset_from_top(const Vector2& size, const Vector2& child_size);
	static Point2 offset_from_bottom(const Vector2& size, const Vector2& child_size);
	bool still_inside;
	bool enabled = true;
	
	float punch_duration = 0.2f;
	float unpunch_duration = 0.2f;
	PunchOrigin punch_origin;

	bool mouse_entered;
	Ref<Curve> punch_curve_x;
	Ref<Curve> unpunch_curve_x;
	Ref<Curve> punch_curve_y;
	Ref<Curve> unpunch_curve_y;

	bool resize_x = false;
	bool resize_y = false;

	void resize_child_by(const float amm);
	void unresize_child_by(const float amm);

	void pop_box();
	void unpop_box();
	void reset_function();
	void reset();

public:
	virtual Vector2 _get_minimum_size() const override;
	virtual PackedInt32Array _get_allowed_size_flags_horizontal() const override;
	virtual PackedInt32Array _get_allowed_size_flags_vertical() const override;
	
	void emit_punch_finished();
	void emit_unpunch_finished();
	
	void punch();
	void unpunch();
	void set_enabled(const bool enb);
	bool get_enabled() const;
	void set_punch_duration(const float dur);
	float get_punch_duration() const;
	void set_unpunch_duration(const float dur);
	float get_unpunch_duration() const;

	bool is_mouse_entered() const;
	Ref<Curve> get_punch_curve_x() const;
	void set_punch_curve_x(const Ref<Curve> cve);
	Ref<Curve> get_unpunch_curve_x() const;
	void set_unpunch_curve_x(const Ref<Curve> cve);
	Ref<Curve> get_punch_curve_y() const;
	void set_punch_curve_y(const Ref<Curve> cve);
	Ref<Curve> get_unpunch_curve_y() const;
	void set_unpunch_curve_y(const Ref<Curve> cve);
	uint32_t  get_punch_origin() const;
	void set_punch_origin(uint32_t  str);
};

#endif