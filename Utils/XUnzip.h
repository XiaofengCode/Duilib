#pragma once

#define zmalloc(len) malloc(len)

#define zfree(p) free(p)

typedef HANDLE HZIP;	// An HZIP identifies a zip file that has been opened

typedef DWORD ZRESULT;
// return codes from any of the zip functions. Listed later.

#define ZIP_HANDLE   1
#define ZIP_FILENAME 2
#define ZIP_MEMORY   3

typedef struct
{ int index;                 // index of this file within the zip
char name[MAX_PATH];       // filename within the zip
DWORD attr;                // attributes, as in GetFileAttributes.
FILETIME atime,ctime,mtime;// access, create, modify filetimes
long comp_size;            // sizes of item, compressed and uncompressed. These
long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
} ZIPENTRYA;

typedef struct
{ int index;                 // index of this file within the zip
TCHAR name[MAX_PATH];      // filename within the zip
DWORD attr;                // attributes, as in GetFileAttributes.
FILETIME atime,ctime,mtime;// access, create, modify filetimes
long comp_size;            // sizes of item, compressed and uncompressed. These
long unc_size;             // may be -1 if not yet known (e.g. being streamed in)
} ZIPENTRYW;

// These are the result codes:
#define ZR_OK         0x00000000     // nb. the pseudo-code zr-recent is never returned,
#define ZR_RECENT     0x00000001     // but can be passed to FormatZipMessage.
// The following come from general system stuff (e.g. files not openable)
#define ZR_GENMASK    0x0000FF00
#define ZR_NODUPH     0x00000100     // couldn't duplicate the handle
#define ZR_NOFILE     0x00000200     // couldn't create/open the file
#define ZR_NOALLOC    0x00000300     // failed to allocate some resource
#define ZR_WRITE      0x00000400     // a general error writing to the file
#define ZR_NOTFOUND   0x00000500     // couldn't find that file in the zip
#define ZR_MORE       0x00000600     // there's still more data to be unzipped
#define ZR_CORRUPT    0x00000700     // the zipfile is corrupt or not a zipfile
#define ZR_READ       0x00000800     // a general error reading the file
// The following come from mistakes on the part of the caller
#define ZR_CALLERMASK 0x00FF0000
#define ZR_ARGS       0x00010000     // general mistake with the arguments
#define ZR_NOTMMAP    0x00020000     // tried to ZipGetMemory, but that only works on mmap zipfiles, which yours wasn't
#define ZR_MEMSIZE    0x00030000     // the memory size is too small
#define ZR_FAILED     0x00040000     // the thing was already failed when you called this function
#define ZR_ENDED      0x00050000     // the zip creation has already been closed
#define ZR_MISSIZE    0x00060000     // the indicated input file size turned out mistaken
#define ZR_PARTIALUNZ 0x00070000     // the file had already been partially unzipped
#define ZR_ZMODE      0x00080000     // tried to mix creating/opening a zip 
// The following come from bugs within the zip library itself
#define ZR_BUGMASK    0xFF000000
#define ZR_NOTINITED  0x01000000     // initialisation didn't work
#define ZR_SEEK       0x02000000     // trying to seek in an unseekable file
#define ZR_NOCHANGE   0x04000000     // changed its mind on storage, but not allowed
#define ZR_FLATE      0x05000000     // an internal error in the de/inflation code

#pragma warning(disable : 4702)   // unreachable code

static ZRESULT zopenerror = ZR_OK; //+++1.2

typedef struct tm_unz_s
{ unsigned int tm_sec;            // seconds after the minute - [0,59]
unsigned int tm_min;            // minutes after the hour - [0,59]
unsigned int tm_hour;           // hours since midnight - [0,23]
unsigned int tm_mday;           // day of the month - [1,31]
unsigned int tm_mon;            // months since January - [0,11]
unsigned int tm_year;           // years - [1980..2044]
} tm_unz;


