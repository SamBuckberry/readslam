#include "bzlib.h"
/*-- General stuff. --*/
extern void BZ2_bz__AssertH__fail ( int errcode );
#define AssertH(cond,errcode) \
   { if (!(cond)) BZ2_bz__AssertH__fail ( errcode ); }

#if BZ_DEBUG
#define AssertD(cond,msg) \
   { if (!(cond)) {       \
      fprintf ( stderr,   \
        "\n\nlibbzip2(debug build): internal error\n\t%s\n", msg );\
      exit(1); \
   }}
#else
#define AssertD(cond,msg) /* */
#endif

#define VPrintf0(zf) \
   fprintf(stderr,zf)
#define VPrintf1(zf,za1) \
   fprintf(stderr,zf,za1)
#define VPrintf2(zf,za1,za2) \
   fprintf(stderr,zf,za1,za2)
#define VPrintf3(zf,za1,za2,za3) \
   fprintf(stderr,zf,za1,za2,za3)
#define VPrintf4(zf,za1,za2,za3,za4) \
   fprintf(stderr,zf,za1,za2,za3,za4)
#define VPrintf5(zf,za1,za2,za3,za4,za5) \
   fprintf(stderr,zf,za1,za2,za3,za4,za5)

#else

extern void bz_internal_error ( int errcode );
#define AssertH(cond,errcode) \
   { if (!(cond)) bz_internal_error ( errcode ); }
#define AssertD(cond,msg)                do { } while (0)
#define VPrintf0(zf)                     do { } while (0)
#define VPrintf1(zf,za1)                 do { } while (0)
#define VPrintf2(zf,za1,za2)             do { } while (0)
#define VPrintf3(zf,za1,za2,za3)         do { } while (0)
#define VPrintf4(zf,za1,za2,za3,za4)     do { } while (0)
#define VPrintf5(zf,za1,za2,za3,za4,za5) do { } while (0)

#endif


#define BZALLOC(nnn) (strm->bzalloc)(strm->opaque,(nnn),1)
#define BZFREE(ppp)  (strm->bzfree)(strm->opaque,(ppp))


/*-- Header bytes. --*/

#define BZ_HDR_B 0x42   /* 'B' */
#define BZ_HDR_Z 0x5a   /* 'Z' */
#define BZ_HDR_h 0x68   /* 'h' */
#define BZ_HDR_0 0x30   /* '0' */
  
/*-- Constants for the back end. --*/

#define BZ_MAX_ALPHA_SIZE 258
#define BZ_MAX_CODE_LEN    23

#define BZ_RUNA 0
#define BZ_RUNB 1

#define BZ_N_GROUPS 6
#define BZ_G_SIZE   50
#define BZ_N_ITERS  4

#define BZ_MAX_SELECTORS (2 + (900000 / BZ_G_SIZE))



/*-- Stuff for randomising repetitive blocks. --*/

extern int BZ2_rNums[512];

#define BZ_RAND_DECLS                          \
   int rNToGo;                               \
   int rTPos                                 \

#define BZ_RAND_INIT_MASK                      \
   s->rNToGo = 0;                              \
   s->rTPos  = 0                               \

#define BZ_RAND_MASK ((s->rNToGo == 1) ? 1 : 0)

#define BZ_RAND_UPD_MASK                       \
   if (s->rNToGo == 0) {                       \
      s->rNToGo = BZ2_rNums[s->rTPos];         \
      s->rTPos++;                              \
      if (s->rTPos == 512) s->rTPos = 0;       \
   }                                           \
   s->rNToGo--;



/*-- Stuff for doing CRCs. --*/

extern unsigned int BZ2_crc32Table[256];

#define BZ_INITIALISE_CRC(crcVar)              \
{                                              \
   crcVar = 0xffffffffL;                       \
}

#define BZ_FINALISE_CRC(crcVar)                \
{                                              \
   crcVar = ~(crcVar);                         \
}

#define BZ_UPDATE_CRC(crcVar,cha)              \
{                                              \
   crcVar = (crcVar << 8) ^                    \
            BZ2_crc32Table[(crcVar >> 24) ^    \
                           ((unsigned char)cha)];      \
}



/*-- States and modes for compression. --*/

#define BZ_M_IDLE      1
#define BZ_M_RUNNING   2
#define BZ_M_FLUSHING  3
#define BZ_M_FINISHING 4

#define BZ_S_OUTPUT    1
#define BZ_S_INPUT     2

#define BZ_N_RADIX 2
#define BZ_N_QSORT 12
#define BZ_N_SHELL 18
#define BZ_N_OVERSHOOT (BZ_N_RADIX + BZ_N_QSORT + BZ_N_SHELL + 2)




/*-- Structure holding all the compression-side stuff. --*/

typedef
   struct {
      /* pointer back to the struct bz_stream */
      bz_stream* strm;

      /* mode this stream is in, and whether inputting */
      /* or outputting data */
      int    mode;
      int    state;

      /* remembers avail_in when flush/finish requested */
      unsigned int   avail_in_expect;

      /* for doing the block sorting */
      unsigned int*  arr1;
      unsigned int*  arr2;
      unsigned int*  ftab;
      int    origPtr;

      /* aliases for arr1 and arr2 */
      unsigned int*  ptr;
      unsigned char*   block;
      unsigned short*  mtfv;
      unsigned char*   zbits;

      /* for deciding when to use the fallback sorting algorithm */
      int    workFactor;

      /* run-length-encoding of the input */
      unsigned int   state_in_ch;
      int    state_in_len;
      BZ_RAND_DECLS;

      /* input and output limits and current posns */
      int    nblock;
      int    nblockMAX;
      int    numZ;
      int    state_out_pos;

      /* map of bytes used in block */
      int    nInUse;
      bool     inUse[256];
      unsigned char    unseqToSeq[256];

      /* the buffer for bit stream creation */
      unsigned int   bsBuff;
      int    bsLive;

      /* block and combined CRCs */
      unsigned int   blockCRC;
      unsigned int   combinedCRC;

      /* misc administratium */
      int    verbosity;
      int    blockNo;
      int    blockSize100k;

      /* stuff for coding the MTF values */
      int    nMTF;
      int    mtfFreq    [BZ_MAX_ALPHA_SIZE];
      unsigned char    selector   [BZ_MAX_SELECTORS];
      unsigned char    selectorMtf[BZ_MAX_SELECTORS];

      unsigned char    len     [BZ_N_GROUPS][BZ_MAX_ALPHA_SIZE];
      int    code    [BZ_N_GROUPS][BZ_MAX_ALPHA_SIZE];
      int    rfreq   [BZ_N_GROUPS][BZ_MAX_ALPHA_SIZE];
      /* second dimension: only 3 needed; 4 makes index calculations faster */
      unsigned int   len_pack[BZ_MAX_ALPHA_SIZE][4];

   }
   EState;



