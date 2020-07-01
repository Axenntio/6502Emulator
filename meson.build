project('6502 emulator', 'cpp',
		version : '0.1',
		default_options : ['warning_level=3', 'cpp_std=c++17'])

jsondep = dependency('jsoncpp')


executable('emulator',
			['main.cpp', 'Config.cpp', 'Device.cpp', 'EEPROM.cpp', 'CPU.cpp', 'RAM.cpp', 'ACIA.cpp'],
			dependencies : jsondep,
			install : true)