// unz_global_info structure contain global data about the ZIPfile
typedef struct unz_global_info_s
{ unsigned long number_entry;         // total number of entries in the central dir on this disk
unsigned long size_comment;         // size of the global comment of the zipfile
} unz_global_info;

// unz_file_info contain information about a file in the zipfile
typedef struct unz_file_info_s
{ unsigned long version;              // version made by                 2 bytes
unsigned long version_needed;       // version needed to extract       2 bytes
unsigned long flag;                 // general purpose bit flag        2 bytes
unsigned long compression_method;   // compression method              2 bytes
unsigned long dosDate;              // last mod file date in Dos fmt   4 bytes
unsigned long crc;                  // crc-32                          4 bytes
unsigned long compressed_size;      // compressed size                 4 bytes
unsigned long uncompressed_size;    // uncompressed size               4 bytes
unsigned long size_filename;        // filename length                 2 bytes
unsigned long size_file_extra;      // extra field length              2 bytes
unsigned long size_file_comment;    // file comment length             2 bytes
unsigned long disk_num_start;       // disk number start               2 bytes
unsigned long internal_fa;          // internal file attributes        2 bytes
unsigned long external_fa;          // external file attributes        4 bytes
tm_unz tmu_date;
} unz_file_info;


#define UNZ_OK                  (0)
#define UNZ_END_OF_LIST_OF_FILE (-100)
#define UNZ_ERRNO               (Z_ERRNO)
#define UNZ_EOF                 (0)
#define UNZ_PARAMERROR          (-102)
#define UNZ_BADZIPFILE          (-103)
#define UNZ_INTERNALERROR       (-104)
#define UNZ_CRCERROR            (-105)







#define ZLIB_VERSION "1.1.3"


// Allowed flush values; see deflate() for details
#define Z_NO_FLUSH      0
#define Z_SYNC_FLUSH    2
#define Z_FULL_FLUSH    3
#define Z_FINISH        4


// compression levels
#define Z_NO_COMPRESSION         0
#define Z_BEST_SPEED             1
#define Z_BEST_COMPRESSION       9
#define Z_DEFAULT_COMPRESSION  (-1)

// compression strategy; see deflateInit2() for details
#define Z_FILTERED            1
#define Z_HUFFMAN_ONLY        2
#define Z_DEFAULT_STRATEGY    0

// Possible values of the data_type field
#define Z_BINARY   0
#define Z_ASCII    1
#define Z_UNKNOWN  2

// The deflate compression method (the only one supported in this version)
#define Z_DEFLATED   8

// for initializing zalloc, zfree, opaque
#define Z_NULL  0

// case sensitivity when searching for filenames
#define CASE_SENSITIVE 1
#define CASE_INSENSITIVE 2


// Return codes for the compression/decompression functions. Negative
// values are errors, positive values are used for special but normal events.
#define Z_OK            0
#define Z_STREAM_END    1
#define Z_NEED_DICT     2
#define Z_ERRNO        (-1)
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR   (-3)
#define Z_MEM_ERROR    (-4)
#define Z_BUF_ERROR    (-5)
#define Z_VERSION_ERROR (-6)



// Basic data types
typedef unsigned char  Byte;  // 8 bits
typedef unsigned int   uInt;  // 16 bits or more
typedef unsigned long  uLong; // 32 bits or more
typedef void *voidpf;
typedef void     *voidp;
typedef long z_off_t;












typedef voidpf (*alloc_func) (voidpf opaque, uInt items, uInt size);
typedef void   (*free_func)  (voidpf opaque, voidpf address);

struct internal_state;

