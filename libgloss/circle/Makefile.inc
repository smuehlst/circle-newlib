multilibtool_DATA += %D%/nosys.specs
multilibtool_LIBRARIES += %D%/libnosys.a

%C%_libnosys_a_SOURCES = \
	%D%/chdir.cpp \
	%D%/chown.c \
	%D%/environ.c \
	%D%/errno.c \
	%D%/execve.c \
	%D%/_exit.cpp \
	%D%/fork.c \
	%D%/getcwd.cpp \
	%D%/getpid.c \
	%D%/gettod.cpp \
	%D%/io.cpp \
	%D%/kill.c \
	%D%/link.c \
	%D%/mem.c \
	%D%/mkdir.cpp \
	%D%/readlink.c \
	%D%/rename.cpp \
	%D%/sbrk.c \
	%D%/stat.cpp \
	%D%/symlink.c \
	%D%/times.c \
	%D%/unlink.cpp \
	%D%/wait.c
