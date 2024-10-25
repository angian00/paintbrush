project(
  'paintbrush',
  'cpp',
  version : '0.0.1',
  default_options : ['warning_level=1']
)

build_args = [
  '-Wno-unused-parameter',
  '-Wno-pedantic',
]


qt5 = import('qt5')
qt5_dep = dependency('qt5', modules: ['Core', 'Gui', 'Widgets'])

sources = [
  'main.cpp',
  'paintbrush_window.cpp',
  'paintbrush_canvas.cpp',
]

sources += qt5.compile_moc(headers: 'paintbrush_canvas.h')

executable(
  'paintbrush.x',
  sources,
  dependencies: qt5_dep,
  cpp_args : build_args,
)