typedef struct z_stream_s {
	Byte    *next_in;  // next input byte
	uInt     avail_in;  // number of bytes available at next_in
	uLong    total_in;  // total nb of input bytes read so far

	Byte    *next_out; // next output byte should be put there
	uInt     avail_out; // remaining free space at next_out
	uLong    total_out; // total nb of bytes output so far

	char     *msg;      // last error message, NULL if no error
	struct internal_state *state; // not visible by applications

	alloc_func zalloc;  // used to allocate the internal state
	free_func  zfree;   // used to free the internal state
	voidpf     opaque;  // private data object passed to zalloc and zfree

	int     data_type;  // best guess about the data type: ascii or binary
	uLong   adler;      // adler32 value of the uncompressed data
	uLong   reserved;   // reserved for future use
} z_stream;

typedef z_stream *z_streamp;


//   The application must update next_in and avail_in when avail_in has
//   dropped to zero. It must update next_out and avail_out when avail_out
//   has dropped to zero. The application must initialize zalloc, zfree and
//   opaque before calling the init function. All other fields are set by the
//   compression library and must not be updated by the application.
//
//   The opaque value provided by the application will be passed as the first
//   parameter for calls of zalloc and zfree. This can be useful for custom
//   memory management. The compression library attaches no meaning to the
//   opaque value.
//
//   zalloc must return Z_NULL if there is not enough memory for the object.
//   If zlib is used in a multi-threaded application, zalloc and zfree must be
//   thread safe.
//
//   The fields total_in and total_out can be used for statistics or
//   progress reports. After compression, total_in holds the total size of
//   the uncompressed data and may be saved for use in the decompressor
//   (particularly if the decompressor wants to decompress everything in
//   a single step).
//


// basic functions

const char *zlibVersion ();
// The application can compare zlibVersion and ZLIB_VERSION for consistency.
// If the first character differs, the library code actually used is
// not compatible with the zlib.h header file used by the application.
// This check is automatically made by inflateInit.






int inflate (z_streamp strm, int flush);
//
//    inflate decompresses as much data as possible, and stops when the input
//  buffer becomes empty or the output buffer becomes full. It may some
//  introduce some output latency (reading input without producing any output)
//  except when forced to flush.
//
//  The detailed semantics are as follows. inflate performs one or both of the
//  following actions:
//
//  - Decompress more input starting at next_in and update next_in and avail_in
//    accordingly. If not all input can be processed (because there is not
//    enough room in the output buffer), next_in is updated and processing
//    will resume at this point for the next call of inflate().
//
//  - Provide more output starting at next_out and update next_out and avail_out
//    accordingly.  inflate() provides as much output as possible, until there
//    is no more input data or no more space in the output buffer (see below
//    about the flush parameter).
//
//  Before the call of inflate(), the application should ensure that at least
//  one of the actions is possible, by providing more input and/or consuming
//  more output, and updating the next_* and avail_* values accordingly.
//  The application can consume the uncompressed output when it wants, for
//  example when the output buffer is full (avail_out == 0), or after each
//  call of inflate(). If inflate returns Z_OK and with zero avail_out, it
//  must be called again after making room in the output buffer because there
//  might be more output pending.
//
//    If the parameter flush is set to Z_SYNC_FLUSH, inflate flushes as much
//  output as possible to the output buffer. The flushing behavior of inflate is
//  not specified for values of the flush parameter other than Z_SYNC_FLUSH
//  and Z_FINISH, but the current implementation actually flushes as much output
//  as possible anyway.
//
//    inflate() should normally be called until it returns Z_STREAM_END or an
//  error. However if all decompression is to be performed in a single step
//  (a single call of inflate), the parameter flush should be set to
//  Z_FINISH. In this case all pending input is processed and all pending
//  output is flushed; avail_out must be large enough to hold all the
//  uncompressed data. (The size of the uncompressed data may have been saved
//  by the compressor for this purpose.) The next operation on this stream must
//  be inflateEnd to deallocate the decompression state. The use of Z_FINISH
//  is never required, but can be used to inform inflate that a faster routine
//  may be used for the single inflate() call.
//
//     If a preset dictionary is needed at this point (see inflateSetDictionary
//  below), inflate sets strm-adler to the adler32 checksum of the
//  dictionary chosen by the compressor and returns Z_NEED_DICT; otherwise
//  it sets strm->adler to the adler32 checksum of all output produced
//  so far (that is, total_out bytes) and returns Z_OK, Z_STREAM_END or
//  an error code as described below. At the end of the stream, inflate()
//  checks that its computed adler32 checksum is equal to that saved by the
//  compressor and returns Z_STREAM_END only if the checksum is correct.
//
//    inflate() returns Z_OK if some progress has been made (more input processed
//  or more output produced), Z_STREAM_END if the end of the compressed data has
//  been reached and all uncompressed output has been produced, Z_NEED_DICT if a
//  preset dictionary is needed at this point, Z_DATA_ERROR if the input data was
//  corrupted (input stream not conforming to the zlib format or incorrect
//  adler32 checksum), Z_STREAM_ERROR if the stream structure was inconsistent
//  (for example if next_in or next_out was NULL), Z_MEM_ERROR if there was not
//  enough memory, Z_BUF_ERROR if no progress is possible or if there was not
//  enough room in the output buffer when Z_FINISH is used. In the Z_DATA_ERROR
//  case, the application may then call inflateSync to look for a good
//  compression block.
//


