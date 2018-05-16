/***********************************************************************************
 *
 *    DESCRIPTION: Intracorporate Header (Абсолютный маст-хэв!)
 *
 *    AUTHOR: Juggernaught
 *
 *    LAST CHANGE:
 *    $Id: DSPLAB.h 1708 2015-07-03 12:27:01Z derpanzer $
 *    $URL: http://svn.dsplab.ru/DSPLab/trunk/Design/include/DSPLAB_H/DSPLAB.h $
 *
 ***********************************************************************************/

#ifndef _DSPLAB_
#define _DSPLAB_

// Header Vesion Self-Control
#define DSPLAB_H_VERSION		1

// CCS v5 SDSCM00039005 workaround
#ifdef __CDT_PARSER__
	#define restrict
	#define far
#endif

#ifdef _MSC_VER
	#pragma warning(disable : 4996)
#endif

// ДлЯ ПЦ
#if defined(_WIN32) || defined(__linux__) || (defined(__APPLE__) && defined (__MACH__))
	#define WORD_ALIGNED(p)	
	#define DWORD_ALIGNED(p)
	#define QWORD_ALIGNED(p)
	
	// Extract Bit Field
	#define EXT(x, length, shift)		(( (INT32)(x) << (32 - (length) - (shift))) >> (32 - (length)))
	#define EXTU(x, length, shift)		(((UINT32)(x) << (32 - (length) - (shift))) >> (32 - (length)))
	#define EXT64(x, length, shift)		(( (INT64)(x) << (64 - (length) - (shift))) >> (64 - (length)))
	#define EXTU64(x, length, shift)	(((UINT64)(x) << (64 - (length) - (shift))) >> (64 - (length)))

#else	// ДлЯ Техаса

	asm("	.length 32767");

	#ifdef __cplusplus
		// Optimizing assert
		#define NASSERT(cond)		std::_nassert(cond)
		// Compile-time assert
		#define _CASSERT(pred)    switch(0){case 0:case pred:;}

		#define WORD_ALIGNED(p)		std::_nassert(((DWORD)(p) & 0x01) == 0)
		#define DWORD_ALIGNED(p)	std::_nassert(((DWORD)(p) & 0x03) == 0)
		#define QWORD_ALIGNED(p)	std::_nassert(((DWORD)(p) & 0x07) == 0)

		#if	  defined(_TMS320C6X)
			#define IRQ_DISABLE()		register UINT32 __ie__ = _disable_interrupts()
			#define IRQ_REDISABLE()		__ie__ = _disable_interrupts()
			#define IRQ_RESTORE()		_restore_interrupts(__ie__)
		#elif defined(__TMS320C28XX__)
			#define IRQ_DISABLE()		register UINT __ie__ = __disable_interrupts()
			#define IRQ_REDISABLE()		__ie__ = __disable_interrupts()
			#define IRQ_RESTORE()		__restore_interrupts(__ie__)
		#endif
		
		#define ATOM_BEGIN			IRQ_DISABLE
		#define ATOM_END			IRQ_RESTORE
	#else
		#define NASSERT(cond)		_nassert(cond)

		#define WORD_ALIGNED(p)		_nassert(((DWORD)(p) & 0x01) == 0)
		#define DWORD_ALIGNED(p)	_nassert(((DWORD)(p) & 0x03) == 0)
		#define QWORD_ALIGNED(p)	_nassert(((DWORD)(p) & 0x07) == 0)

		#define IRQ_DISABLE(gie)	gie = _disable_interrupts()
		#define IRQ_RESTORE(gie)	_restore_interrupts(gie)
	#endif
	
	#ifdef _TMS320C6X
		// Useful macros
		#define EXT(x, length, shift)		(_ext(x, 32 - length - shift, 32 - length))
		#define EXTU(x, length, shift)		(_extu(x, 32 - length - shift, 32 - length))
		#define SETBF(x, length, shift)		(_set(x, shift, length + shift - 1))
		#define CLRBF(x, length, shift)		(_clr(x, shift, length + shift - 1))

		#define	MAKE_DWORD(h, l)			((DWORD)(h) << 16) | (l)

		#define MAKE_QWORD(h, l)			(_itoll(h, l))
		#define LODWORD(l)					(_loll(l))
		#define HIDWORD(l)					(_hill(l))

		// Changing Q-factors for integer calculations
		#define LOWER_Q_FACTOR(x, qold, qnew)			((x) >> ((qold) - (qnew)))
		#define LOWER_Q_FACTOR_R(x, qold, qnew)			(((x) + (1 << ((qold) - (qnew) - 1))) >> ((qold) - (qnew)))
		#define RAISE_Q_FACTOR(x, qold, qnew)			((x) << ((qnew) - (qold)))
		#define CHANGE_Q_SAT(x, qold, qnew, bitwidth)	(_sshl(x, 32 - (bitwidth) - (qold) + (qnew)) >> (32 - (bitwidth)))
		#define CHANGE_Q_SATU(x, qold, qnew, bitwidth)	((UINT)_sshl(x, 31 - (bitwidth) - (qold) + (qnew)) >> (31 - (bitwidth)))
		
		// Saturates n-bit integer (n includes sign bit)
		#define SAT_INT(n, bits)			(_sshl(n, 32 - (bits)) >> (32 - (bits)))

	#endif
	
	#define INT8_OVERFLOW(x)			(_norm(x) < 8)
	#define INT16_OVERFLOW(x)			(_norm(x) < 16)
	#define UINT8_OVERFLOW(x)			((x) >> 8)
	#define UINT16_OVERFLOW(x)			((x) >> 16)
	
	#define RESTRICT_THIS(classname)	classname * const restrict _this = this
	#define THIS(var)					_this->var
	
	#define CREGISTER					extern volatile cregister DWORD
	
	#define LOWORD(dw)					((dw) & 0xFFFF)
	#define HIWORD(dw)					((dw) >> 16)
	
	#define SUCCEEDED(x)				((x) >= 0)
	#define FAILED(x)					((x) < 0)
	#define MAKE_ERROR(x)				(-(x))

	#define ALIGN_FIELD(x)			 	__attribute__ ((aligned(x)))
