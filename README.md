# BouncyUI (Submodule)

[Bouncy UI](https://github.com/jquinl/bouncy_ui) without example project. So that it can be added painlessly as a submodule in a godot project.

From the godot project folder
```
git submodule add git@github.com:jquinl/bouncy_ui_sm.git addons/bouncy_ui_sm
git submodule update --init --recursive
```
Keep in mind that this will also add the godot-cpp submodule which needs to be in the `[Godot Project]/addons/bouncy_ui_sm/bouncy_ui` folder.

After this:
```
cd addons/bouncy_ui_sm/bouncy_ui
scons target=template_debug
```
This will create the binaries in `addons/bin`.