int inflateEnd (z_streamp strm);
//
//     All dynamically allocated data structures for this stream are freed.
//   This function discards any unprocessed input and does not flush any
//   pending output.
//
//     inflateEnd returns Z_OK if success, Z_STREAM_ERROR if the stream state
//   was inconsistent. In the error case, msg may be set but then points to a
//   static string (which must not be deallocated).

// Advanced functions 

//  The following functions are needed only in some special applications.





int inflateSetDictionary (z_streamp strm,
	const Byte *dictionary,
	uInt  dictLength);
//
//     Initializes the decompression dictionary from the given uncompressed byte
//   sequence. This function must be called immediately after a call of inflate
//   if this call returned Z_NEED_DICT. The dictionary chosen by the compressor
//   can be determined from the Adler32 value returned by this call of
//   inflate. The compressor and decompressor must use exactly the same
//   dictionary. 
//
//     inflateSetDictionary returns Z_OK if success, Z_STREAM_ERROR if a
//   parameter is invalid (such as NULL dictionary) or the stream state is
//   inconsistent, Z_DATA_ERROR if the given dictionary doesn't match the
//   expected one (incorrect Adler32 value). inflateSetDictionary does not
//   perform any decompression: this will be done by subsequent calls of
//   inflate().


int inflateSync (z_streamp strm);
// 
//    Skips invalid compressed data until a full flush point can be found, or until all
//  available input is skipped. No output is provided.
//
//    inflateSync returns Z_OK if a full flush point has been found, Z_BUF_ERROR
//  if no more input was provided, Z_DATA_ERROR if no flush point has been found,
//  or Z_STREAM_ERROR if the stream structure was inconsistent. In the success
//  case, the application may save the current current value of total_in which
//  indicates where valid compressed data was found. In the error case, the
//  application may repeatedly call inflateSync, providing more input each time,
//  until success or end of the input data.


int inflateReset (z_streamp strm);
//     This function is equivalent to inflateEnd followed by inflateInit,
//   but does not free and reallocate all the internal decompression state.
//   The stream will keep attributes that may have been set by inflateInit2.
//
//      inflateReset returns Z_OK if success, or Z_STREAM_ERROR if the source
//   stream state was inconsistent (such as zalloc or state being NULL).
//



// checksum functions
// These functions are not related to compression but are exported
// anyway because they might be useful in applications using the
// compression library.

