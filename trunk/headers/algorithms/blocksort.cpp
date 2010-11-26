//Fallback O(N log(N)^2) sorting for repetitive blocks
void crudesort(int x1, int x2)
{
	if (x1 == x2) return;

	if (x2 - x1 > 3)
	{
		for (i=x2-4; i>=x1; i--)
		{
			int block = blocks[i];
			char letter = sequence[block];
			
			for (j=i+4; j<=x2 && letter>sequence[blocks[j]]; j+=4)
			{
				blocks[j-4] = blocks[j];
			}
			blocks[j-4] = block;
		}
	}
	for (int i=x2-1; i>=x1; i--)
	{
		int block = blocks[i];
		char letter = sequence[block];
		
		for (j=i+1; j<=x2 && letter>sequence[blocks[j]]; j++)
		{
			blocks[j-1] = blocks[j];
		}
		blocks[j-1] = block;
	}
}

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

/*---------------------------------------------*/
/* Pre:
      length > 0
      eclass exists for [0 .. length-1]
      ((unsigned char*)eclass) [0 .. length-1] holds block
      ptr exists for [0 .. length-1]

   Post:
      ((unsigned char*)eclass) [0 .. length-1] holds block
      All other areas of eclass destroyed
      fmap [0 .. length-1] holds sorted order
      bhtab [ 0 .. 2+(length/32) ] destroyed
*/

#define       SET_BH(zz)  bhtab[(zz) >> 5] |= (1 << ((zz) & 31))
#define     CLEAR_BH(zz)  bhtab[(zz) >> 5] &= ~(1 << ((zz) & 31))
#define     ISSET_BH(zz)  (bhtab[(zz) >> 5] & (1 << ((zz) & 31)))
#define      WORD_BH(zz)  bhtab[(zz) >> 5]
#define UNALIGNED_BH(zz)  ((zz) & 0x01f)

static void fallbackSort (unsigned int* bhtab)
{
   int totals[257];
   int counts[257];
   int nNotDone;
   int nBhtab;

   /*--
      Initial 1-char radix sort to generate
      initial blocks and initial BH bits.
   --*/
   for (int i = 0; i < 257;    i++) totals[i] = 0;
   for (int i = 0; i < length; i++) totals[sequence[i]]++;
   for (int i = 0; i < 256;    i++) counts[i] = totals[i];
   for (int i = 1; i < 257;    i++) totals[i] += totals[i-1];

   for (int i = 0; i < length; i++){
      int j = sequence[i];
      int k = totals[j] - 1;
      totals[j] = k;
      blocks[k] = i;
   }

   nBhtab = 2 + (length / 32);
   for (int i = 0; i < nBhtab; i++) bhtab[i] = 0;
   for (int i = 0; i < 256; i++) SET_BH(totals[i]);

   /*--
      Inductively refine the buckets.  Kind-of an
      "exponential radix sort" (!), inspired by the
      Manber-Myers suffix array construction algorithm.
   --*/

   /*-- set sentinel bits for block-end detection --*/
   for (int i = 0; i < 32; i++) { 
      SET_BH(length + 2*i);
      CLEAR_BH(length + 2*i + 1);
   }

   /*-- the log(N) loop --*/
   int H = 1;
   while (true) {

      int j = 0;
      for (int i = 0; i < length; i++) {
         if (ISSET_BH(i)) j = i;
         int k = blocks[i] - H; if (k < 0) k += length;
         eclass[k] = j;
      }

      nNotDone = 0;
      int r = -1;
      while (1) {

	 /*-- find the next non-singleton bucket --*/
         int k = r + 1;
         while (ISSET_BH(k) && UNALIGNED_BH(k)) k++;
         if (ISSET_BH(k)) {
            while (WORD_BH(k) == 0xffffffff) k += 32;
            while (ISSET_BH(k)) k++;
         }
         int l = k - 1;
         if (l >= length) break;
         while (!ISSET_BH(k) && UNALIGNED_BH(k)) k++;
         if (!ISSET_BH(k)) {
            while (WORD_BH(k) == 0x00000000) k += 32;
            while (!ISSET_BH(k)) k++;
         }
         int r = k - 1;
         if (r >= length) break;

         /*-- now [l, r] bracket current bucket --*/
         if (r > l) {
            nNotDone += (r - l + 1);
            fallbackQSort3 ( blocks, eclass, l, r );

            /*-- scan bucket and generate header bits-- */
            int cc = -1;
            for (i = l; i <= r; i++) {
               cc1 = eclass[blocks[i]];
               if (cc != cc1) { SET_BH(i); cc = cc1; };
            }
         }
      }
     int  H *= 2;
      if (H > length || nNotDone == 0) break;
   }

   /*-- 
      Reconstruct the original block in
      sequence [0 .. length-1], since the
      previous phase destroyed it.
   --*/
   int j = 0;
   for (int i = 0; i < length; i++) {
      while (counts[j] == 0) j++;
      counts[j]--;
      sequence[blocks[i]] = (unsigned char)j;
   }
}


