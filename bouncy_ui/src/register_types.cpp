#include "register_types.hpp"

#include <godot/gdnative_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "stretchy_container.hpp"
#include "punchy_container.hpp"
#include "jumpy_container.hpp"

using namespace godot;

void initialize_bouncy_ui_module(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
    {
		return;
	}
    ClassDB::register_class<StretchyContainer>();
    ClassDB::register_class<PunchyContainer>();
    ClassDB::register_class<JumpyContainer>();
}
void uninitialize_bouncy_ui_module(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) 
    {
        return;
    }
}

extern "C" {
GDNativeBool GDN_EXPORT bouncy_ui_library_init(const GDNativeInterface *p_interface, const GDNativeExtensionClassLibraryPtr p_library, GDNativeInitialization *r_initialization) {
    godot::GDExtensionBinding::InitObject init_obj(p_interface, p_library, r_initialization);

    init_obj.register_initializer(initialize_bouncy_ui_module);
    init_obj.register_terminator(uninitialize_bouncy_ui_module);

    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);
    return init_obj.init();
}
}