uLong adler32 (uLong adler, const Byte *buf, uInt len);
//     Update a running Adler-32 checksum with the bytes buf[0..len-1] and
//   return the updated checksum. If buf is NULL, this function returns
//   the required initial value for the checksum.
//   An Adler-32 checksum is almost as reliable as a CRC32 but can be computed
//   much faster. Usage example:
//
//     uLong adler = adler32(0L, Z_NULL, 0);
//
//     while (read_buffer(buffer, length) != EOF) {
//       adler = adler32(adler, buffer, length);
//     }
//     if (adler != original_adler) error();

uLong ucrc32   (uLong crc, const Byte *buf, uInt len);
//     Update a running crc with the bytes buf[0..len-1] and return the updated
//   crc. If buf is NULL, this function returns the required initial value
//   for the crc. Pre- and post-conditioning (one's complement) is performed
//   within this function so it shouldn't be done by the application.
//   Usage example:
//
//     uLong crc = crc32(0L, Z_NULL, 0);
//
//     while (read_buffer(buffer, length) != EOF) {
//       crc = crc32(crc, buffer, length);
//     }
//     if (crc != original_crc) error();




const char   *zError           (int err);
int           inflateSyncPoint (z_streamp z);
const uLong *get_crc_table    (void);



typedef unsigned char  uch;
typedef uch uchf;
typedef unsigned short ush;
typedef ush ushf;
typedef unsigned long  ulg;



const char * const z_errmsg[10] = { // indexed by 2-zlib_error
	"need dictionary",     // Z_NEED_DICT       2
	"stream end",          // Z_STREAM_END      1
	"",                    // Z_OK              0
	"file error",          // Z_ERRNO         (-1)
	"stream error",        // Z_STREAM_ERROR  (-2)
	"data error",          // Z_DATA_ERROR    (-3)
	"insufficient memory", // Z_MEM_ERROR     (-4)
	"buffer error",        // Z_BUF_ERROR     (-5)
	"incompatible version",// Z_VERSION_ERROR (-6)
	""};


#define ERR_MSG(err) z_errmsg[Z_NEED_DICT-(err)]

#define ERR_RETURN(strm,err) \
	return (strm->msg = (char*)ERR_MSG(err), (err))
	// To be used only when the state is known to be valid 

	// common constants


#define STORED_BLOCK 0
#define STATIC_TREES 1
#define DYN_TREES    2
	// The three kinds of block type 

#define MIN_MATCH  3
#define MAX_MATCH  258
	// The minimum and maximum match lengths 

#define PRESET_DICT 0x20 // preset dictionary flag in zlib header 

	// target dependencies 

#define OS_CODE  0x0b  // Window 95 & Windows NT



	// functions 

#define zmemzero(dest, len) memset(dest, 0, len)

	// Diagnostic functions
#undef Assert
#undef Trace
#undef Tracev
#undef Tracevv
#undef Tracec
#undef Tracecv

#ifdef DEBUG

	int z_verbose = 0;
	void z_error (char *m) {fprintf(stderr, "%s\n", m); exit(1);}

#define Assert(cond,msg) {if(!(cond)) z_error(msg);}
#define Trace(x) {if (z_verbose>=0) fprintf x ;}
#define Tracev(x) {if (z_verbose>0) fprintf x ;}
#define Tracevv(x) {if (z_verbose>1) fprintf x ;}
#define Tracec(c,x) {if (z_verbose>0 && (c)) fprintf x ;}
#define Tracecv(c,x) {if (z_verbose>1 && (c)) fprintf x ;}

#else

#ifndef __noop
#if _MSC_VER < 1300
#define __noop ((void)0)
#endif
#endif

#define Assert(cond,msg)	__noop
#define Trace(x)			__noop
#define Tracev(x)			__noop
#define Tracevv(x)			__noop
#define Tracec(c,x)			__noop
#define Tracecv(c,x)		__noop

#endif


	typedef uLong (*check_func) (uLong check, const Byte *buf, uInt len);
	voidpf zcalloc (voidpf opaque, unsigned items, unsigned size);
	void   zcfree  (voidpf opaque, voidpf ptr);