string sequence;
int length;
vector<unsigned short> cache;
int budget;

//Compare two blocks (sequence comparison and cache comparison)
bool compare(int x1, int x2)
{
	for (int i=0; i<length; ++i)
	{
		if (sequence[x1] != sequence[x2])
		{
			return sequence[x1] > sequence[x2];
		}
		if (cache[x1] != cache[x2])
		{
			return cache[x1] > cache[x2];
		}
		x1++; if (x1 == length) x1 = 0;
		x2++; if (x2 == length) x2 = 0;
		budget--;
	}
	return false;
}

static int incs[14] = {
	1, 4, 13, 40, 121, 364, 1093, 3280, 9841, 29524, 88573, 265720, 797161, 2391484
};

static void mainSimpleSort(unsigned int* ptr, int x1, int x2, int d)
{
	int bigN = x2 - x1 + 1;
	if (bigN < 2) return;

	unsigned int v;

	int hp = 0;

	while (incs[hp] < bigN)
	{
		hp++;
	}

	for (--hp; hp >= 0; --hp)
	{
		int h = incs[hp];
		int i = x1 + h;
		
		while (true)
		{
			for (int k=0; k<3; ++k)
			{
				if (i > x2) break;
				int v = ptr[i];
				int j = i;

				while (compare(ptr[j-h]+d, v+d))
				{
					ptr[j] = ptr[j-h];
					j = j - h;
					if (j <= (x1 + h - 1)) break;
				}
				ptr[j] = v;
				i++;
			}
			if (*budget < 0) return;
		}
	}
}

static void qsort3(int x1, int x2)
{
	//use own implementation here
}


/*---------------------------------------------*/
/* Pre:
      length > N_OVERSHOOT
      block32 exists for [0 .. length-1 +N_OVERSHOOT]
      ((unsigned char*)block32) [0 .. length-1] holds block
      ptr exists for [0 .. length-1]

   Post:
      ((unsigned char*)block32) [0 .. length-1] holds block
      All other areas of block32 destroyed
      counts [0 .. 65536 ] destroyed
      ptr [0 .. length-1] holds sorted order
      if (*budget < 0), sorting was abandoned
*/

#define BIGFREQ(b) (counts[((b)+1) << 8] - counts[(b) << 8])

