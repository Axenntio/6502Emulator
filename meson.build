project('6502 emulator', 'cpp',
		version : '0.1',
		default_options : ['warning_level=3', 'cpp_std=c++17'])

jsondep = dependency('jsoncpp')

sources = [
	'src/main.cpp',
	'src/Config.cpp',
	'src/CPU.cpp',
	'src/Device.cpp',
	'src/Devices/EEPROM.cpp',
	'src/Devices/RAM.cpp',
	'src/Devices/ACIA.cpp',
	'src/Devices/VIA.cpp'
]

executable('emulator',
			sources,
			dependencies : jsondep,
			install : true)