/*-- externs for compression. --*/

extern void 
BZ2_blockSort ( EState* );

extern void 
BZ2_compressBlock ( EState*, bool );

extern void 
BZ2_bsInitWrite ( EState* );

extern void 
BZ2_hbAssignCodes ( int*, unsigned char*, int, int, int );

extern void 
BZ2_hbMakeCodeLengths ( unsigned char*, int*, int, int );



/*-- states for decompression. --*/

#define BZ_X_IDLE        1
#define BZ_X_OUTPUT      2

#define BZ_X_MAGIC_1     10
#define BZ_X_MAGIC_2     11
#define BZ_X_MAGIC_3     12
#define BZ_X_MAGIC_4     13
#define BZ_X_BLKHDR_1    14
#define BZ_X_BLKHDR_2    15
#define BZ_X_BLKHDR_3    16
#define BZ_X_BLKHDR_4    17
#define BZ_X_BLKHDR_5    18
#define BZ_X_BLKHDR_6    19
#define BZ_X_BCRC_1      20
#define BZ_X_BCRC_2      21
#define BZ_X_BCRC_3      22
#define BZ_X_BCRC_4      23
#define BZ_X_RANDBIT     24
#define BZ_X_ORIGPTR_1   25
#define BZ_X_ORIGPTR_2   26
#define BZ_X_ORIGPTR_3   27
#define BZ_X_MAPPING_1   28
#define BZ_X_MAPPING_2   29
#define BZ_X_SELECTOR_1  30
#define BZ_X_SELECTOR_2  31
#define BZ_X_SELECTOR_3  32
#define BZ_X_CODING_1    33
#define BZ_X_CODING_2    34
#define BZ_X_CODING_3    35
#define BZ_X_MTF_1       36
#define BZ_X_MTF_2       37
#define BZ_X_MTF_3       38
#define BZ_X_MTF_4       39
#define BZ_X_MTF_5       40
#define BZ_X_MTF_6       41
#define BZ_X_ENDHDR_2    42
#define BZ_X_ENDHDR_3    43
#define BZ_X_ENDHDR_4    44
#define BZ_X_ENDHDR_5    45
#define BZ_X_ENDHDR_6    46
#define BZ_X_CCRC_1      47
#define BZ_X_CCRC_2      48
#define BZ_X_CCRC_3      49
#define BZ_X_CCRC_4      50



/*-- Constants for the fast MTF decoder. --*/

#define MTFA_SIZE 4096
#define MTFL_SIZE 16



/*-- Structure holding all the decompression-side stuff. --*/

typedef
   struct {
      /* pointer back to the struct bz_stream */
      bz_stream* strm;

      /* state indicator for this stream */
      int    state;

      /* for doing the final run-length decoding */
      unsigned char    state_out_ch;
      int    state_out_len;
      bool     blockRandomised;
      BZ_RAND_DECLS;

      /* the buffer for bit stream reading */
      unsigned int   bsBuff;
      int    bsLive;

      /* misc administratium */
      int    blockSize100k;
      bool     smallDecompress;
      int    currBlockNo;
      int    verbosity;

      /* for undoing the Burrows-Wheeler transform */
      int    origPtr;
      unsigned int   tPos;
      int    k0;
      int    unzftab[256];
      int    nblock_used;
      int    cftab[257];
      int    cftabCopy[257];

      /* for undoing the Burrows-Wheeler transform (FAST) */
      unsigned int   *tt;

      /* for undoing the Burrows-Wheeler transform (SMALL) */
      unsigned short   *ll16;
      unsigned char    *ll4;

      /* stored and calculated CRCs */
      unsigned int   storedBlockCRC;
      unsigned int   storedCombinedCRC;
      unsigned int   calculatedBlockCRC;
      unsigned int   calculatedCombinedCRC;

      /* map of bytes used in block */
      int    nInUse;
      bool     inUse[256];
      bool     inUse16[16];
      unsigned char    seqToUnseq[256];

      /* for decoding the MTF values */
      unsigned char    mtfa   [MTFA_SIZE];
      int    mtfbase[256 / MTFL_SIZE];
      unsigned char    selector   [BZ_MAX_SELECTORS];
      unsigned char    selectorMtf[BZ_MAX_SELECTORS];
      unsigned char    len  [BZ_N_GROUPS][BZ_MAX_ALPHA_SIZE];

      int    limit  [BZ_N_GROUPS][BZ_MAX_ALPHA_SIZE];
      int    base   [BZ_N_GROUPS][BZ_MAX_ALPHA_SIZE];
      int    perm   [BZ_N_GROUPS][BZ_MAX_ALPHA_SIZE];
      int    minLens[BZ_N_GROUPS];

      /* save area for scalars in the main decompress code */
      int    save_i;
      int    save_j;
      int    save_t;
      int    save_alphaSize;
      int    save_nGroups;
      int    save_nSelectors;
      int    save_EOB;
      int    save_groupNo;
      int    save_groupPos;
      int    save_nextSym;
      int    save_nblockMAX;
      int    save_nblock;
      int    save_es;
      int    save_N;
      int    save_curr;
      int    save_zt;
      int    save_zn; 
      int    save_zvec;
      int    save_zj;
      int    save_gSel;
      int    save_gMinlen;
      int*   save_gLimit;
      int*   save_gBase;
      int*   save_gPerm;

   }
   DState;



/*-- Macros for decompression. --*/

#define BZ_GET_FAST(cccc)                     \
    /* c_tPos is unsigned, hence test < 0 is pointless. */ \
    if (s->tPos >= (unsigned int)100000 * (unsigned int)s->blockSize100k) return True; \
    s->tPos = s->tt[s->tPos];                 \
    cccc = (unsigned char)(s->tPos & 0xff);           \
    s->tPos >>= 8;