static void mainSort(
	unsigned int* ptr, 
	unsigned char*  block,
	unsigned short* cache, 
	vector<unsigned int> &counts,
	int  length,
	int* budget
)
{
   int  runningOrder[256];
   bool   bigDone[256];
   unsigned char  c1;
   unsigned short s;

	//Set up the 2-byte frequency table
	vector<unsigned int> counts;
	counts.clear();
	counts.resize(65536,0);
	
	int j = sequence[0] << 8;
	int i = length - 1;
	
	while (i >= 3)
	{
		cache[i] = 0;
		j = (j >> 8) | ( ((unsigned short)sequence[i]) << 8);
		counts[j]++;

		cache[i-1] = 0;
		j = (j >> 8) | ( ((unsigned short)sequence[i-1]) << 8);
		counts[j]++;

		cache[i-2] = 0;
		j = (j >> 8) | ( ((unsigned short)sequence[i-2]) << 8);
		counts[j]++;

		cache[i-3] = 0;
		j = (j >> 8) | ( ((unsigned short)sequence[i-3]) << 8);
		counts[j]++;

		i -= 4;
	}
	while (i >= 0)
	{
		cache[i] = 0;
		j = (j >> 8) | ( ((unsigned short)sequence[i]) << 8);
		counts[j]++;
	}

   /*-- (emphasises close relationship of block & cache) --*/
   for (i = 0; i < BZ_N_OVERSHOOT; i++) {
      block   [length+i] = sequence[i];
      cache[length+i] = 0;
   }

	/*-- Complete the initial radix sort --*/
	for (int i=1; i<=65536; ++i)
	{
		counts[i] += counts[i-1];
	}

	unsigned short s = sequence[0] << 8;
	int i = length - 1;
	
	while (i >= 3)
	{
		s = (s >> 8) | (sequence[i] << 8);
		j = counts[s] -1;
		counts[s] = j;
		ptr[j] = i;

		s = (s >> 8) | (sequence[i-1] << 8);
		j = counts[s] -1;
		counts[s] = j;
		ptr[j] = i-1;

		s = (s >> 8) | (sequence[i-2] << 8);
		j = counts[s] -1;
		counts[s] = j;
		ptr[j] = i-2;

		s = (s >> 8) | (sequence[i-3] << 8);
		j = counts[s] -1;
		counts[s] = j;
		ptr[j] = i-3;

		i -= 4; 
	}
	while (i >= 0)
	{
		s = (s >> 8) | (sequence[i] << 8);
		j = counts[s] -1;
		counts[s] = j;
		ptr[j] = i;
		i--;
	}

   /*--
      Now counts contains the first loc of every small bucket.
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


	//Main sorting loop. Radix sort is a two-byte affair. The outer loop 
	//covers the first byte (big bucket), the inner loop is for the second byte
	vector<bool> flagged;
	flagged.clear();
	flagged.resize(65536,false);

	int prev_x1s[256];
	int prev_x2s[256];

	for (int i=0; i<=255; ++i)
	{
		/*
			Process the big buckets, starting with least full to most full
			qsort3 is used to sort the small buckets [byte1,byte2]
			A shortcut is used to skip sorting where possible
		*/
		int byte1 = runningOrder[i];

		/*
			Step 1: work on the big bucket [byte1].
			Quicksort all the small buckets [byte1,byte2] where byte1 != byte2.
		*/
		for (int byte2 = 0; byte2 <= 255; ++byte2)
		{
			if (byte2 == byte1) continue;

			int radix = (byte1 << 8) + byte2;
			
			if (!flagged[radix])
			{
				int lo = counts[radix];
				int hi = counts[radix + 1] - 1;
				if (hi > lo)
				{
					qsort3(ptr, block, cache, length, lo, hi, BZ_N_RADIX, budget);
					if (*budget < 0) return;
				}
			}
			flagged[radix] = true;
		}

		/*
			Because byte1 has been sorted, all the radix's that have byte1 as the
			second byte can also be easily ordered. It means that these radix values
			can be skipped in future loops
		*/

		//Loop through all the radix's ending with byte1
		for (int byte = 0; byte <= 255; ++byte)
		{
			//Store the start and end of the range of radix's starting with byte
			prev_x1s[byte] = counts[(byte << 8) + byte1];
			prev_x2s[byte] = counts[(byte << 8) + byte1 + 1] - 1;
			
			//Flag the radix [byte,byte1]
			flagged[(byte << 8) + byte1] = true;
		}
		
		//Loop from radix[byte1,first][first] to radix[byte1,byte1][first]
		for (int x = counts[byte1 << 8]; x < prev_x1s[byte1]; x++)
		{
			int pos = ptr[x] - 1; if (pos < 0) pos += length;
			int c1 = sequence[pos];
			if (bigDone[c1]) continue;
			ptr[prev_x1s[c1]++] = pos;
		}
		
		//Loop from radix[byte1,last][last] to radix[byte1][byte1][last]
		for (int x = (counts[(byte1+1) << 8]) - 1; x > prev_x2s[byte1]; x--)
		{
			int pos = ptr[x] - 1; if (pos < 0) pos += length;
			int c1 = sequence[pos];
			if (bigDone[c1]) continue;
			ptr[prev_x2s[c1]--] = pos;
		}

		/*
		Step 3:
			The [byte1] big bucket is now done.  Record this fact,
			and update the cache descriptors.  Remember to
			update caches in the overshoot area too, if
			necessary.

			The cache array provides a way to incrementally
			cache sort orderings, as they appear, so as to 
			make subsequent comparisons in fullGtU() complete
			faster.  For repetitive blocks this makes a big
			difference (but not big enough to be able to avoid
			the fallback sorting mechanism, exponential radix sort).

			The precise meaning is: at all times:

			for 0 <= i < length and 0 <= j <= length

				if sequence[i] != sequence[j]
					then the relative values of cache[i] and 
					cache[j] are meaningless.

				else
					if cache[i] < cache[j]
						then the string starting at i lexicographically
						precedes the string starting at j

					else if cache[i] > cache[j]
						then the string starting at j lexicographically
						precedes the string starting at i

					else
						the relative ordering of the strings starting
						at i and j has not yet been determined.
		*/
		//Record that the big bucket has been processed
		bigDone[byte1] = true;

		//Pointless to update the last bucket
		if (i == 255) continue;

		int bbStart = counts[byte1 << 8];
		int bbSize  = counts[(byte1+1) << 8] - bbStart;
		int shifts  = 0;

		while ((bbSize >> shifts) > 65534) shifts++;

		for (int j = bbSize-1; j >= 0; j--)
		{
			int a2update = ptr[bbStart + j];
			unsigned short qVal = (unsigned short)(j >> shifts);
			cache[a2update] = qVal;

			if (a2update < BZ_N_OVERSHOOT)
			{
				cache[a2update + length] = qVal;
			}
		}
	}
}