#endif

#include <string.h>

#if (!defined(QT_VERSION) && !defined(OX_CLASS_DECL))
	#define count(x)		(sizeof(x) / sizeof(x[0]))
#endif
#ifndef OX_CLASS_DECL
	#define countof(x)			(sizeof(x) / sizeof(x[0]))
#endif

// Remember, that because on C55x and C28x chips
// MAU = 16 bit (2 bytes), sizeof() operator behaves accordingly
#if defined(__TMS320C55X__) || defined(__TMS320C28XX__)
	#define bytes(x)		(sizeof(x) * 2)
	#define words(x)		(sizeof(x))
	#define dwords(x)		((sizeof(x) + 1) / 2)
	#define qwords(x)		((sizeof(x) + 3) / 4)
#else
	#define bytes(x)		(sizeof(x))
	#define words(x)		((sizeof(x) + 1) / 2)
	#define dwords(x)		((sizeof(x) + 3) / 4)
	#define qwords(x)		((sizeof(x) + 7) / 8)
#endif

// FIX: duplicate min and max from <limits>
#if !defined(_WIN32) && !defined(__linux__) && !(defined(__APPLE__) || defined (__MACH__))
	#define min(x,y) 			(((x) < (y)) ? (x) : (y))
	#define max(x,y) 			(((x) > (y)) ? (x) : (y))
#endif

#define diffptr_b(x, y)		((LPBYTE)(x) - (LPBYTE)(y))
#define diffptr_w(x, y)		((LPWORD)(x) - (LPWORD)(y))
#define diffptr_dw(x, y)	((LPDWORD)(x) - (LPDWORD)(y))


// ----------------------------------------------------------------
// ----------- DATA TYPES DEFINITIONS -----------------------------
// ----------------------------------------------------------------

#ifndef NULL
	#define NULL				0
#endif
#ifndef TRUE
	#define TRUE				1
#endif
#ifndef FALSE
	#define FALSE				0
#endif


#define NRESULT_OK				0

