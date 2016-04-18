#ifndef _MACHINE__TYPES_H
#define	_MACHINE__TYPES_H

#include <machine/_default_types.h>

typedef	__int32_t	__blkcnt_t;
#define	__machine_blkcnt_t_defined

typedef	__int32_t	__blksize_t;
#define	__machine_blksize_t_defined

typedef	__uint64_t	__dev_t;
#define	__machine_dev_t_defined

#if defined(__arm__) || defined(__i386__) || defined(__m68k__) || defined(__mips__) || defined(__PPC__) || defined(__sparc__)
typedef	__int64_t	_off_t;
#else
typedef	__int32_t	_off_t;
#endif
#define	__machine_off_t_defined

typedef	_off_t		_fpos_t;
#define	__machine_fpos_t_defined

typedef	unsigned long	__ino_t;
#define	__machine_ino_t_defined

typedef	__uint32_t	__mode_t;
#define	__machine_mode_t_defined

#ifdef _KERNEL
typedef	int		boolean_t;
typedef	struct device	*device_t;
typedef	char		vm_memattr_t;	/* memory attribute codes */
#endif /* _KERNEL */

#endif /* _MACHINE__TYPES_H */