/*---------------------------------------------*/
/* Pre:
      length > 0
      arr2 exists for [0 .. length-1 +N_OVERSHOOT]
      ((unsigned char*)arr2)  [0 .. length-1] holds block
      arr1 exists for [0 .. length-1]

   Post:
      ((unsigned char*)arr2) [0 .. length-1] holds block
      All other areas of block destroyed
      counts [ 0 .. 65536 ] destroyed
      arr1 [0 .. length-1] holds sorted order
*/
void BZ2_blockSort ( EState* s )
{
   unsigned int* ptr    = s->ptr; 
   unsigned char*  block  = s->block;
   unsigned int* counts   = s->counts;
   int   length = s->length;
   int   wfact  = s->workFactor;
   unsigned short* cache;
   int   budget;
   int   budgetInit;
   int   i;

   if (length < 10000) {
      fallbackSort ( s->arr1, s->arr2, counts, length);
   } else {
      /* Calculate the location for cache, remembering to get
         the alignment right.  Assumes that &(sequence[0]) is at least
         2-byte aligned -- this should be ok since block is really
         the first section of arr2.
      */
      i = length+BZ_N_OVERSHOOT;
      if (i & 1) i++;
      cache = (unsigned short*)(&(sequence[i]));

      /* (wfact-1) / 3 puts the default-factor-30
         transition point at very roughly the same place as 
         with v0.1 and v0.9.0.  
         Not that it particularly matters any more, since the
         resulting compressed stream is now the same regardless
         of whether or not we use the main sort or fallback sort.
      */
      if (wfact < 1  ) wfact = 1;
      if (wfact > 100) wfact = 100;
      budgetInit = length * ((wfact-1) / 3);
      budget = budgetInit;

      mainSort ( ptr, block, cache, counts, length, &budget );
      if (budget < 0){
         fallbackSort ( s->arr1, s->arr2, counts, length);
      }
   }

   s->origPtr = -1;
   for (i = 0; i < s->length; i++)
      if (ptr[i] == 0)
         { s->origPtr = i; break; };
}


/*-------------------------------------------------------------*/
/*--- end                                       blocksort.c ---*/
/*-------------------------------------------------------------*/