#define BZ_GET_FAST_C(cccc)                   \
    /* c_tPos is unsigned, hence test < 0 is pointless. */ \
    if (c_tPos >= (unsigned int)100000 * (unsigned int)ro_blockSize100k) return True; \
    c_tPos = c_tt[c_tPos];                    \
    cccc = (unsigned char)(c_tPos & 0xff);            \
    c_tPos >>= 8;

#define SET_LL4(i,n)                                          \
   { if (((i) & 0x1) == 0)                                    \
        s->ll4[(i) >> 1] = (s->ll4[(i) >> 1] & 0xf0) | (n); else    \
        s->ll4[(i) >> 1] = (s->ll4[(i) >> 1] & 0x0f) | ((n) << 4);  \
   }

#define GET_LL4(i)                             \
   ((((unsigned int)(s->ll4[(i) >> 1])) >> (((i) << 2) & 0x4)) & 0xF)

#define SET_LL(i,n)                          \
   { s->ll16[i] = (unsigned short)(n & 0x0000ffff);  \
     SET_LL4(i, n >> 16);                    \
   }

#define GET_LL(i) \
   (((unsigned int)s->ll16[i]) | (GET_LL4(i) << 16))

#define BZ_GET_SMALL(cccc)                            \
    /* c_tPos is unsigned, hence test < 0 is pointless. */ \
    if (s->tPos >= (unsigned int)100000 * (unsigned int)s->blockSize100k) return True; \
    cccc = BZ2_indexIntoF ( s->tPos, s->cftab );    \
    s->tPos = GET_LL(s->tPos);


/*-- externs for decompression. --*/

extern int 
BZ2_indexIntoF ( int, int* );

extern int 
BZ2_decompress ( DState* );

extern void 
BZ2_hbCreateDecodeTables ( int*, int*, int*, unsigned char*,
                           int,  int, int );


#endif


/*-- BZ_NO_STDIO seems to make NULL disappear on some platforms. --*/

#ifdef BZ_NO_STDIO
#ifndef NULL
#define NULL 0
#endif
#endif


/*-------------------------------------------------------------*/
/*--- end                                   bzlib_private.h ---*/
/*-------------------------------------------------------------*/


//Fallback O(N log(N)^2) sorting for repetitive blocks
void crudesort(unsigned int* fmap, unsigned int* eclass, int x1, int x2)
{
	int i, j, tmp;
	unsigned int ec_tmp;

	if (x1 == x2) return;

	if (x2 - x1 > 3)
	{
		for (i=x2-4; i>=x1; i--)
		{
			tmp = fmap[i];
			ec_tmp = eclass[tmp];
			
			for (j=i+4; j<=x2 && ec_tmp>eclass[fmap[j]]; j+=4)
			{
				fmap[j-4] = fmap[j];
			}
			fmap[j-4] = tmp;
		}
	}
	for (int i=x2-1; i>=x1; i--)
	{
		tmp = fmap[i];
		ec_tmp = eclass[tmp];
		
		for (j=i+1; j<=x2 && ec_tmp>eclass[fmap[j]]; j++)
		{
			fmap[j-1] = fmap[j];
		}
		fmap[j-1] = tmp;
	}
}


/*---------------------------------------------*/
#define fswap(zz1, zz2) \
   { int zztmp = zz1; zz1 = zz2; zz2 = zztmp; }

#define fvswap(zzp1, zzp2, zzn)       \
{                                     \
   int yyp1 = (zzp1);               \
   int yyp2 = (zzp2);               \
   int yyn  = (zzn);                \
   while (yyn > 0) {                  \
      fswap(fmap[yyp1], fmap[yyp2]);  \
      yyp1++; yyp2++; yyn--;          \
   }                                  \
}


#define fmin(a,b) ((a) < (b)) ? (a) : (b)

#define fpush(lz,hz) { stackLo[sp] = lz; \
                       stackHi[sp] = hz; \
                       sp++; }

#define fpop(lz,hz) { sp--;              \
                      lz = stackLo[sp];  \
                      hz = stackHi[sp]; }

#define FALLBACK_QSORT_SMALL_THRESH 10
#define FALLBACK_QSORT_STACK_SIZE   100


static
void fallbackQSort3 ( unsigned int* fmap, 
                      unsigned int* eclass,
                      int   loSt, 
                      int   hiSt )
{
   int unLo, unHi, ltLo, gtHi, n, m;
   int sp, lo, hi;
   unsigned int med, r, r3;
   int stackLo[FALLBACK_QSORT_STACK_SIZE];
   int stackHi[FALLBACK_QSORT_STACK_SIZE];

   r = 0;

   sp = 0;
   fpush ( loSt, hiSt );

   while (sp > 0) {

      AssertH ( sp < FALLBACK_QSORT_STACK_SIZE - 1, 1004 );

      fpop ( lo, hi );
      if (hi - lo < FALLBACK_QSORT_SMALL_THRESH) {
         crudesort ( fmap, eclass, lo, hi );
         continue;
      }

      /* Random partitioning.  Median of 3 sometimes fails to
         avoid bad cases.  Median of 9 seems to help but 
         looks rather expensive.  This too seems to work but
         is cheaper.  Guidance for the magic constants 
         7621 and 32768 is taken from Sedgewick's algorithms
         book, chapter 35.
      */
      r = ((r * 7621) + 1) % 32768;
      r3 = r % 3;
      if (r3 == 0) med = eclass[fmap[lo]]; else
      if (r3 == 1) med = eclass[fmap[(lo+hi)>>1]]; else
                   med = eclass[fmap[hi]];

      unLo = ltLo = lo;
      unHi = gtHi = hi;

      while (1) {
         while (1) {
            if (unLo > unHi) break;
            n = (int)eclass[fmap[unLo]] - (int)med;
            if (n == 0) { 
               fswap(fmap[unLo], fmap[ltLo]); 
               ltLo++; unLo++; 
               continue; 
            };
            if (n > 0) break;
            unLo++;
         }
         while (1) {
            if (unLo > unHi) break;
            n = (int)eclass[fmap[unHi]] - (int)med;
            if (n == 0) { 
               fswap(fmap[unHi], fmap[gtHi]); 
               gtHi--; unHi--; 
               continue; 
            };
            if (n < 0) break;
            unHi--;
         }
         if (unLo > unHi) break;
         fswap(fmap[unLo], fmap[unHi]); unLo++; unHi--;
      }

      AssertD ( unHi == unLo-1, "fallbackQSort3(2)" );

      if (gtHi < ltLo) continue;

      n = fmin(ltLo-lo, unLo-ltLo); fvswap(lo, unLo-n, n);
      m = fmin(hi-gtHi, gtHi-unHi); fvswap(unLo, hi-m+1, m);

      n = lo + unLo - ltLo - 1;
      m = hi - (gtHi - unHi) + 1;

      if (n - lo > hi - m) {
         fpush ( lo, n );
         fpush ( m, hi );
      } else {
         fpush ( m, hi );
         fpush ( lo, n );
      }
   }
}