#if defined(_WIN32) || defined(__linux__) || (defined(__APPLE__) && defined (__MACH__))
	typedef signed char			INT8;
	typedef signed short		INT16;
	typedef signed int			INT32;
	
	typedef unsigned char		UINT8;
	typedef unsigned short		UINT16;    
	typedef unsigned int		UINT32;     
	
	typedef unsigned char		BYTE;
	typedef unsigned short		WORD;
#if defined(__LP64__) // for a target where 'long int' and pointer both use 64-bits and 'int' uses 32-bit
    typedef unsigned int		DWORD;
#else
    typedef unsigned long		DWORD;
#endif
	
	typedef float				FLOAT;
	typedef double				DOUBLE;
	
	typedef int					BOOL;
	
	typedef int					NRESULT;
#endif

#if defined(_WIN32)
    typedef signed __int64 		INT64;
    typedef unsigned __int64 	UINT64;
#elif defined(__linux__) || (defined(__APPLE__) && defined (__MACH__))
	#include <stdint.h>
	
	#define LANG_RUSSIAN        0x19
	#define SUCCEEDED(Status)   ((HRESULT)(Status) >= 0)
    
    typedef long long		    INT64;
    typedef unsigned long long 	UINT64;
    typedef unsigned            HINSTANCE;
    typedef unsigned            HANDLE;
    typedef intptr_t            INT_PTR;
    typedef const wchar_t*      LPCWSTR;
    typedef WORD                LANGID;
    typedef unsigned long       ULONG;
    typedef char*               LPSTR;
    typedef long                HRESULT;
#endif

#ifdef _TMS320C6X
	typedef char				CHAR;

	typedef char				INT8;
	typedef short				INT16;
	typedef int					INT32;
	typedef __int40_t			INT40;
	typedef long long 			INT64;

	typedef unsigned char		UINT8;
	typedef unsigned short		UINT16;    
	typedef unsigned int		UINT32;     
	typedef unsigned __int40_t	UINT40;
	typedef unsigned long long 	UINT64;
	
	typedef unsigned char		BYTE;
	typedef unsigned short		WORD;
	typedef unsigned int		DWORD;
	typedef unsigned long		LONG;
	typedef unsigned long long 	QWORD;
	
	typedef float				FLOAT;
	typedef double				DOUBLE;
	
	typedef const char *		LPCSTR;
	typedef char *				LPSTR;
								
	typedef unsigned int		BOOL;

	typedef int					NRESULT;

	#ifdef USE_DSPLAB_HANDLE
		typedef DWORD			HANDLE;
	#endif
	
	#define INT16_MIN			-32768
	#define INT16_MAX			32767
	#define UINT16_MAX			65535U
	#define WORD_MAX			0xFFFFU
	
	#define INT32_MIN			-2147483648
	#define INT32_MAX			2147483647
	#define UINT32_MAX			4294967295U
	#define DWORD_MAX			0xFFFFFFFFU
#endif
#ifdef __TMS320C55X__
	typedef short				INT16;
	typedef long				INT32;
	typedef long long			INT40;
	
	typedef unsigned short		UINT16;    
	typedef unsigned long		UINT32;     
	typedef unsigned long long	UINT40;
	
	typedef unsigned char		BYTE;
	typedef unsigned short		WORD;
	typedef unsigned long		DWORD;
	typedef unsigned long long	LONG;
	
	typedef float				FLOAT;
	
	typedef bool				BOOL;
	
	typedef int					NRESULT;
#endif
#ifdef __TMS320C28XX__
	typedef int					INT16;
	typedef long				INT32;
	typedef long long			INT64;
	
	typedef const char*			LPCSTR;
	typedef char*				LPSTR;
	
	typedef unsigned int		UINT16;    
	typedef unsigned long		UINT32;     
	typedef unsigned long long	UINT64;
	typedef unsigned long long	QWORD;
	
	typedef unsigned char		BYTE;
	typedef unsigned short		WORD;
	typedef unsigned long		DWORD;
	typedef unsigned long long	LONG;
	
	typedef float				FLOAT;
	
	typedef int					BOOL;
	
	typedef int					NRESULT;

	typedef DWORD				HANDLE;

	#define	MAKE_DWORD(hi, lo)	((UINT32)(hi) << 16) | (lo)
