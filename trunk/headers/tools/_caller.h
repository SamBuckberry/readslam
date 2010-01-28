namespace ReadSlam
{
	/*
	 * The caller is designed to gather significance stats for a single base
	 */
	struct Caller
	{
		BaseCaller a;
		BaseCaller t;
		BaseCaller c;
		BaseCaller g;
		MethBaseCaller mc;
		
		Stacker stacker;
		
		//Load the genome, stack all reads, optimize the base callers and save stacks to disk
		void stack(string genome, string reads, string stacks, string ref, float falsepos)
		{
			stacker.clear();
			stacker.load_genome(genome);
			stacker.stack(reads);
			stacker.call();
			
			a.optimize(stacker.stacks, 'A', ref, falsepos);
			t.optimize(stacker.stacks, 'T', ref, falsepos);
			c.optimize(stacker.stacks, 'C', ref, falsepos);
			g.optimize(stacker.stacks, 'G', ref, falsepos);
			
			call();
			
			stacker.split(stacks);
		}
		
		//Make all the base calls for the entire genome
		void call()
		{
			Stack s;

			for (stacker.stacks_itr = stacker.stacks.begin(); stacker.stacks_itr != stacker.stacks.end(); ++stacker.stacks_itr)
			{
				for (int i=0; i<stacker.stacks_itr->second.length; ++i)
				{
					s = &(stacker.stacks_itr->second.stacks[i]);

					//Determine if each of the 4 base calls is significant
					int fN = f->a + f->t + f->c + f->g + f->n;
					int rN = r->a + r->t + r->c + r->g + r->n;

					f->call = iupac(
						a.sig(f->a, fN), 
						t.sig(f->t, fN), 
						c.sig(f->c, fN), 
						g.sig(f->g, fN)
					);
					r->call = iupac(
						a.sig(r->a, rN), 
						t.sig(r->t, rN), 
						c.sig(r->c, rN), 
						g.sig(r->g, rN)
					);
				}
			}
		}
	
		//Return the IUPAC code for a combination of bases
		char iupac(bool a, bool t, bool c, bool g)
		{
			if ( a && !t && !c && !g) return 'A';
			if (!a &&  t && !c && !g) return 'T';
			if (!a && !t &&  c && !g) return 'C';
			if (!a && !t && !c &&  g) return 'G';

			if (!a &&  t &&  c &&  g) return 'B';
			if ( a && !t &&  c &&  g) return 'V';
			if ( a &&  t && !c &&  g) return 'D';
			if ( a &&  t &&  c && !g) return 'H';

			if ( a &&  t && !c && !g) return 'W';
			if ( a && !t &&  c && !g) return 'M';
			if (!a &&  t &&  c && !g) return 'Y';
			if ( a && !t && !c &&  g) return 'R';
			if (!a &&  t && !c &&  g) return 'K';
			if (!a && !t &&  c &&  g) return 'S';

			if ( a &&  t &&  c &&  g) return 'N';
			if (!a && !t && !c && !g) return 'X';
		}
	};
}