#undef fmin
#undef fpush
#undef fpop
#undef fswap
#undef fvswap
#undef FALLBACK_QSORT_SMALL_THRESH
#undef FALLBACK_QSORT_STACK_SIZE


/*---------------------------------------------*/
/* Pre:
      nblock > 0
      eclass exists for [0 .. nblock-1]
      ((unsigned char*)eclass) [0 .. nblock-1] holds block
      ptr exists for [0 .. nblock-1]

   Post:
      ((unsigned char*)eclass) [0 .. nblock-1] holds block
      All other areas of eclass destroyed
      fmap [0 .. nblock-1] holds sorted order
      bhtab [ 0 .. 2+(nblock/32) ] destroyed
*/

#define       SET_BH(zz)  bhtab[(zz) >> 5] |= (1 << ((zz) & 31))
#define     CLEAR_BH(zz)  bhtab[(zz) >> 5] &= ~(1 << ((zz) & 31))
#define     ISSET_BH(zz)  (bhtab[(zz) >> 5] & (1 << ((zz) & 31)))
#define      WORD_BH(zz)  bhtab[(zz) >> 5]
#define UNALIGNED_BH(zz)  ((zz) & 0x01f)

static
void fallbackSort ( unsigned int* fmap, 
                    unsigned int* eclass, 
                    unsigned int* bhtab,
                    int   nblock,
                    int   verb )
{
   int ftab[257];
   int ftabCopy[256];
   int H, i, j, k, l, r, cc, cc1;
   int nNotDone;
   int nBhtab;
   unsigned char* eclass8 = (unsigned char*)eclass;

   /*--
      Initial 1-char radix sort to generate
      initial fmap and initial BH bits.
   --*/
   if (verb >= 4)
      VPrintf0 ( "        bucket sorting ...\n" );
   for (i = 0; i < 257;    i++) ftab[i] = 0;
   for (i = 0; i < nblock; i++) ftab[eclass8[i]]++;
   for (i = 0; i < 256;    i++) ftabCopy[i] = ftab[i];
   for (i = 1; i < 257;    i++) ftab[i] += ftab[i-1];

   for (i = 0; i < nblock; i++) {
      j = eclass8[i];
      k = ftab[j] - 1;
      ftab[j] = k;
      fmap[k] = i;
   }

   nBhtab = 2 + (nblock / 32);
   for (i = 0; i < nBhtab; i++) bhtab[i] = 0;
   for (i = 0; i < 256; i++) SET_BH(ftab[i]);

   /*--
      Inductively refine the buckets.  Kind-of an
      "exponential radix sort" (!), inspired by the
      Manber-Myers suffix array construction algorithm.
   --*/

   /*-- set sentinel bits for block-end detection --*/
   for (i = 0; i < 32; i++) { 
      SET_BH(nblock + 2*i);
      CLEAR_BH(nblock + 2*i + 1);
   }

   /*-- the log(N) loop --*/
   H = 1;
   while (1) {

      if (verb >= 4) 
         VPrintf1 ( "        depth %6d has ", H );

      j = 0;
      for (i = 0; i < nblock; i++) {
         if (ISSET_BH(i)) j = i;
         k = fmap[i] - H; if (k < 0) k += nblock;
         eclass[k] = j;
      }

      nNotDone = 0;
      r = -1;
      while (1) {

	 /*-- find the next non-singleton bucket --*/
         k = r + 1;
         while (ISSET_BH(k) && UNALIGNED_BH(k)) k++;
         if (ISSET_BH(k)) {
            while (WORD_BH(k) == 0xffffffff) k += 32;
            while (ISSET_BH(k)) k++;
         }
         l = k - 1;
         if (l >= nblock) break;
         while (!ISSET_BH(k) && UNALIGNED_BH(k)) k++;
         if (!ISSET_BH(k)) {
            while (WORD_BH(k) == 0x00000000) k += 32;
            while (!ISSET_BH(k)) k++;
         }
         r = k - 1;
         if (r >= nblock) break;

         /*-- now [l, r] bracket current bucket --*/
         if (r > l) {
            nNotDone += (r - l + 1);
            fallbackQSort3 ( fmap, eclass, l, r );

            /*-- scan bucket and generate header bits-- */
            cc = -1;
            for (i = l; i <= r; i++) {
               cc1 = eclass[fmap[i]];
               if (cc != cc1) { SET_BH(i); cc = cc1; };
            }
         }
      }

      if (verb >= 4) 
         VPrintf1 ( "%6d unresolved strings\n", nNotDone );

      H *= 2;
      if (H > nblock || nNotDone == 0) break;
   }

   /*-- 
      Reconstruct the original block in
      eclass8 [0 .. nblock-1], since the
      previous phase destroyed it.
   --*/
   if (verb >= 4)
      VPrintf0 ( "        reconstructing block ...\n" );
   j = 0;
   for (i = 0; i < nblock; i++) {
      while (ftabCopy[j] == 0) j++;
      ftabCopy[j]--;
      eclass8[fmap[i]] = (unsigned char)j;
   }
   AssertH ( j < 256, 1005 );
}

#undef       SET_BH
#undef     CLEAR_BH
#undef     ISSET_BH
#undef      WORD_BH
#undef UNALIGNED_BH


/*---------------------------------------------*/
/*--- The main, O(N^2 log(N)) sorting       ---*/
/*--- algorithm.  Faster for "normal"       ---*/
/*--- non-repetitive blocks.                ---*/
/*---------------------------------------------*/