#endif

typedef unsigned int			uint;
typedef int						INT;     
typedef unsigned int			UINT;

typedef BYTE*					LPBYTE;
typedef WORD*					LPWORD;
typedef DWORD*					LPDWORD;

// Волатильные типы
typedef volatile BYTE			VBYTE;
typedef volatile WORD			VWORD;
typedef volatile DWORD			VDWORD;
typedef VBYTE*					LPVBYTE;
typedef VWORD*					LPVWORD;
typedef VDWORD*					LPVDWORD;

typedef	void*					LPVOID;

// Указатели на константы
typedef const void *			LPCVOID;
typedef const BYTE *			LPCBYTE;


#define PIdp	3.1415926535897932384626433832795
#define PIsp	3.1415926535897932f



// ----------------------------------------------------------------
// ------------ VERSIONING ----------------------------------------
// ----------------------------------------------------------------

#define	DSPLAB_TMSVER_TYPE_RELEASE	0
#define	DSPLAB_TMSVER_TYPE_ALPHA	1
#define	DSPLAB_TMSVER_TYPE_BETA		2
#define	DSPLAB_TMSVER_TYPE_RC		3

#ifndef __STRUCT_VERSIONPARENT__
#define __STRUCT_VERSIONPARENT__
	typedef struct STMSVersionParent
	{
		WORD	wStructVersion	: 8;		
		WORD	wVersionType	: 8;	/// Тип информации о версии
		WORD	wSizeof;				/// Размер структуры в байтах
	} STMSVersionParent, *PTMSVERSIONPARENT;
#endif

#ifdef __cplusplus
	/// ИнформациЯ о версии Out-файла.
	/// wStructVersion == 0x02, wVersionType == 0x01
	/// При использовании желательно отключить длЯ cpp-файла автоинлайн (-pi)
	struct S6000OutVersionInfo2 : public STMSVersionParent
	{
		inline S6000OutVersionInfo2() {	wSizeof = 0;	}
		inline S6000OutVersionInfo2(char *_szName, DWORD _dwClassID, DWORD _dwCaps,
									int _nMajor, int _nMinor, int _nBuild, int _nType, DWORD dwDT)
		{	strcpy(szName, _szName);
			wStructVersion = 0x02;	 wVersionType = 0x01;	wSizeof = sizeof(*this);
			dwClassID = _dwClassID;	 dwCaps = _dwCaps;
			nMajor = _nMajor, nMinor = _nMinor; nBuild = _nBuild; nType = _nType; nDT.dw = dwDT;	}
	
		char	szName[44];
	
		DWORD	dwClassID;			/// ClassID of this Software, defined in DSPLABclassID.h
		DWORD	dwCaps;				/// Firmware capabilities flags
		
		DWORD	nMajor			: 4;
		DWORD	nMinor			: 7;
		DWORD	nBuild			: 13;
		DWORD	nType			: 8;
		union
		{
			DWORD	dw;
			struct
			{
				DWORD	nYear	: 4;	// с 2005 года
				DWORD	nMonth	: 4;
				DWORD	nDay	: 5;
				DWORD	nHour   : 5;
				DWORD	nMinute	: 7;
				DWORD	__dumb	: 7;
			} bf;
		}	nDT;
	};
	
	/// ИнформациЯ о версии Out-файла.
	/// wStructVersion == 0x02, wVersionType == 0x02.
	/// При использовании желательно отключить длЯ cpp-файла автоинлайн (-pi)
	struct S6000LibVersionInfo2 : public STMSVersionParent
	{
		inline S6000LibVersionInfo2() {}
        inline S6000LibVersionInfo2(char *_szName, DWORD /*_dwCaps*/, int _nMajor, int _nMinor, int _nBuild, int _nType, DWORD dwDT)
		{	strncpy(szName, _szName, sizeof(szName));
			wStructVersion = 0x02;	 wVersionType = 0x02;	wSizeof = sizeof(*this);
			nMajor = _nMajor, nMinor = _nMinor; nBuild = _nBuild; nType = _nType; nDT.dw = dwDT;	}
	
		char	szName[32];
		
		DWORD	dwCaps;				/// Firmware capabilities flags
		
		DWORD	nMajor			: 4;
		DWORD	nMinor			: 7;
		DWORD	nBuild			: 13;
		DWORD	nType			: 8;
		union
		{
			DWORD	dw;
			struct
			{
				DWORD	nYear	: 4;	// с 2005 года
				DWORD	nMonth	: 4;
				DWORD	nDay	: 5;
				DWORD	nHour   : 5;
				DWORD	nMinute	: 7;
				DWORD	__dumb	: 7;
			} bf;
		}	nDT;
	};