#define ZALLOC(strm, items, size) \
	(*((strm)->zalloc))((strm)->opaque, (items), (size))
#define ZFREE(strm, addr)  (*((strm)->zfree))((strm)->opaque, (voidpf)(addr))

	//void ZFREE(z_streamp strm,voidpf addr)
	//{ *((strm)->zfree))((strm)->opaque, addr);
	//}

#define TRY_FREE(s, p) {if (p) ZFREE(s, p);}




	// Huffman code lookup table entry--this entry is four bytes for machines
	// that have 16-bit pointers (e.g. PC's in the small or medium model).


	typedef struct inflate_huft_s inflate_huft;

	struct inflate_huft_s {
		union {
			struct {
				Byte Exop;        // number of extra bits or operation
				Byte Bits;        // number of bits in this code or subcode
			} what;
			uInt pad;           // pad structure to a power of 2 (4 bytes for
		} word;               //  16-bit, 8 bytes for 32-bit int's)
		uInt base;            // literal, length base, distance base, or table offset
	};

	// Maximum size of dynamic tree.  The maximum found in a long but non-
	//   exhaustive search was 1004 huft structures (850 for length/literals
	//   and 154 for distances, the latter actually the result of an
	//   exhaustive search).  The actual maximum is not known, but the
	//   value below is more than safe.
#define MANY 1440

	int inflate_trees_bits (
		uInt *,                    // 19 code lengths
		uInt *,                    // bits tree desired/actual depth
		inflate_huft * *,       // bits tree result
		inflate_huft *,             // space for trees
		z_streamp);                // for messages

	int inflate_trees_dynamic (
		uInt,                       // number of literal/length codes
		uInt,                       // number of distance codes
		uInt *,                    // that many (total) code lengths
		uInt *,                    // literal desired/actual bit depth
		uInt *,                    // distance desired/actual bit depth
		inflate_huft * *,       // literal/length tree result
		inflate_huft * *,       // distance tree result
		inflate_huft *,             // space for trees
		z_streamp);                // for messages

	int inflate_trees_fixed (
		uInt *,                    // literal desired/actual bit depth
		uInt *,                    // distance desired/actual bit depth
		const inflate_huft * *,       // literal/length tree result
		const inflate_huft * *,       // distance tree result
		z_streamp);                // for memory allocation





	struct inflate_blocks_state;
	typedef struct inflate_blocks_state inflate_blocks_statef;

	inflate_blocks_statef * inflate_blocks_new (
		z_streamp z,
		check_func c,               // check function
		uInt w);                   // window size

	int inflate_blocks (
		inflate_blocks_statef *,
		z_streamp ,
		int);                      // initial return code

	void inflate_blocks_reset (
		inflate_blocks_statef *,
		z_streamp ,
		uLong *);                  // check value on output

	int inflate_blocks_free (
		inflate_blocks_statef *,
		z_streamp);

	void inflate_set_dictionary (
		inflate_blocks_statef *s,
		const Byte *d,  // dictionary
		uInt  n);       // dictionary length

	int inflate_blocks_sync_point (
		inflate_blocks_statef *s);




	struct inflate_codes_state;
	typedef struct inflate_codes_state inflate_codes_statef;

	inflate_codes_statef *inflate_codes_new (
		uInt, uInt,
		const inflate_huft *, const inflate_huft *,
		z_streamp );

	int inflate_codes (
		inflate_blocks_statef *,
		z_streamp ,
		int);

	void inflate_codes_free (
		inflate_codes_statef *,
		z_streamp );




	typedef enum {
		IBM_TYPE,     // get type bits (3, including end bit)
		IBM_LENS,     // get lengths for stored
		IBM_STORED,   // processing stored block
		IBM_TABLE,    // get table lengths
		IBM_BTREE,    // get bit lengths tree for a dynamic block
		IBM_DTREE,    // get length, distance trees for a dynamic block
		IBM_CODES,    // processing fixed or dynamic block
		IBM_DRY,      // output remaining window bytes
		IBM_DONE,     // finished last block, done 
		IBM_BAD}      // got a data error--stuck here 
	inflate_block_mode;

	// inflate blocks semi-private state 
	struct inflate_blocks_state {

		// mode 
		inflate_block_mode  mode;     // current inflate_block mode 

		// mode dependent information 
		union {
			uInt left;          // if STORED, bytes left to copy 
			struct {
				uInt table;               // table lengths (14 bits) 
				uInt index;               // index into blens (or border)
				uInt *blens;             // bit lengths of codes
				uInt bb;                  // bit length tree depth 
				inflate_huft *tb;         // bit length decoding tree 
			} trees;            // if DTREE, decoding info for trees 
			struct {
				inflate_codes_statef 
					*codes;
			} decode;           // if CODES, current state 
		} sub;                // submode
		uInt last;            // true if this block is the last block 

		// mode independent information 
		uInt bitk;            // bits in bit buffer 
		uLong bitb;           // bit buffer 
		inflate_huft *hufts;  // single malloc for tree space 
		Byte *window;        // sliding window 
		Byte *end;           // one byte after sliding window 
		Byte *read;          // window read pointer 
		Byte *write;         // window write pointer 
		check_func checkfn;   // check function 
		uLong check;          // check on output 

	};


	// defines for inflate input/output
	//   update pointers and return 
