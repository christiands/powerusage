powerusage: "poweruser" utilities for lazy programmers (me)

Programs:
- cprd: CoPy Recent Download: Gets the most recent file from the downloads
  folder, and copies it to the current folder

Future Programs:
- cvrd: Combines cprd and mvrd into one command, which is aliased into two

Todo:
- Add extraneous error handling to cprd and common.h, since it's pretty bad
  right now
- Optimize copy_file for Linux's sendfile() command
- Add error strings to common.h (for an operation kinda like OpenGL's
  glGetShaderInfoLog)
- Add a `make install` command to get rid of the crappy shell script