#endif	// __cplusplus

typedef struct S6000OutVersionInfo2_s
{
	STMSVersionParent	hdr;

	char	szName[44];

	DWORD	dwClassID;			/// Software Class ID, defined in DL_SCID.h
	DWORD	dwCaps;				/// Firmware capabilities flags
	
	DWORD	nMajor			: 4;
	DWORD	nMinor			: 7;
	DWORD	nBuild			: 13;
	DWORD	nType			: 8;
	union
	{
		DWORD	dw;
		struct
		{
			DWORD	nYear	: 4;	// с 2005 года
			DWORD	nMonth	: 4;
			DWORD	nDay	: 5;
			DWORD	nHour   : 5;
			DWORD	nMinute	: 7;
			DWORD	__dumb	: 7;
		} bf;
	}	nDT;
} S6000OutVersionInfo2_s;

typedef struct S6000LibVersionInfo2_s
{
	STMSVersionParent	hdr;

	char	szName[32];

	DWORD	dwCaps;				/// Firmware capabilities flags
	
	DWORD	nMajor			: 4;
	DWORD	nMinor			: 7;
	DWORD	nBuild			: 13;
	DWORD	nType			: 8;
	union
	{
		DWORD	dw;
		struct
		{
			DWORD	nYear	: 4;	// с 2005 года
			DWORD	nMonth	: 4;
			DWORD	nDay	: 5;
			DWORD	nHour   : 5;
			DWORD	nMinute	: 7;
			DWORD	__dumb	: 7;
		} bf;
	}	nDT;
} S6000LibVersionInfo2_s;

/// ИнформациЯ о версии Out-файла.
/// wStructVersion == 0x03, wVersionType == 0x01.
/// ВАЖНО!!! ДлЯ Альфа-бета-РЦ версий, 8 LSB билда уходят в 8 MSB ревизии
typedef struct S6000OutVersionInfo3_s
{
	STMSVersionParent	hdr;

	char	szName[44];

	DWORD	dwCaps;				/// Firmware capabilities flags
	DWORD	dwSCID;				/// Software Class ID, defined in DL_SCID.h
	
	DWORD	nMajor			: 5;
	DWORD	nMinor			: 10;
	DWORD	nBuild			: 13; // ДлЯ Альфа-бета-РЦ версий, 8 LSB билда уходят в 8 MSB ревизии
	DWORD	nType			: 4;

	DWORD	nRevision		: 8;  // ДлЯ Альфа-бета-РЦ версий, 8 LSB билда уходят в 8 MSB ревизии
	DWORD	nTimestamp		: 24; // Minutes from 22.10.2010 16:00 UTC

} S6000OutVersionInfo3_s;

/// ИнформациЯ о версии Out-файла.
/// wStructVersion == 0x03, wVersionType == 0x02.
/// ВАЖНО!!! ДлЯ Альфа-бета-РЦ версий, 8 LSB билда уходят в 8 MSB ревизии
typedef struct S6000LibVersionInfo3_s
{
	STMSVersionParent	hdr;

	char	szName[44];

	DWORD	dwCaps;				/// Firmware capabilities flags
	
	DWORD	nMajor			: 5;
	DWORD	nMinor			: 10;
	DWORD	nBuild			: 13; // ДлЯ Альфа-бета-РЦ версий, 8 LSB билда уходят в 8 MSB ревизии
	DWORD	nType			: 4;

	DWORD	nRevision		: 8;  // ДлЯ Альфа-бета-РЦ версий, 8 LSB билда уходят в 8 MSB ревизии
	DWORD	nTimestamp		: 24; // Minutes from 22.10.2010 16:00 UTC

} S6000LibVersionInfo3_s;