/*---------------------------------------------*/
static
__inline__
bool mainGtU ( unsigned int  i1, 
               unsigned int  i2,
               unsigned char*  block, 
               unsigned short* quadrant,
               unsigned int  nblock,
               int*  budget )
{
   int  k;
   unsigned char  c1, c2;
   unsigned short s1, s2;

   AssertD ( i1 != i2, "mainGtU" );
   /* 1 */
   c1 = block[i1]; c2 = block[i2];
   if (c1 != c2) return (c1 > c2);
   i1++; i2++;
   /* 2 */
   c1 = block[i1]; c2 = block[i2];
   if (c1 != c2) return (c1 > c2);
   i1++; i2++;
   /* 3 */
   c1 = block[i1]; c2 = block[i2];
   if (c1 != c2) return (c1 > c2);
   i1++; i2++;
   /* 4 */
   c1 = block[i1]; c2 = block[i2];
   if (c1 != c2) return (c1 > c2);
   i1++; i2++;
   /* 5 */
   c1 = block[i1]; c2 = block[i2];
   if (c1 != c2) return (c1 > c2);
   i1++; i2++;
   /* 6 */
   c1 = block[i1]; c2 = block[i2];
   if (c1 != c2) return (c1 > c2);
   i1++; i2++;
   /* 7 */
   c1 = block[i1]; c2 = block[i2];
   if (c1 != c2) return (c1 > c2);
   i1++; i2++;
   /* 8 */
   c1 = block[i1]; c2 = block[i2];
   if (c1 != c2) return (c1 > c2);
   i1++; i2++;
   /* 9 */
   c1 = block[i1]; c2 = block[i2];
   if (c1 != c2) return (c1 > c2);
   i1++; i2++;
   /* 10 */
   c1 = block[i1]; c2 = block[i2];
   if (c1 != c2) return (c1 > c2);
   i1++; i2++;
   /* 11 */
   c1 = block[i1]; c2 = block[i2];
   if (c1 != c2) return (c1 > c2);
   i1++; i2++;
   /* 12 */
   c1 = block[i1]; c2 = block[i2];
   if (c1 != c2) return (c1 > c2);
   i1++; i2++;

   k = nblock + 8;

   do {
      /* 1 */
      c1 = block[i1]; c2 = block[i2];
      if (c1 != c2) return (c1 > c2);
      s1 = quadrant[i1]; s2 = quadrant[i2];
      if (s1 != s2) return (s1 > s2);
      i1++; i2++;
      /* 2 */
      c1 = block[i1]; c2 = block[i2];
      if (c1 != c2) return (c1 > c2);
      s1 = quadrant[i1]; s2 = quadrant[i2];
      if (s1 != s2) return (s1 > s2);
      i1++; i2++;
      /* 3 */
      c1 = block[i1]; c2 = block[i2];
      if (c1 != c2) return (c1 > c2);
      s1 = quadrant[i1]; s2 = quadrant[i2];
      if (s1 != s2) return (s1 > s2);
      i1++; i2++;
      /* 4 */
      c1 = block[i1]; c2 = block[i2];
      if (c1 != c2) return (c1 > c2);
      s1 = quadrant[i1]; s2 = quadrant[i2];
      if (s1 != s2) return (s1 > s2);
      i1++; i2++;
      /* 5 */
      c1 = block[i1]; c2 = block[i2];
      if (c1 != c2) return (c1 > c2);
      s1 = quadrant[i1]; s2 = quadrant[i2];
      if (s1 != s2) return (s1 > s2);
      i1++; i2++;
      /* 6 */
      c1 = block[i1]; c2 = block[i2];
      if (c1 != c2) return (c1 > c2);
      s1 = quadrant[i1]; s2 = quadrant[i2];
      if (s1 != s2) return (s1 > s2);
      i1++; i2++;
      /* 7 */
      c1 = block[i1]; c2 = block[i2];
      if (c1 != c2) return (c1 > c2);
      s1 = quadrant[i1]; s2 = quadrant[i2];
      if (s1 != s2) return (s1 > s2);
      i1++; i2++;
      /* 8 */
      c1 = block[i1]; c2 = block[i2];
      if (c1 != c2) return (c1 > c2);
      s1 = quadrant[i1]; s2 = quadrant[i2];
      if (s1 != s2) return (s1 > s2);
      i1++; i2++;

      if (i1 >= nblock) i1 -= nblock;
      if (i2 >= nblock) i2 -= nblock;

      k -= 8;
      (*budget)--;
   }
      while (k >= 0);

   return False;
}


/*---------------------------------------------*/
/*--
   Knuth's increments seem to work better
   than Incerpi-Sedgewick here.  Possibly
   because the number of elems to sort is
   usually small, typically <= 20.
--*/
static
int incs[14] = { 1, 4, 13, 40, 121, 364, 1093, 3280,
                   9841, 29524, 88573, 265720,
                   797161, 2391484 };

static
void mainSimpleSort ( unsigned int* ptr,
                      unsigned char*  block,
                      unsigned short* quadrant,
                      int   nblock,
                      int   lo, 
                      int   hi, 
                      int   d,
                      int*  budget )
{
   int i, j, h, bigN, hp;
   unsigned int v;

   bigN = hi - lo + 1;
   if (bigN < 2) return;

   hp = 0;
   while (incs[hp] < bigN) hp++;
   hp--;

   for (; hp >= 0; hp--) {
      h = incs[hp];

      i = lo + h;
      while (True) {

         /*-- copy 1 --*/
         if (i > hi) break;
         v = ptr[i];
         j = i;
         while ( mainGtU ( 
                    ptr[j-h]+d, v+d, block, quadrant, nblock, budget 
                 ) ) {
            ptr[j] = ptr[j-h];
            j = j - h;
            if (j <= (lo + h - 1)) break;
         }
         ptr[j] = v;
         i++;

         /*-- copy 2 --*/
         if (i > hi) break;
         v = ptr[i];
         j = i;
         while ( mainGtU ( 
                    ptr[j-h]+d, v+d, block, quadrant, nblock, budget 
                 ) ) {
            ptr[j] = ptr[j-h];
            j = j - h;
            if (j <= (lo + h - 1)) break;
         }
         ptr[j] = v;
         i++;

         /*-- copy 3 --*/
         if (i > hi) break;
         v = ptr[i];
         j = i;
         while ( mainGtU ( 
                    ptr[j-h]+d, v+d, block, quadrant, nblock, budget 
                 ) ) {
            ptr[j] = ptr[j-h];
            j = j - h;
            if (j <= (lo + h - 1)) break;
         }
         ptr[j] = v;
         i++;

         if (*budget < 0) return;
      }
   }
}


