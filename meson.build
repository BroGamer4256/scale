project('scale', 'c', 'cpp', version: '1.0.0')
warning_level = 3
cpp_std = 'c++23'
debug = true
optimization = 3
b_lto = true
b_pgo = 'use'

cpp = meson.get_compiler('cpp')

add_project_arguments(
	cpp.get_supported_arguments(
		'-D_WIN32_WINNT=_WIN32_WINNT_WIN10',
		'-std=c++23',
		'-masm=intel',
		'-O3',
	),
	language: 'cpp',
)

add_project_link_arguments(
	cpp.get_supported_arguments(
		'-static',
		# '-s',
		'-std=c++23',
	),
	language: 'cpp',
)

minhook = subproject('minhook')
tomlc99 = subproject('tomlc99')

library(
	'scale',
	include_directories: [
		'src',
	],
	cpp_pch : 'src/helpers.h',
	dependencies : [
		minhook.get_variable('minhook_dep'),
		tomlc99.get_variable('tomlc99_dep'),
	],
	sources : [
		'src/mod.cpp',
		'src/diva.cpp',
	],
	name_prefix: ''
)
