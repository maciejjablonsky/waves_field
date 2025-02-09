with section("lint"):
    disabled_codes = [  # A custom command with one output doesn't really need a comment because
        # the default "generating XXX" is a good message already.
        "C0113", ]

with section("parse"):
    additional_commands = {
        "qt_add_qml_module": {
            "kwargs": {
                 "URI": 1,
                 "VERSION": 1,
                 "QML_FILES": "*",
                 "RESOURCES": "*",
                 "RESOURCE_PREFIX": 1,
                 "OUTPUT_DIRECTORY": 1
            }
        },
        "find_package": {
            "kwargs": {
                "REQUIRED": "+",
                "COMPONENTS": "*"
            }
        }
    }

with section("format"):
    always_wrap: []
    max_pargs_hwrap = 2
    max_subgroups_hwrap = 2
    line_width = 80
    tab_size = 4
    use_tabchars = False

    dangle_parens = True
    dangle_align = "prefix"