/*---------------------------------------------*/
/*--
   The following is an implementation of
   an elegant 3-way quicksort for strings,
   described in a paper "Fast Algorithms for
   Sorting and Searching Strings", by Robert
   Sedgewick and Jon L. Bentley.
--*/

#define mswap(zz1, zz2) \
   { int zztmp = zz1; zz1 = zz2; zz2 = zztmp; }

#define mvswap(zzp1, zzp2, zzn)       \
{                                     \
   int yyp1 = (zzp1);               \
   int yyp2 = (zzp2);               \
   int yyn  = (zzn);                \
   while (yyn > 0) {                  \
      mswap(ptr[yyp1], ptr[yyp2]);    \
      yyp1++; yyp2++; yyn--;          \
   }                                  \
}

static 
__inline__
unsigned char mmed3 ( unsigned char a, unsigned char b, unsigned char c )
{
   unsigned char t;
   if (a > b) { t = a; a = b; b = t; };
   if (b > c) { 
      b = c;
      if (a > b) b = a;
   }
   return b;
}

#define mmin(a,b) ((a) < (b)) ? (a) : (b)

#define mpush(lz,hz,dz) { stackLo[sp] = lz; \
                          stackHi[sp] = hz; \
                          stackD [sp] = dz; \
                          sp++; }

#define mpop(lz,hz,dz) { sp--;             \
                         lz = stackLo[sp]; \
                         hz = stackHi[sp]; \
                         dz = stackD [sp]; }


#define mnextsize(az) (nextHi[az]-nextLo[az])

#define mnextswap(az,bz)                                        \
   { int tz;                                                  \
     tz = nextLo[az]; nextLo[az] = nextLo[bz]; nextLo[bz] = tz; \
     tz = nextHi[az]; nextHi[az] = nextHi[bz]; nextHi[bz] = tz; \
     tz = nextD [az]; nextD [az] = nextD [bz]; nextD [bz] = tz; }


#define MAIN_QSORT_SMALL_THRESH 20
#define MAIN_QSORT_DEPTH_THRESH (BZ_N_RADIX + BZ_N_QSORT)
#define MAIN_QSORT_STACK_SIZE 100

static
void mainQSort3 ( unsigned int* ptr,
                  unsigned char*  block,
                  unsigned short* quadrant,
                  int   nblock,
                  int   loSt, 
                  int   hiSt, 
                  int   dSt,
                  int*  budget )
{
   int unLo, unHi, ltLo, gtHi, n, m, med;
   int sp, lo, hi, d;

   int stackLo[MAIN_QSORT_STACK_SIZE];
   int stackHi[MAIN_QSORT_STACK_SIZE];
   int stackD [MAIN_QSORT_STACK_SIZE];

   int nextLo[3];
   int nextHi[3];
   int nextD [3];

   sp = 0;
   mpush ( loSt, hiSt, dSt );

   while (sp > 0) {

      AssertH ( sp < MAIN_QSORT_STACK_SIZE - 2, 1001 );

      mpop ( lo, hi, d );
      if (hi - lo < MAIN_QSORT_SMALL_THRESH || 
          d > MAIN_QSORT_DEPTH_THRESH) {
         mainSimpleSort ( ptr, block, quadrant, nblock, lo, hi, d, budget );
         if (*budget < 0) return;
         continue;
      }

      med = (int) 
            mmed3 ( block[ptr[ lo         ]+d],
                    block[ptr[ hi         ]+d],
                    block[ptr[ (lo+hi)>>1 ]+d] );

      unLo = ltLo = lo;
      unHi = gtHi = hi;

      while (True) {
         while (True) {
            if (unLo > unHi) break;
            n = ((int)block[ptr[unLo]+d]) - med;
            if (n == 0) { 
               mswap(ptr[unLo], ptr[ltLo]); 
               ltLo++; unLo++; continue; 
            };
            if (n >  0) break;
            unLo++;
         }
         while (True) {
            if (unLo > unHi) break;
            n = ((int)block[ptr[unHi]+d]) - med;
            if (n == 0) { 
               mswap(ptr[unHi], ptr[gtHi]); 
               gtHi--; unHi--; continue; 
            };
            if (n <  0) break;
            unHi--;
         }
         if (unLo > unHi) break;
         mswap(ptr[unLo], ptr[unHi]); unLo++; unHi--;
      }

      AssertD ( unHi == unLo-1, "mainQSort3(2)" );

      if (gtHi < ltLo) {
         mpush(lo, hi, d+1 );
         continue;
      }

      n = mmin(ltLo-lo, unLo-ltLo); mvswap(lo, unLo-n, n);
      m = mmin(hi-gtHi, gtHi-unHi); mvswap(unLo, hi-m+1, m);

      n = lo + unLo - ltLo - 1;
      m = hi - (gtHi - unHi) + 1;

      nextLo[0] = lo;  nextHi[0] = n;   nextD[0] = d;
      nextLo[1] = m;   nextHi[1] = hi;  nextD[1] = d;
      nextLo[2] = n+1; nextHi[2] = m-1; nextD[2] = d+1;

      if (mnextsize(0) < mnextsize(1)) mnextswap(0,1);
      if (mnextsize(1) < mnextsize(2)) mnextswap(1,2);
      if (mnextsize(0) < mnextsize(1)) mnextswap(0,1);

      AssertD (mnextsize(0) >= mnextsize(1), "mainQSort3(8)" );
      AssertD (mnextsize(1) >= mnextsize(2), "mainQSort3(9)" );

      mpush (nextLo[0], nextHi[0], nextD[0]);
      mpush (nextLo[1], nextHi[1], nextD[1]);
      mpush (nextLo[2], nextHi[2], nextD[2]);
   }
}

#undef mswap
#undef mvswap
#undef mpush
#undef mpop
#undef mmin
#undef mnextsize
#undef mnextswap
#undef MAIN_QSORT_SMALL_THRESH
#undef MAIN_QSORT_DEPTH_THRESH
#undef MAIN_QSORT_STACK_SIZE