#define S6000OUTVERSIONINFO2_HDR				{ 2, 1, sizeof(S6000OutVersionInfo2_s)}
#define S6000LIBVERSIONINFO2_HDR				{ 2, 2, sizeof(S6000LibVersionInfo2_s)}
#define S6000OUTVERSIONINFO3_HDR				{ 3, 1, sizeof(S6000OutVersionInfo3_s)}
#define S6000LIBVERSIONINFO3_HDR				{ 3, 2, sizeof(S6000LibVersionInfo3_s)}
#define S2800OUTVERSIONINFO2_HDR				{ 2, 3, bytes( S2800OutVersionInfo2_s)}

#ifdef _TMS320C6X
	#define DECLARE_S6000OUTVERSION2(variable)		extern const S6000OutVersionInfo2_s variable

	#define DEFINE_S6000OUTVERSION2(variable, name, SCID, Caps)																	\
													const S6000OutVersionInfo2_s variable = {S6000OUTVERSIONINFO2_HDR,			\
													name, SCID, Caps,
	#define DEFINE_S6000OUTVERSION2_CAPS_SCID(variable, name, Caps, SCID)														\
													const S6000OutVersionInfo2_s variable = {S6000OUTVERSIONINFO2_HDR,			\
													name, SCID, Caps,
	#define DEFINE_S6000OUTVERSION2_END				};


	#define DECLARE_S6000LIBVERSION2(variable)		extern const S6000LibVersionInfo2_s variable

	#define DEFINE_S6000LIBVERSION2(variable, name, Caps)																		\
													const S6000LibVersionInfo2_s variable = {S6000LIBVERSIONINFO2_HDR,			\
													name, Caps,
	#define DEFINE_S6000VERSION2_END				};


	#define DECLARE_S6000OUTVERSION3(variable)		extern const S6000OutVersionInfo3_s variable
	#define DEFINE_S6000OUTVERSION3(variable, name, Caps, SCID)		const S6000OutVersionInfo3_s variable =						\
													{ S6000OUTVERSIONINFO3_HDR, name, Caps, SCID,
	#define DEFINE_S6000OUTVERSION3_END				};

	#define DECLARE_S6000LIBVERSION3(variable)		extern const S6000LibVersionInfo3_s variable
	#define DEFINE_S6000LIBVERSION3(variable, name, Caps)		const S6000LibVersionInfo3_s variable =							\
													{ S6000LIBVERSIONINFO3_HDR, name, Caps,
	#define DEFINE_S6000LIBVERSION3_END				};

	#define DEFINE_VERSION_END						}

#endif

struct S5500VersionInfo
{
#ifdef __TMS320C55X__
	char	szName[32];

	WORD	nMajor			: 4;
	WORD	nMinor			: 7;
	WORD	nType			: 5;
	WORD	nBuild			: 13;
	WORD	__dumb__		: 3;
	union
	{
		DWORD	dw;
		struct
		{
			WORD	nYear	: 4;	// с 2005 года
			WORD	nMonth	: 4;
			WORD	nDay	: 5;
			WORD	__dumb1	: 3;
			WORD	nHour   : 5;
			WORD	nMinute	: 7;
			WORD	__dumb2	: 4;
		} bf;
	}	nDT;
#else
	char	szName[32*2];

	// ---
	WORD	__dumb__		: 3;
	WORD	nBuild			: 13;
	// ---
	WORD	nType			: 5;
	WORD	nMinor			: 7;
	WORD	nMajor			: 4;
	union
	{
		DWORD	dw;
		struct
		{
			WORD	__dumb2	: 4;
			WORD	nMinute	: 7;
			WORD	nHour   : 5;
			// ---
			WORD	__dumb1	: 3;
			WORD	nDay	: 5;
			WORD	nMonth	: 4;
			WORD	nYear	: 4;	// с 2005 года
		} bf;
	}	nDT;
#endif
};

/// ИнформациЯ о версии Out-файла длЯ семейства TMS320C2800.
/// wStructVersion == 0x02, wVersionType == 0x03.
#ifdef __cplusplus
	struct S2800OutVersionInfo2 : public STMSVersionParent
	{
		#ifdef __TMS320C28XX__
			char	szName[44];
		#else
			char	szName[44*2];
		#endif

		DWORD	dwClassID;			/// ClassID of this Software, defined in DSPLABclassID.h
		DWORD	dwCaps;				/// Firmware capabilities flags
		
		WORD	nMajor			: 4;
		WORD	nMinor			: 7;
		WORD	nType			: 5;

		WORD	nBuild			: 13;
		WORD	nType2			: 3;
			
		DWORD	nTimestamp;			/// Кол-во минут с 10.04.2008 04:00 UTC
	};
#endif

typedef struct S2800OutVersionInfo2_s
{
	STMSVersionParent	hdr;

	#ifdef __TMS320C28XX__
		char	szName[44];
	#else
		char	szName[44*2];
	#endif

	DWORD	dwClassID;			/// ClassID of this Software, defined in DSPLABclassID.h
	DWORD	dwCaps;				/// Firmware capabilities flags
	
	WORD	nMajor			: 4;
	WORD	nMinor			: 7;
	WORD	nType			: 5;

	WORD	nBuild			: 13;
	WORD	nType2			: 3;
		
	DWORD	nTimestamp;			/// Кол-во минут с 10.04.2008 04:00 UTC
} S2800OutVersionInfo2_s;

#if defined(__TMS320C28XX__)
	#define DECLARE_S2800OUTVERSION2(variable)		extern const S2800OutVersionInfo2_s variable

	#define DEFINE_S2800OUTVERSION2(variable, name, Caps, SCID)																\
													const S2800OutVersionInfo2_s variable = {S2800OUTVERSIONINFO2_HDR,		\
													name, SCID, Caps,
	#define DEFINE_S2800OUTVERSION2_END				};
#endif

// ----------------------------------------------------------------
// ------------ PRODUCT INFO --------------------------------------
// ----------------------------------------------------------------

#ifndef __STRUCT_DEVICEINFO__
#define __STRUCT_DEVICEINFO__
	/// База длЯ структур с информацией об устройстве
	struct SDeviceInfoGen
	{
		WORD	wDIStructVersion;		///< ВерсиЯ структуры с информацией об устройстве
		WORD	wSizeof;				///< Размер структуры в байтах
	};

	#ifdef __cplusplus
		/// Структура с информацией об устройстве DSPLAB.
		/// wDIStructVersion == 1
		struct SDeviceInfo : public SDeviceInfoGen
		{
			char	achCorp[8];				///< Название организации (неиспользуемые символы == 0)
			char	achLine[10];			///< Название темы (неиспользуемые символы == 0) 
			char	achDevice[10];			///< Название устройства (неиспользуемые символы == 0)
			DWORD	dwSerialNum;			///< Серийный номер изделиЯ
			DWORD	dwTimestamp;			
		};
	#endif // __cplusplus
#endif
 
// ----------------------------------------------------------------
// ------------- DEBUGGING ----------------------------------------
// ----------------------------------------------------------------

#ifndef WIN32
	#if defined(_DEBUG) && defined(_LOGAUX)
		#define DTRACE(x,y,z) 	LOG_printf(&LOG_aux, (x), (y), (z))
	#else
		#ifdef _cplusplus
			static inline void DTRACE(const char* /* _format */,...)
			{
			}
		#endif
	#endif
#endif

// ----------------------------------------------------------------
// ------------- PIPE ALLOC/FREE BUG FIX --------------------------
// ----------------------------------------------------------------

#if defined(__TMS320C55X__)
    #define PIP_undoAlloc(pipe)			(pipe)->writerSock.numFrames++;
#elif defined(_TMS320C6X)
	#define PIP_undoAlloc(pipe)			(pipe)->writerNumFrames++;
#endif

	
// ----------------------------------------------------------------
// -------------------- E P I L O G -------------------------------
// ----------------------------------------------------------------

#endif