#define UPDBITS {s->bitb=b;s->bitk=k;}
#define UPDIN {z->avail_in=n;z->total_in+=(uLong)(p-z->next_in);z->next_in=p;}
#define UPDOUT {s->write=q;}
#define UPDATE {UPDBITS UPDIN UPDOUT}
#define LEAVE {UPDATE return inflate_flush(s,z,r);}
	//   get bytes and bits 
#define LOADIN {p=z->next_in;n=z->avail_in;b=s->bitb;k=s->bitk;}
#define NEEDBYTE {if(n)r=Z_OK;else LEAVE}
#define NEXTBYTE (n--,*p++)
#define NEEDBITS(j) {while(k<(j)){NEEDBYTE;b|=((uLong)NEXTBYTE)<<k;k+=8;}}
#define DUMPBITS(j) {b>>=(j);k-=(j);}
	//   output bytes 
#define WAVAIL (uInt)(q<s->read?s->read-q-1:s->end-q)
#define LOADOUT {q=s->write;m=(uInt)WAVAIL;m;}
#define WRAP {if(q==s->end&&s->read!=s->window){q=s->window;m=(uInt)WAVAIL;}}
#define FLUSH {UPDOUT r=inflate_flush(s,z,r); LOADOUT}
#define NEEDOUT {if(m==0){WRAP if(m==0){FLUSH WRAP if(m==0) LEAVE}}r=Z_OK;}
#define OUTBYTE(a) {*q++=(Byte)(a);m--;}
	//   load local pointers 
#define LOAD {LOADIN LOADOUT}

#define OpenZip OpenZipU
#define CloseZip(hz) CloseZipU(hz)
extern HZIP OpenZipU(void *z,unsigned int len,DWORD flags);
extern ZRESULT CloseZipU(HZIP hz);
#ifdef _UNICODE
#define ZIPENTRY ZIPENTRYW
#define GetZipItem GetZipItemW
#define FindZipItem FindZipItemW
#else
#define GetZipItem GetZipItemA
#define FindZipItem FindZipItemA
#endif
extern ZRESULT GetZipItemA(HZIP hz, int index, ZIPENTRYA *ze);
extern ZRESULT GetZipItemW(HZIP hz, int index, ZIPENTRYW *ze);
extern ZRESULT FindZipItemA(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRYA *ze);
extern ZRESULT FindZipItemW(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRYW *ze);
extern ZRESULT UnzipItem(HZIP hz, int index, void *dst, unsigned int len, DWORD flags);