/*---------------------------------------------*/
/* Pre:
      nblock > N_OVERSHOOT
      block32 exists for [0 .. nblock-1 +N_OVERSHOOT]
      ((unsigned char*)block32) [0 .. nblock-1] holds block
      ptr exists for [0 .. nblock-1]

   Post:
      ((unsigned char*)block32) [0 .. nblock-1] holds block
      All other areas of block32 destroyed
      ftab [0 .. 65536 ] destroyed
      ptr [0 .. nblock-1] holds sorted order
      if (*budget < 0), sorting was abandoned
*/

#define BIGFREQ(b) (ftab[((b)+1) << 8] - ftab[(b) << 8])
#define SETMASK (1 << 21)
#define CLEARMASK (~(SETMASK))

static
void mainSort ( unsigned int* ptr, 
                unsigned char*  block,
                unsigned short* quadrant, 
                unsigned int* ftab,
                int   nblock,
                int   verb,
                int*  budget )
{
   int  i, j, k, ss, sb;
   int  runningOrder[256];
   bool   bigDone[256];
   int  copyStart[256];
   int  copyEnd  [256];
   unsigned char  c1;
   int  numQSorted;
   unsigned short s;
   if (verb >= 4) VPrintf0 ( "        main sort initialise ...\n" );

   /*-- set up the 2-byte frequency table --*/
   for (i = 65536; i >= 0; i--) ftab[i] = 0;

   j = block[0] << 8;
   i = nblock-1;
   for (; i >= 3; i -= 4) {
      quadrant[i] = 0;
      j = (j >> 8) | ( ((unsigned short)block[i]) << 8);
      ftab[j]++;
      quadrant[i-1] = 0;
      j = (j >> 8) | ( ((unsigned short)block[i-1]) << 8);
      ftab[j]++;
      quadrant[i-2] = 0;
      j = (j >> 8) | ( ((unsigned short)block[i-2]) << 8);
      ftab[j]++;
      quadrant[i-3] = 0;
      j = (j >> 8) | ( ((unsigned short)block[i-3]) << 8);
      ftab[j]++;
   }
   for (; i >= 0; i--) {
      quadrant[i] = 0;
      j = (j >> 8) | ( ((unsigned short)block[i]) << 8);
      ftab[j]++;
   }

   /*-- (emphasises close relationship of block & quadrant) --*/
   for (i = 0; i < BZ_N_OVERSHOOT; i++) {
      block   [nblock+i] = block[i];
      quadrant[nblock+i] = 0;
   }

   if (verb >= 4) VPrintf0 ( "        bucket sorting ...\n" );

   /*-- Complete the initial radix sort --*/
   for (i = 1; i <= 65536; i++) ftab[i] += ftab[i-1];

   s = block[0] << 8;
   i = nblock-1;
   for (; i >= 3; i -= 4) {
      s = (s >> 8) | (block[i] << 8);
      j = ftab[s] -1;
      ftab[s] = j;
      ptr[j] = i;
      s = (s >> 8) | (block[i-1] << 8);
      j = ftab[s] -1;
      ftab[s] = j;
      ptr[j] = i-1;
      s = (s >> 8) | (block[i-2] << 8);
      j = ftab[s] -1;
      ftab[s] = j;
      ptr[j] = i-2;
      s = (s >> 8) | (block[i-3] << 8);
      j = ftab[s] -1;
      ftab[s] = j;
      ptr[j] = i-3;
   }
   for (; i >= 0; i--) {
      s = (s >> 8) | (block[i] << 8);
      j = ftab[s] -1;
      ftab[s] = j;
      ptr[j] = i;
   }

   /*--
      Now ftab contains the first loc of every small bucket.
      Calculate the running order, from smallest to largest
      big bucket.
   --*/
   for (i = 0; i <= 255; i++) {
      bigDone     [i] = False;
      runningOrder[i] = i;
   }

   {
      int vv;
      int h = 1;
      do h = 3 * h + 1; while (h <= 256);
      do {
         h = h / 3;
         for (i = h; i <= 255; i++) {
            vv = runningOrder[i];
            j = i;
            while ( BIGFREQ(runningOrder[j-h]) > BIGFREQ(vv) ) {
               runningOrder[j] = runningOrder[j-h];
               j = j - h;
               if (j <= (h - 1)) goto zero;
            }
            zero:
            runningOrder[j] = vv;
         }
      } while (h != 1);
   }

   /*--
      The main sorting loop.
   --*/

   numQSorted = 0;

   for (i = 0; i <= 255; i++) {

      /*--
         Process big buckets, starting with the least full.
         Basically this is a 3-step process in which we call
         mainQSort3 to sort the small buckets [ss, j], but
         also make a big effort to avoid the calls if we can.
      --*/
      ss = runningOrder[i];

      /*--
         Step 1:
         Complete the big bucket [ss] by quicksorting
         any unsorted small buckets [ss, j], for j != ss.  
         Hopefully previous pointer-scanning phases have already
         completed many of the small buckets [ss, j], so
         we don't have to sort them at all.
      --*/
      for (j = 0; j <= 255; j++) {
         if (j != ss) {
            sb = (ss << 8) + j;
            if ( ! (ftab[sb] & SETMASK) ) {
               int lo = ftab[sb]   & CLEARMASK;
               int hi = (ftab[sb+1] & CLEARMASK) - 1;
               if (hi > lo) {
                  if (verb >= 4)
                     VPrintf4 ( "        qsort [0x%x, 0x%x]   "
                                "done %d   this %d\n",
                                ss, j, numQSorted, hi - lo + 1 );
                  mainQSort3 ( 
                     ptr, block, quadrant, nblock, 
                     lo, hi, BZ_N_RADIX, budget 
                  );   
                  numQSorted += (hi - lo + 1);
                  if (*budget < 0) return;
               }
            }
            ftab[sb] |= SETMASK;
         }
      }

      AssertH ( !bigDone[ss], 1006 );

      /*--
         Step 2:
         Now scan this big bucket [ss] so as to synthesise the
         sorted order for small buckets [t, ss] for all t,
         including, magically, the bucket [ss,ss] too.
         This will avoid doing Real Work in subsequent Step 1's.
      --*/
      {
         for (j = 0; j <= 255; j++) {
            copyStart[j] =  ftab[(j << 8) + ss]     & CLEARMASK;
            copyEnd  [j] = (ftab[(j << 8) + ss + 1] & CLEARMASK) - 1;
         }
         for (j = ftab[ss << 8] & CLEARMASK; j < copyStart[ss]; j++) {
            k = ptr[j]-1; if (k < 0) k += nblock;
            c1 = block[k];
            if (!bigDone[c1])
               ptr[ copyStart[c1]++ ] = k;
         }
         for (j = (ftab[(ss+1) << 8] & CLEARMASK) - 1; j > copyEnd[ss]; j--) {
            k = ptr[j]-1; if (k < 0) k += nblock;
            c1 = block[k];
            if (!bigDone[c1]) 
               ptr[ copyEnd[c1]-- ] = k;
         }
      }

      AssertH ( (copyStart[ss]-1 == copyEnd[ss])
                || 
                /* Extremely rare case missing in bzip2-1.0.0 and 1.0.1.
                   Necessity for this case is demonstrated by compressing 
                   a sequence of approximately 48.5 million of character 
                   251; 1.0.0/1.0.1 will then die here. */
                (copyStart[ss] == 0 && copyEnd[ss] == nblock-1),
                1007 )

      for (j = 0; j <= 255; j++) ftab[(j << 8) + ss] |= SETMASK;

      /*--
         Step 3:
         The [ss] big bucket is now done.  Record this fact,
         and update the quadrant descriptors.  Remember to
         update quadrants in the overshoot area too, if
         necessary.  The "if (i < 255)" test merely skips
         this updating for the last bucket processed, since
         updating for the last bucket is pointless.

         The quadrant array provides a way to incrementally
         cache sort orderings, as they appear, so as to 
         make subsequent comparisons in fullGtU() complete
         faster.  For repetitive blocks this makes a big
         difference (but not big enough to be able to avoid
         the fallback sorting mechanism, exponential radix sort).

         The precise meaning is: at all times:

            for 0 <= i < nblock and 0 <= j <= nblock

            if block[i] != block[j], 

               then the relative values of quadrant[i] and 
                    quadrant[j] are meaningless.

               else {
                  if quadrant[i] < quadrant[j]
                     then the string starting at i lexicographically
                     precedes the string starting at j

                  else if quadrant[i] > quadrant[j]
                     then the string starting at j lexicographically
                     precedes the string starting at i

                  else
                     the relative ordering of the strings starting
                     at i and j has not yet been determined.
               }
      --*/
      bigDone[ss] = True;

      if (i < 255) {
         int bbStart  = ftab[ss << 8] & CLEARMASK;
         int bbSize   = (ftab[(ss+1) << 8] & CLEARMASK) - bbStart;
         int shifts   = 0;

         while ((bbSize >> shifts) > 65534) shifts++;

         for (j = bbSize-1; j >= 0; j--) {
            int a2update     = ptr[bbStart + j];
            unsigned short qVal        = (unsigned short)(j >> shifts);
            quadrant[a2update] = qVal;
            if (a2update < BZ_N_OVERSHOOT)
               quadrant[a2update + nblock] = qVal;
         }
         AssertH ( ((bbSize-1) >> shifts) <= 65535, 1002 );
      }

   }

   if (verb >= 4)
      VPrintf3 ( "        %d pointers, %d sorted, %d scanned\n",
                 nblock, numQSorted, nblock - numQSorted );
}

