# config file for cmake-fromat
# See https://cmake-format.readthedocs.io/en/latest/configuration.html

# flake8: noqa

with section("format"):

    tab_size = 4

with section("lint"):

    # a list of lint codes to disable
    disabled_codes = [
        'C0103' # Invalid INTERNAL variable name, which also applies to e.g. "CMAKE_EXE_LINKER_FLAGS"
      ]