#undef BIGFREQ
#undef SETMASK
#undef CLEARMASK


/*---------------------------------------------*/
/* Pre:
      nblock > 0
      arr2 exists for [0 .. nblock-1 +N_OVERSHOOT]
      ((unsigned char*)arr2)  [0 .. nblock-1] holds block
      arr1 exists for [0 .. nblock-1]

   Post:
      ((unsigned char*)arr2) [0 .. nblock-1] holds block
      All other areas of block destroyed
      ftab [ 0 .. 65536 ] destroyed
      arr1 [0 .. nblock-1] holds sorted order
*/
void BZ2_blockSort ( EState* s )
{
   unsigned int* ptr    = s->ptr; 
   unsigned char*  block  = s->block;
   unsigned int* ftab   = s->ftab;
   int   nblock = s->nblock;
   int   verb   = s->verbosity;
   int   wfact  = s->workFactor;
   unsigned short* quadrant;
   int   budget;
   int   budgetInit;
   int   i;

   if (nblock < 10000) {
      fallbackSort ( s->arr1, s->arr2, ftab, nblock, verb );
   } else {
      /* Calculate the location for quadrant, remembering to get
         the alignment right.  Assumes that &(block[0]) is at least
         2-byte aligned -- this should be ok since block is really
         the first section of arr2.
      */
      i = nblock+BZ_N_OVERSHOOT;
      if (i & 1) i++;
      quadrant = (unsigned short*)(&(block[i]));

      /* (wfact-1) / 3 puts the default-factor-30
         transition point at very roughly the same place as 
         with v0.1 and v0.9.0.  
         Not that it particularly matters any more, since the
         resulting compressed stream is now the same regardless
         of whether or not we use the main sort or fallback sort.
      */
      if (wfact < 1  ) wfact = 1;
      if (wfact > 100) wfact = 100;
      budgetInit = nblock * ((wfact-1) / 3);
      budget = budgetInit;

      mainSort ( ptr, block, quadrant, ftab, nblock, verb, &budget );
      if (verb >= 3) 
         VPrintf3 ( "      %d work, %d block, ratio %5.2f\n",
                    budgetInit - budget,
                    nblock, 
                    (float)(budgetInit - budget) /
                    (float)(nblock==0 ? 1 : nblock) ); 
      if (budget < 0) {
         if (verb >= 2) 
            VPrintf0 ( "    too repetitive; using fallback"
                       " sorting algorithm\n" );
         fallbackSort ( s->arr1, s->arr2, ftab, nblock, verb );
      }
   }

   s->origPtr = -1;
   for (i = 0; i < s->nblock; i++)
      if (ptr[i] == 0)
         { s->origPtr = i; break; };

   AssertH( s->origPtr != -1, 1003 );
}


/*-------------------------------------------------------------*/
/*--- end                                       blocksort.c ---*/
/*-------------------------------------------------------------*/
